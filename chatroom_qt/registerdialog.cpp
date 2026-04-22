#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmgr.h"

RegsisterDialog::RegsisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegsisterDialog)
{
    ui->setupUi(this);
    // 设置密码显示***
    ui->pass_editor->setEchoMode(QLineEdit::Password);
    ui->confirm_editor->setEchoMode(QLineEdit::Password);
    ui->err_tip->setProperty("state", "normal");  //设置状态，对应qss
    repolish(ui->err_tip);

    //httpmgr发出来的是注册信号，告诉注册模块请求完成
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegsisterDialog::slot_reg_mod_finish);
    initHttpHandlers();
}

RegsisterDialog::~RegsisterDialog()
{
    delete ui;
}

//获取验证码的click事件
void RegsisterDialog::on_get_code_clicked()
{
    auto email = ui->email_editor->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(match){
        //发送http请求获取验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_verifycode"),
                                            json_obj, ReqId::ID_GET_VARIFY_CODE, Modules::REGISTERMOD);
    }else{
        //提示邮箱不正确
        showTip(tr("邮箱地址不正确"), false);
    }
}


void RegsisterDialog::slot_reg_mod_finish(ReqId req_id, QString res, ErrorCodes err)
{
    if (err != ErrorCodes::SUCCESS)
    {
        showTip(tr("网络请求错误"), false);
        return;
    }
    //解析JSON字符串, res -> QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());  //转为JSON文件
    if (jsonDoc.isNull())
    {
        showTip(tr("json解析失败"), false);
        return;
    }

    //JSON错误
    if (!jsonDoc.isObject())
    {
        showTip(tr("json解析失败"), false);
        return;
    }

    //成功
    _handlers[req_id](jsonDoc.object());     //jsonDoc.object()是得到的json对象
    return;

}

void RegsisterDialog::showTip(QString str, bool b_ok)
{
    if (b_ok) {
        ui->err_tip->setProperty("state", "normal");
    } else {
        ui->err_tip->setProperty("state", "err");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

void RegsisterDialog::initHttpHandlers()
{
    //注册获取验证码
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS)
        {
            showTip(tr("参数错误"), false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已经发送到邮箱注意查收"), true);
        qDebug() << "email is :" << email;
    });
}


