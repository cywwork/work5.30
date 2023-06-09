# 第一章 linux设备驱动程序入门



## 1.1设备驱动程序基础

### 驱动程序的概念

设备驱动程序主要负责硬件设备的参数配置、数据读写、中断处理

Linux的运行空间分为内核空间与用户空间，为了保护系统安全，两个空间运行在不同的级别

，不能相互直接访问和共享数据 Linux内核为应用层提供系统调用接口，应用程序可以通过这些接口获得os内核提供的服务 

设备驱动程序是操作系统的一部分，运行在内核态 应用程序呀控制硬件要系统调用访问内核 内核层 硅胶系统调用号调用驱动程序对应的接口函数来访问设备

`内核模块：`Linux内核往外部提供的一个接口 其全称为动态可加载内核模块

Linux是单内核 效率高 可扩展性和可维护性差 模块机制就是为此诞生的



## 设备驱动的作用

为了使软件工程师没必要去学习硬件的知识 趋使硬件设备行动 应用程序只需要调用系统软件的应用编程接口API让硬件工作



## 1.2无操作系统



```c
int main(int argc, char* argv[])
{
    while(1)
    {
        if (serialInt == 1)
    /* 有串口中断*/
    {
        ProcessSerialInt(); /*处理串口中断*/
        serialInt = 0；    /*中断标志变量清 0*/
    }
    if (keyInt == 1)
    /* 有按键中断*/
    {
        ProcessKeyInt(); /* */
        keyInt = 0;
    }
    status = Checkxxx();
    switch (status)
    }    



}
```

```c
/***********
*serial.h 文件
***********/
extern void SerialInit(void);
extern void SerialSent(const char buf*,int count);
extern void SerialRecv(char buf*,int count);

/*   初始化串口  */

void SerialInit(void)
{
    
}

/*   串口发送  */
 void SerialSent(const char buf*,int count)
{
    

}
/*   串口接收  */
void SerialRecv(char buf*,int count)
{
    

}

/*   串口中断处理函数  */

void SerialIsr(void)
{
    

}
```





## 1.3 有操作系统的设备驱动



关键在驱动融入内核 驱动作为连接硬件和内核的桥梁



## 1.4 Linux设备驱动

设备的分类及特点

存储器和外设分为3基础大类 字符设备 块设备 网络设备

字符设备必须串行顺序访问

块设备可以任意访问

内核与网络设备通信与字符设备、网络设备的通信完全不一样 前者主要还是使用套接字接口

## 1.6设备驱动 LED驱动



### 无os的led驱动

GPIO一般由两组寄存器控制 控制寄存器和数据寄存器

- 控制寄存器可设置

GPIO 口的工作方式为输入或者输出。

- 当引脚被设置为输出时，向数据寄存器的对应位写入 1 和 0 会分别在引脚上产生高电平和低电平

- 当引脚设置为输入时，读取数据寄存器的对应位可获得引脚上的电平为高或低。



假设在 GPIO_REG_CTRL 物理地址中控制寄
存器处的第 n 位写入 1 可设置 GPIO 口为输出，在地址 GPIO_REG_DATA 物理地址中数据寄
存器的第 n 位写入 1 或 0 可在引脚上产生高或低电平

```c

#def ine reg_gpio_ctrl *(volatile int *)(ToVirtual(GPIO_REG_CTRL))
#def ine reg_gpio_data *(volatile int *)(ToVirtual(GPIO_REG_DATA))
  /* 初始化 LED */
  void LightInit(void)
  {
  reg_gpio_ctrl |= (1 << n); /* 设置 GPIO 为输出 */
  }
  
  /* 点亮 LED */
 void LightOn(void)
 {
 reg_gpio_data |= (1 << n); /* 在 GPIO 上输出高电平 */
 }
 
 /* 熄灭 LED */
 void LightOff(void)
 {
 reg_gpio_data &= ～ (1 << n); /* 在 GPIO 上输出低电平 */
}
```





### Linux下的LED驱动



```c
#include .../* 包含内核中的多个头文件 */
/* 设备结构体 */
struct light_dev {
    struct cdev cdev; /* 字符设备 cdev 结构体 */
    unsigned char vaule; /* LED 亮时为 1，熄灭时为 0，用户可读写此值 */
};

struct light_dev *light_devp; 
int light_major = LIGHT_MAJOR; 

MODULE_AUTHOR("Barry Song  <@>")
MODULE_LICENSE("Dual BSD/GPL");

 /* 打开和关闭函数 */
int light_open(struct inode *inode, struct file *f ilp) 
{
    struct light_dev *dev; 
     /* 获得设备结构体指针 */
     dev = container_of(inode->i_cdev, struct light_dev, cdev); 
     /* 让设备结构体作为设备的私有信息 */
     filp->private_data = dev; 
     return 0; 
 }
 int light_release(struct inode *inode, struct file *filp) 
{
    return 0;
}

/* 读写设备 : 可以不需要 */
 ssize_t light_read(struct f ile *f ilp, char __user *buf, size_t count, 
 loff_t *f_pos) 
{
 struct light_dev *dev = f ilp->private_data; /* 获得设备结构体 */
 if (copy_to_user(buf, &(dev->value), 1)) 
 return -EFAULT; 
 return 1; 
}

ssize_t light_write(struct f ile *f ilp, const char __user *buf, size_t count, 
 loff_t *f_pos) 
{
    struct light_dev *dev = f ilp->private_data; 
     if (copy_from_user(&(dev->value), buf, 1)) 
         return -EFAULT; 
     /* 根据写入的值点亮和熄灭 LED */
     if (dev->value == 1) 
         light_on();
     else
         light_off();
     return 1; 
 }

 /* ioctl 函数 */
 int light_ioctl(struct inode *inode, struct f ile *f ilp, unsigned int cmd, 
 unsigned long arg) 
 {
    struct light_dev *dev = f ilp->private_data;
    switch (cmd) {
    case LIGHT_ON: 
        dev->value = 1; 
        light_on();
        break; 
    case LIGHT_OFF: 
        dev->value = 0; 
        light_off();
        break; 
    default: 
     /* 不能支持的命令 */
    return -ENOTTY; 
 }
  return 0; 
}

struct file_operations light_fops = {
    .owner = THIS_MODULE,
    .read = light_read,
    .write = light_write,
    .ioctl = light_ioctl,
    .open = light_open,
    .release = light_release,
};
/* 设置字符设备cdev结构体 */
static void light_setup_cdev(struct light_dev *dev, int index)
{
    int err, devno = MKDEV(light_major, index); 
    cdev_init(&dev->cdev, &light_fops); 
    dev->cdev.owner = THIS_MODULE; 
    dev->cdev.ops = &light_fops; 
    err = cdev_add(&dev->cdev, devno, 1); 
    if (err) 
        printk(KERN_NOTICE "Error %d adding LED%d", err, index); 
 }



```














