## 实现单例模板类

`Singleton.h`

```cpp
#ifndef SINGLETON_H
#define SINGLETON_H
#include "global.h"

template <typename T>

class Singleton {
protected:
    Singleton() = default;  //单例类的构造函数不允许外部访问，子类继承父类时，子类能构造基类，所以设置为protected
    Singleton(const Singleton<T>&) = delete;    //不允许拷贝构造
    Singleton& operator= (const Singleton<T>& st) = delete;    //不允许拷贝赋值

    // static std::shared_ptr<T> _instance;

public:

    //现代C++的写法
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> _instance = std::make_shared<T>();
        return _instance;
    }

    // //在C++11之后，static局部变量是线程安全的，可以使用以下写法，不用智能指针的写法
    // static T& GetInstance() {
    //     static T instance;
    //     return instance;
    // }

    //打印指针的地址
    void PrintAddress()
    {
        std::cout << Singleton::GetInstance().get() << std::endl;
    }

    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    }
};


#endif // SINGLETON_H

```





- CRTP, 奇异递归模版，把子类自身作为父类的模板，模板在运行的时候才会动态实例

  `class HttpMgr : public QObject, public Singleton<HttpMgr>`

- 为什么使用`protected`，子类可以调用父类的构造函数

  ```cpp
  class ConfigManager : public Singleton<ConfigManager>
  {
      friend class Singleton<ConfigManager>;
  private:
      ConfigManager() {}
  };
  ```

- **从C++11开始，局部的`static`变量是线程安全的，如果多个线程同时进入函数，只有一个线程会初始化，其他线程等待**



**`GetInstance()`的设计**

- 继承单例的子类析构要设置为`public`（改了C++11的写法好像无关了）

  单例中的`static T instance`是静态的，在程序结束后执行析构，先调用子类析构再调用父类析构。如果析构不是`public`编译器无法访问析构

- 不要使用`static std::shared_ptr<T> instance = std::make_shared<T>();`

  应该使用`static std::shared_ptr<T> instance(new T());`

  因为`std::make_shared`在`std`命名空间内调用构造，不在`Singlton`作用域

  



- 为什么继承单例的子类构造是不能是`public`？

  

- 为什么要声明友元？

  当第一次执行`Httpmgr::GetInstance()`的时候，执行到`static HttpMgr instance;`

  - 分配静态存储区内存
  - 调用`Httpmgr()`构造函数，此时父类`Singleton`需要访问子类`private`的构造函数，所以在子类中声明父类是`friend`
  - 构造完成之后返回引用

  

- `httpmgr.h`中`void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);`的参数说明

  - `QUrl url`请求的服务器地址
  - `QJsonObject json`POST请求发送的json数据
  - `ReqId req_id`请求的功能id，写在`global.h`中的`enum`
  - `Modules mod`请求属于什么模块，登录/聊天等，写在`global.h`中的`enum`



- `globa.h`中定义一些`enum`类型





**`PostHttpReq()`的设计**

- http传输的是字节流，因此用`QByteArray data = QJsonDocument(json).toJson();`把JSON转为字节数组


- `QNetworkReply *reply = _manager.post(request, data);`其中`*reply`表示这次请求

- `auto self = shared_from_this();`


  - `self`是一个关于`this`的智能指针，与其他管理`this`的智能指针共享引用计数
  - 如果直接使用智能指针管理`this`，出现两个智能指针管理同一块内存可能会出现问题
  - 要使用这个技术，则要继承`class HttpMgr : public std::enable_shared_from_this<HttpMgr>`
  - 从当前对象 `this` 生成一个 `std::shared_ptr`，防止lambda表达式在没调用完之前，httpmanager被回收。

  - 因为`QObject::connect(reply, &QNetworkReply::finished, [self, reply, req_id, mod](){...};`是一个异步回调的lambda表达式。`PostHttpReq()` 执行完就返回了，Lambda可能几秒后才执行，此时`HttpMgr`被摧毁，this变为野指针
  - 因此httpmgr使用智能指针管理，避免该lambda表达式回调触发之前，httpmgr存在。
  - `self`放入lambda表达式的捕获列表，`self`的引用计数+1

  引用计数的变化具体如下

  ```
  创建对象
  ref = 1   (mgr)
  
  ↓ shared_from_this()
  ref = 2   (mgr + self)
  
  ↓ lambda 捕获
  ref = 3   (mgr + self + lambda)
  
  ↓ 函数结束
  ref = 2   (mgr + lambda)
  
  ↓ mgr 释放
  ref = 1   (lambda)
  
  ↓ 回调执行完
  ref = 0   → 析构 ✅
  ```

- 收到回包之后，`*reply`会发出一个信号，`QNetworkReply::finished`

- 注意手动回收`*reply`



- 在`RegisterDialog`的构造函数中连接slot与sig

**`void RegsisterDialog::slot_reg_mod_finish(ReqId req_id, QString res, ErrorCodes err)`的设计**

- client中的`class A`想要与Server中的`class B`通信
    1. `A`类对象序列化变为`QByteArray`字节流01类型
    2. 发送给`B`
    3. `B`收到字节流组装为`ByteArray`
    4. 反序列化变为类对象发给`B`

- `_handlers`是一个`map`，key是请求Id, value是用于处理的函数对象



## http相关理论





## QT相关理论

- 槽函数是对信号进行响应的函数，槽函数的参数数量要 <= 信号数量，参数顺序要一致

- `emit`是QT的语法糖，没啥具体意义，表示发信号

- 一般信号谁发的就在哪定义

- `QJsonDocument`辅助帮助我们把字符串转为json文档（类似`.json`）

  `string`->`QByteArray`->`QJsonDocument`

- `jsonObj`是json的对象，类似

  ```
  {
  	"name": "zhang san",
  }
  ```

  
