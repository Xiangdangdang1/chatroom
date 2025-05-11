#include "httpmgr.h"

HttpMgr::~HttpMgr()
{

}

HttpMgr::HttpMgr()
{

}

void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    //将JSON序列化为字节数组
    QByteArray data = QJsonDocument(json).toJson();
    //构造url
    QNetworkRequest request(url);
    //设置请求头部
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");  //设置请求类型
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));  //设置请求长度
    auto self = shared_from_this();
    QNetworkReply *reply = _manager.post(request, data);    //利用post
    QObject::connect(reply, &QNetworkReply::finished, [](){

    });
}
