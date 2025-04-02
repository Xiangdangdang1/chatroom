#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"

RegsisterDialog::RegsisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegsisterDialog)
{
    ui->setupUi(this);
    // 设置密码
    ui->pass_editor->setEchoMode(QLineEdit::Password);
    ui->confirm_editor->setEchoMode(QLineEdit::Password);
    ui->err_tip->setProperty("state", "normal");  //设置状态
    repolish(ui->err_tip);
}

RegsisterDialog::~RegsisterDialog()
{
    delete ui;
}

void RegsisterDialog::on_get_code_clicked()
{
    auto email = ui->email_editor->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(match){
        //发送http请求获取验证码
    }else{
        //提示邮箱不正确
        showTip(tr("邮箱地址不正确"), false);
    }
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

