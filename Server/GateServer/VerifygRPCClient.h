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

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;


class VerifygRPCClient : public Singleton<VerifygRPCClient>
{
	friend class Singleton<VerifygRPCClient>;
public:
	GetVarifyRsp GetVarifyCode(std::string email)
	{
		ClientContext context;
		GetVarifyRsp reply;
		GetVarifyReq request;
		request.set_email(email);

		Status status = _stub->GetVarifyCode(&context, request, &reply);
		if (status.ok())	return reply;
		else reply.set_error(ErrorCodes::RPCFailed);
	}

private:
	VerifygRPCClient() 
	{
		std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:50051",
			grpc::InsecureChannelCredentials());	//智能指针生成的channel, grpc通过channel与服务器通信
		_stub = VarifyService::NewStub(channel);
	}
	std::unique_ptr<VarifyService::Stub> _stub;	//只有通过stub才能通信
};

