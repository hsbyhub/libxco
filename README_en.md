# libxco

---

作者：SenXinXie

[点击查看中文README](./README.md)

## Introduction

libxco is a lightweight and fast coroutine network library, users can easily have a coroutine environment using libxco.
Extra, lixco related components like Http server class, can easily also provide an http server, can easily provide an http server.
Users only need to register different URI as different business logics that can be easily and quickly implemented.

## Pressure Test

Single-machine stress test on Centos7 (4-core) under VirtualBox virtual machine  
![image-20220218095832880](https://gitee.com/hsby/img/raw/master/202202181115621.png)

Process status  
![image-20220218100409436](https://gitee.com/hsby/img/raw/master/202202181115548.png)

## Directory

```
┃
┠─── CMakeLists.txt             CMake工程配置文件
┠─── test                       测试用例
┠─── src                        库实现
┃    ┠─── common.cpp            公共工具
┃    ┠─── sys_context_swap.S    协程上下文切换(参考腾讯的libco)
┃    ┠─── coroutine.cpp	        协程
┃    ┠─── scheduler.cpp         协程调度
┃    ┠─── timer.cpp             计时器事件
┃    ┠─── fdmanager.cpp         套接字管理
┃    ┠─── iomanager.cpp         协程调度下的IO管理器
┃    ┠─── hook.cpp              协程环境下针对系统调用进行hook(包括阻塞和影响协程上下文的系统调用)
┃    ┠─── address.cpp           套接字地址（对sockaddr结构族进行封装）
┃    ┠─── socket.cpp            套接字（主要针对TCP套接字进行封装）
┃    ┠─── bytearray.cpp         序列化
┃    ┠─── socket_stream.cpp     套接字流
┃    ┠─── tcp_server.cpp        tcp服务器
┃    ┗─── http                  http实现(包括HttpServer)
┃
┃
```

## Develpment Environment
- OS：Centos7
- Development Tool: Neovim5.0 & Clion
- Compiler: GCC4.8
- Build Tool: CMake2.8
- Version Control: Git
- Debug Tool: GDB

## Installation & Usage
```
mkdir build && cd build
cmake ../src
make && make install
```
- Create, resume and yield a coroutine  
![image-20220218102720523](https://gitee.com/hsby/img/raw/master/202202181115734.png)
- Use the coroutine shceduler  
![image-20220218110441629](https://gitee.com/hsby/img/raw/master/202202181104671.png)

See the test examples to learn more usage

## Hook system calls under coroutine environment

sleep  
usleep  
nanosleep  
socket  
connect  
accept  
read  
readv  
recv  
recvfrom  
recvmsg  
write  
writev  
send  
sendto  
sendmsg  
close  
fcntl  
ioctl  
getsockopt  
setsockopt  