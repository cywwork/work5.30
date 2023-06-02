```shell
#!/bin/bash
echo "Hello World !"
```

#!是约定的标记告诉系统这个脚本需要什么解析器来执行

./test.sh 执行脚本

或者传参的形式

/bin/sh/ test.sh

/bin/php test.php

# 变量

**变量名之间不能有空格**

花括号是区分变量用的无歧义可不加

```shell
your_name="chenyuanwei"
echo $your_name
echo ${your_name}
```

二次定义是允许的只有使用变量时才用$ 

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

- 引号内任何字符原样输出，变量是无效的

- 内部不能出现单独一个单引号（转义无效），可成对出现

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


```

在数组前加一个 ! 可以获取数组的所有键
