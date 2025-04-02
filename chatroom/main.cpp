#include "mainwindow.h"
#include <QFile>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 加载qss
    QFile qss(":/style/stylesheet.qss");
    if (qss.open(QFile::ReadOnly)) {
        qDebug("open success");
        QString style = QLatin1String(qss.readAll());      //QArray转为QString
        a.setStyleSheet(style);
        qss.close();
    } else {
        qDebug("open failed");
    }
    MainWindow w;
    w.show();
    return a.exec();
}
