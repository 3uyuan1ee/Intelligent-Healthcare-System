#include <QtTest/QtTest>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "../../Fun./DataManager.h"
#include "../config/test_config.h"

class DataManagerTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // 单例模式测试
    void testSingletonPattern();

    // 预约信息提取测试
    void testExtractAppointments();
    void testExtractAppointmentsEmptyData();
    void testExtractAppointmentsInvalidData();

    // 患者信息提取测试
    void testExtractPatients();
    void testExtractPatientsEmptyData();
    void testExtractPatientsInvalidData();

    // 医生信息提取测试
    void testExtractDoctors();
    void testExtractDoctorsEmptyData();
    void testExtractDoctorsInvalidData();

    // 病例信息提取测试
    void testExtractCases();
    void testExtractCasesEmptyData();
    void testExtractCasesInvalidData();

    // 医嘱信息提取测试
    void testExtractAdvices();
    void testExtractAdvicesEmptyData();
    void testExtractAdvicesInvalidData();

    // 通知信息提取测试
    void testExtractNotices();
    void testExtractNoticesEmptyData();
    void testExtractNoticesInvalidData();

    // 健康评估问卷提取测试
    void testExtractQuestions();
    void testExtractQuestionsEmptyData();
    void testExtractQuestionsInvalidData();

    // 边界条件测试
    void testExtractDataWithMissingFields();
    void testExtractDataWithExtraFields();
    void testExtractDataWithNullValues();

private:
    DataManager* m_dataManager;

    // 创建测试用的JSON数据
    QJsonObject createAppointmentJson();
    QJsonObject createPatientJson();
    QJsonObject createDoctorJson();
    QJsonObject createCaseJson();
    QJsonObject createAdviceJson();
    QJsonObject createNoticeJson();
    QJsonObject createQuestionJson();
};

void DataManagerTest::initTestCase()
{
    qDebug() << "DataManager测试开始";
}

void DataManagerTest::cleanupTestCase()
{
    qDebug() << "DataManager测试完成";
}

void DataManagerTest::init()
{
    m_dataManager = &DataManager::instance();
}

void DataManagerTest::cleanup()
{
    // 清理工作
}

void DataManagerTest::testSingletonPattern()
{
    DataManager& instance1 = DataManager::instance();
    DataManager& instance2 = DataManager::instance();

    QCOMPARE(&instance1, &instance2);
    qDebug() << "单例模式测试通过";
}

void DataManagerTest::testExtractAppointments()
{
    QJsonObject jsonData = createAppointmentJson();

    QList<DataManager::AppointmentInfo> appointments = m_dataManager->extractAppointments(jsonData);

    QCOMPARE(appointments.size(), 2);

    // 验证第一个预约信息
    QCOMPARE(appointments[0].patientUsername, QString("patient001"));
    QCOMPARE(appointments[0].doctorUsername, QString("doctor001"));
    QCOMPARE(appointments[0].date, QString("2024-01-15"));
    QCOMPARE(appointments[0].time, QString("09:00"));
    QCOMPARE(appointments[0].cost, QString("50"));
    QCOMPARE(appointments[0].status, QString("confirmed"));

    // 验证第二个预约信息
    QCOMPARE(appointments[1].patientUsername, QString("patient002"));
    QCOMPARE(appointments[1].doctorUsername, QString("doctor002"));
    QCOMPARE(appointments[1].date, QString("2024-01-16"));
    QCOMPARE(appointments[1].time, QString("14:30"));
    QCOMPARE(appointments[1].cost, QString("80"));
    QCOMPARE(appointments[1].status, QString("pending"));

    qDebug() << "预约信息提取测试通过";
}

void DataManagerTest::testExtractAppointmentsEmptyData()
{
    QJsonObject emptyJson;
    emptyJson["appointments"] = QJsonArray();

    QList<DataManager::AppointmentInfo> appointments = m_dataManager->extractAppointments(emptyJson);

    QCOMPARE(appointments.size(), 0);
    qDebug() << "空预约数据提取测试通过";
}

void DataManagerTest::testExtractAppointmentsInvalidData()
{
    QJsonObject invalidJson;
    // 没有appointments字段

    QList<DataManager::AppointmentInfo> appointments = m_dataManager->extractAppointments(invalidJson);

    QCOMPARE(appointments.size(), 0);
    qDebug() << "无效预约数据提取测试通过";
}

