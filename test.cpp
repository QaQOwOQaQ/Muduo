#include <muduo/net/TcpServer.h>
#include <muduo/net/TcpClient.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include <string>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace std::placeholders;

/** 
 * @brief 基于 muduo 网络库开发服务器程序
 * * 1. 组合 TcpServer 对象
 * * 2. 创建 EventLoop 事件循环对象的指针
 * * 3. 明确 TcpServer 构造函数需要什么参数，设置 ChatServer 的构造函数
 * * 4. 在当前服务器类的构造函数中，注册处理连接的回调函数和处理读写事件的回调函数
 * * 5. 设置合适的服务端线程数量，muduo 库会自己分配 I/O Thread 和 Worker Thread 的数量
 */

class ChatServer
{
public:
    ChatServer(EventLoop *loop,               // reactor
               const InetAddress &listenAddr, // IP + Port
               const string &nameArg)         // server name
        : _server(loop, listenAddr, nameArg), _loop(loop)
    {
        // 给服务器注册用户连接的创建和断开回调
            // 在这里我们的 onConnection 需要两个参数，其中第一个参数是隐式的 this 指针
            // 而 setConnectionCallback 中所接受的参数的函数签名是 void (const TcpConnectionPtr&)
            // 因此我们需要通过绑定器 bind 将 this 指针绑定到该函数上
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
       // 给服务器注册用户读写事件回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
        // 设置服务器端的线程数量
            // 一般是一个 I/O Thread 和 三个 worker Thread
        _server.setThreadNum(4);
    }

    // 开启事件循环
    void start() 
    {
        _server.start();
    }

private:
    // 处理用户连接的创建和断开
    void onConnection(const TcpConnectionPtr &conn)
    {
        // Peer Address: 对端地址
        // Local Address: 本地地址
        if(conn->connected()) {
            cout << conn->peerAddress().toIpPort() << " -> " << 
                conn->localAddress().toIpPort() << "state:online" << endl;
        }
        else {
            cout << conn->peerAddress().toIpPort() << " -> " << 
                conn->localAddress().toIpPort() << "state:offline" << endl;
            conn->shutdown(); // close(fd)
            // _loop ->
        }
    }

    // 处理用户读写事件
    void onMessage(const TcpConnectionPtr &conn,    // 连接
                   Buffer * buffer,                 // 缓冲区
                   Timestamp time)                  // 接收到数据的时间信息
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data: " << buf << " time:" << time.toString() << endl;

    }

    TcpServer _server;
    EventLoop *_loop; // 操作 epoll
};

int main()
{
    EventLoop loop; // epoll
    InetAddress listenAddr("127.0.0.1", 6000);
    ChatServer server(&loop, listenAddr, "chatServer");

    server.start(); // listenfd epoll_ctl=>epoll
    loop.loop();  // epoll_wait 以阻塞方式等待新用户连接，已连接用户的读写事件等
    return 0;
}