#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmgr.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog), _countdown(5)
{
    ui->setupUi(this);
    // 设置密码显示***
    ui->pass_editor->setEchoMode(QLineEdit::Password);
    ui->confirm_editor->setEchoMode(QLineEdit::Password);
    ui->err_tip->setProperty("state", "normal");  //设置状态，对应qss
    repolish(ui->err_tip);

    //设置密码可见
    setupPasswordToggle(ui->pass_editor);
    setupPasswordToggle(ui->confirm_editor);


    //httpmgr发出来的是注册信号，告诉注册模块请求完成
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);
    initHttpHandlers();


    //设定输入框输入后清空字符串
    ui->err_tip->clear();

    connect(ui->user_editor,&QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });

    connect(ui->email_editor, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });

    connect(ui->pass_editor, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });

    connect(ui->confirm_editor, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });

    connect(ui->verify_editor, &QLineEdit::editingFinished, this, [this](){
        checkVerifyValid();
    });



    //创建计时器
    _countdown_timer = new QTimer(this);
    connect(_countdown_timer, &QTimer::timeout, [this](){
        if (_countdown == 0) {
            _countdown_timer->stop();
            emit sigSwitchLogin();
            return;
        }
        _countdown--;
        auto str = QString("注册成功，%1 s后返回登录").arg(_countdown);
        ui->tip_label->setText(str);
    });



}

RegisterDialog::~RegisterDialog()
{
    qDebug() << "destruct RegDlg";
    delete ui;
}

//获取验证码的click事件
void RegisterDialog::on_get_code_clicked()
{
    auto email = ui->email_editor->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(match){
        //发送http请求获取验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_verifycode"),
                                            json_obj, ReqId::ID_GET_VERIFY_CODE, Modules::REGISTERMOD);
    }else{
        //提示邮箱不正确
        showTip(tr("邮箱地址不正确"), false);
    }
}


void RegisterDialog::slot_reg_mod_finish(ReqId req_id, QString res, ErrorCodes err)
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

void RegisterDialog::showTip(QString str, bool b_ok)
{
    if (b_ok) {
        ui->err_tip->setProperty("state", "normal");
    } else {
        ui->err_tip->setProperty("state", "err");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

void RegisterDialog::initHttpHandlers()
{
    //注册获取验证码
    _handlers.insert(ReqId::ID_GET_VERIFY_CODE, [this](const QJsonObject& jsonObj){
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

    //注册用户逻辑
    _handlers.insert(ReqId::ID_REG_USER, [this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        if (error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"), false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("用户注册成功"), true);
        qDebug() << "user uuid is " << jsonObj["uuid"].toString();
        qDebug() << "email is " << email;
        //注册成功切换页面
        ChangeTipPage();
    });
}


//参数分别是错误类型和提示话语，以map显示
void RegisterDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void RegisterDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
        ui->err_tip->clear();
        return;
    }

    showTip(_tip_errs.first(), false);
}

bool RegisterDialog::checkUserValid()
{
    if(ui->user_editor->text() == ""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool RegisterDialog::checkPassValid()
{
    auto pass = ui->pass_editor->text();
    auto confirm = ui->confirm_editor->text();

    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    if (pass != confirm) {
        //密码不匹配
        AddTipErr(TipErr::TIP_PWD_CONFIRM, tr("密码和确认密码不匹配"));
        return false;
    } else {
        DelTipErr(TipErr::TIP_PWD_CONFIRM);
    }

    return true;
}

bool RegisterDialog::checkEmailValid()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_editor->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }

    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool RegisterDialog::checkVerifyValid()
{
    auto pass = ui->verify_editor->text();
    if(pass.isEmpty()){
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }

    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

bool RegisterDialog::checkConfirmValid()
{
    auto pass = ui->pass_editor->text();
    auto confirm = ui->confirm_editor->text();

    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }

    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;
    }

    DelTipErr(TipErr::TIP_PWD_ERR);

    if (pass != confirm) {
        //密码不匹配
        AddTipErr(TipErr::TIP_PWD_CONFIRM, tr("密码和确认密码不匹配"));
        return false;
    } else {
        DelTipErr(TipErr::TIP_PWD_CONFIRM);
    }

    return true;
}

void RegisterDialog::setupPasswordToggle(QLineEdit *edit)
{
    QAction *act = new QAction(edit);
    act->setCheckable(true);

    bool visible = false;
    edit->setEchoMode(QLineEdit::Password);

    act->setChecked(visible);
    act->setIcon(QIcon(":/img/eye_off.png")); // 统一：闭眼=隐藏

    edit->addAction(act, QLineEdit::TrailingPosition);

    QObject::connect(act, &QAction::toggled, edit,
                     [=](bool checked) {

                         if (checked) {
                             // 显示密码
                             edit->setEchoMode(QLineEdit::Normal);
                             act->setIcon(QIcon(":/img/eye.png"));
                         } else {
                             // 隐藏密码
                             edit->setEchoMode(QLineEdit::Password);
                             act->setIcon(QIcon(":/img/eye_off.png"));
                         }
                     });
}

void RegisterDialog::ChangeTipPage()
{
    _countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);

    //启动定时器
    _countdown_timer->start(1000);
}

void RegisterDialog::on_confirm_btn_clicked()
{

    //day11 发送http请求注册用户
    QJsonObject json_obj;
    json_obj["user"] = ui->user_editor->text();
    json_obj["email"] = ui->email_editor->text();
    json_obj["passwd"] = xorString(ui->pass_editor->text());
    json_obj["confirm"] = xorString(ui->confirm_editor->text());
    json_obj["verifycode"] = ui->verify_editor->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/user_register"),
                                        json_obj, ReqId::ID_REG_USER,Modules::REGISTERMOD);
}

void RegisterDialog::on_return_btn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}

