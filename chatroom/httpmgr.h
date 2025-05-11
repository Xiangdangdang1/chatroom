#ifndef HTTPMGR_H
#define HTTPMGR_H

#include "singleton.h"
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>    //网络管理
#include <QJsonObject>
#include <QJsonDocument>

//CRTP，模版类是自身
class HttpMgr:public QObject, public Singleton<HttpMgr>, public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
public:
    /*
     * 子类HttpMgr需要首先构造基类，基类的构造是protected，合理；
     * 但是Singleton返回的实例_instance是T类型，C++在执行析构会回收成员变量，在回收_instance会调用_instance的析构，是智能指针的析构，
     * 此时T类型智能指针就是HttpMgr的析构，所以需要public
     *
     */
    ~HttpMgr();

private:
    //声明基类是子类的友元，因为在Singleton中会调用new T，T是HttpMgr，在Singlton中调用HttpMgr的构造需要声明友元，此时基类可以访问子类的构造函数
    friend class Singleton<HttpMgr>;
    HttpMgr();  //单例的构造函数不能是public
    QNetworkAccessManager _manager; //QT原生的http管理者
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);
signals:
    void sig_http_finish(ReqId req_id, QString res, ErrorCodes err, Modules mod); //http发送之后，发送信号给其他模块
};

#endif // HTTPMGR_H
