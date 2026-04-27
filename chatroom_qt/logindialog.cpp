#include "logindialog.h"
#include "ui_logindialog.h"
#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->pass_editor->setEchoMode(QLineEdit::Password);
    //连接按钮事件，点击reg_btn，发出click信号，LoginDialog接收信号，同时发送信号switchRegister给mainwindow，mainwindow收到信号调用槽函数SlotSwitchReg
    connect(ui->reg_btn, &QPushButton::clicked, this, &LoginDialog::switchRegister);
}

LoginDialog::~LoginDialog()
{
    qDebug() << "destruct LoginDlg";
    delete ui;
}
