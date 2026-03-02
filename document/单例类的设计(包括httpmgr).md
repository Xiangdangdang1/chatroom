### 实现单例模板类



`Singleton.h`

```cpp
#ifndef SINGLETON_H
#define SINGLETON_H
#include "global.h"

template <typename T>

class Singleton {
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;    //不允许拷贝构造
    Singleton& operator = (const Singleton<T>& st) = delete;    //不允许拷贝赋值
    static std::shared_ptr<T> _instance;

public:
    //在C++11之后，static局部变量是线程安全的
    static std::shared_ptr<T> GetInstance(){
        static std::once_flag s_flag;
        std::call_once(s_flag, [&]() {
            _instance = std::shared_ptr<T>(new T);
        });
    }

    void PrintAddress() {
        //打印智能指针的地址
        std::cout << _instance.get() << std::endl;
    }

    ~Singleton() {
        std::cout << "this is singleton destruct" << std::endl;
    }
};

template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;

#endif // SINGLETON_H

```

关于使用19行使用`new T`而不是`make share`的原因：

`std::make_shared<T>` 需要一个有效的构造函数，而 `Singleton`的构造函数是`protected`。这意味着它不能直接通过 `std::make_shared` 来创建实例，因为 `std::make_shared`会尝试直接调用 `T` 的构造函数，而这个构造函数不能被外部代码访问

- 为什么使用`protected`，子类可以调用父类的构造函数

  ```cpp
  class ConfigManager : public Singleton<ConfigManager>
  {
      friend class Singleton<ConfigManager>;
  private:
      ConfigManager() {}
  };
  ```

- **从C++11开始，局部的`static`变量是线程安全的，如果多个线程同时进入函数，只有一个线程会初始化，其他线程等待**

- 继承单例的子类析构要设置为`public`

  单例中的`static T instance`是静态的，在程序结束后执行析构，先调用子类析构再调用父类析构。如果析构不是`public`编译器无法访问析构

- 为什么要声明友元

  当第一次执行`Httpmgr::GetInstance()`的时候，执行到`static HttpMgr instance;`

  - 分配静态存储区内存
  - 调用`Httpmgr()`构造函数，此时父类`Singleton`需要访问子类`private`的构造函数，所以在子类中声明父类是`friend`
  - 构造完成之后返回引用

  
