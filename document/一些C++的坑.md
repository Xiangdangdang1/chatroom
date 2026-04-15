### 关于Redis的API

- `redisReply* r = (redisReply*)redisCommand(c, "AUTH %s", redis_password);` 要改成

  `redisReply* r = (redisReply*)redisCommand(c, "AUTH %s", redis_password.c_str());`

  `redisCommand`接受的是C风格字符串(因为来自`hiredis`)只接受`const char*`，原本的`redis_password`是`std::string`类型

  **C风格字符串`char*`最后是以`\0`结尾**

  `std::string`也有`\0`，为了兼容C风格，但是不以此结束





### boost::asio

- ```cpp
  AsioIOServicePool::AsioIOServicePool(std::size_t size) :_ioServices(size),
  _works(size), _nextIOService(0) {}
  
  std::vector<WorkPtr> _works;
  ```

  不能赋值的类型，必须放在初始化列表中初始化，例如`io_context`删除了拷贝赋值
  
- ```cpp
  std::unique_ptr<VerifyService::Stub> stub = VerifyService::NewStub(channel);
  _connections.push(std::move(stub));
  ```

  `unique_ptr`没有拷贝，只能用移动