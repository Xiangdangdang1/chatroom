#pragma once
#include "const.h"
#include "Singleton.h"

class HttpConnection;
typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;

class LogicSystem : public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem() {}
	bool HandleGet(std::string, std::shared_ptr<HttpConnection>);	//处理get请求
	bool HandlePost(std::string, std::shared_ptr<HttpConnection>);
	void RegGet(std::string, HttpHandler handler);		//注册get请求
	void RegPost(std::string, HttpHandler handler);

private:
	LogicSystem();
	std::unordered_map<std::string, HttpHandler> _post_handlers;
	std::unordered_map<std::string, HttpHandler> _get_handlers;
};

