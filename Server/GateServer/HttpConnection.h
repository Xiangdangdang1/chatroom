#pragma once
#include "const.h"

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
public:
	friend class LogicSystem;
	HttpConnection(boost::asio::io_context& ioc);
	void Start();	//监听读写事件

	tcp::socket& GetSocket() { return _socket; }	//因为socket不允许被拷贝，获取socket引用，供LogicSystem使用

private:
	tcp::socket _socket;
	//超时检测
	void CheckDeadLine();
	//应答
	void WriteResponse();
	//处理请求
	void HandleReq();	
	beast::flat_buffer _buffer{ 8192 };	//8k大小的buffer接收数据
	http::request<http::dynamic_body> _request;		
	http::response<http::dynamic_body> _response;		
	net::steady_timer _deadline		//定时器，60s
	{
		//初始化列表
		_socket.get_executor(), std::chrono::seconds(60)
	};

	void PreParseGetParam();

	std::string _get_url;
	std::unordered_map<std::string, std::string> _get_params;
};

