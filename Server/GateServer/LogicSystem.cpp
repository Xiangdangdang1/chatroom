#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifygRPCClient.h"

//在实际开发中出现两个类互相包含的情况，不要在头文件互相包含，要在cpp里，在LogicSystem头文件中声明HttpConnection类，在HttpConnection类中声明友元，前置声明解决户引用？

void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
	_get_handlers.insert({ url, handler });
}

void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
	_post_handlers.insert({ url, handler });
}


LogicSystem::LogicSystem()
{
	RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection)
	{
		beast::ostream(connection->_response.body()) << "receive get_test req";
		int i = 0;
		for (auto& elem : connection->_get_params)
		{
			i++;
			beast::ostream(connection->_response.body()) << ", param" << i << "key : " << elem.first;
			beast::ostream(connection->_response.body()) << ", param" << i << "value : " << elem.second << std::endl; 
		}

	});

	//注册一个post请求
	RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection)
	{
		auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
		std::cout << "receive body is " << body_str << std::endl;
		connection->_response.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;	//来源
		bool parse_success = reader.parse(body_str, src_root);
		if (!parse_success)
		{
			std::cout << "Failed to parse Json data!" << std::endl;
			root["error"] = ErrorCodes::Error_Json;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return true;
		}

		//如果key不存在
		if (!src_root.isMember("email"))
		{
			std::cout << "Failed to parse Json data!" << std::endl;
			root["error"] = ErrorCodes::Error_Json;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return true;
		}




		//key存在的情况
		auto email = src_root["email"].asString();

		//验证服务
		GetVarifyRsp rsp = VerifygRPCClient::GetInstance()->GetVarifyCode(email);

		std::cout << "email is " << email << std::endl;
		root["error"] = rsp.error();
		root["email"] = src_root["email"];
		//http是面向字节流的，要转为字符串
		std::string jsonstr = root.toStyledString();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;

	});
}

bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> connection)
{
	if (_get_handlers.find(path) == _get_handlers.end())
		return false;	//没找到返回false，让网络层返回404

	_get_handlers[path](connection);
	return true;
}

bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection> connection)
{
	if (_post_handlers.find(path) == _post_handlers.end())
		return false;	//没找到返回false，让网络层返回404

	_post_handlers[path](connection);
	return true;
}
