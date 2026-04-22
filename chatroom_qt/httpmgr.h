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
class HttpMgr : public QObject, public Singleton<HttpMgr>, public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT

public:
    ~HttpMgr();
    //POST请求，要带json
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);

private:
    friend class Singleton<HttpMgr>;
    HttpMgr();  //单例的构造函数不能是public
    QNetworkAccessManager _manager; //QT原生的http管理者



private slots:
    void slot_http_finish(ReqId req_id, QString res, ErrorCodes err, Modules mod);  //要与信号的参数匹配

signals:
    void sig_http_finish(ReqId req_id, QString res, ErrorCodes err, Modules mod); //http发送之后，发送信号给其他模块
    void sig_reg_mod_finish(ReqId req_id, QString res, ErrorCodes err);
};

#endif // HTTPMGR_H
