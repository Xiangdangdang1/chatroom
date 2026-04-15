#pragma once
#pragma warning(push)
#pragma warning(disable:4996)
#include <grpcpp/grpcpp.h>
#pragma warning(pop)
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"


using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;


//新增stub池
class RPConPool
{
public:
	RPConPool(size_t poolsize, std::string host, std::string port) :
		_poolsize(poolsize), _host(host), _port(port), _b_stop(false)
	{
		//为每一个连接创建一个stub,并放入连接池中
		for (size_t i = 0; i < _poolsize; ++i)
		{
			std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
				grpc::InsecureChannelCredentials());
			std::unique_ptr<VerifyService::Stub> stub = VerifyService::NewStub(channel);
			_connections.push(std::move(stub));
		}
	}

	//关于锁？
	~RPConPool()
	{
		//加锁
		std::lock_guard<std::mutex> lock(_mutex);
		Close();
		//清空连接池
		while (!_connections.empty())
		{
			_connections.pop();
		}
	}

	//?
	void Close()
	{
		_b_stop = true;
		_condi.notify_all();
	}

	//从连接池中获取一个stub
	std::unique_ptr<VerifyService::Stub> getConnection()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_condi.wait(lock, [this] { 
			if (_b_stop)	return true;
			return !_connections.empty();
		});
		if (_b_stop)	return nullptr;
		std::unique_ptr<VerifyService::Stub> stub = std::move(_connections.front());
		_connections.pop();
		return stub;
	}

	void returnConnection(std::unique_ptr<VerifyService::Stub> stub)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (_b_stop)	return;
		_connections.push(std::move(stub));
		_condi.notify_one();
	}

private:
	std::atomic<bool> _b_stop;	//停止标记
	size_t _poolsize;
	std::string _host;
	std::string _port;
	std::queue<std::unique_ptr<VerifyService::Stub>> _connections;
	std::condition_variable _condi;
	std::mutex _mutex;
};



class VerifygRPCClient : public Singleton<VerifygRPCClient>
{
	friend class Singleton<VerifygRPCClient>;
public:
	GetVerifyRsp GetVerifyCode(std::string email);

private:

	std::unique_ptr<RPConPool> _pool;

	VerifygRPCClient();
};

