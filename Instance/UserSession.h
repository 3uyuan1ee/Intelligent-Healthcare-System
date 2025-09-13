#ifndef USERSESSION_H
#define USERSESSION_H

#include <QString>
#include <QJsonObject>
#include<QFile>
#include <QDir>
#include<QThread>

class UserSession {
public:
    static UserSession& instance() {
        static UserSession instance;
        return instance;
    }

    void setUserInfo(const QJsonObject &personalInfo, const QString &type, const QString &username) {

        // 根据类型和用户名生成缓存文件路径
        filename = getCacheFilePath(type, username);

        qDebug()<<"UserSession::setUserInfo:正在设置用户信息...";


        //------------------------------------test--------------------------------------------------

        //检查迭代器是否正常

        for (auto it = personalInfo.begin(); it != personalInfo.end(); ++it) {
            qDebug() << "Iterating: " << it.key() << ": " << it.value().toString();

        }

        //------------------------------------test--------------------------------------------------




        // 遍历 QJsonObject，将所有 key-value 转成 QString 存入 Map
        for (auto it = personalInfo.begin(); it != personalInfo.end(); ++it) {
            // 如果 Map 中已经有这个 key，就更新其值
            if (userInfoMap.contains(it.key())) {
                userInfoMap[it.key()] = it.value().toString();

                qDebug()<<"serUserInfo:更新"<<it.key()<<":"<<it.value().toString();

            } else {
                // 如果 Map 中没有这个 key，就新增
                userInfoMap.insert(it.key(), it.value().toString());

                qDebug()<<"serUserInfo:新增"<<it.key()<<":"<<it.value().toString();

                //----------------------------------test-----------------------------------
                //检查map是否正常

                qDebug() << "userInfoMap contains key: " << userInfoMap.contains(it.key());

                //----------------------------------test-----------------------------------
            }
        }

        qDebug() << "UserSession::setUserInfo: 保存用户信息到本地文件...";
        saveUserInfoToLocal();
        qDebug() << "UserSession::setUserInfo: 数据已保存";


    }

    QJsonObject getAllInfoAsJson() const {
        QJsonObject obj;

        for (auto it = userInfoMap.begin(); it != userInfoMap.end(); ++it) {
            obj.insert(it.key(), it.value());
        }

        qDebug() << "getAllInfoAsJson:返回所有用户信息";

        return obj;
    }

    QString getValue(const QString &key) const {


        QString value = userInfoMap.value(key, "");


        return value;
    }

    QMap<QString, QString> getAllInfo() const {

        return userInfoMap;
    }

    void saveUserInfoToLocal() {
        QJsonObject obj = getAllInfoAsJson();
        QFile file(filename);

        //  输出文件保存的绝对路径
        QString absolutePath = QDir::currentPath() + "/" + filename;
        qDebug() << "saveUserInfoToLocal:Saving user info to: " << absolutePath;

        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(obj).toJson());
            file.close();
        }
    }

    bool loadUserInfoFromLocal(const QString &filename) {

        qDebug() << "loadUserInfoFromLocal: 线程" << QThread::currentThreadId() << "开始加载缓存文件：" << filename;

        QFile file(filename);

        if (!file.exists() || !file.open(QIODevice::ReadOnly)) return false;

        qDebug()<<"loadUserInfoFromLocal:正在读取缓存...";
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()){

        qDebug()<<"loadUserInfoFromLocal:文件为空，读取缓存失败";
        return false;
        };

        QJsonObject obj = doc.object();
        userInfoMap.clear(); // 先清空旧数据
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            userInfoMap[it.key()] = it.value().toString();
        }

        qDebug()<<"loadUserInfoFromLocal:读取缓存成功!";
        file.close();
        return true;
    }

    // 获取缓存文件的路径
    QString getCacheFilePath(const QString &type, const QString &username) const {
        return "cache/" + type + "_" + username + "_local_user_info.json";
    }


private:
    UserSession() {}  // 私有构造函数，确保只能通过单例访问
    QMap<QString, QString> userInfoMap;
    QString filename;

};


#endif


//-----------------------调用方法----------------------------//

// QJsonObject userInfo = UserSession::instance().loadUserInfoFromLocal();
// if (!userInfo.isEmpty()) {
//     QString name = userInfo["name"].toString();
//     QString id = userInfo["id"].toString();
//     QString phoneNumber = userInfo["phone_number"].toString();
//     qDebug() << "用户信息：";
//     qDebug() << "姓名：" << name;
//     qDebug() << "身份证号：" << id;
//     qDebug() << "手机号：" << phoneNumber;
// }

//-----------------------调用方法----------------------------//

//存储备选方案
/*
    void getUserInfoDetails(QString &name,QString &id,QString &phoneNumber){
        name = m_personalInfo["name"].toString();
        id = m_personalInfo["id"].toString();
        phoneNumber = m_personalInfo["phone_number"].toString();
    }
*/
