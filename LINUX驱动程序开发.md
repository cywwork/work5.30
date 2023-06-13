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

**难点二**   (void) (&_x == &_y) 是为了校验 _x   _y的类型是否一致,原理是取地址比较是否一样先要验证类型是否一致不一样会报warning

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

增加、减少模块计数函数以及

```c
int try_module_get(struct module *module);
void try_module_put(struct module *module);
```

总结：来说就是有设备使用就增加 没有就减少，只有没有设备使用这模块时才能被卸载

## 4.9 模块的编译

```makefile
KVERS = $(shell uname -r)
# Kernel modules
obj-m += hello.o
# Specify flags for the module compilation.
#EXTRA_CFLAGS=-g -O0
build: kernel_modules
kernel_modules:
 make -C /lib/modules/$(KVERS)/build M=$(CURDIR) modules
clean:
 make -C /lib/modules/$(KVERS)/build M=$(CURDIR) clean
```

uname -r 显示操作系统的发行编号。

多文件使用下面这种避免重复编译没有修改的部分

```makefile
obj-m := modulename.o
modulename-objs := file1.o file2.o
```

#EXTRA_CFLAGS=-g -O0  开启后是获取调试信息

## 4.10使用模块“绕开”GPL

EXPORT_SYMBOL_GPL（）导出的符号是不可以被非GPL模块引用

Linux内核不能使用非GPL许可权。

https://zhuanlan.zhihu.com/p/420678888

# 第五章 linux文件系统与设备文件

## 5.1Linux 文件操作

### 5.1.1文件操作系统调用

**1 创建**

参数mode指定权限 umask是去掉一些权限

```c
int creat(const char *filename, mode_t mode);
//调用把umask设置为newmask，返回旧umask
int umask(int newmask);
```

**2 打开**

前面是路径加名字 缺省就是当前路径下的文件

```c
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

flags

| 标志        | 含义                          |
| --------- | --------------------------- |
| O_RDONLY  | 只读                          |
| O_WRONLY  | 只写                          |
| O_RDWR    | 读写                          |
| O_APPEDN  | 追加                          |
| O_CREAT   | 创建一个文件                      |
| O_EXEC    | 如O_CREAT文件已存在，就发生一个错误 返回 -1 |
| O_NOBLOCK | 非阻塞打开一个文件                   |
| O_TRUNC   | 如文件存在就删除内容                  |

O_RDONLY、O_WRONLY、O_RDWR三个标志只能使用任意的一个。

以上参数作为选用参数用位或 | 连接

```c
不写O_NONBLOCK参数时会默认使用阻塞方式打开文件，
用位或的方式加上 O_NONBLOCK参数就可设置为非阻塞式打开。

阻塞模式下时用read函数读取文件数据时文件有数据时则返回数据，
文件内没有内容时则等待，写操作时和读操作也会停留等待，
导致后续的程序无法执行
但采用轮询的方式访问外接设备时，
显而易见阻塞模式下无法实现轮询的操作。
这时我们就需要用到非阻塞方式打开文件了，
非阻塞模式下如果读操作没有读取到数据不会等待会立即返回一个错误信息，
然后继续向下执行程序。
```

使用了O_CREAT标志，则使用的函数是int open（const char*pathname，int flags，mode_t mode）；这个时候我们还要指定mode标志，以表示文件的访问权限。mode可以是表5.2中所列值的组合。

不写O_NONBLOCK参数时会默认使用阻塞方式打开文件，用位或的方式加上 O_NONBLOCK参数就可设置为非阻塞式打开。

![](./md-LINUX驱动程序开发/20230612110015.png)

Linux用5个数字来表示文件的各种权限：第一位表示设置用户ID；第二位表示设置组ID；第三位表示用户自己的权限位；第四位表示组的权限；最后一位表示其他人的权限。每个数字可以取1（执行权限）、2（写权限）、4（读权限）、0（无）或者是这些值的和。

例如，要创建一个用户可读、可写、可执行，但是组没有权限，其他人可以读、可以执行的文件，并设置用户ID位，那么应该使用的模式是1（设置用户ID）、

0（不设置组ID）、7（1+2+4，读、写、执行）、0（没有权限）、5（1+4，读、执行）即10705：

下面的式子等价

```c
open("test", O_CREAT, 10 705);
open("test", O_CREAT, S_IRWXU | S_IROTH | S_IXOTH | S_ISUID );
```

**3 读写**

```c
int read(int fd, const void *buf, size_t length);
int write(int fd, const void *buf, size_t length);
```

参数buf为指向缓冲区的指针

length为缓冲区的大小（以字节为单位）

函数read:从**文件描述符fd**所指定的文件中读取length个字节到buf所指向的**缓冲区**中，返回值为实际读取的字节数。

函数write:把length个字节从buf指向的**缓冲区**中写到**文件描述符fd**所指向的文件中，返回值为实际写入的字节数。

以O_CREAT为标志的open实际上实现了文件创建的功能，因此，下面的函数等同于creat（）函数：（把打开的文件清空）

int open(pathname, O_CREAT | O_WRONLY | O_TRUNC, mode);

**4 定位**

```c
int lseek(int fd, offset_t offset, int whence);
```

```c
lseek（）将文件读写指针相对whence移动offset个字节。
操作成功时，返回文件指针相对于文件头的位置。
参数whence可使用下述值：
SEEK_SET：相对文件开头
SEEK_CUR：相对文件读写指针的当前位置
SEEK_END：相对文件末尾offset可取负值

