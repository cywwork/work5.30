#!/bin/bash


your_name="cywwork"
your_name="tom"
echo $your_name
your_name="cyw" 
echo ${your_name}

myUrl="www.once.com"
readonly myUrl
unset your_name
echo $your_name

your_name="runoob"
str="Hello, I know you are \"$your_name\"! \n"
echo -e $str

your_name="runoob"
# 使用双引号拼接
greeting="hello, "$your_name"!"
greeting_1="hello1, ${your_name} !"
echo $greeting $greeting_1
#使用单引号
greeting_2='hello, '$your_name' !'
greeting_3='hello, ${your_name} !'
echo $greeting_2 $greeting_3

string="abcd"
echo ${#string}
#变量为数组时，${#string} 等价于 ${#string[0]}
echo ${#string[0]}

string="runoob is a great site"
echo ${string:1:4}
#第一个字符索引值为0

echo `expr index "$string" io`
array_num=(10 20 30 40 50)
echo ${array_num[2]}
num1=${array_num[3]}
echo $num1
echo ${array_num[@]}

length=${#array_num[@]}
echo $length
length=${#array_num[*]}
echo $length
#单元素长度
lengthn=${#array_num[n]}
echo $lengthn

:<<!
多行注释实例
试一试
chenyuanwei
echo $num1
!
echo $num1

