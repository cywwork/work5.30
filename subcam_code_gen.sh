#!/bin/bash
if [ ! -n "$1" ]
then
	echo "请输入芯片名"
	echo "请在包含要修改的芯片的文件下运行"
	exit -1
fi
path="./"	#当前目录
replaced=$1	#替换的芯片名字
replacing=$1"s"
echo "'$replaced' under the folder '$path'"
#统计修改的文件或者目录个数
count=1

for rename in `find $path -name "*$replaced*" | tac`
do
	newpath=`echo $rename | sed "s/\(.*\)$replaced/\1$replacing/"`
	sudo mv "$rename" "$newpath"
	echo "${count}: renaming $rename to $newpath"
	let count++
done
#改文件内容
echo "开始修改小写文本内容"
sed -i "s/$replaced/$replacing/g" `grep "$replaced" -rl $path`
echo "修改结束"
#改大写
replaced=`echo $replaced |tr a-z A-Z`
replacing=`echo $replacing |tr a-z A-Z`

for rename in `find $path -name "*$replaced*" | tac`
do
	newpath=`echo $rename | sed "s/\(.*\)$replaced/\1$replacing/"`
	sudo mv "$rename" "$newpath"
	echo "${count}: renaming $rename to $newpath"
	let count++
done
echo "开始修改大写文本内容"
sed -i "s/$replaced/$replacing/g" `grep "$replaced" -rl $path`
echo "修改结束"

