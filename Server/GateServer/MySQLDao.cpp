#include "MySQLDao.h"
#include <mysqlx/xdevapi.h>
#include "ConfigMgr.h"

// 确保包含 MySQLDao.h 的头文件中有 MySQLDao 的完整类定义
// 并且 MySQLDao.h 头文件没有被循环引用或遗漏 include guard

MySQLDao::MySQLDao()
{
    auto& cfg = ConfigMgr::GetInstance();
    const auto& host = cfg["MySQL"]["Host"];
    int port = std::stoi(cfg["MySQL"]["Port"]);
    const auto& pwd = cfg["MySQL"]["Passwd"];
    const auto& schema = cfg["MySQL"]["Schema"];
    const auto& user = cfg["MySQL"]["User"];
    _pool = std::make_unique<MySQLPool>(host, port, user, pwd, schema, 5);
}

MySQLDao::~MySQLDao()
{
    _pool->Close();
}

int MySQLDao::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
    try {
        // 从池中获取 Session (sess 超出作用域会自动归还连接)
        mysqlx::Session sess = _pool->getSession();
        // 1. 执行存储过程
        // mysqlx 直接通过 .sql().bind() 处理参数，非常简洁
        sess.sql("CALL reg_user(?, ?, ?, @result)")
            .bind(name)
            .bind(email)
            .bind(pwd)
            .execute();

        // 2. 获取输出参数 @result
        auto res = sess.sql("SELECT @result AS result").execute();

        // fetchOne() 返回一行数据
        mysqlx::Row row = res.fetchOne();
        if (row) {
            // row[0] 对应 SELECT 的第一个字段，转换为 int
            int result = static_cast<int>(row[0]);
            std::cout << "Result: " << result << std::endl;
            return result;
        }

        return -1;
    }
    catch (const mysqlx::Error& e) {
        // 异常处理：mysqlx 会自动清理资源，无需手动 returnConnection
        std::cerr << "MySQL Error: " << e.what() << std::endl;
        return -1;
    }
}