#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include <QList>
#include <QJsonObject>

class DataManager
{
public:
    // 预约信息结构体
    struct AppointmentInfo {
        QString appointmentKey;
        QString patientUsername;
        QString doctorUsername;
        QString date;
        QString time;
        QString cost;
        QString status;
    };

    // 患者信息结构体
    struct PatientInfo {
        QString username;
        QString name;
        QString gender;
        QString birthday;
        QString id;
        QString phoneNumber;
        QString email;
    };

    // 医生信息结构体
    struct DoctorInfo {
        QString username;
        QString name;
        QString id;
        QString department;
        QString cost;
        QString begin; // 0~24
        QString end;   // 0~24
        QString limit;
    };

    // 病例信息结构体
    struct CaseInfo {
        QString patientUsername;
        QString doctorUsername;
        QString date;
        QString time;
        QString main;
        QString now;
        QString past;
        QString check;
        QString diagnose;
    };

    // 医嘱信息结构体
    struct AdviceInfo {
        QString patientUsername;
        QString doctorUsername;
        QString date;
        QString time;
        QString medicine;
        QString check;
        QString therapy;
        QString care;
    };

    // 通知信息结构体
    struct NoticeInfo {
        QString username;
        QString type; // "patient", "doctor", "admin"
        QString message;
        QString time;
    };

    // 健康评估问卷结构体
    struct QuestionInfo {
        QString name;
        QString gender;
        QString age;
        QString height;
        QString weight;
        QString heart;
        QString pressure;
        QString lung;
    };

    // 提供静态单例接口
    static DataManager& instance();

    // 提取预约信息
    QList<AppointmentInfo> extractAppointments(const QJsonObject &jsonData);

    // 提取患者信息
    QList<PatientInfo> extractPatients(const QJsonObject &jsonData);

    // 提取医生信息
    QList<DoctorInfo> extractDoctors(const QJsonObject &jsonData);

    // 提取病例信息
    QList<CaseInfo> extractCases(const QJsonObject &jsonData);

    // 提取医嘱信息
    QList<AdviceInfo> extractAdvices(const QJsonObject &jsonData);

    // 提取通知信息
    QList<NoticeInfo> extractNotices(const QJsonObject &jsonData);

    // 提取健康评估问卷
    QList<QuestionInfo> extractQuestions(const QJsonObject &jsonData);

private:
    // 构造函数
    DataManager();

    // 禁用拷贝构造函数和赋值操作符
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;
};

#endif // DATAMANAGER_H
