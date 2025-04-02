#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

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

private:
    void showTip(QString str, bool b_ok);
    Ui::RegsisterDialog *ui;
};

#endif // REGISTERDIALOG_H
