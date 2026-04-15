#pragma once
#include <vector>
#include <boost/asio.hpp>
#include "Singleton.h"
class AsioIOServicePool :public Singleton<AsioIOServicePool>
{
    friend Singleton<AsioIOServicePool>;
public:
	using IOService = boost::asio::io_context;  //老版本叫ioservice，asio 1.66版本后改名为io_context
    //using Work = boost::asio::io_context::work;
    //新版work弃用，使用executor_work_guard来保持io_service的运行状态
	//work的作用是：当io_service没有任何事件时，run函数会立即返回，work的存在可以让run函数一直阻塞等待事件的发生
	using Work = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
    using WorkPtr = std::unique_ptr<Work>;

    // 使用 round-robin 的方式返回一个 io_service
    boost::asio::io_context& GetIOService();
    void Stop();

    ~AsioIOServicePool();
    AsioIOServicePool(const AsioIOServicePool&) = delete;
    AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;
private:
    AsioIOServicePool(std::size_t size = 2);
    std::vector<IOService> _ioServices;
    std::vector<WorkPtr> _works;
    std::vector<std::thread> _threads;
    std::size_t _nextIOService;
};