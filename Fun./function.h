#ifndef FUNCTION_H
#define FUNCTION_H

#include <QString>
#include <QWidget>
#include <QFile>
#include <QJsonObject>
#include "../Client/patient_client.h"
#include "../Client/doctor_client.h"
#include "../Client/admin_client.h"  // 根据需要包含相关客户端类
#include"../Instance/UserSession.h"

class Function {
public:
    // 构造函数
    Function();

    // 负责窗口跳转的函数
    static void navigateToClientWindow(const QString &type, QWidget *currentWindow);

    // 处理用户缓存文件的函数
    static void handleUserCache(const QString &type, const QString &username, const QJsonObject &jsonResponse);

    //Combo box中英转换
    static QString switchChinese(QString type);
};

#endif // FUNCTION_H
