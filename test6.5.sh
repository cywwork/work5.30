#!/bin/bash

val=`expr 2 + 2`
echo "两数之和为：$val"
a=10
b=20
echo `expr $a + $b`
echo `expr $a - $b`
echo `expr $a \* $b`
echo `expr $b / $a`
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

#布尔运算实验

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
#逻辑运算符
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

#字符串运算符
echo “字符串运算符实验”

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

#文件实验
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


#echo 实验

echo "it is a test"
#读一行
read name
echo "$name it is a test"
#换行
echo -e "OK! \n" # -e 开启转义
echo " it is a test"

echo -e "ok!\c" # \c不换行
echo "is test"

echo "todat is 6.5  " > test6.5.txt

echo '$name\"'

echo `date`

echo "hello,shell"
printf "Hello, Shell\n"

printf "%-10s %-8s %-4s\n" 姓名 性别 体重kg
printf "%-10s %-8s %-4.2f\n" 郭靖 男 66.1234
printf "%-10s %-8s %-4.2f\n" 杨过 男 48.6543
printf "%-10s %-8s %-4.2f\n" 郭芙 女 47.9876
#单双引号效果一样
printf "%d %s\n" 1 "adc"
printf '%d %s\n' 1 "adc"

printf %s abcdcds

printf %s abc def

printf "%s %s %s\n" a b c d f g h i j

printf "%s and %d \n"

printf "a string no:<%s>\n" "A\nB"

printf "a string no:<%b>\n" "A\nB"

printf "www.com \a"
echo "www"

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

