#include "global.h"

QString gate_url_prefix = "";


// lambda表达式，后面的函数对象赋值给repolish
std::function<void(QWidget*)> repolish = [](QWidget* w){
    //去掉原来的样式重新刷新
    w->style()->unpolish(w);
    w->style()->polish(w);
};


std::function<QString(QString)> xorString = [](QString input) {
    QString res = input;
    int length = input.size();
    length = length % 255;
    for (int i = 0; i < length; i++) {
        //假设字符都是ASCII，转为QChar
        res[i] = QChar (static_cast<ushort>(input[i].unicode() ^ static_cast<ushort>(length)));
    }
    return res;
};
