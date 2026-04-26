#pragma once
#include "const.h"
#include "MySQLDao.h"

class MySQLMgr : public Singleton<MySQLMgr>
{
	friend Singleton<MySQLMgr>;
public:
    ~MySQLMgr();
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
private:
    MySQLMgr();
    MySQLDao _dao;
};