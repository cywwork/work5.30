```shell
#!/bin/bash
echo "Hello World !"
```

#!是约定的标记告诉系统这个脚本需要什么解析器来执行

./test.sh 执行脚本

或者传参的形式

/bin/sh/ test.sh

/bin/php test.php

# Shell变量

**变量名与赋值之间不能有空格**

花括号是区分变量用的无歧义可不加

```shell
your_name="chenyuanwei"
echo $your_name
echo ${your_name}
```

二次定义是允许的**只有使用变量时才用\$** 

```shell
your_name="work"
echo $your_name
your_name="cyw"
echo ${your_name}
```

### 只读变量

```shell
myUrl="www.once.com"
readonly myUrl
myUrl="wwwwwwwww"
```

### 删除变量不能删除只读变量

```shell
unset your_name
echo $your_name
```

### 变量类型

1. 局部变量：在脚本或命令中定义，仅在当前shell实例中有效

2. 环境变量：所有程序都能访问，shell脚本也可以定义环境变量

3. shell变量：是shell程序设置的特殊变量一部分是局部一部分是环境变量

## Shell字符串

可以使用单双引号或者不用也行

单引号限制：

- 引号内任何字符**原样输出**，变量是无效的

- 内部不能出现单独一个单引号（**转义无效**），可成对出现

双引号优点：

- 可有变量

- 可出现转义字符

```shell
your_name="runoob"
str="Hello, I know you are \"$your_name\"! \n"
echo -e $str #-e开启转义
```

拼接字符串

```shell
your_name="runoob"
# 使用双引号拼接
greeting="hello, "$your_name"!"
greeting_1="hello1, ${your_name} !"
echo $greeting $greeting_1
#使用单引号
greeting_2='hello, '$your_name' !'
greeting_3='hello, ${your_name} !'
echo $greeting_2 $greeting_3
```

运算结果

`hello, runoob! hello1, runoob !
hello, runoob ! hello, ${your_name} !`

```shell
string="abcd"
echo ${#string}
#变量为数组时，${#string} 等价于 ${#string[0]}
echo ${#string[0]}
#输出结果均为4
```

提取子字符串

```shell
string="runoob is a great site"
echo ${string:1:4}
#第一个字符索引值为0
```

寻找子字符串

寻找字符i或o的位置那个先出现就出现那个，这时第一个字母位置为1

```shell
string="runoob is a great site"
echo `expr index "$string" io`
```

## 注释

一行用#

多行用

```shell
:<<!
a
b
c
!


:<<'
a
b
c
'
```

# Shell传递参数

`$ ./test_01.sh 1 2 3
Shell 传递参数实例!
执行的文件名：./test_01.sh
第一个参数为：1
第二个参数为：2
第三个参数为：3`

```shell
echo "Shell 传递参数实例!"
echo "执行的文件名：$0";
echo "第一个参数为：$1";
echo "第二个参数为：$2";
echo "第三个参数为：$3";
```

```shell
$# 传递脚本的参数个数
$* 以字符串显示所有的传参 如："$1$2...$n"
```

```shell
echo "-- \$* 演示 ---"
for i in "$*"; do
        echo $i
done

echo "-- \$@ 演示 ---"
for i in "$@"; do
        echo $i
done
```

-- $* 演示 ---
1 2 3
-- 

$@ 演示 ---
1
2
3

可以看出都是传全部参数，但*传的是“1 2 3”一份参数，而@ 是“1” “2” “3” 三份参数

# Shell数组

只支持一维数组，但没有限定数组大小

定义数组

```shell
array_num=(10 20 30 40 50)
array_num=(
    10
    20
    30
    40
    50
)
array_num[0]=10
arrat_num[1]=20
array_num[2]=30
array_num[4]=50
#下标可以不连续
```

读取数组

```shell
${array_num[0]}
echo ${array_num[2]}
num1=${array_num[3]}
echo $num1
echo ${array_num[@]}
```

@ 是打印全部

获取数组的长度

```shell
length=${#array_num[@]}
echo $length 
#5
length=${#array_num[*]}
echo $length 
#5
#单元素长度
lengthn=${#array_num[n]}
echo $lengthn
#2
```

关联数组

`declare -A array_cyw`

两种建立方式和访问（形式上类似字典）

```shell
declare -A site=(["google"]="www.googel.com" ["runoob"]="www.runoob.com" ["taobao"]="www.taobao.com")

declare -A cyw
cyw["age"]=25
cyw["year"]=1998
cyw["work"]="work"

echo ${cyw["age"]}
echo ${site["runoob"]} 
echo "数组的元素为: ${site[*]}"
echo "数组的键为: ${!site[*]}"
```

