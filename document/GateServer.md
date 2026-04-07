## Pre



### boost库安装

[官网](https://www.boost.org/)

本机下载目录在`C:\dev\boost`

如果没有`b2.exe`可以先运行`bootstrapbat`批处理文件，检测是否缺少文件，并且生成`b2.exe`，可以用它编译`boost`库

在终端中执行`.\b2.exe install --toolset=msvc --build-type=complete --prefix="C:\dev\boost_1_89_0_complie" link=static runtime-link=shared threading=multi debug release`

VS2022对应MSVC14.3.	 PS:此处没法起作用，改为去掉版本

`static`静态库，`shared`动态库



编译后生成两个文件夹

- `include`头文件
- `lib`文件库



在VS中配置boost（假设在Debug | x64环境下）

视图->其他窗口->属性管理器->右键对应的文件夹->添加新项->双击配置VC++目录添加boost





### 配置jsoncpp

[下载](https://github.com/open-source-parsers/jsoncpp)

本机下载在`C:\dev\jsoncpp-1.9.7`

用cmake编译得到可以在VS上使用的动态库

[编译流程](https://subingwen.cn/cpp/jsoncpp/#1-2-%E7%94%9F%E6%88%90VS%E9%A1%B9%E7%9B%AE)

新建`C:\dev\jsonlib`文件夹，下面新建`include`，和`lib`

- `include`从下载的`jsoncpp`源码拷贝`include\json`

- `lib`拷贝`jsoncpp.lib`和`jsoncpp.dll`

PS:**错误原因： debug_heap.cpp Line:980 Expression:__acrt_first_block==header"**

　　**原来debug是采用MD的方式配置运行时库。**

　　**解决方式：将debug的运行时库的使用方式配置为：MDd, 重新编辑，则没有问题。**



## Code





### `const.h`

- 包含必要的头文件
- 简化作用域声明

- `enum ErrorCode`



### `CServer`

类成员包括

- `io_context`
- `acceptor`
- `socket`

类成员函数`void Start()`实现了异步的连接

将当前`socket`的所有权转移给`HttpConnection`，用智能指针管理该类对象，让`HttpConnection`来处理连接后的读写



### `HttpConnection`类

管理HTTP请求，负责收发，具体的业务逻辑处理交给`LogicSystem`

主要功能(部分成员函数)

- 构造函数:接收转移来的`std::move(_socket)`socket

- `void HttpConnection::Start()`，异步读取http请求`http::async_read(_socket, _buffer, _request, 回调函数)`，回调函数调用`HandleReq()`处理请求

- `void HttpConnection::CheckDeadLine()`，超时检测确保服务器在收到连接后，如果在60s内没有发送完整请求，将会关闭连接

- `void HttpConnection::HandleReq()`，分别处理Get和Post请求

  本质是调用**单例**类`LogicSystem`的`HandleGet`和`HandlePost`

- `void HttpConnection::WriteResponse()`，发送响应，告诉客户端`response_body`的长度，避免粘包

  调用`http::async_write(_socket, _response, handler)`，表示把`response`写到`socket`中，写完调用回调函数，并且关闭发送方向的`socket`，取消定时器

- URL编码与解码，URL只允许ASCII码，汉字“测”，在UTF-8会编码为`E6 B5 8B`一般占3个字节

  ```
  //Encode
  "a b" → "a+b"
  "@" → "%40"
  
  //Decode
  + → 空格
  %xx → 原字符
  ```

- `void HttpConnection::PreParseGetParam()`，获取GET请求的参数

  ```
  /user?id=123&name=abc
  ```

  1. 先找`?`
  2. `?`前面是`_get_url`,后面拆成`key = value`的形式，并且以`_get_params[key] = value`(`std::unordered_map<std::string, std::string> _get_params`)的形式保存



#### 小结

```
Start()
  ↓
async_read (读HTTP请求)
  ↓
HandleReq()
  ├── GET → 解析参数 → 业务处理(LogicSystem::GetInstance()->HandleGet(_get_url, shared_from_this()))
  └── POST → 业务处理
  ↓
WriteResponse()
  ↓
async_write (发送响应)
  ↓
关闭socket
```





### `LogicSystem`类

单例类，主要功能是路由，决定谁来处理连接

之所以用单例因为路由表只需要一份

核心成员

- `typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;`这是一个函数对象，参数是`HttpConnection`类型的智能指针

- `std::unordered_map<std::string, HttpHandler> _post_handlers, _post_handlers;`

主要成员函数

- `void LogicSystem::RegGet(std::string url, HttpHandler handler)`，根据url注册GET路由
- 构造函数
  - 注册了`/get_test`的GET请求，往 HTTP body 写数据
  - 注册了`/get_varifycode`POST请求，把HTTP body转为`string`，构造json写入`response`

- `bool LogicSystem::HandleGet()`根据url执行对应的回调



### `GateServer`主函数

- 创建服务器的监听窗口`8080`

- 创建`ioc_context`，参数`1`表示单线程

- 创建信号集合`signals`

- 创建并启动服务器`CServer`，用智能指针避免异步回调函数中`CServer`对象被提前销毁





## 总体逻辑



### 用户请求

1. 用户点击**获取验证码**，执行`void RegsisterDialog::on_get_code_clicked()`，构造`QJsonObject`

2. ```cpp
   HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_varifycode"),
                                               json_obj, ReqId::ID_GET_VARIFY_CODE, Modules::REGISTERMOD);
   //发出POST请求
   ```



### 服务器接收

1. Server启动
2. `CServer::Start()`函数中调用`_acceptor.async_accept`接收连接
3. 创建`HttpConnection`管理连接
4. `http::async_read()`异步读数据，同时检测发送超时，调用`HttpConnection::HandleReq()`处理请求
5. 处理读请求调用`LogicSystem`来处理，传入url和`connection`的智能指针
6. 调用`HandleGet`通过`map`映射查找，发现没有注册过返回false，下面会返回404，`_get_handlers`返回回调
7. 在`HttpConnection`中调用`HandleReq()`
8. 调用**单例**`LogicSystem::HandleGet()`
9. 如果是POST请求再`_post_handlers`找到对应的处理函数(哈希表，key是URL)
10. 解析JSON构造
11. 调用`HttpConnection::Write()`发送HTTP response



### 客户端接收响应

1. `QNetworkReply::finished`信号触发
2. 根据情况触发`emit self->sig_http_finish()`





## 小技巧

- `std::enable_shared_from_this`，保证在异步回调函数执行期间，当前对象不会被释放

- 以路由映射的方式处理请求，哈希表key-value
