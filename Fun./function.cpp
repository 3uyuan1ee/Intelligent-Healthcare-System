
#include "function.h"
#include <QDebug>

Function::Function() {}

//登录成功后窗口跳转
void Function::navigateToClientWindow(const QString &type, QWidget *currentWindow) {
    if (type == "patient") {
        Patient_Client *patient_client = new Patient_Client; //不能在栈上创建对象，一定要在堆空间上创建对象
        patient_client->show();
        currentWindow->hide();  // 当前窗口隐藏
    } else if (type == "doctor") {
        Doctor_Client *doctor_client = new Doctor_Client;
        doctor_client->show();
        currentWindow->hide();
    } else if (type == "admin") {
        Admin_Client *admin_client = new Admin_Client;
        admin_client->show();
        currentWindow->hide();
    } else {
        qWarning() << "Unknown user type: " << type;
    }
}


void Function::handleUserCache(const QString &type, const QString &username, const QJsonObject &jsonResponse) {

    QString filename = UserSession::instance().getCacheFilePath(type, username);

    qDebug()<<"handleUserCache:正在处理缓存...";

    // 判断文件是否存在且能够加载到内存
    if (QFile::exists(filename) && UserSession::instance().loadUserInfoFromLocal(filename)) {
        qDebug() << "handleUserCache:文件存在:" << filename;
    } else {
        qDebug() << "handleUserCache:文件不存在或加载失败，缓存新数据:" << filename;

        // 缓存个人信息到内存
        UserSession::instance().setUserInfo(jsonResponse, type, username);

        // 保存个人信息到本地文件
        // UserSession::instance().saveUserInfoToLocal();
        // 已集成到setUserInfo中

        qDebug() << "handleUserCache:setUserInfo完成";
    }
}

//Combo box中英转换
QString Function::switchChinese(QString type){

    if(type=="患者"){
        return "patient";
    }else if(type=="医生"){
        return "doctor";
    }else if(type=="管理员"){
        return "admin";
    }

    return type;
}
