#pragma once
#include <memory>
#include <iostream>

template <typename T>
class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;

public:
    static std::shared_ptr<T> GetInstance() {
        // C++11 保证静态局部变量初始化是线程安全的
        static std::shared_ptr<T> instance(new T());
        return instance;
    }

    void PrintAddress() const {
        std::cout << GetInstance().get() << std::endl;
    }

    virtual ~Singleton() {
        std::cout << "this is singleton destruct\n";
    }
};