#!/bin/bash
if [ ! -n  "$1" ]
then
	echo "请输入设备名！"
	echo "请在有/shell_test_project_code路径下运行"
	exit -1
fi
target=$1 #目标芯片名
IcPath="./shell_test_project_code/device/agenew" #芯片内核号所在的路径
ConfigPath="./shell_test_project_code" #config所在文件

#找到内核版本
for icpath in `find $IcPath -name "*$target*" -type d`
do
	
	ProjectPath="$icpath/ProjectConfig.mk"
	#内核版本号
	version=$(grep "LINUX_KERNEL_VERSION" $ProjectPath |cut -d ' ' -f3)
	

	#找到config信息,不获取debug版本的配置信息
	for configpath in `find "$ConfigPath/$version" -name "*$target*config" ! -name "*debug*" -type f`
	do
		config=$(grep "CONFIG_CUSTOM_KERNEL_LCM" $configpath |cut -d "\"" -f2)
		
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

if [ ! -n "$icpath" ]
then
	echo "没有这个设备名字，请检测输入设备名"
fi
