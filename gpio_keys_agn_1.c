#include <linux/module.h> //module_init module_exit
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>  //cdev_init

#include <linux/fs.h> //register_chrdev_region 
#include <asm/uaccess.h>//copy_from_user copy_to_user

#include <linux/platform_device.h> //平台设备驱动模型
#include <linux/of.h>		//设备树相关头文件
#include <linux/of_gpio.h>
#include <linux/of_address.h>
#include <linux/of_fdt.h>
#include <linux/of_platform.h> 

#include <linux/miscdevice.h> //混杂设备注册
#include <linux/interrupt.h> //中断
#include <linux/wait.h>   //等待队列
#define GPIO_MAX_COUNT    保存最大引脚数
//保存引脚的数组
//static struct gpio key_gpios[GPIO_MAX_COUNT];
//按键的设备号
//static int key_major = 0;
//定义按键的键值
//#define KEY0_VAL            0xF0
//#define INV_KEY_VAL         0x00
//定义关于设备注册的参数
//#define KEY_CNT             1               /* 按键设备数 */

#define KEY_NAME            "key"           /* 按键名称 */
//增加__must_be_array(arr)来检查参数是否是一个数组，如果不是则会在编译期间报错
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]) + __must_be_array(arr))


/* key结构体 */
struct key_data {
    int irq;                // 中断号
    int gpio;               // GPIO号
    bool value;             // 按键状态
    wait_queue_head_t wait_queue; //等待队列头
};
/*第一个参数：中断函数注册时的中断号irq
第二个参数：注册的时候最后一个参数dev_id*/
static irqreturn_t key_irq_handler(int irq, void *dev_id)
{
    struct key_data *key = dev_id
    //获取按键的值
    key->value  = !gpio_get_value(key->gpio)
    //有中断就唤醒等待队列
    wake_up_interruptible(key->wait_queue);

    return IRQ_HANDLED
}
//实例化 改在probe里面分配空间方便释放

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
    //函数会等待一个条件成立，如果条件（第二个参数）不成立，则会将当前进程或线程挂起，
    //直到条件成立或者被信号中断 这里是等待按键的状态变化实现阻塞
    wait_event_interruptible(key->wait_queue,key->value != gpio_get_value(key->gpio))

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



static int mykey_probe(struct platform_device *pdev)
{
	//初始化
	struct device *dev = &pdev->dev;
	struct key_data *key;
	int i, ret,irq;
	//遍历获取gpio口的值
	for (i = 0; i < ARRAY_SIZE(key_miscdev); i++) {
		//这样分配的空间设备被卸载时会自动释放
        key = devm_kzalloc(dev, sizeof(*key), GFP_KERNEL);
        if (!key)
            return -ENOMEM;
        //获取端口号
        key->gpio = of_get_gpio(dev->of_node, i);
        if (key->gpio < 0) {
            dev_err(dev, "failed to get gpio%d\n", i);
            return key->gpio;
        }
		//请求端口资源
        ret = gpio_request(key->gpio, "agn,gpio_key");
        if (ret) {
            dev_err(dev, "failed to request gpio%d\n", i);
            return ret;
        }
		//设置输入模式
        ret = gpio_direction_input(key->gpio);
        if (ret) {
            dev_err(dev, "failed to set gpio%d direction\n", i);
            goto err_gpio;
        }

        //获取端口的中断号
        key->irq = gpio_to_irq(key->gpio);
        if(key->irq < 0){
            dev_err(&pdev->dev, "failed to get the irq%d", i)
        }

        // 将中断号与处理函数对应
        /*
            参数一：中断号
            参数二：处理函数
            参数三：触发方式 
            四 名字 
            五 devid
        */
        ret = request_irq(key->irq, key_irq_handler, IRQF_TRIGGER_LOW,
                            ("key%d", i),key);
        if (ret) {
            dev_err(&pdev->dev, "failed to request irq%d", i);
            return ret;
        }

        key_miscdev[i].name = devm_kasprintf(dev, GFP_KERNEL, "%s%d",
                                                   KEY_NAME, i);
        if (!key_miscdev[i].name) {
            ret = -ENOMEM;
            goto err_gpio;
        }

        key_miscdev[i].parent = dev;
        key_miscdev[i].minor = MISC_DYNAMIC_MINOR;
        key_miscdev[i].mode = S_IRUSR | S_IWUSR;
        key_miscdev[i].private_data = data;

        ret = misc_register(&key_miscdev[i]);
        if (ret) {
            dev_err(dev, "failed to register misc device %d\n", i);
            goto err_gpio;
        }

        init_waitqueue_head(&key->wait_queue);  // 初始化等待队列头
        //保存数据
        platform_set_drvdata(pdev, key);
    }

    pr_info(DRIVER_NAME ": initialized\n");

    return 0;

err_gpio:
while (--i >= 0) {
	key = platform_get_drvdata(pdev);
	misc_deregister(&key_miscdev[i]);
	gpio_free(key->gpio);
}
return ret;	
		
}
		
static int mykey_remove(struct platform_device *dev)
{	
    struct key_data *data;
    int i;

    for (i = 0; i < ARRAY_SIZE(key_miscdev); i++) {
        key = platform_get_drvdata(pdev);
        misc_deregister(&key_miscdev[i]);
        gpio_free(key->gpio);
        free_irq(key->irq, key)
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


