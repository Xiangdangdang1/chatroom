## 封装MySQL连接池

本来跟着视频敲了半天，发现`jdbc`在`mysql-connector-cpp`的8.X版本之后通过vcpkg途径安装之后找不到，取而代之的是新版的

`X devAPI`，默认封装了MySQL的连接池`mysqlx::Client`

```c++
// 1. 构建连接字符串，默认端口是33060
std::string connStr = "mysqlx://user:pwd@host:33060/schema";

// 2. 创建配置对象
mysqlx::ClientSettings settings(connStr);

// 3. 配置连接池参数 (这些都是原生支持的)
settings.set(mysqlx::ClientOption::POOL_MAX_SIZE, 10);      // 池中最大连接数
settings.set(mysqlx::ClientOption::POOL_MAX_IDLE_TIME, 600); // 连接空闲多久后自动关闭(秒)
settings.set(mysqlx::ClientOption::POOL_QUEUE_TIMEOUT, 2000); // 池满时等待连接的超时时间(毫秒)

// 4. 创建 Client (此时连接池已就绪)
mysqlx::Client myPool(settings);
```

PS，可以通过`SHOW VARIABLES LIKE 'mysqlx_port';`查看端口

把上述操作再套一层封装在`MySQLPool`类



#### `MySQLDao`类

操作数据库，主要成员

```cpp
int RegUser(const std::string& name, const std::string& email, const std::string& pwd); //执行存储过程

std::unique_ptr<MySQLPool> _pool; 
```



### `MySQLMgr`类

封装`MySQLMgr`单例类作为数据库管理者实现服务层，对接业务逻辑的调用



## MySQL的存储过程

```sql
CREATE DEFINER=`root`@`localhost` PROCEDURE `reg_user`(
    IN `new_name` VARCHAR(255), 
    IN `new_email` VARCHAR(255), 
    IN `new_pwd` VARCHAR(255), 
    OUT `result` INT)
BEGIN
    -- 如果在执行过程中遇到任何错误，则回滚事务
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        -- 回滚事务
        ROLLBACK;
        -- 设置返回值为-1，表示错误
        SET result = -1;
    END;
    
    -- 开始事务
    START TRANSACTION;

    -- 检查用户名是否已存在
    IF EXISTS (SELECT 1 FROM `user` WHERE `name` = new_name) THEN
        SET result = 0; -- 用户名已存在
        COMMIT;
    ELSE
        -- 用户名不存在，检查email是否已存在
        IF EXISTS (SELECT 1 FROM `user` WHERE `email` = new_email) THEN
            SET result = 0; -- email已存在
            COMMIT;
        ELSE
            -- email也不存在，更新user_id表
            UPDATE `user_id` SET `id` = `id` + 1;
            
            -- 获取更新后的id
            SELECT `id` INTO @new_id FROM `user_id`;
            
            -- 在user表中插入新记录
            INSERT INTO `user` (`uid`, `name`, `email`, `pwd`) VALUES (@new_id, new_name, new_email, new_pwd);
            -- 设置result为新插入的uid
            SET result = @new_id; -- 插入成功，返回新的uid
            COMMIT;
        
        END IF;
    END IF;
    
END
```



