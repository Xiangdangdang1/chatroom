#include "VerifygRPCClient.h"
#include "ConfigMgr.h"

GetVerifyRsp VerifygRPCClient::GetVerifyCode(std::string email)
{
	ClientContext context;
	GetVerifyRsp reply;
	GetVerifyReq request;
	request.set_email(email);

	auto stub = _pool->getConnection();

	Status status = stub->GetVerifyCode(&context, request, &reply);
	if (status.ok())
	{
		_pool->returnConnection(std::move(stub));	//回收stub
		return reply;
	}
	else
	{
		_pool->returnConnection(std::move(stub));
		reply.set_error(ErrorCodes::RPCFailed);
		return reply;
	}
}


VerifygRPCClient::VerifygRPCClient()
{
	ConfigMgr& gCfgMgr = ConfigMgr::GetInstance();
	std::string host = gCfgMgr["VerifyServer"]["Host"];
	std::string port = gCfgMgr["VerifyServer"]["Port"];
	_pool.reset(new RPConPool(5, host, port));	//连接池大小为5
}