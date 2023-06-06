path="./"	#当前目录
replaced=$1	#替换的芯片名字
replacing=$1"s"
echo "'$replaced' under the folder '$path'"

count=1
for i in `find $path -name "*$replaced*" | tac`
do
	newpath=`echo $i | sed "s/\(.*\)$replaced/\1$replacing/"`
	sudo mv "$i" "$newpath"
	echo "${count}: renaming $i to $newpath"
	let count++
done
#改文件内容
echo "开始修改小写文本内容"
sed -i "s/$replaced/$replacing/g" `grep "$replaced" -rl $path`
echo "修改结束"
#改大写
replaced=`echo $replaced |tr a-z A-Z`
replacing=`echo $replacing |tr a-z A-Z`

for i in `find $path -name "*$replaced*" | tac`
do
	newpath=`echo $i | sed "s/\(.*\)$replaced/\1$replacing/"`
	sudo mv "$i" "$newpath"
	echo "${count}: renaming $i to $newpath"
	let count++
done
echo "开始修改大写文本内容"
sed -i "s/$replaced/$replacing/g" `grep "$replaced" -rl $path`
echo "修改结束"

