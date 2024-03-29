# libxco

---
作者：xiesenxin(hsbymail@163.com)

[English README](./README_en.md)

## 简介
libxco是一个轻量级高性能协程网络库，用户使用libxco可以轻松快速地拥有一个协程环境，
除此之外，lixco还提供http的相关组件，比如HttpServer类，可以轻松快速地搭建一个Http服务器，
用户只需要为不同的URI注册不同的回调，即可轻松快速地实现不同的业务逻辑。

## 压测效果
在VirtualBox虚拟机下的Centos7(四核)进行单机压测  
![image-20220312105339382](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121055924.png)

服务器开4个进程  
![image-20220312105415361](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121055068.png)

## 目录
```
┃
┠─── install.sh         安装脚本
┠─── test               测试用例
┠─── src                库实现
┃    ┠─── CMakeLists.txt        CMake工程配置文件
┃    ┠─── common.cpp            工具
┃    ┠─── sys_context_swap.S    协程上下文切换
┃    ┠─── coroutine.cpp         协程
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
┃    ┠─── http                  http实现(包括HttpServer)
┃    ┗─── util                  工具
┃
```

## 安装和使用
```
git clone https://gitee.com/hsby/libxco
cd libxco
chmod a+x install.sh
./install.sh
```
- 创建、唤起、挂起协程  
![image-20220312105541763](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121055805.png)

- 使用协程调度器  
![image-20220312105555931](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121055967.png)

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
