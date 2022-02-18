# libxco

---

Author：SenXinXie

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
┠─── CMakeLists.txt             CMake configuration file
┠─── test                       Test examples
┠─── src                        Implementation
┃    ┠─── common.cpp            Common tools
┃    ┠─── sys_context_swap.S    Coroutine context swap
┃    ┠─── coroutine.cpp         Coroutine
┃    ┠─── scheduler.cpp         Coroutine scheduler
┃    ┠─── timer.cpp             Timer
┃    ┠─── fdmanager.cpp         Fd manager
┃    ┠─── iomanager.cpp         Io manager under coroutine schedule
┃    ┠─── hook.cpp              Hook to system calls under under coroutine schedule
┃    ┠─── address.cpp           Wrap to struct sockaddr family
┃    ┠─── socket.cpp            Wrap to TCP socket
┃    ┠─── bytearray.cpp         Serialization
┃    ┠─── socket_stream.cpp     Socket stream
┃    ┠─── tcp_server.cpp        TCP server
┃    ┗─── http                  About HTTP
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