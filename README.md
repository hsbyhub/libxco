# libxco

---
作者：谢森鑫

[Click to view english README](./README_en.md)

## 简介
libxco是一个轻量级高性能协程网络库，用户使用libxco可以轻松快速地拥有一个协程环境，
除此之外，lixco还提供http的相关组件，比如HttpServer类，可以轻松快速地搭建一个Http服务器，
用户只需要为不同的URI注册不同的回调，即可轻松快速地实现不同的业务逻辑。

## 压测效果
在VirtualBox虚拟机下的Centos7(四核)进行单机压测  
![image-20220218095832880](https://gitee.com/hsby/img/raw/master/202202180958352.png)

服务器开4个进程  
![image-20220218100409436](https://gitee.com/hsby/img/raw/master/202202181004507.png)

## 目录
```
┃
┠─── CMakeLists.txt             CMake configuration file
┠─── test                       Test examples
┠─── src                        Implementation
┃    ┠─── common.cpp            Common tools
┃    ┠─── sys_context_swap.S    Coroutine context swap
┃    ┠─── coroutine.cpp	        Coroutine
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

## 开发环境
- 操作系统：  Centos7
- 开发工具： Neovim5.0 & Clion
- 编译器： GCC4.8
- 工程构建： CMake2.8
- 版本控制： Git
- 调试工具： GDB

## 安装和使用
```
mkdir build && cd build
cmake ../src
make && make install
```
- 创建、唤起、挂起协程  
![image-20220218102720523](https://gitee.com/hsby/img/raw/master/202202181027571.png)
- 使用协程调度器  
![image-20220218110441629](https://gitee.com/hsby/img/raw/master/202202181104671.png)  
更多使用方法可以参考test下的用例

## Hook的系统调用(协程调度环境下)
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