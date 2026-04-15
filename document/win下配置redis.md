## 安装配置

[github](https://github.com/tporadowski/redis/releases)下载一路next

修改一下`redis.windows.conf`

- port为默认6379
- requirepass 123456

在redis目录下启动redis服务`.\redis-server.exe .\redis.windows.conf`

启动redis客户端输入密码`.\redis-cli.exe -p 6379`测试是否连接成功

**添加依赖库**

`.\vcpkg install hiredis:x64-windows`