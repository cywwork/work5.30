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

/* 实例结构体 */
struct light_dev *light_devp; 
int light_major = LIGHT_MAJOR; 

/* 作者和证书略 */
MODULE_AUTHOR("Barry Song  <@>")
MODULE_LICENSE("Dual BSD/GPL");

 /* 打开和关闭函数 */
int light_open(struct inode *inode, struct file *filp) 
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
 ssize_t light_read(struct file *filp, char __user *buf, size_t count, 
 loff_t *f_pos) 
{
 struct light_dev *dev = filp->private_data; /* 获得设备结构体 */
 if (copy_to_user(buf, &(dev->value), 1)) 
 return -EFAULT; 
 return 1; 
}

ssize_t light_write(struct file *f ilp, const char __user *buf, size_t count, 
 loff_t *f_pos) 
{
    struct light_dev *dev = filp->private_data; 
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
 int light_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, 
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

# 第三章 Linux内核

## 3.1Linux编程习惯

win编码风格

```c
#defin PI 3.1415926 /* 用大写字母代表宏 */
int minValue, maxValue; /* 变量： 第一行单词全小写，其后单词的第一个字母大写*/
void SendData(void) /* 函数所有单词的第一个单词大写*/
```

Linux编码风格

```c
#defin PI 3.1415926 /* 用大写字母代表宏 */
int min_value, max_value; /* 变量： */
void send_data(void) /* 函数*/
```

缩进用 TAB { }的使用

```c
// 对于结构体、if/for/while/switch { 不另起一行
struct var_data{
    int len;
    char data[0];
};
if (a == b){
    a = c;
    d = a;
}
for (i = 0; i < 10; i++) {
    a = c;
    d = a;
}
//if for循环只有一行 不加 {}
for (i = 0; i < 10; i++) 
    a =c;
// if else混用 else不另起一行

if(x == y) {

} else if (x > y) {

} else {

}
//对于函数 {另起一行
int add(int a, int b)
{
    return a + b;
}
//swith、case 对齐使用
switch (suffix) {
case 'G':
case 'g':
    mem <<= 30;
    break;
case 'M':
case 'm'


default:
    break;    
}
```

### 1.零长度和变量长度数组

0长度不分配空间 sizeof (var_data) =sizeof (int)

也可以使用一个变量定义数组

```c
struct var_data {
    int len;
    char data[0];
}
```

### 2、case范围

```c
switch (ch) {

case '0'...'9': c -= '0';
    break;
case 'a'...'f': c-= 'a' - 10;
    break;
case 'A'...'F': c -= 'A' -10;
    break;

}
```

### 3、语句表达式

```c
#define min_t(type,x,y) \
(｛type _ _x =(x);type _ _y = (y); _ _x<_ _y? _ _x: _ _y; })

int ia, ib, mini;
float fa, fb, minf;
mini = min_t(int, ia, ib);
minf = min_t(float, fa, fb);
```

重新定义局部变量是因为define会出bug

例如 

**难点一**：代码 min(++ia,++ib) 会展开为 ((++ia) < (++ib) ? (++ia): (++ib))，传入宏的“参数”会增加

两次

### 4、typeof关键字

```c
#define min(x,y) ({ \
 const typeof(x) _x = (x); \
 const typeof(y) _y = (y); \
 (void) (&_x == &_y); \
 _x < _y ? _x : _y; })
```

**难点二**   (void) (&_x == &_y) 是为了校验 _x   _y的类型是否一致

### 5 可变参数宏

```c
int printf ( const char *format [, argument]... );
#define pr_debug(fmt,arg...) \
    printk(fmt,##arg)
//arg可以有多个参数
pr_debug("%s:%d",filename,line)
//会替换成下面的
printk("%s:%d", filename, line)
//‘##’是为了处理arg不代表任何参数的情况，这时前面的逗号就会给丢弃
pr_debug("success!\n")
//
printk("success!\n")
//而不是
printk("success!\n",)
```

### 6 标号元素

指定数组索引的方法是在初始化值前添加“ [INDEX] =”，当然也可以用“ [FIRST ... LAST] =”的形式指定一个范围。例如，下面的代码定义了一个数组，并把其中的所有元素赋值为 0：

```c
unsigned char data[MAX] = { [0 ... MAX-1] = 0 };
```

```c
struct file_operations ext2_file_operations = {
 llseek: generic_file_llseek,
 read: generic_file_read,
 write: generic_file_write,
 ioctl: ext2_ioctl,
 mmap: generic_file_mmap,
 open: generic_file_open,
 release: ext2_release_file,
 fsync: ext2_sync_file,
};
```

```c
struct file_operations ext2_fiel_operations = {
    .llseek        =  generic_file_llseek,
    .read          =  generic_file_read,
    .write         =  generic_file_write,
    .aio_read      =  generic_file_aio_read,
    .aio_write     =  generic_file_aio_write,
    .ioct          =  generic_file_ioct,
    .mmap          =  generic_file_mmap,
    .open          =  generic_file_open,
    .release       =  generic_file_release,
    .fsync         =  generic_file_fsync,
    .readv         =  generic_file_readv,
    .writev        =  generic_file_writev,
    .sendfile      =  generic_file_sendfile,
};
```

### 7.当前函数名

GNU C预定义了两个标识符保存当前函数的名字，__FUNCTION__保存函数在源码中的名字，__PRETTY_FUNCTION__保存带语言特色的名字。在C函数中，这两个名字是相同的。

代码中的__FUNCTION__意味着字符串“example” 。C99已经支持__func__宏，因此建议在Linux编程中
不再使用__FUNCTION__，而转而使用__func_\_:

```c
void example()
{
    printf("this is function:%s",__FUBCTION__);
}
```

```c
void example()
{
    printf("this is function:%s",__func__);
}
```

### 8特殊属性声明

- noreturn 属性作用于函数，表示该函数从不返回。这会让编译器优化代码，并消除不必
  要的警告信息

```c
# define ATTRIB_NORET _ _attribute_ _((noreturn)) ....
asmlinkage NORET_TYPE void do_exit(long error_code) ATTRIB_NORET;
```

- format 属性也用于函数，表示该函数使用 printf、scanf 或 strftime 风格的参数，指定format 属性可以让编译器根据格式串检查参数类型。例如：

```c
smlinkage int printk(const char * fmt, ...) _ _attribute_ _ ((format (printf, 1, 2)))
```

上述代码中的第 1 个参数是格式串，从第 2 个参数开始都会根据 printf() 函数的格式串规
则检查参数。

- unused 属性作用于函数和变量，表示该函数或变量可能不会用到，这个属性可以避免编
  译器产生警告信息。

- aligned 属性用于变量、结构体或联合体，指定变量、结构体或联合体的对齐方式，以字节为单位，例如

```c
struct example_struct {
     char a;
     int b;
     long c;
} _ _attribute_ _((aligned(4)))
```

表示该结构类型的变量以 4 字节对齐

- packed 属性作用于变量和类型，用于变量或结构体成员时表示使用最小可能的对齐，用
  于枚举、结构体或联合体类型时表示该类型使用最小的内存。例如

```c
struct example_struct {
 char a;
 int b;
 long c _ _attribute_ _((packed));
};
```

编译器对结构体成员及变量对齐的目的是为了更快地访问结构体成员及变量占据的内存

### 9、内建函数

- 内建函数 _ _builtin_return_address (LEVEL) 返回当前函数或其调用者的返回地址，参
  数 LEVEL 指定调用栈的级数，如 0 表示当前函数的返回地址，1 表示当前函数的调
  用者的返回地址。

- 内建函数 _ _builtin_constant_p(EXP) 用于判断一个值是否为编译时常数，如果参数EXP 的值是常数，函数返回 1，否则返回 0。

- 内建函数 _ _builtin_expect(EXP, C) 用于为编译器提供分支预测信息，其返回值是整数表达式 EXP 的值，C 的值必须是编译时常数

## 3.2 do{ }while（0）

```c
//正确示范
#define SAFE_FREE(p) do{ free(p); p = NULL;} while(0)
//
#define SAFE_FREE(p) free(p); p = NULL;
//
if(NULL != p)
 SAFE_DELETE(p)
else
 .../* do something */

//if语句后两个语句编译出错
if(NULL != p)
 free(p); p = NULL; 
else
 .../* do something */

//如果
#define SAFE_FREE(p) { free(p); p = NULL;}

if(NULL != p)
 SAFE_DELETE(p);
else
 ... /* do something */

//将被扩展为：else 就找不到if

if(NULL != p)
 { free(p); p = NULL; };
else
 ... /* do something */
//do while(0) 就是为了保证代码块
```

## 3.3 goto语句

将 goto 用于错误处理的用法，只需保证在错误处理时注销、资源释放等，与正常的注册、资源申请顺序相反。

```c
if(register_a()!=0) 
 goto err;
if(register_b()!=0)
 goto err1;
if(register_c()!=0)
 goto err2;
if(register_d()!=0)
 goto err3;
...
err3:
 unregister_c();
err2:
 unregister_b();
err1:
 unregister_a(); 
err:
 return ret; 
```

# 第四章 linux内核模块

## 4.1 Linux内核模块

```c
// simple kernel module: hello
#include <linux/init.h>
#include <linux/module.h>
static int __init hello_init(void)
{
 printk(KERN_INFO "Hello World enter\n");
 return 0;
}
module_init(hello_init);
static void __exit hello_exit(void)
{
 printk(KERN_INFO "Hello World exit\n ");
}
module_exit(hello_exit);


MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A simple Hello World Module");
MODULE_ALIAS("a simplest module");
```

编译它会产生 hello.ko目标文件，通过“insmod./hello.ko”命令可以加载它，通过“rmmod hello”命令可以卸载它，加载时输出“Hello World enter” ，卸载时输出“Hello World exit” 。

lsmod和cat /proc/modules效果一样的

modprobe命令比insmod命令要强大，它在加载某模块时，会同时加载该模块所依赖的其他模块。使用

modprobe命令加载的模块若以“modprobe-r filename”的方式卸载，将同时卸载其依赖的模块。

模块之间的依赖关系存放在根文件系统的/lib/modules//modules.dep文件中，实际上是在整体编译内核的时候由depmod工具生成的

modinfo<模块名>命令可以获得模块的信息

## 4.2 Linux内核模块程序结构

1. 模块加载函数 通过insmod 或modprobe命令加载内核模块，内核自动运行加载函数

2. 模块卸载函数 同理 rmmod命令卸载模块时执行

3. 模块许可证声明：MODULE_LICENSE（“GPL v2”）语句声明模块采用GPL v2。
   
   在Linux内核模块领域，可接受的LICENSE包括“GPL” 、 “GPL v2” 、 “GPL and additional rights” 、 “Dual BSD/GPL” 、 “Dual MPL/GPL”和“Proprietary”（关于模块是否可以采用非GPL许可权，如“Proprietary” ，这个在学术界和法律界都有争议）。

4. 模块参数 模块被加载的时候可以传递给它的值 本身对应模块内部全局变量

5. 模块导出符号

6. 模块作者等信息声明

## 4.3 模块加载函数

```c
static int _ _init initialization_function(void)
{
/* 初始化代码 */
}
module_init(initialization_function);
```

模块加载函数以“module_init（函数名）”的形式被指定。它返回整型值，若初始化成功，应返回0。而在初始化失败时，应该返回错误编码。(在Linux内核里，错误编码是一个接近于0的负值，在<linux/errno.h>中定义，包含-ENODEV、-ENOMEM之类的符号值。总是返回相应的错误编码是种非常好的习惯，因为只有这样，用户程序才可以利用perror等方法把它们转换成有意义的错误信息字符串。

- 在内核中调用request_module(module_name);函数加载内核模块

- 标识为__init的函数直接编译进入内核会成为内核镜像的一部分在链接的时候都会放在.init.text这个区段内

```c
#define __init __attribute__ ((__section__ (".init.text")))
```

所有的__init 函数在区段 保存了函数指针，初始化是调用

__init函数 初始化后释放init区段

## 4.4 模块卸载函数

一般以__exit标识声明 

```c
static void _ _exit cleanup_function(void)
{
    /* 释放代码*/
}
module_exit(cleanup_function);
```

模块卸载函数在模块卸载的时候执行，而不返回任何值，且必须以“module_exit（函数名）”的形式来指定。通常来说，模块卸载函数要完成与模块加载函数相反的功能。

我们用__exit来修饰模块卸载函数，可以告诉内核如果相关的模块被直接编译进内核，cleanup_function（）函数会被省略，直接不链进最后的镜像。

既然模块被内置了，就不可能卸载它了，卸载函数也就没有存在的必要了。除了函数以外，只是退出阶段采用的数据也可以用__exitdata来形容。

## 4.5 模块参数

“module_param（参数名，参数类型，参数读/写权限）”为模块定义一个参数

例如下列代码定义了1个整型参数和1个字符指针参数：

```c
static char *book_name = "dissecting Linux Device Driver";
module_param(book_name, charp, S_IRUGO);
static int book_num = 4000;
module_param(book_num, int, S_IRUGO);
```

- 在装载内核模块时，用户可以向模块传递参数

        形式为“insmode（或modprobe）模块名参数名=参数值”

- 如果不传递，参数将使用模块内定义的缺省值。

- 如果模块被内置，就无法insmod了，但是bootloader可以通过在bootargs里设置“模块名.参数名=值”的形式给该内置的模块传递参数。

- 参数类型可以是byte、short、ushort、int、uint、long、ulong、charp（字符指针）、bool或invbool（布尔的反），在模块被编译时会将module_param中声明的类型与变量定义的类型进行比较，判断是否一致。

参数数组，形式为“module_param_array（数组名，数组类型，数组长，参数读/写权限）”。

模块被加载后，在/sys/module/目录下将出现以此模块名命名的目录。

- 当“参数读/写权限”为0时，表示此参数不存在sysfs文件系统下对应的文件节点，

- 此模块存在“参数读/写权限”不为0的命令行参数，在此模块的目录下还将出现parameters目录，其中包含一系列以参数名命名的文件节点，这些文件的权限值就是传入module_param（）的“参数读/写权限”，而文件的内容为参数的值。

运行insmod或modprobe命令时，应使用逗号分隔输入的数组元素。

```c
 #include <linux/init.h> 
 #include <linux/module.h> 
 static char *book_name = "dissecting Linux Device Driver";
 module_param(book_name, charp, S_IRUGO);
 static int book_num = 4000;
 module_param(book_num, int, S_IRUGO);
 static int __init book_init(void)
 {
 printk(KERN_INFO "book name:%s\n", book_name);
 printk(KERN_INFO "book num:%d\n", book_num);
 return 0;
 }
 module_init(book_init);
 static void __exit book_exit(void)
 {
 printk(KERN_INFO "book module exit\n ");
 }
 module_exit(book_exit);
 //MODULE_AUTHOR("Barry Song <baohua@kernel.org>");
 MODULE_LICENSE("GPL v2");
 MODULE_DESCRIPTION("A simple Module for testing module params");
 MODULE_VERSION("V1.0");
```

对上述模块运行“insmod book.ko”命令加载，相应输出都为模块内的默认值，通过查看“/var/log/messages”日志文件可以看到内核的输出：

```c
# tail -n 2 /var/log/messages
Jul  2 01:03:10 localhost kernel:  <6> book name:dissecting Linux Device Driver
Jul  2 01:03:10 localhost kernel:  book num:4000
```

当用户运行“insmod book.ko book_name='GoodBook'book_num=5000”命令时，输出的是用户传递的参数

```c
#tail -n 2 /var/log/messages
Jul  2 01:06:21 localhost kernel:  <6> book name:GoodBook
Jul  2 01:06:21 localhost kernel:  book num:5000
```

在/sys下 可以看到book模块的参数



```c
barry@barry-VirtualBox:/sys/module/book/parameters$ tree
.├── book_name└── book_num
```

cat book_name”和“cat book_num可以查看他们的值

## 4.6 导出符号

Linux的“/proc/kallsyms”文件对应着内核符号表，它记录了符号以及符号所在的内存地址。模块可以使用如下宏导出符号到内核符号表中：

EXPORT_SYMBOL(符号名);

EXPORT_SYMBOL_GPL(符号名);

导出的符号可以被其他模块使用，只需使用前声明一下即可。

EXPORT_SYMBOL_GPL（）只适用于包含GPL许可权的模块。

导出整数加、减运算函数符号的内核模块的例子。



```c
 #include <linux/init.h> 
 #include <linux/module.h> 
 int add_integar(int a, int b) 
 { 
 return a + b;
 }
 EXPORT_SYMBOL_GPL(add_integar);
 int sub_integar(int a, int b) 
 {
 return a - b;
 }
 EXPORT_SYMBOL_GPL(sub_integar);
 MODULE_LICENSE("GPL v2");
```

从“/proc/kallsyms”文件中找出add_integar、sub_integar的相关信息：



```c
#grep integar /proc/kallsyms
e679402c r __ksymtab_sub_integar    [export_symb]
e679403c r __kstrtab_sub_integar    [export_symb]
e6794038 r __kcrctab_sub_integar    [export_symb]
e6794024 r __ksymtab_add_integar    [export_symb]
e6794048 r __kstrtab_add_integar    [export_symb]
e6794034 r __kcrctab_add_integar    [export_symb]
e6793000 t add_integar    [export_symb]
e6793010 t sub_integar    [export_symb]
```

可以看到导出的符号



## 4.7 模块声明与描述



```c
MODULE_AUTHOR(author);
MODULE_DESCRIPTION(description);
MODULE_VERSION(version_string);
MODULE_DEVICE_TABLE(table_info);
MODULE_ALIAS(alternate_name);
```



## 4.8模块的使用计数
