#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "global.h"
#include "qlineedit.h"

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void on_get_code_clicked();
    //参数要与httpmgr发过来的信号参数匹配
    void slot_reg_mod_finish(ReqId req_id, QString res, ErrorCodes err);



    void on_confirm_btn_clicked();

    void on_return_btn_clicked();

private:
    void showTip(QString str, bool b_ok);
    Ui::RegisterDialog *ui;
    void initHttpHandlers();    //初始化http处理器，不同请求通过ID区分

    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers; //key是请求Id, value是用于处理的函数对象


    QMap<TipErr, QString> _tip_errs;
    void AddTipErr(TipErr te, QString tips);
    void DelTipErr(TipErr te);

    //一系列错误验证
    bool checkUserValid();
    bool checkPassValid();
    bool checkEmailValid();
    bool checkVerifyValid();
    bool checkConfirmValid();

    //密码显示隐藏
    void setupPasswordToggle(QLineEdit *edit);

    //用于注册成功之后返回登录界面的计时器
    QTimer *_countdown_timer;
    int _countdown;

    void ChangeTipPage();


signals:
    void sigSwitchLogin();


};

#endif // REGISTERDIALOG_H
