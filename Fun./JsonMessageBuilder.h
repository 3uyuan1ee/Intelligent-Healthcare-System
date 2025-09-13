#ifndef JSONMESSAGEBUILDER_H
#define JSONMESSAGEBUILDER_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QString>
#include <QDateTime>
#include"../Instance/UserSession.h"

class JsonMessageBuilder {
public:
    // 构造函数，设置基本参数
    JsonMessageBuilder(const QString command, const QString username = UserSession::instance().getValue("username"),
                       const QString type = UserSession::instance().getValue("type"));

    // 设置基本数据参数
    void setBasicData(const QString username=UserSession::instance().getValue("username"), const QString type=UserSession::instance().getValue("type"));

    // 构建最终的JSON对象
    QJsonObject build();

    // 转换为JSON字符串
    QString toString();

    // 1. 添加患者信息
    void addPatientInfo(
                        const QString name, const QString gender,
                        const QString birthday, const QString id,
                        const QString phoneNumber, const QString email,
                        const QString username=UserSession::instance().getValue("username"));

    // 2. 添加医生信息
    void addDoctorInfo(const QString name, const QString id,
                       const QString department, QString cost,
                       QString begin, QString end, QString limit,
                       const QString username=UserSession::instance().getValue("username"));

    // 3. 添加预约信息
    void addAppointment(const QString patientUsername, const QString doctorUsername,
                        const QString date, QString time, QString cost, const QString status);

    // 4. 添加病例信息
    void addCase(const QString patientUsername, const QString doctorUsername,
                 const QString date, QString time, const QString main,
                 const QString now, const QString past,
                 const QString check, const QString diagnose);

    // 5. 添加医嘱信息
    void addAdvice(const QString patientUsername, const QString doctorUsername,
                   const QString date, QString time, const QString medicine,
                   const QString check, const QString therapy, const QString care);

    // 6. 添加患者简略信息
    void addPatientSimple(const QString username, const QString name);

    // 7. 添加医生简略信息
    void addDoctorSimple(const QString username, const QString name);

    // 8. 添加通知信息
    void addNotice(const QString username, const QString type,
                   const QString message, const QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    // 9. 添加健康评估问卷
    void addQuestionnaire(const QString name, const QString gender, QString age,
                          QString height, QString weight, const QString heart,
                          const QString pressure, const QString lung);

    // 清空所有额外添加的数据（保留基本数据）
    void clearAdditionalData();

    //添加自定义数据
    void addAdditionalData(const QString key,const QString value);

private:
    QJsonObject m_rootObject;
    QJsonObject m_dataObject;
};

#endif // JSONMESSAGEBUILDER_H
