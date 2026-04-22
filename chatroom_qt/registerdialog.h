#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class RegsisterDialog;
}

class RegsisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegsisterDialog(QWidget *parent = nullptr);
    ~RegsisterDialog();

private slots:
    void on_get_code_clicked();
    //参数要与httpmgr发过来的信号参数匹配
    void slot_reg_mod_finish(ReqId req_id, QString res, ErrorCodes err);



private:
    void showTip(QString str, bool b_ok);
    Ui::RegsisterDialog *ui;
    void initHttpHandlers();    //初始化http处理器，不同请求通过ID区分

    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers; //key是请求Id, value是用于处理的函数对象
};

#endif // REGISTERDIALOG_H