void DataManagerTest::testExtractPatients()
{
    QJsonObject jsonData = createPatientJson();

    QList<DataManager::PatientInfo> patients = m_dataManager->extractPatients(jsonData);

    QCOMPARE(patients.size(), 2);

    // 验证第一个患者信息
    QCOMPARE(patients[0].username, QString("patient001"));
    QCOMPARE(patients[0].name, QString("张三"));
    QCOMPARE(patients[0].gender, QString("男"));
    QCOMPARE(patients[0].birthday, QString("1990-01-01"));
    QCOMPARE(patients[0].id, QString("110101199001011234"));
    QCOMPARE(patients[0].phoneNumber, QString("13800138000"));
    QCOMPARE(patients[0].email, QString("zhangsan@example.com"));

    qDebug() << "患者信息提取测试通过";
}

void DataManagerTest::testExtractPatientsEmptyData()
{
    QJsonObject emptyJson;
    emptyJson["patients"] = QJsonArray();

    QList<DataManager::PatientInfo> patients = m_dataManager->extractPatients(emptyJson);

    QCOMPARE(patients.size(), 0);
    qDebug() << "空患者数据提取测试通过";
}

void DataManagerTest::testExtractPatientsInvalidData()
{
    QJsonObject invalidJson;
    // 没有patients字段

    QList<DataManager::PatientInfo> patients = m_dataManager->extractPatients(invalidJson);

    QCOMPARE(patients.size(), 0);
    qDebug() << "无效患者数据提取测试通过";
}

void DataManagerTest::testExtractDoctors()
{
    QJsonObject jsonData = createDoctorJson();

    QList<DataManager::DoctorInfo> doctors = m_dataManager->extractDoctors(jsonData);

    QCOMPARE(doctors.size(), 2);

    // 验证第一个医生信息
    QCOMPARE(doctors[0].username, QString("doctor001"));
    QCOMPARE(doctors[0].name, QString("李医生"));
    QCOMPARE(doctors[0].id, QString("D001"));
    QCOMPARE(doctors[0].department, QString("内科"));
    QCOMPARE(doctors[0].cost, QString("50"));
    QCOMPARE(doctors[0].begin, QString("8"));
    QCOMPARE(doctors[0].end, QString("17"));
    QCOMPARE(doctors[0].limit, QString("20"));

    qDebug() << "医生信息提取测试通过";
}

void DataManagerTest::testExtractDoctorsEmptyData()
{
    QJsonObject emptyJson;
    emptyJson["doctors"] = QJsonArray();

    QList<DataManager::DoctorInfo> doctors = m_dataManager->extractDoctors(emptyJson);

    QCOMPARE(doctors.size(), 0);
    qDebug() << "空医生数据提取测试通过";
}

void DataManagerTest::testExtractDoctorsInvalidData()
{
    QJsonObject invalidJson;
    // 没有doctors字段

    QList<DataManager::DoctorInfo> doctors = m_dataManager->extractDoctors(invalidJson);

    QCOMPARE(doctors.size(), 0);
    qDebug() << "无效医生数据提取测试通过";
}

void DataManagerTest::testExtractCases()
{
    QJsonObject jsonData = createCaseJson();

    QList<DataManager::CaseInfo> cases = m_dataManager->extractCases(jsonData);

    QCOMPARE(cases.size(), 1);

    // 验证病例信息
    QCOMPARE(cases[0].patientUsername, QString("patient001"));
    QCOMPARE(cases[0].doctorUsername, QString("doctor001"));
    QCOMPARE(cases[0].date, QString("2024-01-15"));
    QCOMPARE(cases[0].time, QString("09:30"));
    QCOMPARE(cases[0].main, QString("头痛、发热"));
    QCOMPARE(cases[0].now, QString("体温38.5℃，血压正常"));
    QCOMPARE(cases[0].past, QString("无重大疾病史"));
    QCOMPARE(cases[0].check, QString("血常规、CT"));
    QCOMPARE(cases[0].diagnose, QString("上呼吸道感染"));

    qDebug() << "病例信息提取测试通过";
}

void DataManagerTest::testExtractCasesEmptyData()
{
    QJsonObject emptyJson;
    emptyJson["cases"] = QJsonArray();

    QList<DataManager::CaseInfo> cases = m_dataManager->extractCases(emptyJson);

    QCOMPARE(cases.size(), 0);
    qDebug() << "空病例数据提取测试通过";
}

