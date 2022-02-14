/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：test_tcp_server.cpp
      	创 建 者：hsby
      	创建日期：2022/2/14
 *================================================================*/

#include <string>
#include "tcp_server.h"

const std::string rsp = "HTTP/1.1 200 OK\r\nContent-length:8\r\n\r\nabcdefgh\r\n";

class SimpleHttpServer : public xco::TcpServer {
public:
    void ClientHandle(xco::Socket::Ptr client) override {
        std::string req;
        req.resize(4096);
        while(client->Recv(&req[0], req.size()) > 0) {
            client->Send(rsp);
        }
        LOGDEBUG("client close, " << client->ToString());
        client->Close();
    }
};

int main() {
    //SetLogLevel(4);
    auto shs = std::make_shared<SimpleHttpServer>();
    if (!shs->Init(xco::Ipv4Address::Create("0.0.0.0", 80))) {
        return -1;
    }
    if (!shs->Start()) {
        return -1;
    }
}