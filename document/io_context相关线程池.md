### GateServer存在的问题

目前所有的连接和连接上的读写处理都共用一个`ioc`，只调用了一次`ioc.run();`

```
main线程
   │
   ├── ioc.run()
   │     ├── accept连接
   │     ├── 处理请求
   │     ├── 回调 handler
   │
   └── 全部串行执行
```



### `AsioIOServicePool`池

继承单例类，主要用于构建`io_context`池

**主要成员**

```cpp
std::vector<IOService> _ioServices;		//多个ioc组成的池子
std::vector<WorkPtr> _works;		//数量与ioc一致
std::vector<std::thread> _threads;	//线程数量与ioc一致
std::size_t _nextIOService;	//轮询，上一次访问的ioc++得到这次访问的
```

`_work`的作用：当`io_context`没有任何事件的时候，直接返回，而`_work`的存在让`run()`函数一直阻塞等待事件的发生



- **构造函数**`AsioIOServicePool(std::size_t size = 2);`，根据不同成员，分别初始化

  1. 创建`size`个`io_context`
  2. 每个`ioc`绑定一个`work`

  3. 遍历每个`ioc`，各自开线程都执行各自的`ioc.run()`

  ```
  io_context[0]  ---> thread[0]
  io_context[1]  ---> thread[1]
  ...
  ```

  

- 保证轮询

  ```cpp
  boost::asio::io_context& AsioIOServicePool::GetIOService() {
      auto& service = _ioServices[_nextIOService++];
      if (_nextIOService == _ioServices.size()) {
          _nextIOService = 0;
      }
      return service;
  }
  ```

  

- `void Stop()`，遍历每个`ioc`，停止服务。一般在析构之前

  ```cpp
  for (std::size_t i = 0; i < _ioServices.size(); ++i) {
      _ioServices[i].stop();	//通知ioc停止
      _works[i].reset();	//释放智能指针
  }
  //等待每个线程执行完才退出
  for (auto& t : _threads) {
      t.join();
  }	
  ```

  

- 析构函数调用`Stop()`





### `RPConPool`池

gRPC的`Stub`连接池，`Stub`是一个“信使”，把本地的调用转为对gRPC服务器的请求，作用如下

1. 把 req 序列化（protobuf）
2. 通过 channel 发送到服务器
3. 等待返回
4. 反序列化 response
5. 填充 resp



**主要成员**

```cpp
std::atomic<bool> _b_stop;	//连接池停止标记, false表示连接池正常运行
size_t _poolsize;	//池大小
std::string _host;	//记录gRPC服务器位置
std::string _port;
std::queue<std::unique_ptr<VerifyService::Stub>> _connections;
std::condition_variable _condi;		//没有连接的时候阻塞等待，有连接唤醒线程
std::mutex _mutex;	//线程锁
```

补充

- `_b_stop`用原子变量，因为会被多个线程读写
- 用`unique_ptr`和队列管理`stub`



- **构造函数**，根据gRPC服务的ip + port创建连接，为每个连接创建`stub`，并且push到连接池队列中

- **析构函数**，由于多个线程同时访问同一个连接池`_connections`，加锁



- `getConnection()`，从池子里取一个`stub`

  1. 先加锁

  2. 等待资源`_condi.wait()`

     如果连接池空，并且连接池没有关闭 → 等待

  3. 判断是否连接池关闭`_b_stop`

  4. 取出来连接`auto stub = std::move(_connections.front());`



- `returnConnection()`把连接还到池子中



**整个流程**

```
线程A ─┐
线程B ─┼─→ getConnection → 拿stub → 执行RPC → returnConnection
线程C ─┘

A 调用 getConnection()
A 加锁
A 拿到 stub1
A 解锁
.....
C 调用 getConnection()
C 加锁
发现池空
→ 进入 wait（释放锁 + 阻塞）
```





### `VerifygRPCClient`类

`GateServer`通过该类作为gRPC的Client访问服务器

- 成员变量只有 `std::unique_ptr<RPConPool> _pool;`

- 构造函数：读配置文件，构造连接池
- `GetVerifyRsp GetVerifyCode(std::string email)`通过gRPC stub 调用gRPC的`GetVerifyCode`获取验证码的客户端封装函数



### `CServer`的改动

```cpp
//从连接池获取ioc
auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
//创建连接，传给HttpConnection的构造函数，生成一个新的socket连接，HttpConnection类管理这个连接
std::shared_ptr<HttpConnection> new_connection = std::make_shared<HttpConnection>(io_context);
```







`configmgr`改为单例