void DataManagerTest::testExtractCasesInvalidData()
{
    QJsonObject invalidJson;
    // 没有cases字段

    QList<DataManager::CaseInfo> cases = m_dataManager->extractCases(invalidJson);

    QCOMPARE(cases.size(), 0);
    qDebug() << "无效病例数据提取测试通过";
}

void DataManagerTest::testExtractAdvices()
{
    QJsonObject jsonData = createAdviceJson();

    QList<DataManager::AdviceInfo> advices = m_dataManager->extractAdvices(jsonData);

    QCOMPARE(advices.size(), 1);

    // 验证医嘱信息
    QCOMPARE(advices[0].patientUsername, QString("patient001"));
    QCOMPARE(advices[0].doctorUsername, QString("doctor001"));
    QCOMPARE(advices[0].date, QString("2024-01-15"));
    QCOMPARE(advices[0].time, QString("10:00"));
    QCOMPARE(advices[0].medicine, QString("阿司匹林，每日一次"));
    QCOMPARE(advices[0].check, QString("三天后复查血常规"));
    QCOMPARE(advices[0].therapy, QString("物理降温"));
    QCOMPARE(advices[0].care, QString("多休息，多喝水"));

    qDebug() << "医嘱信息提取测试通过";
}

void DataManagerTest::testExtractAdvicesEmptyData()
{
    QJsonObject emptyJson;
    emptyJson["advices"] = QJsonArray();

    QList<DataManager::AdviceInfo> advices = m_dataManager->extractAdvices(emptyJson);

    QCOMPARE(advices.size(), 0);
    qDebug() << "空医嘱数据提取测试通过";
}

void DataManagerTest::testExtractAdvicesInvalidData()
{
    QJsonObject invalidJson;
    // 没有advices字段

    QList<DataManager::AdviceInfo> advices = m_dataManager->extractAdvices(invalidJson);

    QCOMPARE(advices.size(), 0);
    qDebug() << "无效医嘱数据提取测试通过";
}

void DataManagerTest::testExtractNotices()
{
    QJsonObject jsonData = createNoticeJson();

    QList<DataManager::NoticeInfo> notices = m_dataManager->extractNotices(jsonData);

    QCOMPARE(notices.size(), 2);

    // 验证第一个通知信息
    QCOMPARE(notices[0].username, QString("patient001"));
    QCOMPARE(notices[0].type, QString("patient"));
    QCOMPARE(notices[0].message, QString("您的预约已确认"));
    QCOMPARE(notices[0].time, QString("2024-01-15 09:00:00"));

    // 验证第二个通知信息
    QCOMPARE(notices[1].username, QString("doctor001"));
    QCOMPARE(notices[1].type, QString("doctor"));
    QCOMPARE(notices[1].message, QString("有新的预约请求"));
    QCOMPARE(notices[1].time, QString("2024-01-15 08:30:00"));

    qDebug() << "通知信息提取测试通过";
}

void DataManagerTest::testExtractNoticesEmptyData()
{
    QJsonObject emptyJson;
    emptyJson["notices"] = QJsonArray();

    QList<DataManager::NoticeInfo> notices = m_dataManager->extractNotices(emptyJson);

    QCOMPARE(notices.size(), 0);
    qDebug() << "空通知数据提取测试通过";
}

void DataManagerTest::testExtractNoticesInvalidData()
{
    QJsonObject invalidJson;
    // 没有notices字段

    QList<DataManager::NoticeInfo> notices = m_dataManager->extractNotices(invalidJson);

    QCOMPARE(notices.size(), 0);
    qDebug() << "无效通知数据提取测试通过";
}

void DataManagerTest::testExtractQuestions()
{
    QJsonObject jsonData = createQuestionJson();

    QList<DataManager::QuestionInfo> questions = m_dataManager->extractQuestions(jsonData);

    QCOMPARE(questions.size(), 1);

    // 验证健康评估问卷信息
    QCOMPARE(questions[0].name, QString("张三"));
    QCOMPARE(questions[0].gender, QString("男"));
    QCOMPARE(questions[0].age, QString("30"));
    QCOMPARE(questions[0].height, QString("175"));
    QCOMPARE(questions[0].weight, QString("70"));
    QCOMPARE(questions[0].heart, QString("75"));
    QCOMPARE(questions[0].pressure, QString("120/80"));
    QCOMPARE(questions[0].lung, QString("正常"));

    qDebug() << "健康评估问卷提取测试通过";
}

