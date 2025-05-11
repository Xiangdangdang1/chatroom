#ifndef SINGLETON_H
#define SINGLETON_H
#include "global.h"

template <typename T>

class Singleton {
protected:
    Singleton() = default;  //单例类的构造函数不允许外部访问，子类继承父类时，子类能构造基类，所以设置为protected
    Singleton(const Singleton<T>&) = delete;    //不允许拷贝构造
    Singleton& operator = (const Singleton<T>& st) = delete;    //不允许拷贝赋值
    static std::shared_ptr<T> _instance;

public:
    //在C++11之后，static局部变量是线程安全的，可以使用以下写法
    static std::shared_ptr<T> GetInstance(){
        // static std::once_flag s_flag;
        // std::call_once(s_flag, [&]() {
        //     _instance = std::shared_ptr<T>(new T);
        // });
        _instance = std::shared_ptr<T>(new T);
        return _instance;
    }

    void PrintAddress() {
        //打印智能指针的地址
        std::cout << _instance.get() << std::endl;
    }

    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    }
};
//如果不写下面的部分，直接使用Singleton::GetInstance()，编译可以通过，链接出错。因为类的static变量必须被初始化，如果该变量不是模版类，初始化要在CPP中，如果是模板类，放在.h
template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;

#endif // SINGLETON_H
