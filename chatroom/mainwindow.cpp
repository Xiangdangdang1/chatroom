#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置父窗口
    _login_dlg = new LoginDialog(this); //登录界面初始化
    setCentralWidget(_login_dlg);   //登录界面设置到主界面中，主界面加载LoginDialog，如果_login_dlg没有边框，自然嵌入主窗口


    // 登录和注册消息链接，LoginDialog有一个信号，mainwindow收到信号之后，执行切换的操作，mainwindow的slot执行该操作，切换到注册界面
    connect(_login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);


    _reg_dlg = new RegsisterDialog(this);   //注册界面初始化

    //把注册、登录的窗口的边框，菜单隐藏
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);\
    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);\
    _reg_dlg->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
//     if (_login_dlg) {
//         delete _login_dlg;
//         _login_dlg = nullptr;
//     }

//     if (_reg_dlg) {
//         delete _reg_dlg;
//         _reg_dlg = nullptr;
//     }
}


void MainWindow::SlotSwitchReg()
{
    setCentralWidget(_reg_dlg);
    _login_dlg->hide();
    _reg_dlg->show();
}
