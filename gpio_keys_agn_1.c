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

#define GPIO_MAX_COUNT    保存最大引脚数
//保存引脚的数组
static struct gpio key_gpios[GPIO_MAX_COUNT];
//按键的设备号
static int key_major = 0;
//定义按键的键值
#define KEY0_VAL            0xF0
#define INV_KEY_VAL         0x00
//定义关于设备注册的参数

#define KEY_CNT             1               /* 按键设备数 */
#define KEY_NAME            "key"           /* 按键名称 */


/* key设备结构体 */
struct key_dev {
    dev_t devid;                /* 设备号 */
    struct cdev chrdev;         /* cdev */
    struct class *devclass;     /* 设备类指针 */
    struct device *device;      /* 设备指针 */
    int major;                  /* 主设备号 */
    int minor;                  /* 次设备号 */
    struct device_node *nd;     /* 设备节点 */
    int key_gpio;               /* key使用的GPIO编号 */
    atomic_t key_val;           /* 按键值 */
};
//实例化
struct key_dev keydev;

//按键io口初始化
static int key_io_init(struct device_node *pdevnode)
{
	int rt;
	int count, index;
	unsigned int pins[GPIO_MAX_COUNT];	
	const char *str;
	
	//从设备树中提取GPIO口信息 
	/*第一个参数是设备节点 
	第二个参数要读取的属性名字 
	第三个参数是数组  
	第四个参数数组元素个数
	*/
	rt = of_property_read_u32_array(pdevnode, "key-gpios", pins, count);
	
	if(rt < 0)
	{
		printk(KERN_ERR"of_property_read_u32_array key-gpios fail\n");	
		goto err_of_property_read_u32_array;	
	}
    //统计引脚个数
	for (index = 0; index < count; index++) 
	{
		key_gpios[index].gpio =  pins[index];
		
		printk(KERN_INFO"正确获取pin %d\n",pins[index]);
	}
	
   
	//初始化key所用的IO口 
	gpio_free_array(key_gpios, count);
	//申请一组gpio
	rt = gpio_request_array(key_gpios,count);
	//设置端口为输入 
    gpio_direction_input(key_gpios[0]);
	gpio_direction_input(key_gpios[1]);
	if(rt < 0)
	{
		printk(KERN_ERR"gpio_request_array error\n");
		goto err_gpio_request_array;
	}
	
	//错误跳转
	err_of_property_read_u32_array:
	err_gpio_request_array:
    return 0;
}

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
    int ret = 0;
	//保存读取引脚的值
    char key_val[2];
	//私有数据
	struct key_dev *dev = filp->private_data;
	key_val[0]=key_read_handle(key_gpios[0])
	key_val[1]=key_read_handle(key_gpios[1])
	//输出数据
    ret = copy_to_user(buf, &key_val, sizeof(key_val));

    return ret;
}
//按键处理函数
char key_read_handle(static struct gpio key_gpios)
{
	char key_val;
    if (gpio_get_value(key_gpios) == 0 |) {
        /* key按下 */

        /* 等待按键释放 */
        while(gpio_get_value(key_gpios) == 0);

        /* 设置按键值 
		将原子变量设置为KEY0_VAL  0xF0 证明按下了按键
		*/
        atomic_set(&dev->key_val, KEY0_VAL);
    } else {
        //设置按键值 没有按下设置为0
        atomic_set(&dev->key_val, INV_KEY_VAL);
    }

    /* 保存按键值 */
    key_val=atomic_read(&dev->key_val);
	return key_val;
	
}
//设备操作函数 
static struct file_operations key_fops = {
    .owner = THIS_MODULE,
    .open = key_open,
	.release = key_close,
    .read = key_read,
};
static int mykey_probe(struct platform_device *pdev)
{
	//初始化
	struct device_node *pdevnode = pdev->dev.of_node;
	key_io_init(pdevnode);
	
	//注册字符设备驱动 创建设备号 */
    if (keydev.major) {
        /* 已经定义主设置号 根据主设备号申请设备ID */
        keydev.devid = MKDEV(keydev.major, 0);

	 //注册字符设备 
        register_chrdev_region(keydev.devid, KEY_CNT, KEY_NAME);
    } else {
		
    //未定义主设备号,让内核动态分配设备ID *	
	//第一个参数：获取系统分配的设备号
	//第二个参数：次设备号的开始值
	//第三个参数：次设备的数量为1
	//第四个参数：若设备号有效，则在/proc/devices文件中找到"key"/
        alloc_chrdev_region(&keydev.devid, 0, KEY_CNT, KEY_NAME);

        /* 获取主设备号 */
        keydev.major = MAJOR(keydev.devid);

        /* 获取次设备号 */
        keydev.minor = MINOR(keydev.devid);
    }

    /* 初始化cdev成员 */
    keydev.chrdev.owner = THIS_MODULE;
	cdev_init(&keydev.chrdev, &key_fops);

    /* 添加一个cdev */
    cdev_add(&keydev.chrdev, keydev.devid, KEY_CNT);
	
	/* 创建类 */
    keydev.devclass = class_create(THIS_MODULE, KEY_NAME);
	// IS_ERR()就是用来判断指针是否有错
    if (IS_ERR(keydev.devclass)) {
        return PTR_ERR(keydev.devclass);
    }

    /* 创建设备 */
    keydev.device = device_create(keydev.devclass, NULL, keydev.devid, NULL, KEY_NAME);
    if (IS_ERR(keydev.device)) {
        return PTR_ERR(keydev.device);
    }

    return 0;
		
		
		
		
}
		
static int mykey_remove(struct platform_device *dev)
{	
	int count;
	struct device_node *pdevnode = dev->dev.of_node;
	printk(KERN_INFO "mykey: key platform driver remove!\r\n");
	atomic_set(&keydev.key_val, INV_KEY_VAL);


	/* 计数 */
    count = sizeof(key_gpios)/sizeof(key_gpios[0]);
	
	/* 删除cdev */
	cdev_del(&keydev.chrdev);

	/* 注销设备号 */
	unregister_chrdev_region(keydev.devid, KEY_CNT);
	
	/* 注销设备 */
	device_destroy(keydev.devclass, keydev.devid);

	/* 注销设备类 */
	class_destroy(keydev.devclass);

	//释放引脚
	gpio_free_array(key_gpios, count);

	return 0;
}






//匹配列表
static const struct of_device_id key_of_match[] = {
	{ .compatible = "agn,gpio_key" },//兼容属性与设备树上的一致
	{ /* Sentinel 最后一个要空的*/ }
};

/* platform驱动结构体 */
static struct platform_driver key_driver = {
	.driver = {
		.name			= "mykey",		// 驱动名字，用于和设备匹配
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



//驱动程序的入口
module_init(_key_init);

//驱动程序的出口
module_exit(_key_exit);

MODULE_DESCRIPTION("MYKEY Device Driver");
MODULE_LICENSE("GPL");


