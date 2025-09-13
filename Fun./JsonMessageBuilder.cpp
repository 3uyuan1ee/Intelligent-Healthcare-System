#include "JsonMessageBuilder.h"

JsonMessageBuilder::JsonMessageBuilder(const QString command, const QString username, const QString type)
{
    m_rootObject["command"] = command;

    QJsonObject dataObject;
    dataObject["username"] = username;
    dataObject["type"] = type;
    m_dataObject = dataObject;
}

void JsonMessageBuilder::setBasicData(const QString username, const QString type)
{
    m_dataObject["username"] = username;
    m_dataObject["type"] = type;
}

QJsonObject JsonMessageBuilder::build()
{
    QJsonObject finalObject = m_rootObject;
    finalObject["data"] = m_dataObject;
    return finalObject;
}

QString JsonMessageBuilder::toString()
{
    QJsonDocument doc(build());
    return doc.toJson(QJsonDocument::Compact);
}

void JsonMessageBuilder::addPatientInfo(
                                        const QString name, const QString gender,
                                        const QString birthday, const QString id,
                                        const QString phoneNumber, const QString email,
                                        const QString username)
{
    QJsonObject patientInfo;
    patientInfo["username"]=username;
    patientInfo["name"] = name;
    patientInfo["gender"] = gender;
    patientInfo["birthday"] = birthday;
    patientInfo["id"] = id;
    patientInfo["phoneNumber"] = phoneNumber;
    patientInfo["email"] = email;

    m_dataObject["patientInfo"] = patientInfo;
}

void JsonMessageBuilder::addDoctorInfo(const QString name, const QString id,
                                       const QString department, QString cost,
                                       QString begin, QString end, QString limit,
                                       const QString username)
{
    QJsonObject doctorInfo;
    doctorInfo["username"]=username;
    doctorInfo["name"] = name;
    doctorInfo["id"] = id;
    doctorInfo["department"] = department;
    doctorInfo["cost"] = cost;
    doctorInfo["begin"] = begin;
    doctorInfo["end"] = end;
    doctorInfo["limit"] = limit;

    m_dataObject["doctorInfo"] = doctorInfo;
}

void JsonMessageBuilder::addAppointment(const QString patientUsername, const QString doctorUsername,
                                        const QString date, QString time, QString cost, const QString status)
{
    QJsonObject appointment;
    appointment["patientUsername"] = patientUsername;
    appointment["doctorUsername"] = doctorUsername;
    appointment["date"] = date;
    appointment["time"] = time;
    appointment["cost"] = cost;
    appointment["status"] = status;

    m_dataObject["appointment"] = appointment;
}

void JsonMessageBuilder::addCase(const QString patientUsername, const QString doctorUsername,
                                 const QString date, QString time, const QString main,
                                 const QString now, const QString past,
                                 const QString check, const QString diagnose)
{
    QJsonObject caseInfo;
    caseInfo["patientUsername"] = patientUsername;
    caseInfo["doctorUsername"] = doctorUsername;
    caseInfo["date"] = date;
    caseInfo["time"] = time;
    caseInfo["main"] = main;
    caseInfo["now"] = now;
    caseInfo["past"] = past;
    caseInfo["check"] = check;
    caseInfo["diagnose"] = diagnose;

    m_dataObject["case"] = caseInfo;
}

void JsonMessageBuilder::addAdvice(const QString patientUsername, const QString doctorUsername,
                                   const QString date, QString time, const QString medicine,
                                   const QString check, const QString therapy, const QString care)
{
    QJsonObject advice;
    advice["patientUsername"] = patientUsername;
    advice["doctorUsername"] = doctorUsername;
    advice["date"] = date;
    advice["time"] = time;
    advice["medicine"] = medicine;
    advice["check"] = check;
    advice["therapy"] = therapy;
    advice["care"] = care;

    m_dataObject["advice"] = advice;
}

void JsonMessageBuilder::addPatientSimple(const QString username, const QString name)
{
    QJsonObject patientSimple;
    patientSimple["username"] = username;
    patientSimple["name"] = name;

    m_dataObject["patient_0"] = patientSimple;
}

void JsonMessageBuilder::addDoctorSimple(const QString username, const QString name)
{
    QJsonObject doctorSimple;
    doctorSimple["username"] = username;
    doctorSimple["name"] = name;

    m_dataObject["doctor_0"] = doctorSimple;
}

void JsonMessageBuilder::addNotice(const QString username, const QString type,
                                   const QString message, const QString time)
{
    QJsonObject notice;
    notice["username"] = username;
    notice["type"] = type;
    notice["message"] = message;
    notice["time"] = time;

    m_dataObject["notice"] = notice;
}

void JsonMessageBuilder::addQuestionnaire(const QString name, const QString gender, QString age,
                                          QString height, QString weight, const QString heart,
                                          const QString pressure, const QString lung)
{
    QJsonObject questionnaire;
    questionnaire["name"] = name;
    questionnaire["gender"] = gender;
    questionnaire["age"] = age;
    questionnaire["height"] = height;
    questionnaire["weight"] = weight;
    questionnaire["heart"] = heart;
    questionnaire["pressure"] = pressure;
    questionnaire["lung"] = lung;

    m_dataObject["question"] = questionnaire;
}

void JsonMessageBuilder::clearAdditionalData()
{
    QString username = m_dataObject["username"].toString();
    QString type = m_dataObject["type"].toString();

    m_dataObject = QJsonObject();
    m_dataObject["username"] = username;
    m_dataObject["type"] = type;
}

void JsonMessageBuilder::addAdditionalData(const QString key,const QString value){
    m_dataObject[key]=value;

}
