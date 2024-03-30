#!/bin/bash

echo "================   zbwos build $1 begin   ================"

#初始化平台
rm ../include/soc/* -rf
cp ../soc/$1/*.h ../include/soc/
cp ../soc/$1/lcd/*.h ../include/soc/
cp makefile_$1 makefile

#编译平台启动模块
make -C ../soc/$1/start/ $2

#编译平台驱动
make -C ../soc/$1/ $2

#编译内核
make -C ../zbwos_core/ $2

#编译文件系统
make -C ../file_system/ $2

#编译应用程序
cd ../user/
../user/build.sh $2
cd -

#链接
make $2 PLATFORM=$1

