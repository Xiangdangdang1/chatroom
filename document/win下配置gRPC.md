## 环境配置



### 编译gRPC

[官方教程](https://grpc.io/docs/languages/cpp/quickstart/)

**其他准备**

- [nasm](https://www.nasm.us/)安装，并配置环境变量
- [Go](https://studygolang.com/dl)安装，安装到默认位置，`go version`验证
- Perl安装，从网盘中装，一路next



用cmake编译gRPC到`dev/grpc/forvs`，打开`grpc.sln`，在Debug x64模式下右键`ALL_BUILD`重新生成

- 配置包含目录（C/C++->常规->附加包含目录）
- 库目录（链接器->常规->附加库目录）
- 添加依赖项（链接器->输入->附加依赖项）





编译之后找不到`upb.lib`

为了避免各种版本问题，直接用博主[网盘](https://pan.baidu.com/s/1BBaAZ8-R-GSxxcy2s7TRWA?pwd=ybno)里面的grpc

好像用vcpkg再屏蔽错误消息也可以，就这样吧



## 使用vcpkg实现包管理

如果在VS中使用vcpkg，在命令行输入`vcpkg integrate install`

```
CMake projects should use: "-DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"

All MSBuild C++ projects can now #include any installed libraries. Linking will be handled automatically. Installing new libraries will make them instantly available.
```

在项目中只需要`#include`已经安装的库，vcpkg自动链接这些库

直接使用全局安装gRPC

```
vcpkg install grpc:x64-windows
```





## Code

### proto文件编写

在项目目录下新建`message.proto`

```shell
syntax = "proto3";	

package message;

service VarifyService {
  rpc GetVarifyCode (GetVarifyReq) returns (GetVarifyRsp) {}
}

message GetVarifyReq {
  string email = 1;
}

message GetVarifyRsp {
  int32 error = 1;
  string email = 2;
  string code = 3;
}
```

终端中执行

```shell
C:\dev\grpc\forvs\third_party\protobuf\Debug\protoc.exe  -I="." --grpc_out="." --plugin=protoc-gen-grpc="C:\dev\grpc\forvs\Debug\grpc_cpp_plugin.exe" "message.proto"
```

生成了两个文件`message.grpc.pb.cc`，`message.grpc.pb.h`

执行命令，用`protoc.exe`把`message.proto`生成对应的C++代码

```shell
C:\dev\grpc\forvs\third_party\protobuf\Debug\protoc.exe --cpp_out=. "message.proto"
```

生成了两个文件`message.pb.cc`，`message.pb.h`

把刚才创建的五个文件添加到项目中





### 新建`ConfigMgr`类读取配置文件

单例









### 服务器参数配置

在文件目录下，新建`config.ini`加入项目

```cpp
[GateServer]	//section，类似map的key-value
Port = 8080		//value
[VarifyServer]
Port = 50051
```

添加`ConfigMgr`类管理配置文件

