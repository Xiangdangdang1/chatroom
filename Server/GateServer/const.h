#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include "Singleton.h"
#include <functional>
#include <unordered_map>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <map>
#include <boost/filesystem.hpp>	//用于跨平台的读文件
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>	//ini解析器
#include "hiredis/hiredis.h"
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cassert>




namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>


//前缀
#define CODEPREFIX "code_"

enum ErrorCodes
{
	Success = 0,
	Error_Json = 1001,	//JSON解析错误
	RPCFailed = 1002,	//RPC请求错误
	VerifyExpired = 1003,	//验证码过期
	VerifyCodeErr = 1004,	//验证码错误
	UserExist = 1005,	//用户已存在
	PasswdErr = 1006,	//密码错误
	EmailNotMatch = 1007,	//邮箱不匹配
	PasswdUpFailed = 1008,	//密码修改失败
	PasswdInvalid = 1009,	//密码不合法


};