将文件指针相对当前位置向前移动5个字节：
lseek(fd, -5, SEEK_CUR);

由于lseek函数的返回值为文件指针相对于文件头的位置，
返回值是文件的长度：
lseek(fd, 0, SEEK_END);
```

**5.关闭**

调用完关闭

```c
int close(int fd);
```

```c
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <stdio.h>
#define LENGTH 100 
main() 
{ 
int fd, len;
char str[LENGTH];
fd = open("hello.txt", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
//创建并打开文件 
if (fd) {
    write(fd, "Hello World", strlen("Hello World")); 
//写入字符串
    close(fd);
        }
fd = open("hello.txt", O_RDWR);
len = read(fd, str, LENGTH); //读取文件内容 返回长度
str[len] = '\0';
printf("%s\n", str);
close(fd);

}
```

### 5.1.2c库文件操作

**1.创建和打开**

```c
fiLE *fopen(const char *path, const char *mode);
```

![](./md-LINUX驱动程序开发/20230612113019.png)

```c
int fgetc(fiLE *stream);
int fputc(int c, fiLE *stream);
char *fgets(char *s, int n, fiLE *stream);
int fputs(const char *s, fiLE *stream);
int fprintf(fiLE *stream, const char *format, ...);
int fscanf (fiLE *stream, const char *format, ...);
size_t fread(void *ptr, size_t size, size_t n, fiLE *stream);
size_t fwrite (const void *ptr, size_t size, size_t n, fiLE *stream);
```

fread（）实现从流（stream）中读取n个字段，每个字段为size字节，并将读取的字段放入ptr所指的字符数组中，返回实际已读取的字段数。

当读取的字段数小于num时，可能是在函数调用时出现了错误，也可能是读到了文件的结尾。因此要通过调用feof（）和ferror（）来判断。

fwrite（）实现从缓冲区ptr所指的数组中把n个字段写到流（stream）中，每个字段长为size个字节，返回实际写入的字段数。

定位

```c
int fgetpos(fiLE *stream, fpos_t *pos);
int fsetpos(fiLE *stream, const fpos_t *pos);
int fseek(fiLE *stream, long offset, int whence);
```

 **3、关闭**

```c
int fclose (fiLE *stream);
```

4.和上面5.1的代码效果一样

```c
#include <stdio.h> 
#define LENGTH 100 
main() 
{
    fiLE *fd;
    char str[LENGTH];
    fd = fopen("hello.txt", "w+");/* 创建并打开文件 */ 
    if (fd) {
        fputs("Hello World", fd); /* 写入字符串 */
        fclose(fd); 
            }
    fd = fopen("hello.txt", "r");
    gets(str, LENGTH, fd);       /* 读取文件内容 */
    printf("%s\n", str);
    fclose(fd);
}
```

## 5.2Linux文件系统

### 5.2.1Linux文件系统目录结构

1./bin

包含基本命令，如ls、cp、mkdir等，这个目录中的文件都是可执行的。

2./sbin

    包含系统命令，如modprobe、hwclock、ifconfig等，大多是涉及系统管理的命令，这个目录中的文件都是可执行的。

3./dev

设备文件存储目录，应用程序通过对这些文件的读写和控制以访问实际的设备。

5./lib

系统库文件存放目录等。

6./mnt

/mnt这个目录一般是用于存放挂载储存设备的挂载目录

7./opt

opt是“可选”的意思，有些软件包会被安装在这里。

8./proc

操作系统运行时，进程及内核信息（比如CPU、硬盘分区、内存信息等）存放在这里。/proc目录为伪文件系统proc的挂载目录，proc并不是真正的文件系统，它存在于内存之中。

9./tmp

用户运行程序的时候，有时会产生临时文件，/tmp用来存放临时文件。

10./usr

这个是系统存放程序的目录，比如用户命令、用户库等。

11./var

var表示的是变化的意思，这个目录的内容经常变动，如/var的/var/log目录被用来存放系统日志。

12./sys

Linux 2.6以后的内核所支持的sysfs文件系统被映射在此目录上。Linux设备驱动模型中的总线、驱动和设备都可以在sysfs文件系统中找到对应的节点。当内核检测到在系统中出现了新设备后，内核会在sysfs文件系统中为该新设备生成一项新的记录。

sysfs被看成是与proc、devfs和devpty同类别的文件系统，

该文件系统是一个虚拟的文件系统，它可以产生一个包括所有系统硬件的层级视图，与提供进程和状态信息的proc文件系统十分类似。

### 5.2.2 文件系统与设备驱动

应用程序和VFS之间的接口是系统调用，而VFS与文件系统以及设备文件之间的接口是file_operations结构体成员函         （虚拟文件系统(VFS,Vitual Filesystem)）

![](./md-LINUX驱动程序开发/20230612134922.png)

由于字符设备的上层没有类似于磁盘的ext2等文件系统，所以字符设file_operations成员函数就直接由设备驱动提供了

file_operations正是字符设备驱动的核心。

块设备有两种访问方法

一：不通过文件系统直接访问裸设备，在Linux内核实现了统一的def_blk_fops这一file_operations，它的源代码位于fs/block_dev.c

当运行类似于“dd if=/dev/sdb1of=sdb1.img”的命令把整个/dev/sdb1裸分区复制到sdb1.img的时候内核走的是def_blk_fops这个file_operations；

二：是通过**文件系统**来访问块设备，file_operations的实现则位于文件系统内，文件系统会把针对文件的读写转换为针对块设备原始扇区的读写。ext2、fat、Btrfs等文件系统中会实现针对VFS的file_operations成员函数，设备驱动层看不到file_operations的存在。

1.file结构体

file结构体代表一个打开的文件，系统中每个打开的文件在内核空间都有一个关联的struct file。它由内核在打开文件时创建，并传递给在文件上进行操作的任何函数。在文件的所有实例都关闭后，内核释放这个数据结构。在内核和驱动源代码中，struct file的指针通常被命名为file或filp

文件结构体的定义如下

```c
struct file {
union {
    struct llist_node    fu_llist;
    struct rcu_head      fu_rcuhead;
    } f_u;
struct path              f_path;
#define f_dentry          f_path.dentry 
//dentry存放目录项和其下的文件链接信息
struct inode             *f_inode;      /* cached value */
const struct file_operations*f_op;      /* 和文件关联的操作*/
spinlock_t          f_lock;
atomic_long_t       f_count;
unsigned int        f_flags;       
/*文件标志，如O_RDONLY、O_NONBLOCK、O_SYNC*/
fmode_t             f_mode;        
/*文件读/写模式，FMODE_READ和FMODE_WRITE*/
struct mutex        f_pos_lock;
loff_t              f_pos;         /* 当前读写位置 */
struct fown_struct  f_owner;
const struct cred   *f_cred;
struct file_ra_statef_ra;
u64                f_version;
#ifdef CONfiG_SECURITY27  
 void         *f_security;
#endif
void         *private_data;        /*文件私有数据*/
//私有数据指针private_data在设备驱动中被广泛应用，
//大多被指向设备驱动自定义以用于描述设备的结构体。
#ifdef CONfiG_EPOLL33 
 /* 通过使用fs/eventpoll.c 去链接所有 与这个文件有挂钩的*/ 
struct list_head     f_ep_links;
struct list_head     f_tfile_llink;
#endif  
/* #ifdef CONfiG_EPOLL */
struct address_space*f_mapping;
} __attribute__((aligned(4))); 
/* 表示该结构类型的变量以 4 字节对齐*/
```

判断以阻塞还是非阻塞方式打开设备文件：

```c
if (file->f_flags & O_NONBLOCK)     /* 非阻塞 */      
    pr_debug("open: non-blocking\n");
else                                /* 阻塞 */     
    pr_debug("open: blocking\n");
```

2.inode结构体

inode结点包含：文件访问权限、属主、组、大小、生成时间、访问时间、最后修改时间等信息

```c
struct inode { 
    ...      
    umode_t i_mode;            /* inode的权限 */
    uid_t i_uid;               /* inode拥有者的id */
    gid_t i_gid;               /* inode所属的群组id */
    dev_t i_rdev;              /* 若是设备文件，此字段将记录设备的设备号 */ 
    //Linux内核设备编号分为主设备编号和次设备编号，
    //前者为dev_t的高12位，后者为dev_t的低20位。
    loff_t i_size;             /* inode所代表的文件大小 */
    struct timespec i_atime;   /* inode最近一次的存取时间 */
    struct timespec i_mtime;   /* inode最近一次的修改时间 */    
    struct timespec i_ctime;   /* inode的产生时间 */
    unsigned int        i_blkbits;    
    blkcnt_t            i_blocks;  
/* inode所使用的block数，一个block为512 字节 */    
    union {
    struct pipe_inode_info  *i_pipe;
    struct block_device     *i_bdev;
// 若是块设备，为其对应的block_device结构体指针 
    struct cdev *i_cdev;     /* 若是字符设备，为其对应的cdev结构体指针 */
    }
    ...
};
```

查看/proc/devices文件可以获知系统中注册的设备，第1列为主设备号，第2列为设备名

查看/dev目录可以获知系统中包含的设备文件，日期的前两列给出了对应设备的主设备号和次设备号

```
crw-rw---- 1 root uucp 4, 64 Jan 30 2003 /dev/ttyS0b
rw-rw---- 1 root disk 8, 0 Jan 30 2003 /dev/sda
```

主设备号是与驱动对应的概念，同一类设备一般使用相同的<u>主设备号</u>，不同类的设备一般使用不同的主设备号。因为同一驱动可支持多个同类设备，用<u>次设备号</u>来描述使用该驱动的设备的序号，序号一般从0开始。

内核Documents目录下的devices.txt文件描述了Linux设备号的分配情况

## 5.3　devfs

作用：设备驱动程序能自主地管理自己的设备文件

1）可以通过程序在设备初始化时在/dev目录下创建设备文件，卸载设备时将它删除。

2）设备驱动程序可以指定设备名、所有者和权限位，用户空间程序仍可以修改所有者和权限位。

3）不再需要为设备驱动程序分配主设备号以及处理次设备号，在程序中可以直接给

register_chrdev（）传递0主设备号以获得可用的主设备号，并在devfs_register（）中指定次设备号。

```c
/* 创建设备目录 */
devfs_handle_t devfs_mk_dir(devfs_handle_t dir, const char *name, \
void *info);
/* 创建设备文件 */
devfs_handle_t devfs_register(devfs_handle_t dir, const char *name,\
unsigned   int flags, unsigned int major, unsigned int minor, \
umode_t mode, void *ops,   void *info);
/* 撤销设备文件 */
void devfs_unregister(devfs_handle_t de);
```

devfs使用模板

```c
static devfs_handle_t devfs_handle;
static int _ _init xxx_init(void)
{
    int ret;//接收返回的设备号
    int i;
    /* 在内核中注册设备 */
    ret = register_chrdev(XXX_MAJOR, DEVICE_NAME, &xxx_fops);
    if (ret < 0) {
        printk(DEVICE_NAME " can't register major number\n");
        return ret;}
    /* 创建设备文件 */
 devfs_handle =devfs_register(NULL, DEVICE_NAME, DEVFS_FL_DEFAULT,
 XX_MAJOR, 0, S_IFCHR | S_IRUSR | S_IWUSR, &xxx_fops, NULL);
 ...
     printk(DEVICE_NAME " initialized\n");
     return 0;
 }
 static void _ _exit xxx_exit(void)
 {
     devfs_unregister(devfs_handle);
     /* 撤销设备文件 */
     unregister_chrdev(XXX_MAJOR, DEVICE_NAME);  
    /* 注销设备 */
 }
module_init(xxx_init);
module_exit(xxx_exit);
```

## 5.4 udev用户空间设备管理

### 5.4.1 udev与devfs的区别

devfs所做的工作被确信可以在用户态来完成

devfs与udev的另一个显著区别在于：采用devfs，当一个并不存在的/dev节点被打开的时候，devfs能自动加载对应的驱动

udev则不这么做,因为udev的设计者认为Linux应该在设备被发现的时候加载驱动模块，而不是当它被访问的时候,devfs所提供的打开/dev节点时自动加载驱动的功能对一个配置正确的计算机来说是多余的。系统中所有的设备都应该产生热插拔事件并加载恰当的驱动，而udev能注意到这点并且为它创建对应的设备节点。

### 5.4.2　sysfs文件系统与Linux设备模型

sysfs把连接在系统上的设备和总线组织成为一个分级的文件，它们可以由用户空间存取，向用户空间导出内核数据结构以及它们的属性。sysfs的一个目的就是展示设备驱动模型中各组件的层次关系，其顶级目录包括block、bus、dev、devices、class、fs、kernel、power和firmware等。

- block目录包含所有的块设备

- devices目录包含系统所有的设备，并根据设备挂接的总线类型组织成层次结构

- bus目录包含系统中所有的总线类型

- class目录包含系统中的设备类型（如网卡设备、声卡设备、输入设备等）。

## 5.5　总结

- Linux用户空间的文件编程有两种方法，即通过Linux API和通过C库函数访问文件。用户空间看不到设备驱动，能看到的只有与设备对应的文件，因此文件编程也就是用户空间的设备编程。

- Linux按照功能对文件系统的目录结构进行了良好的规划。/dev是设备文件的存放目录，devfs和udev分别是Linux 2.4和Linux 2.6以后的内核生成设备文件节点的方法，前者运行于内核空间，后者运行于用户空间。

- Linux 2.6以后的内核通过一系列数据结构定义了设备模型，设备模型与sysfs文件系统中的目录和文件存在一种对应关系。设备和驱动分离，并通过总线进行匹配。

- udev可以利用内核通过netlink发出的uevent信息动态创建设备文件节点。

# 第6章　字符设备驱动

## 6.1　Linux字符设备驱动结构

### 6.1.1　cdev结构体

```c
struct cdev {

struct kobject kobj; /* 内嵌的kobject对象 */
struct module *owner; /* 所属模块*/
struct file_operations *ops; /* 文件操作结构体*/
struct list_head list;
dev_t dev; /* 设备号*/
unsigned int count;
};
//cdev结构体的dev_t成员定义了设备号，为32位，其中12位为主设备号，
//20位为次设备号。使用下列宏可以从dev_t获得主设备号和次设备号：
MAJOR(dev_t dev)
MINOR(dev_t dev)
//通过主设备号和次设备号生成dev_t：
MKDEV(int major, int minor)
//初始化并建立cdev和file_operations之间的连接
void cdev_init(struct cdev *, struct file_operations *);
//cdev_alloc()用来给cdev结构体动态申请内存
struct cdev *cdev_alloc(void);
//
void cdev_put(struct cdev *p);
//cdev_add()函数和cdev_del()函数用来向系统添加和删除一个cdev，
//完成字符设备的注册和注销。
int cdev_add(struct cdev *, dev_t, unsigned);
void cdev_del(struct cdev *);
```

```c
void cdev_init(struct cdev *cdev, struct file_operations *fops)

{

memset(cdev, 0, sizeof *cdev);

INIT_LIST_HEAD(&cdev->list);

kobject_init(&cdev->kobj, &ktype_cdev_default);

cdev->ops = fops; /* 将传入的文件操作结构体指针赋值给cdev的ops*/

}
```

```c
struct cdev *cdev_alloc(void)

{

 struct cdev *p = kzalloc(sizeof(struct cdev), GFP_KERNEL);

    if (p) {

          INIT_LIST_HEAD(&p->list);

          kobject_init(&p->kobj, &ktype_cdev_dynamic);

           }

     return p;

}
```

## 6.1.2　分配和释放设备号
