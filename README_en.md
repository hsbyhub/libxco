# libxco

---

Author：xiesenxin(hsbymail@163.com)

[中文README](./README.md)

## Introduction

libxco is a lightweight and fast coroutine network library, users can easily have a coroutine environment using libxco.
Extra, lixco related components like Http server class, can easily also provide an http server, can easily provide an http server.
Users only need to register different URI as different business logics that can be easily and quickly implemented.

## Pressure Test

Single-machine stress test on Centos7 (4-core) under VirtualBox virtual machine  
![image-20220312105339382](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121055924.png)

Process status  
![image-20220312105415361](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121055068.png)

## Directory

```
┃
┠─── install.sh     Install shell script
┠─── test           Test examples
┠─── src            Implementation
┃    ┠─── CMakeLists.txt        CMake configuration file
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
┃    ┠─── http                  About HTTP
┃    ┗─── util                  Util
┃
```

## Installation & Usage
```
git clone https://gitee.com/hsby/libxco
cd libxco
chmod a+x install.sh
./install.sh
```
- Create, resume and yield a coroutine  
![image-20220312105541763](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121055805.png)
- Use the coroutine shceduler  
![image-20220312105555931](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121055967.png)

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
