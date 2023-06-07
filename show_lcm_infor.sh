#!/bin/bash
target=$1 #目标芯片名
dpath="./shell_test_project_code/device/agenew" #芯片内核号
cpath="./shell_test_project_code" #cofig所在文件
#找到内核版本
for i in `find $dpath -name "*$target*" -type d`
do
	newpath="$i/ProjectConfig.mk"
	#echo $newpath
	version=$(grep "LINUX_KERNEL_VERSION" $newpath |cut -d ' ' -f3)
	#echo $version

	#找到config信息,不获取debug版本的配置信息
	for j in `find "$cpath/$version" -name "*$target*config" ! -name "*debug*" -type f`
	do
		config=$(grep "CONFIG_CUSTOM_KERNEL_LCM" $j |cut -d "\"" -f2)
		
	done
	#切割输出对应的信息
	#echo $config
	for LCM in ${config}
	do
		echo "LCM_NAME is : $(echo $LCM)"
		echo "IC is : $(echo $LCM | cut -d '_' -f1)"
		echo "Module : $(echo $LCM | cut -d '_' -f2)"
		echo "lane is : $(echo $LCM | cut -d '_' -f3)"
		echo "resolution is : $(echo $LCM | cut -d '_' -f4)"
		echo " "	

	done

	
done


