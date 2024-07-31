#!/bin/bash

# 检查是否传递了参数
if [ $# -ne 1 ]; then
    echo "Usage: $0 <copy_size_in_bytes>"
    exit 1
fi

COPY_SIZE=$1

# 加载内核模块，并传递copy_size参数
sudo insmod dma_memcpy.ko copy_size=$COPY_SIZE

# 卸载内核模块
sudo rmmod dma_memcpy 2>/dev/null

# 查看dmesg输出
dmesg | tail -n 5