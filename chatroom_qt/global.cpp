#include "global.h"

QString gate_url_prefix = "";


// lambda表达式，后面的函数对象赋值给repolish
std::function<void(QWidget*)> repolish = [](QWidget* w){
    //去掉原来的样式重新刷新
    w->style()->unpolish(w);
    w->style()->polish(w);
};