void DataManagerTest::testExtractQuestionsEmptyData()
{
    QJsonObject emptyJson;
    emptyJson["questions"] = QJsonArray();

    QList<DataManager::QuestionInfo> questions = m_dataManager->extractQuestions(emptyJson);

    QCOMPARE(questions.size(), 0);
    qDebug() << "空健康评估问卷数据提取测试通过";
}

void DataManagerTest::testExtractQuestionsInvalidData()
{
    QJsonObject invalidJson;
    // 没有questions字段

    QList<DataManager::QuestionInfo> questions = m_dataManager->extractQuestions(invalidJson);

    QCOMPARE(questions.size(), 0);
    qDebug() << "无效健康评估问卷数据提取测试通过";
}

void DataManagerTest::testExtractDataWithMissingFields()
{
    QJsonObject incompleteData;
    QJsonArray appointmentArray;

    QJsonObject appointment;
    appointment["patientUsername"] = "patient001";
    // 缺少其他必要字段
    appointmentArray.append(appointment);

    incompleteData["appointments"] = appointmentArray;

    QList<DataManager::AppointmentInfo> appointments = m_dataManager->extractAppointments(incompleteData);

    // 应该仍然能提取出一条记录，缺少的字段为空字符串
    QCOMPARE(appointments.size(), 1);
    QCOMPARE(appointments[0].patientUsername, QString("patient001"));
    QVERIFY(appointments[0].doctorUsername.isEmpty());

    qDebug() << "缺失字段数据提取测试通过";
}

void DataManagerTest::testExtractDataWithExtraFields()
{
    QJsonObject dataWithExtraFields = createAppointmentJson();

    // 添加额外字段
    QJsonArray appointmentArray = dataWithExtraFields["appointments"].toArray();
    QJsonObject firstAppointment = appointmentArray[0].toObject();
    firstAppointment["extraField"] = "extraValue";
    appointmentArray[0] = firstAppointment;
    dataWithExtraFields["appointments"] = appointmentArray;

    QList<DataManager::AppointmentInfo> appointments = m_dataManager->extractAppointments(dataWithExtraFields);

    // 应该能正常处理额外字段
    QCOMPARE(appointments.size(), 2);
    QCOMPARE(appointments[0].patientUsername, QString("patient001"));

    qDebug() << "额外字段数据提取测试通过";
}

void DataManagerTest::testExtractDataWithNullValues()
{
    QJsonObject dataWithNullValues;
    QJsonArray appointmentArray;

    QJsonObject appointment;
    appointment["patientUsername"] = QJsonValue::Null;
    appointment["doctorUsername"] = "doctor001";
    appointmentArray.append(appointment);

    dataWithNullValues["appointments"] = appointmentArray;

    QList<DataManager::AppointmentInfo> appointments = m_dataManager->extractAppointments(dataWithNullValues);

    // 应该能处理null值，转换为空字符串
    QCOMPARE(appointments.size(), 1);
    QVERIFY(appointments[0].patientUsername.isEmpty());
    QCOMPARE(appointments[0].doctorUsername, QString("doctor001"));

    qDebug() << "null值数据提取测试通过";
}

// 创建测试用数据的辅助方法
QJsonObject DataManagerTest::createAppointmentJson()
{
    QJsonObject jsonData;
    QJsonArray appointmentArray;

    QJsonObject appointment1;
    appointment1["patientUsername"] = "patient001";
    appointment1["doctorUsername"] = "doctor001";
    appointment1["date"] = "2024-01-15";
    appointment1["time"] = "09:00";
    appointment1["cost"] = "50";
    appointment1["status"] = "confirmed";
    appointment1["appointmentKey"] = "A001";
    appointmentArray.append(appointment1);

    QJsonObject appointment2;
    appointment2["patientUsername"] = "patient002";
    appointment2["doctorUsername"] = "doctor002";
    appointment2["date"] = "2024-01-16";
    appointment2["time"] = "14:30";
    appointment2["cost"] = "80";
    appointment2["status"] = "pending";
    appointment2["appointmentKey"] = "A002";
    appointmentArray.append(appointment2);

    jsonData["appointments"] = appointmentArray;
    return jsonData;
}