在数组前加一个 ! 可以获取数组的所有键

# Shell 运算符

### 算数运算符

bash不支持简单数学运算 通过 awk和expr使用计算

```shell
val=`expr 2 + 2`
echo "两数之和为：$val"
a=10
b=20
echo `expr $a + $b`
echo `expr $a - $b` 
#乘
echo `expr $a \* $b` 
#除
echo `expr $b / $a`
#取余
val=`expr $b % $a`
echo "b % a : $val"
```

 = 是赋值    ==是比较相等

```shell
a=$b
echo $a

if [ $a == $b ]
then
  echo "a 等于 b "
fi

if [ $a != $b ]
then
  echo "a 不等于 b"
fi
```

### 关系运算符

| -eq | 检测是否相等     |
| --- | ---------- |
| -ne | 检测是否不相等    |
| -gt | 检测左是否大于右   |
| -lt | 检测左是否小于右   |
| -ge | 检测左是否大于等于右 |
| -le | 检测左是否小于等于右 |

```shell
#关系运算符实验
if [ $a -eq $b ]
then
  echo "a 等于 b"
else
  echo "a 不等于 b"
fi

if [ $a -ne $b ]
then
  echo "$a -ne $b: a 不等于 b"
else
  echo "$a -ne $b: a 等于 b"
fi 
a=10

if [ $a -gt $b ]
then
  echo "a 大于 b"
else
  echo "a 不大于 b"
fi

if [ $a -lt $b ]
then
  echo "a 小于 b"
else
  echo "a 不小于 b"
fi

if [ $a -ge $b ]
then
  echo "a 大于或等于 b"
else
  echo "a 小于 b"
fi

if [ $a -le $b ]
then
  echo "a 小于或等于 b"
else
  echo "a 大于 b"
fi
```

### 布尔运算符

| ！   | 非运算 |
| --- | --- |
| -o  | 或运算 |
| -a  | 与运算 |

```shell
echo "布尔运算实验 a=$a b=$b"
#非运算
if [ $a != $b ]
then
  echo "$a != $b : a 不等于 b"
else
  echo "$a == $b : a 等于 b "
fi
#与运算
if [ $a -lt 100 -a $b -gt 15 ]
then
  echo "$a 小于 100 且 $b 大于 15 ：返回 true"
else
  echo "$a 小于 100 且 $b 大于 15 ：返回 false"
fi    

#或运算
if [ $a -lt 5 -o $b -gt 100 ]
then
  echo "$a 小于5 或 $b 大于100 ：返回true"
else
  echo "$a 小于5 或 $b 大于100 ：返回false"
fi
```

### 逻辑运算符

&& 逻辑与

|| 逻辑或

```shell
echo "逻辑运算符实验"
if [[ $a -lt 100 && $b -gt 100 ]]
then
  echo "返回 true"
else
  echo "返回 false"
fi

if [[ $a -lt 100 || $b -gt 100 ]]
then
  echo "返回 true"
else
  echo "返回 false"
fi
```

### 字符串运算符

| =   | 检测字符串是否相等 |
| --- | --------- |
| ！=  | 是否不等      |
| -z  | 是否长度为0    |
| -n  | 是否不为0     |
| $   | 是否为空      |

```shell
a="abc"
b="efg"

if [ $a = $b ]
then
  echo "$a =　$b : a 等于 b"
else
  echo "$a != $b : a 不等于 b"
fi

if [ $a != $b ]
then
  echo "$a !=　$b : a 不等于 b"
else
  echo "$a = $b : a 等于 b"
fi  


if [ -z $a ]
then
  echo "-z $a : 字符串长度为0"
else
  echo "-z $a : 长度不为0"
fi


if [ -n $a ]
then
  echo "-n $a : 字符串长度不为0"
else
  echo "-n $a : 长度为0"
fi


if [ $a ]
then
  echo "$a : 字符串不为空"
else
  echo "$a : 字符串为空"
fi
```

### 文件检测运算符

| -r  | 可读           |
| --- | ------------ |
| -w  | 可写           |
| -x  | 可执行          |
| -f  | 普通文件非路径或设备文件 |
| -d  | 是目录          |
| -s  | 不为空          |
| -e  | 文件存在         |

