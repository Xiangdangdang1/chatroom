#include "MySQLMgr.h"

MySQLMgr::~MySQLMgr() {}

MySQLMgr::MySQLMgr() {}

int MySQLMgr::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
	return _dao.RegUser(name, email, pwd);
}