QJsonObject DataManagerTest::createPatientJson()
{
    QJsonObject jsonData;
    QJsonArray patientArray;

    QJsonObject patient1;
    patient1["username"] = "patient001";
    patient1["name"] = "张三";
    patient1["gender"] = "男";
    patient1["birthday"] = "1990-01-01";
    patient1["id"] = "110101199001011234";
    patient1["phoneNumber"] = "13800138000";
    patient1["email"] = "zhangsan@example.com";
    patientArray.append(patient1);

    QJsonObject patient2;
    patient2["username"] = "patient002";
    patient2["name"] = "李四";
    patient2["gender"] = "女";
    patient2["birthday"] = "1985-05-15";
    patient2["id"] = "110101198505154321";
    patient2["phoneNumber"] = "13900139000";
    patient2["email"] = "lisi@example.com";
    patientArray.append(patient2);

    jsonData["patients"] = patientArray;
    return jsonData;
}

QJsonObject DataManagerTest::createDoctorJson()
{
    QJsonObject jsonData;
    QJsonArray doctorArray;

    QJsonObject doctor1;
    doctor1["username"] = "doctor001";
    doctor1["name"] = "李医生";
    doctor1["id"] = "D001";
    doctor1["department"] = "内科";
    doctor1["cost"] = "50";
    doctor1["begin"] = "8";
    doctor1["end"] = "17";
    doctor1["limit"] = "20";
    doctorArray.append(doctor1);

    QJsonObject doctor2;
    doctor2["username"] = "doctor002";
    doctor2["name"] = "王医生";
    doctor2["id"] = "D002";
    doctor2["department"] = "外科";
    doctor2["cost"] = "80";
    doctor2["begin"] = "9";
    doctor2["end"] = "18";
    doctor2["limit"] = "15";
    doctorArray.append(doctor2);

    jsonData["doctors"] = doctorArray;
    return jsonData;
}

QJsonObject DataManagerTest::createCaseJson()
{
    QJsonObject jsonData;
    QJsonArray caseArray;

    QJsonObject case1;
    case1["patientUsername"] = "patient001";
    case1["doctorUsername"] = "doctor001";
    case1["date"] = "2024-01-15";
    case1["time"] = "09:30";
    case1["main"] = "头痛、发热";
    case1["now"] = "体温38.5℃，血压正常";
    case1["past"] = "无重大疾病史";
    case1["check"] = "血常规、CT";
    case1["diagnose"] = "上呼吸道感染";
    caseArray.append(case1);

    jsonData["cases"] = caseArray;
    return jsonData;
}

QJsonObject DataManagerTest::createAdviceJson()
{
    QJsonObject jsonData;
    QJsonArray adviceArray;

    QJsonObject advice1;
    advice1["patientUsername"] = "patient001";
    advice1["doctorUsername"] = "doctor001";
    advice1["date"] = "2024-01-15";
    advice1["time"] = "10:00";
    advice1["medicine"] = "阿司匹林，每日一次";
    advice1["check"] = "三天后复查血常规";
    advice1["therapy"] = "物理降温";
    advice1["care"] = "多休息，多喝水";
    adviceArray.append(advice1);

    jsonData["advices"] = adviceArray;
    return jsonData;
}

QJsonObject DataManagerTest::createNoticeJson()
{
    QJsonObject jsonData;
    QJsonArray noticeArray;

    QJsonObject notice1;
    notice1["username"] = "patient001";
    notice1["type"] = "patient";
    notice1["message"] = "您的预约已确认";
    notice1["time"] = "2024-01-15 09:00:00";
    noticeArray.append(notice1);

    QJsonObject notice2;
    notice2["username"] = "doctor001";
    notice2["type"] = "doctor";
    notice2["message"] = "有新的预约请求";
    notice2["time"] = "2024-01-15 08:30:00";
    noticeArray.append(notice2);

    jsonData["notices"] = noticeArray;
    return jsonData;
}

QJsonObject DataManagerTest::createQuestionJson()
{
    QJsonObject jsonData;
    QJsonArray questionArray;

    QJsonObject question1;
    question1["name"] = "张三";
    question1["gender"] = "男";
    question1["age"] = "30";
    question1["height"] = "175";
    question1["weight"] = "70";
    question1["heart"] = "75";
    question1["pressure"] = "120/80";
    question1["lung"] = "正常";
    questionArray.append(question1);

    jsonData["questions"] = questionArray;
    return jsonData;
}

QTEST_MAIN(DataManagerTest)
#include "DataManager_test.moc"