```shell
echo "文件实验"
file="./test6.2.sh"
if [ -r $file ]
then
  echo "文件可读"
else
  echo "文件不可读"
fi

if [ -w $file ]
then
  echo "文件可写"
else
  echo "文件不可写"
fi  

if [ -x $file ]
then
  echo "文件可执行"
else
  echo "文件不能执行"
fi

if [ -f $file ]
then
  echo "文件为普通文件"
else
  echo "文件为特殊文件"
fi  

if [ -d $file ]
then
  echo "文件是个目录"
else
  echo "文件不是个目录"
fi

if [ -s $file ]
then
  echo "文件不为空"
else
  echo "文件为空"
fi  

if [ -e $file ]
then
  echo "文件存在"
else
  echo "文件不存在"
fi
```

# Shell echo 命令

```shell
echo "it is a test"
#读一行
read name
echo "$name it is a test"
#换行
echo -e "OK! \n" # -e 开启转义
echo " it is a test"

echo -e "ok!\c" # \c不换行
echo "is test"
#写如test.6.5.txt里面
echo "todat is 6.5  " > test6.5.txt
#单引号直接输出
echo '$name\"'
#反引号加date可以出现当前日期
echo `date`
```

# Shell printf 命令

echo 自动换行 printf 手动换行

```shell
echo "hello,shell"
printf "Hello, Shell\n"
```

%s 字符串 %c字符 %d整数   %f浮点

%-10s 宽度为10字符，-表示左对齐 没有就是右对齐 不足加空格

%-4.2f格式化为小数

```shell
printf "%-10s %-8s %-4s\n" 姓名 性别 体重kg
printf "%-10s %-8s %-4.2f\n" 郭靖 男 66.1234
printf "%-10s %-8s %-4.2f\n" 杨过 男 48.6543
printf "%-10s %-8s %-4.2f\n" 郭芙 女 47.9876
```

printf特点

```shell
printf "%-10s %-8s %-4s\n" 姓名 性别 体重kg
printf "%-10s %-8s %-4.2f\n" 郭靖 男 66.1234
printf "%-10s %-8s %-4.2f\n" 杨过 男 48.6543
printf "%-10s %-8s %-4.2f\n" 郭芙 女 47.9876
#单双引号效果一样
printf "%d %s\n" 1 "adc"
printf '%d %s\n' 1 "adc"
#无引号也一样
printf %s abcdef
#abcdef
printf %s abc def
#不够就重复利用格式
printf "%s %s %s\n" a b c d f g h i j 
# a b c
# d e f
# g h i 
#j
```

printf转义

%b 是开启了转义 \a是不换行

```shell
printf "%s and %d \n"

printf "a string no:<%s>\n" "A\nB"

printf "a string no:<%b>\n" "A\nB"

printf "www.com \a"
echo "www"
```

# Shell test 命令

文件测试在前面的文件检测运算符中

代码【】执行基本算数

```shell
a=5
b=6

result=$[a+b]
echo "result = $result"

if test -e ./test6.2.sh -o -e./bash
then
  echo ' 至少有一个文件存在'
else
  echo '两个文件都不存在'
fi
```

# Shell流程控制

sh的流程控制不可为空

适用一行写完（在终端命令提示符）

`if [ $(ps -ef | grep -c "ssh") -gt 1 ]; then echo "true"; fi`

```shell
#if
if condition
then
    command1

fi
#if else
if condition
then
    command1
else
    command2
fi
#if else-if else
if condition1
then
    command1
elif cindition2
then
    command2
else
    commandn
fi

#[]用-gt -lt (()) 用< > 

a=10
b=20
if [ $a == b ]
then
  echo "a 等于 b"
elif [ $a -gt $b ]
then
  echo "a 大于 b"
elif [ $a -lt $b ]
then
  echo "a 小于 b"
else
  echo "没有符合条件"
fi


if (( $a == $b ))
then
  echo "a = b "
elif(( $a < $b ))
then
  echo "a < b "
else
  echo "a > b"
fi  
```

### for循环和while循环

```shell
for var in item1 item2 ... itemN
do
    command1
    command2
    ...
    commandN
done 

for var in item1 item2 ... itemN; do command1; command2… done; 


while condition
do
    command
done
```

 命令包含所有shell命令和语句 in列表可以包含替换、字符串文件名

但不使用in列表 for循环就使用命令行的位置参数

```shell
#循环
echo "循环"
for loop in 1 2 3 4 5
do
        echo "the value is : $loop"
done

for str in this is a stirng
do
        echo $str
done

int=1
while(( $int<=5 ))
do
        echo $int
        let "int++"
done  
```

### 循环读入

```shell
echo '按下<CTRL-D>退出 '
echo -n '输入你最喜欢的网站名：'
while read FILM
do
        echo "是的！$FILM 是一个好网站"
done
```

### let命令

bash中用于计算的工具

