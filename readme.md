手写 C++ moduo 网络库，针对服务器实现高并发（客户端高并发没有必要）
# REFERENCE

[chenshuo/muduo](https://github.com/chenshuo/muduo)

[video: 21h](https://www.bilibili.com/video/BV1UE4m1R72y/?spm_id_from=333.337.search-card.all.click&vd_source=38033fe3a1f136728a1d6f8acf710b51)


可参考的实现：
* https://github.com/shenmingik/muduo
* https://github.com/L1w-Y/muduo


# 一、基础知识

## 1. 网络 IO 模型

同步和异步主要不同点：
1. 是否有通知机制
2. 将数据从内核缓冲区写入到用户缓冲区，这个工作是应用程序还是操作系统（内核）在做？

Node.js 基于异步非阻塞模式下的高性能服务器

为什么说 non-blocking 和 I/O multiplexing 一起用是 1+1>2，单独使用就是一坨呢？
首先，对于 non-blocking，busy-pooling 是很浪费 cpu 时间的
其次对于 I/O multiplexing，以 select 为例，如果我们使用 blocking，那么在 select 遍历 fd 的过程中，如果某个 fd 没有数据，就需要阻塞直到内核缓冲区准备好数据，那么此时显然后续的 fd 就无法处理了，这显然会造成一个饥饿的问题。
但是 I/O multiplexing 和 non-blocking 结合起来就很好的解决了它们两者之间的问题。

对于 Demultiplexing，它只是负责事件的分发（epoll_wait，增删改，我们可以将它视为一个 epoll），至于事件怎么处理，它不知道，事件的 handler 是存储在 reactor 里面的，可能是个 map，因此 Demultiplexing 将事件返回给 reactor，reactor 查询 event handler
reactor 主要就是存储了 event 以及对应 handler
当然，有时候也可以将 reactor 和 demultiplexing 合二为一，作为一个整体看待

## 2. muduo 简介
[muduo 网络库的安装](https://www.cnblogs.com/conefirst/articles/15224039.html)
核心理念：one loop per thread




