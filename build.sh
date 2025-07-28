#!/bin/bash

# 开启错误处理模式，遇到错误时立即退出 
set -e
# 开始调试模式，显示执行的每一条命令
set -x

if [ ! -d "$(pwd)/build" ];then
    mkdir -p "$(pwd)/build"
fi

rm -rf "$(pwd)/build/*"
rm -rf "$(pwd)/lib/*"

cd "$(pwd)/build" && cmake .. && make

# # 回到项目根目录
cd ..

# 把头文件拷贝到/usr/local/include/muduo-core   so库拷贝到 /usr/local/lib

if [ ! -d "/usr/local/include/muduo_core" ]; then
    mkdir -p "/usr/local/include/muduo_core"
fi

for header in $(ls *.h)
do
    cp "$header" "/usr/local/include/muduo_core"
done

cp "$(pwd)/lib/libmuduo-core.so" "/usr/local/lib"

ldconfig
