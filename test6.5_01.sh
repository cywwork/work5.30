#!/bin/bash
# author :chenyuanwei
# url:

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

num1=$[2*3]
num2=$[1+5]
if test $[num1] -eq $[num2]
then
  echo ' 相等'
else
  echo '两个数字不等'
fi

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

echo '按下<CTRL-D>退出 '
echo -n '输入你最喜欢的网站名：'
while read FILM
do
	echo "是的！$FILM 是一个好网站"
done

a=0

until [ !  $a -lt 10 ] 

do
	echo $a
	a=`expr $a + 1`
done

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
                #continue
		break
		echo "游戏结束"
        ;;
        esac
done

demoFun(){
	echo "这是第一个shell函数"
}
echo "函数的开始"
demoFun
echo "函数的结束"


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

funWithParam(){
	echo "第一个参数为 $1 !"
	echo "第二个参数为 $2 !"
	echo "第十个参数为 $10 !"
	echo "第十个参数为 ${10} !"
	echo "第十一个参数为 ${11} !"
	echo "参数总数有 $# 个！"
	echo "作为一个字符串输出所有参数 $*"
}
funWithParam 1 2 3 4 5 6 7 8 9 34 77

