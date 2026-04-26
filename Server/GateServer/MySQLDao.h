#pragma once
#include "const.h"
#include <thread>
#include "mysqlx/xdevapi.h"


class MySQLPool {
public:
    // mysqlx 官方推荐使用 Client 来管理连接池
    MySQLPool(const std::string& host, int port, const std::string& user, const std::string& pass, const std::string& schema, int poolSize)
        : _b_stop(false) {
        try {
            // 构建连接字符串，例如: mysqlx://user:pass@host:port/schema
            std::string connStr = "mysqlx://" + user + ":" + pass + "@" + host + ":" + std::to_string(port) + "/" + schema;
            // 配置连接池选项
            // ClientSettings 允许你直接定义连接池大小 (MaxSize)
            mysqlx::ClientSettings settings(connStr);
            settings.set(mysqlx::ClientOption::POOL_MAX_SIZE, poolSize);
            settings.set(mysqlx::ClientOption::POOL_QUEUE_TIMEOUT, 1000); // 等待空闲连接的超时时间(ms)

            // 初始化 Client，它会自动创建并管理连接池
            _client = std::make_unique<mysqlx::Client>(settings);

            std::cout << "MySQL X DevAPI Client (Pool) 初始化成功" << std::endl;
        }
        catch (const mysqlx::Error& e) {
            std::cerr << "MySQL 客户端初始化失败: " << e.what() << std::endl;
        }
    }

    // 获取会话（Session）
    // mysqlx::Session 是轻量级的，从 client 获取时会自动从池中取出
    mysqlx::Session getSession() {
        if (_b_stop) throw std::runtime_error("连接池已关闭");

        // getSession 会自动管理从池中借出连接的过程
        return _client->getSession();
    }

    void Close() {
        _b_stop = true;
        // Client 析构时会自动关闭池中所有连接
    }

    ~MySQLPool() = default;

private:
    std::unique_ptr<mysqlx::Client> _client;
    std::atomic<bool> _b_stop;
};


class MySQLDao
{
public:
    MySQLDao();
    ~MySQLDao();
    /**
     * @brief 注册用户
     * @return int 存储过程返回的结果（例如：0成功，-1失败，1用户已存在等）
     */
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);

private:
    std::unique_ptr<MySQLPool> _pool;
};