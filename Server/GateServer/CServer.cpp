#include "CServer.h"
#include "HttpConnection.h"
#include "AsioIOServicePool.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& port) //类的成员是引用类型，在初始化列表初始化
	: _ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port)) {}

//监听连接
void CServer::Start()
{
	//防止一些回调还没处理的情况下，类被析构的情况
	auto self = shared_from_this();

	//从连接池获取ioc
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	//创建连接，传给HttpConnection的构造函数，生成一个新的socket连接，HttpConnection类管理这个连接
	std::shared_ptr<HttpConnection> new_connection = std::make_shared<HttpConnection>(io_context);

	_acceptor.async_accept(new_connection->GetSocket(), [self, new_connection](beast::error_code ec)
	{
		try
		{
			//出错则放弃socket连接，继续监听其他连接
			if (ec)
			{
				self->Start();
				return;
			}

			//创建新连接，创建HttpConnection类管理这个连接
			//std::make_shared<HttpConnection>(std::move(self->_socket))->Start();
			//上面17行创建了连接，这里启动就可以
			new_connection->Start();

			//继续监听
			self->Start();

		}
		catch (std::exception& exp)
		{
			std::cout << "exception is " << exp.what() << std::endl;
			self->Start();
		}
	});

}