执行一个或多个表达式，变量不用$ 表示 含空格特殊符号等等要引起来

```shell
let a=5+4
let b=9-3
let no++
let no--
let no+=10 
```

### 无限循环

```shell
while :
do
    command
done


while true
do
    command
done


for(( ; ; ))
```

### until循环

循环到条件为true停止 ,(until哪里不用再加【】了) 输出是0-9

```shell
a=0

until [ !  $a -lt 10 ]

do
        echo $a
        a=`expr $a + 1`
done
```

### case ...esac

两个分号;;表示break defult 使用*替代

```shell
case 值 in
模式1)
    command1
    command2
    ...
    ;;
模式2)
    command1
    command2
    ...
    ;;
esac
```

```shell
echo '输入 1到4之间数字 '
echo '你的输入是：'
read aNum
case $aNum in
        1) echo '你选择了 1'
        ;;
        2) echo '你选择了 2'
        ;;
        3) echo '你选择了 3'
        ;;
        4) echo '你选择了 4'
        ;;
        *) echo '输入有误'
        ;;
esac 

site="runoob"
case "$site" in
        "runoob") echo "菜鸟"
        ;;
        "google") echo "谷歌"
        ;;
        "taobao") echo "淘宝"
        ;;
esac
```

### 跳出循环

#### break命令

跳出循环

#### continue命令

跳出单次循环

```shell
while :
do
        echo -n "输入 1到5自己的数字:"
        read aNum
        case $aNum in
                1|2|3|4|5) echo "你输入的数字为 $aNum!"
                ;;
        *) echo "你输入的数字不是1到5之间的！游戏结束"
                break
        ;;
        esac
done

while :
do
        echo -n "输入 1到5自己的数字:"
        read aNum
        case $aNum in
                1|2|3|4|5) echo "你输入的数字为 $aNum!"
                ;;
        *) echo "你输入的数字不是1到5之间的！"
                continue
                echo "游戏结束"#永远不会被执行
        ;;
        esac
done
```

# Shell 函数

如何定义函数：先定义才能使用

```shell
[ function ] funname [()]

{

    action;

    [return int;]

} 
demoFun(){
        echo "这是第一个shell函数"
}
echo "函数的开始"
demoFun
echo "函数的结束"
```

return 可以使用 $ ?来接收结果如下

```shell
funWithReturn(){
        echo "这个函数会对输入两个数字相加"
        echo "请输入第一个数字"
        read aNum
        echo "请输入第二个数字"
        read anotherNum
        echo "第一个数字是 $aNum 第二个数字是 $anotherNum"
        return $(($aNum+$anotherNum))

}
funWithReturn
echo "输入的两个数字之和为$? !" 
```

### 函数参数

调用函数是可以传参数 函数体

```shell
funWithParam(){
        echo "第一个参数为 $1 !"
        echo "第二个参数为 $2 !"
        echo "第十个参数为 $10 !" #输出一个和0而不是第10个参数
        echo "第十个参数为 ${10} !"
        echo "第十一个参数为 ${11} !"
        echo "参数总数有 $# 个！"
        echo "作为一个字符串输出所有参数 $*"
}
funWithParam 1 2 3 4 5 6 7 8 9 34 77
```

# Shell 输入/输出重定向

### 输出重定向

command1 > file1

不覆盖的使用 >>

### 输入重定向

```shell
Administrator@PC-202009090933 MINGW64 /d/chenyuanwei (6.1)
$ wc -l test6.5.txt
2 test6.5.txt

Administrator@PC-202009090933 MINGW64 /d/chenyuanwei (6.1)
$ wc -l < test6.5.txt
2
```

 第一个例子，会输出文件名；第二个不会

command1 < infile > outfile

执行command1，从文件infile读取内容，然后将输出写入到outfile中。

```shell
$ wc -l << EOF

> 欢迎来到
> 菜鸟教程
> wwwwwww
> EOF
> 3
#执行命令不显示结果
$ command > /dev/null
#屏蔽 stdout 和 stderr

$ command > /dev/null 2>&1
```

/dev/null 是一个特殊的文件，写入到它的内容都会被丢弃 也读不了

2>是一个整体

# Shell文件包含

```shell
test1.sh
#!/bin/bash
# author:菜鸟教程
# url:www.runoob.com

url="http://www.runoob.com"

test2.sh
#!/bin/bash
# author:菜鸟教程
# url:www.runoob.com

#使用 . 号来引用test1.sh 文件
. ./test1.sh

# 或者使用以下包含文件代码
# source ./test1.sh

echo "菜鸟教程官网地址：$url"
```

还得对test2添加权限

chmod +x test2.sh
