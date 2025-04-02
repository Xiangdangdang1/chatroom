#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <functional>
#include <QRegularExpression>
#include "QStyle"

/**
 * @brief 刷新qss，加上extern因为要在多个文件中引用头文件，避免多次定义
 */
extern std::function<void(QWidget*)> repolish;



#endif // GLOBAL_H
