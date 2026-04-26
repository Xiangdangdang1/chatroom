## 封装redis连接池

首先封装了`RedisMgr`操作类，来提供操作Redis数据库的一系列接口

在`RedisMgr`类下新增连接池类`RedisConPool`

与之前的连接池类似

**主要成员**

```cpp
std::atomic<bool> _b_stop;
std::mutex _mutex;
std::condition_variable _cond;

size_t _pool_size;
const char* _host;
int _port;

std::queue<redisContext*> _connections;
```



**主要功能**

- 构造函数：初始化各个成员，把连接构造好加入池子，*hiredis*获取`reply`是`malloc`记得用`freeReplyObject`释放

  ```cpp
  auto reply = (redisReply*)redisCommand(context, "AUTH %s", pwd);
  ```

- 析构函数：`std::lock_guard<std::mutex> lock(_mutex)`，置空连接池

- 获取连接：`std::unique_lock<std::mutex> lock(_mutex)`，没连接的时候用`_cond.wait`通知其他线程等待

  - 取队头返回
  - 不要忘记弹出

- 归还连接：`std::lock_guard<std::mutex> lock(_mutex)`，入队
- 关闭连接
  - `_b_stop`置为`true`
  - `_cond`通知所有线程
