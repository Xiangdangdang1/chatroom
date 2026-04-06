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

### beast

添加新Class `CServer`接受端口号，创建acceptor接收新到来的链接

- `const.h`包含必要的头文件，以及简化作用域声明

- 







总体逻辑

1. Server启动
2. 监听连接
3. 对方发出连接请求，交给`HttpConnection`管理
4. `HttpConnection`监听读事件，处理读请求，同时检测发送超时
5. 处理读请求调用`LogicSystem`来处理，传入url和`connection`的智能指针
6. 调用`HandleGet`通过`map`映射查找，发现没有注册过返回false，下面会返回404，`_get_handlers`返回回调
7. 在`HttpConnection`中调用`HandleReq`





关于URL的解析写在`HttpConnection.cpp`

URL只允许ASCII码，汉字“测”，在UTF-8会编码为`E6 B5 8B`一般占3个字节



在代码文件夹添加配置文件`config.ini`，添加到项目中

在`chatroom.pro`添加`win32:CONFIG`
