#include "DataManager.h"
#include <QJsonValue>
#include <QJsonObject>
#include <QDebug>

DataManager& DataManager::instance()
{
    static DataManager instance;
    return instance;
}

DataManager::DataManager() {}

// 提取预约信息
QList<DataManager::AppointmentInfo> DataManager::extractAppointments(const QJsonObject &jsonData)
{
    QList<AppointmentInfo> appointments;
    for (auto it = jsonData.constBegin(); it != jsonData.constEnd(); ++it) {
        QString key = it.key();
        if (key.startsWith("appointment_")) {
            QJsonValue value = it.value();
            if (value.isObject()) {
                QJsonObject appointmentObj = value.toObject();
                AppointmentInfo info;
                info.appointmentKey = key;
                info.patientUsername = appointmentObj["patientUsername"].toString();
                info.doctorUsername = appointmentObj["doctorUsername"].toString();
                info.date = appointmentObj["date"].toString();
                info.time = appointmentObj["time"].toString();
                info.cost = appointmentObj["cost"].toString();
                info.status = appointmentObj["status"].toString();
                appointments.append(info);
                qDebug() << "提取预约:" << key
                         << "患者:" << info.patientUsername
                         << "医生:" << info.doctorUsername;
            }
        }
    }
    return appointments;
}

// 提取患者信息
QList<DataManager::PatientInfo> DataManager::extractPatients(const QJsonObject &jsonData)
{
    QList<PatientInfo> patients;
    for (auto it = jsonData.constBegin(); it != jsonData.constEnd(); ++it) {
        if (it.key().startsWith("patient_")) {
            QJsonObject patientObj = it.value().toObject();
            PatientInfo info;
            info.username = patientObj["username"].toString();
            info.name = patientObj["name"].toString();
            info.gender = patientObj["gender"].toString();
            info.birthday = patientObj["birthday"].toString();
            info.id = patientObj["id"].toString();
            info.phoneNumber = patientObj["phoneNumber"].toString();
            info.email = patientObj["email"].toString();
            patients.append(info);
        }
    }
    return patients;
}

// 提取医生信息
QList<DataManager::DoctorInfo> DataManager::extractDoctors(const QJsonObject &jsonData)
{
    QList<DoctorInfo> doctors;
    for (auto it = jsonData.constBegin(); it != jsonData.constEnd(); ++it) {
        if (it.key().startsWith("doctor_")) {
            QJsonObject doctorObj = it.value().toObject();
            DoctorInfo info;
            info.username = doctorObj["username"].toString();
            info.name = doctorObj["name"].toString();
            info.id = doctorObj["id"].toString();
            info.department = doctorObj["department"].toString();
            info.cost = doctorObj["cost"].toString();
            info.begin = doctorObj["begin"].toString();
            info.end = doctorObj["end"].toString();
            info.limit = doctorObj["limit"].toString();
            doctors.append(info);
        }
    }
    return doctors;
}

// 提取病例信息
QList<DataManager::CaseInfo> DataManager::extractCases(const QJsonObject &jsonData)
{
    QList<CaseInfo> cases;
    for (auto it = jsonData.constBegin(); it != jsonData.constEnd(); ++it) {
        if (it.key().startsWith("case_")) {
            QJsonObject caseObj = it.value().toObject();
            CaseInfo info;
            info.patientUsername = caseObj["patientUsername"].toString();
            info.doctorUsername = caseObj["doctorUsername"].toString();
            info.date = caseObj["date"].toString();
            info.time = caseObj["time"].toString();
            info.main = caseObj["main"].toString();
            info.now = caseObj["now"].toString();
            info.past = caseObj["past"].toString();
            info.check = caseObj["check"].toString();
            info.diagnose = caseObj["diagnose"].toString();
            cases.append(info);
        }
    }
    return cases;
}

// 提取医嘱信息
QList<DataManager::AdviceInfo> DataManager::extractAdvices(const QJsonObject &jsonData)
{
    QList<AdviceInfo> advices;
    for (auto it = jsonData.constBegin(); it != jsonData.constEnd(); ++it) {
        if (it.key().startsWith("advice_")) {
            QJsonObject adviceObj = it.value().toObject();
            AdviceInfo info;
            info.patientUsername = adviceObj["patientUsername"].toString();
            info.doctorUsername = adviceObj["doctorUsername"].toString();
            info.date = adviceObj["date"].toString();
            info.time = adviceObj["time"].toString();
            info.medicine = adviceObj["medicine"].toString();
            info.check = adviceObj["check"].toString();
            info.therapy = adviceObj["therapy"].toString();
            info.care = adviceObj["care"].toString();
            advices.append(info);
        }
    }
    return advices;
}

// 提取通知信息
QList<DataManager::NoticeInfo> DataManager::extractNotices(const QJsonObject &jsonData)
{
    QList<NoticeInfo> notices;
    for (auto it = jsonData.constBegin(); it != jsonData.constEnd(); ++it) {
        if (it.key().startsWith("notice_")) {
            QJsonObject noticeObj = it.value().toObject();
            NoticeInfo info;
            info.username = noticeObj["username"].toString();
            info.type = noticeObj["type"].toString();
            info.message = noticeObj["message"].toString();
            info.time = noticeObj["time"].toString();
            notices.append(info);
        }
    }
    return notices;
}

// 提取健康评估问卷
QList<DataManager::QuestionInfo> DataManager::extractQuestions(const QJsonObject &jsonData)
{
    QList<QuestionInfo> questions;
    for (auto it = jsonData.constBegin(); it != jsonData.constEnd(); ++it) {
        if (it.key().startsWith("question_")) {
            QJsonObject questionObj = it.value().toObject();
            QuestionInfo info;
            info.name = questionObj["name"].toString();
            info.gender = questionObj["gender"].toString();
            info.age = questionObj["age"].toString();
            info.height = questionObj["height"].toString();
            info.weight = questionObj["weight"].toString();
            info.heart = questionObj["heart"].toString();
            info.pressure = questionObj["pressure"].toString();
            info.lung = questionObj["lung"].toString();
            questions.append(info);
        }
    }
    return questions;
}
