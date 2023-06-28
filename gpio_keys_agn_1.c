#include <linux/module.h> //module_init module_exit
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>  //cdev_init
#include <linux/err.h>
#include <linux/fs.h> //register_chrdev_region 
#include <asm/uaccess.h>//copy_from_user copy_to_user
#include <linux/platform_device.h> //平台设备驱动模型
#include <linux/of.h>		//设备树相关头文件
#include <linux/of_gpio.h>
#include <linux/gpio/consumer.h>    // gpiod
#include <linux/of_address.h>
#include <linux/of_fdt.h>
#include <linux/of_platform.h> 
#include <linux/miscdevice.h> //混杂设备注册
#include <linux/interrupt.h> //中断
#include <linux/wait.h>   //等待队列
#include <linux/string.h>

//引脚资源// 兼容性

#if 0  //  不编译方便查看设备树
gpio_keys_agn 
    {        
        compatible = "agn,gpio_key";        
		key-gpios = <&pio 2 GPIO_ACTIVE_LOW 
			&pio 7 GPIO_ACTIVE_LOW>;
    };   
#endif

#define KEY_NAME            "key"           /* 按键名称 */
//增加__must_be_array(arr)来检查参数是否是一个数组，如果不是则会在编译期间报错
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))


/* key结构体 */
struct key_data {
    int irq;                // 中断号
    int gpio;               // GPIO号
    int value;             // 按键状态
    struct gpio_desc desc;    //定义设备描述符
};
//等待队列头
wait_queue_head_t wait_queue; 


/*第一个参数：中断函数注册时的中断号irq
第二个参数：注册的时候最后一个参数dev_id*/
static irqreturn_t key_irq_handler(int irq, void *dev_id)
{
    struct key_data *key = dev_id;
    // 调度tasklet
    tasklet_schedule(&key_tasklet); 
    return IRQ_HANDLED
}

// tasklet处理函数
static void key_tasklet(unsigned long data)
{
    printk(KERN_INFO "tasklet is running...\n");

    // 处理按键事件
    //获取按键的逻辑值
    key->value  = gpiod_get_value(key->desc);
    //有中断就唤醒等待队列
    wake_up_interruptible(wait_queue);

}
// 定义tasklet
DECLARE_TASKLET(key_tasklet, key_tasklet, 0);


//打开开关函数
static int key_open(struct inode *node, struct file *filp)
{
	printk(KERN_INFO"key open\n");
    return 0;
}
static int key_close (struct inode *inode, struct file *file)
{
	
	printk(KERN_INFO"key close\n");
	return 0;
}


//读取数据函数
/*
	filp是打开的设备文件
	buf是返回用户空间的数据缓存区
	len读取长度
	fpos相对文件首地址的偏移 
*/
static ssize_t key_read(struct file *filp, char __user *buf, size_t len, 
loff_t *fpos)
{
    struct key_data *key = container_of(filp->private_data, struct key_data, gpio);
    int value;
    key->value = 0;
    //函数会等待一个条件成立，如果条件（第二个参数）不成立，则会将当前进程或线程挂起，
    //直到条件成立或者被信号中断 这里是等待按键的状态变化实现阻塞
    wait_event_interruptible(wait_queue,key->value)

    value = key->value;

    if (value) {
        if (put_user('1', buf))
            return -EFAULT;
    } else {
        if (put_user('0', buf))
            return -EFAULT;
    }

    return 1;

}

//设备操作函数 
static struct file_operations key_fops = {
    .owner = THIS_MODULE,
    .open = key_open,
	.release = key_close,
    .read = key_read,
};
//混杂设备的结构体
static struct miscdevice key_miscdev[] = {
    {
        .minor = MISC_DYNAMIC_MINOR,//255
        .name = "my-key0",
        .fops = &key_fops,
    },
    {
        .minor = MISC_DYNAMIC_MINOR,
        .name = "my-key1",
        .fops = &key_fops,
    },
};
//创建结构体数组保存两个key的信息
struct key_data key_struct[ARRAY_SIZE(key_miscdev)];


