### 简介

C++全栈聊天项目实战

> 出自bilibili恋恋风辰zack
>
> [代码](https://gitee.com/secondtonone1/llfcchat)
>
> [文档](https://gitbookcpp.llfc.club/sections/cpp/project/day01.html)

- C端QT界面编程
- asio异步服务器设计（Sever与Client之间）
- beast网络库搭建http网关
- node.js搭建验证服务
- 服务器之间用grpc通信

![https://cdn.llfc.club/1709009717000.jpg](readme-img/1709009717000.jpg)

1. GateServer为网关服务，主要应对客户端的连接和注册请求，服务器是是分布式，所以GateServer收到用户连接请求后会查询状态服务选择一个负载较小的Server地址给客户端，客户端拿着这个地址直接和Server通信建立长连接。
2. 