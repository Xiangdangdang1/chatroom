#ifndef SINGLETON_H
#define SINGLETON_H
#include "global.h"

template <typename T>
class Singleton {
protected:
    Singleton() = default;  //单例类的构造函数不允许外部访问，子类继承父类时，子类能构造基类，所以设置为protected
    Singleton(const Singleton<T>&) = delete;    //不允许拷贝构造
    Singleton& operator= (const Singleton<T>& st) = delete;    //不允许拷贝赋值

    // static std::shared_ptr<T> _instance;

public:

    //现代C++的写法
    static std::shared_ptr<T> GetInstance() {
        static std::shared_ptr<T> instance(new T());
        return instance;
    }

    // //在C++11之后，static局部变量是线程安全的，可以使用以下写法，不用智能指针的写法
    // static T& GetInstance() {
    //     static T instance;
    //     return instance;
    // }

    //打印指针的地址
    void PrintAddress()
    {
        std::cout << Singleton::GetInstance().get() << std::endl;
    }

    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    }
};


#endif // SINGLETON_H