static int mykey_probe(struct platform_device *pdev)
{
	//初始化
	struct device *dev = &pdev->dev;
	struct key_data *key;
	int i, ret,irq;
    // 初始化等待队列头
    init_waitqueue_head(wait_queue); 

	//遍历获取gpio口的属性
	for (i = 0; i < ARRAY_SIZE(key_miscdev); i++) {
		//这样分配的空间设备被卸载时会自动释放
        key = devm_kzalloc(dev, sizeof(*key), GFP_KERNEL);
        if (!key)
            return -ENOMEM;
        //获取gpio描述符 从设备树读取名为"key-gpios"的gpio口第i个端口
        key->desc = gpiod_get_index(dev->of_node, "key-gpios", i, GPIO_ACTIVE_LOW);
		if (IS_ERR(key->desc)) {
            dev_err(dev, "failed to gpiod_get %d \n", i);
            goto err_gpio_get;
        }
        //设置输入模式
        ret = gpiod_direction_input(key->desc);
        if (ret) {
            dev_err(dev, "failed to set gpio%d direction\n", i);
        }
        //获取端口的中断号
        key->irq = gpiod_to_irq(key->desc);
        if(key->irq < 0){
            dev_err(dev, "failed to get the irq%d", i)
        }

        // 将中断号与处理函数对应
        /*
            参数一：中断号
            参数二：处理函数
            参数三：触发方式 
            参数四 名字 
            参数五 devid
        */
        
        
        
        ret = request_irq(key->irq, key_irq_handler, IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING,
                            KEY_NAME, key);
        if (ret) {
            dev_err(dev, "failed to request irq%d", i);
            return ret;
        }

        key_miscdev[i].name = devm_kasprintf(dev, GFP_KERNEL, "%s%d",
                                                   KEY_NAME, i);
        if (!key_miscdev[i].name) {
            ret = -ENOMEM;
            goto err_gpio_misc;
        }
        key_miscdev[i].parent = dev;
        key_miscdev[i].minor = MISC_DYNAMIC_MINOR;
        key_miscdev[i].mode = S_IRUSR | S_IWUSR;
        key_miscdev[i].private_data = data;

        ret = misc_register(&key_miscdev[i]);
        if (ret) {
            dev_err(dev, "failed to register misc device %d\n", i);
            goto err_gpio_misc;
        }
        key_struct[i] = key;
        
    }
//保存数据
    platform_set_drvdata(pdev, key_struct);
    pr_info(DRIVER_NAME ": initialized\n");
    return 0;

err_gpio_get:
    gpiod_put(key->desc);
err_gpio_misc:
while (--i >= 0) {
    misc_deregister(&key_miscdev[i]);
}
return ret;	
		
}
		
static int mykey_remove(struct platform_device *dev)
{	
    struct key_data *data;
    int i;

    for (i = 0; i < ARRAY_SIZE(key_miscdev); i++) {
        key_struct = platform_get_drvdata(pdev);
        misc_deregister(&key_miscdev[i]);
        free_irq(key_struct[i]->irq, key);
        gpiod_put(key_struct[i]->desc);
        tasklet_kill(&key_tasklet); // 杀死tasklet
    }

    return 0;
}






//匹配列表
static const struct of_device_id key_of_match[] = {
	{ .compatible = "agn,gpio_key" },//兼容属性与设备树上的一致
	{ /* Sentinel 最后一个要空的*/ }
};
//定义设备ID列表，以便内核可以自动将驱动程序与设备进行匹配
MODULE_DEVICE_TABLE(of, key_of_match);

/* platform驱动结构体 */
static struct platform_driver key_driver = {
	.driver = {
		.name			= KEY_NAME,		// 驱动名字，用于和设备匹配
		.of_match_table	= key_of_match,		// 设备树匹配表，用于和设备树中定义的设备匹配
	},
	.probe		= mykey_probe,	// probe函数
	.remove		= mykey_remove,	// remove函数
};

//驱动的入口
static int __init _key_init(void)
{	

	
	//平台驱动设备绑定
	int rt = platform_driver_register(&key_driver);
	if (rt <0){
        printk(KERN_INFO "platform_driver_register err\n");

        return rt;
    }

    printk(KERN_INFO "mykey: key driver init！\n");
    return 0;
}

//驱动的出口
static void __exit _key_exit(void)
{
	printk(KERN_INFO "mykey: key driver remove!\r\n");
	/* 平台解绑 */
	platform_driver_unregister(&key_driver);
}

//也可以将该平台设备驱动程序注册到内核中
//module_platform_driver(key_driver);

//驱动程序的入口
module_init(_key_init);
//驱动程序的出口
module_exit(_key_exit);

MODULE_DESCRIPTION("MYKEY Device Driver");
MODULE_LICENSE("GPL");


