#include "httpmgr.h"

HttpMgr::~HttpMgr()
{

}



HttpMgr::HttpMgr()
{
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}


void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    //将JSON序列化为字节数组，底层都是字节流
    QByteArray data = QJsonDocument(json).toJson();
    //构造url
    QNetworkRequest request(url);
    //设置请求头部
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");  //设置请求头，告诉对方发送的是json
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));  //设置请求长度
    auto self = shared_from_this();
    QNetworkReply *reply = _manager.post(request, data);    //发送post请求
    //收到reply信号之后
    QObject::connect(reply, &QNetworkReply::finished, [self, reply, req_id, mod](){
        //处理错误情况
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << reply->errorString();
            //发送信号通知完成
            emit self->sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();
            return;
        }

        //无错误
        QString res = reply->readAll();
        //发送信号通知完成
        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater();
        return;
    });
}

void HttpMgr::slot_http_finish(ReqId req_id, QString res, ErrorCodes err, Modules mod)
{
    //如果是注册模块
    if (mod == Modules::REGISTERMOD)
    {
        //发送信号通知指定模块的http的响应结束了
        emit sig_reg_mod_finish(req_id, res, err);
    }
}






