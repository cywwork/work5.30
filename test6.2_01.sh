#!/bin/bash
# author:chenyuanwei
# url:

echo "Shell 传递参数实例!"
echo "执行的文件名：$0";
echo "第一个参数为：$1";
echo "第二个参数为：$2";
echo "第三个参数为：$3";

echo "-- \$* 演示 ---"
for i in "$*"; do
	echo $i
done

echo "-- \$@ 演示 ---"
for i in "$@"; do
	echo $i
done
my_array=(A B "C" D)

echo "第一个元素为：${my_array[0]}"
echo "第二个元素为: ${my_array[1]}"
echo "第三个元素为: ${my_array[2]}"
echo "第四个元素为: ${my_array[3]}"

declare -A site=(["google"]="www.googel.com" ["runoob"]="www.runoob.com" ["taobao"]="www.taobao.com")

declare -A cyw
cyw["age"]=25 
cyw["year"]=1998 
cyw["work"]="work"

echo ${cyw["age"]}
echo ${site["runoob"]}
echo "数组的元素为: ${site[*]}"
echo "数组的键为: ${!site[*]}"

