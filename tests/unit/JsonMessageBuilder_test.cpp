#include <QtTest/QtTest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include "../../Fun./JsonMessageBuilder.h"
#include "../config/test_config.h"

class JsonMessageBuilderTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // 构造函数和基础功能测试
    void testConstructorWithCommand();
    void testConstructorWithAllParameters();
    void testSetBasicData();
    void testBuild();
    void testToString();

    // 患者信息测试
    void testAddPatientInfo();
    void testAddPatientInfoWithCustomUsername();
    void testPatientInfoDataStructure();

    // 医生信息测试
    void testAddDoctorInfo();
    void testAddDoctorInfoWithCustomUsername();
    void testDoctorInfoDataStructure();

    // 预约信息测试
    void testAddAppointment();
    void testAppointmentDataStructure();

    // 病例信息测试
    void testAddCase();
    void testCaseDataStructure();

    // 医嘱信息测试
    void testAddAdvice();
    void testAdviceDataStructure();

    // 简略信息测试
    void testAddPatientSimple();
    void testAddDoctorSimple();
    void testSimpleInfoDataStructure();

    // 通知信息测试
    void testAddNotice();
    void testAddNoticeWithCustomTime();
    void testNoticeDataStructure();

    // 问卷信息测试
    void testAddQuestionnaire();
    void testQuestionnaireDataStructure();

    // 数据管理测试
    void testAddAdditionalData();
    void testClearAdditionalData();
    void testMultipleDataOperations();

    // JSON格式验证测试
    void testJsonStructureValidation();
    void testJsonStringConversion();
    void testJsonParsing();

    // 边界条件测试
    void testEmptyParameters();
    void testSpecialCharactersInData();
    void testLargeDataValues();

    // 数据完整性测试
    void testDataIntegrity();
    void testCommandPreservation();
    void testBasicDataPreservation();

private:
    void setupUserSession();
    void cleanupUserSession();
    bool compareJsonObjects(const QJsonObject &obj1, const QJsonObject &obj2);
    bool validateJsonStructure(const QJsonObject &json, const QStringList &expectedKeys);

    JsonMessageBuilder *m_builder;
};

void JsonMessageBuilderTest::initTestCase()
{
    qDebug() << "Initializing JsonMessageBuilder test suite...";
    TestConfig::setupTestData();

    // 设置UserSession测试数据
    setupUserSession();
}

void JsonMessageBuilderTest::cleanupTestCase()
{
    qDebug() << "Cleaning up JsonMessageBuilder test suite...";
    cleanupUserSession();
    TestConfig::cleanupTestData();
}

void JsonMessageBuilderTest::init()
{
    // 每个测试前创建新的builder
    m_builder = new JsonMessageBuilder("testCommand");
}

void JsonMessageBuilderTest::cleanup()
{
    // 每个测试后清理builder
    if (m_builder) {
        delete m_builder;
        m_builder = nullptr;
    }
}

void JsonMessageBuilderTest::testConstructorWithCommand()
{
    JsonMessageBuilder builder("login");
    QJsonObject json = builder.build();

    QVERIFY(json.contains("command"));
    QCOMPARE(json["command"].toString(), QString("login"));
    QVERIFY(json.contains("data"));

    QJsonObject data = json["data"].toObject();
    QVERIFY(data.contains("username"));
    QVERIFY(data.contains("type"));
}

void JsonMessageBuilderTest::testConstructorWithAllParameters()
{
    JsonMessageBuilder builder("getUserInfo", "testuser", "patient");
    QJsonObject json = builder.build();

    QCOMPARE(json["command"].toString(), QString("getUserInfo"));

    QJsonObject data = json["data"].toObject();
    QCOMPARE(data["username"].toString(), QString("testuser"));
    QCOMPARE(data["type"].toString(), QString("patient"));
}

void JsonMessageBuilderTest::testSetBasicData()
{
    m_builder->setBasicData("newuser", "doctor");
    QJsonObject json = m_builder->build();

    QJsonObject data = json["data"].toObject();
    QCOMPARE(data["username"].toString(), QString("newuser"));
    QCOMPARE(data["type"].toString(), QString("doctor"));
}

void JsonMessageBuilderTest::testBuild()
{
    m_builder->setBasicData("testuser", "admin");
    QJsonObject json = m_builder->build();

    // 验证基本结构
    QVERIFY(json.isObject());
    QVERIFY(json.contains("command"));
    QVERIFY(json.contains("data"));

    // 验证数据对象
    QJsonObject data = json["data"].toObject();
    QVERIFY(data.isObject());
    QVERIFY(data.contains("username"));
    QVERIFY(data.contains("type"));
}

void JsonMessageBuilderTest::testToString()
{
    m_builder->addAdditionalData("test", "value");
    QString jsonString = m_builder->toString();

    // 验证字符串不为空
    QVERIFY(!jsonString.isEmpty());

    // 验证可以解析回JSON
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    // 验证内容与build()方法一致
    QJsonObject jsonFromString = doc.object();
    QJsonObject jsonFromBuild = m_builder->build();
    QVERIFY(compareJsonObjects(jsonFromString, jsonFromBuild));
}

void JsonMessageBuilderTest::testAddPatientInfo()
{
    m_builder->addPatientInfo("张三", "男", "1990-01-01", "123456789012345678", "13800138000", "zhangsan@example.com");
    QJsonObject json = m_builder->build();

    QJsonObject data = json["data"].toObject();
    QVERIFY(data.contains("patientInfo"));

    QJsonObject patientInfo = data["patientInfo"].toObject();
    QCOMPARE(patientInfo["name"].toString(), QString("张三"));
    QCOMPARE(patientInfo["gender"].toString(), QString("男"));
    QCOMPARE(patientInfo["birthday"].toString(), QString("1990-01-01"));
    QCOMPARE(patientInfo["id"].toString(), QString("123456789012345678"));
    QCOMPARE(patientInfo["phoneNumber"].toString(), QString("13800138000"));
    QCOMPARE(patientInfo["email"].toString(), QString("zhangsan@example.com"));
}

void JsonMessageBuilderTest::testAddPatientInfoWithCustomUsername()
{
    m_builder->addPatientInfo("李四", "女", "1985-05-15", "987654321098765432", "13900139000", "lisi@example.com", "customuser");
    QJsonObject json = m_builder->build();

    QJsonObject data = json["data"].toObject();
    QJsonObject patientInfo = data["patientInfo"].toObject();
    QCOMPARE(patientInfo["username"].toString(), QString("customuser"));
}

void JsonMessageBuilderTest::testPatientInfoDataStructure()
{
    m_builder->addPatientInfo("王五", "男", "1992-03-10", "111111111111111111", "13700137000", "wangwu@example.com");
    QJsonObject json = m_builder->build();

    QStringList expectedKeys = {"username", "name", "gender", "birthday", "id", "phoneNumber", "email"};
    QJsonObject data = json["data"].toObject();
    QJsonObject patientInfo = data["patientInfo"].toObject();

    QVERIFY(validateJsonStructure(patientInfo, expectedKeys));
    QCOMPARE(patientInfo.size(), expectedKeys.size());
}

void JsonMessageBuilderTest::testAddDoctorInfo()
{
    m_builder->addDoctorInfo("赵医生", "DOC001", "内科", "100", "08:00", "17:00", "20");
    QJsonObject json = m_builder->build();

    QJsonObject data = json["data"].toObject();
    QVERIFY(data.contains("doctorInfo"));

    QJsonObject doctorInfo = data["doctorInfo"].toObject();
    QCOMPARE(doctorInfo["name"].toString(), QString("赵医生"));
    QCOMPARE(doctorInfo["id"].toString(), QString("DOC001"));
    QCOMPARE(doctorInfo["department"].toString(), QString("内科"));
    QCOMPARE(doctorInfo["cost"].toString(), QString("100"));
    QCOMPARE(doctorInfo["begin"].toString(), QString("08:00"));
    QCOMPARE(doctorInfo["end"].toString(), QString("17:00"));
    QCOMPARE(doctorInfo["limit"].toString(), QString("20"));
}

void JsonMessageBuilderTest::testAddDoctorInfoWithCustomUsername()
{
    m_builder->addDoctorInfo("钱医生", "DOC002", "外科", "150", "09:00", "18:00", "15", "doctor123");
    QJsonObject json = m_builder->build();

    QJsonObject data = json["data"].toObject();
    QJsonObject doctorInfo = data["doctorInfo"].toObject();
    QCOMPARE(doctorInfo["username"].toString(), QString("doctor123"));
}

void JsonMessageBuilderTest::testDoctorInfoDataStructure()
{
    m_builder->addDoctorInfo("孙医生", "DOC003", "儿科", "80", "07:30", "16:30", "25");
    QJsonObject json = m_builder->build();

    QStringList expectedKeys = {"username", "name", "id", "department", "cost", "begin", "end", "limit"};
    QJsonObject data = json["data"].toObject();
    QJsonObject doctorInfo = data["doctorInfo"].toObject();

    QVERIFY(validateJsonStructure(doctorInfo, expectedKeys));
    QCOMPARE(doctorInfo.size(), expectedKeys.size());
}

void JsonMessageBuilderTest::testAddAppointment()
{
    m_builder->addAppointment("patient001", "doctor001", "2024-12-10", "14:30", "100", "confirmed");
    QJsonObject json = m_builder->build();

    QJsonObject data = json["data"].toObject();
    QVERIFY(data.contains("appointment"));

    QJsonObject appointment = data["appointment"].toObject();
    QCOMPARE(appointment["patientUsername"].toString(), QString("patient001"));
    QCOMPARE(appointment["doctorUsername"].toString(), QString("doctor001"));
    QCOMPARE(appointment["date"].toString(), QString("2024-12-10"));
    QCOMPARE(appointment["time"].toString(), QString("14:30"));
    QCOMPARE(appointment["cost"].toString(), QString("100"));
    QCOMPARE(appointment["status"].toString(), QString("confirmed"));
}

void JsonMessageBuilderTest::testAppointmentDataStructure()
{
    m_builder->addAppointment("patient002", "doctor002", "2024-12-11", "10:00", "150", "pending");
    QJsonObject json = m_builder->build();

    QStringList expectedKeys = {"patientUsername", "doctorUsername", "date", "time", "cost", "status"};
    QJsonObject data = json["data"].toObject();
    QJsonObject appointment = data["appointment"].toObject();

    QVERIFY(validateJsonStructure(appointment, expectedKeys));
    QCOMPARE(appointment.size(), expectedKeys.size());
}

void JsonMessageBuilderTest::testAddCase()
{
    m_builder->addCase("patient003", "doctor003", "2024-12-09", "15:00", "头痛", "发热38°C", "无特殊病史", "血常规检查", "感冒");
    QJsonObject json = m_builder->build();

    QJsonObject data = json["data"].toObject();
    QVERIFY(data.contains("case"));

    QJsonObject caseInfo = data["case"].toObject();
    QCOMPARE(caseInfo["patientUsername"].toString(), QString("patient003"));
    QCOMPARE(caseInfo["doctorUsername"].toString(), QString("doctor003"));
    QCOMPARE(caseInfo["date"].toString(), QString("2024-12-09"));
    QCOMPARE(caseInfo["time"].toString(), QString("15:00"));
    QCOMPARE(caseInfo["main"].toString(), QString("头痛"));
    QCOMPARE(caseInfo["now"].toString(), QString("发热38°C"));
    QCOMPARE(caseInfo["past"].toString(), QString("无特殊病史"));
    QCOMPARE(caseInfo["check"].toString(), QString("血常规检查"));
    QCOMPARE(caseInfo["diagnose"].toString(), QString("感冒"));
}

void JsonMessageBuilderTest::testCaseDataStructure()
{
    m_builder->addCase("patient004", "doctor004", "2024-12-08", "11:30", "咳嗽", "干咳无痰", "哮喘病史", "胸部X光", "支气管炎");
    QJsonObject json = m_builder->build();

    QStringList expectedKeys = {"patientUsername", "doctorUsername", "date", "time", "main", "now", "past", "check", "diagnose"};
    QJsonObject data = json["data"].toObject();
    QJsonObject caseInfo = data["case"].toObject();

    QVERIFY(validateJsonStructure(caseInfo, expectedKeys));
    QCOMPARE(caseInfo.size(), expectedKeys.size());
}

void JsonMessageBuilderTest::testAddAdvice()
{
    m_builder->addAdvice("patient005", "doctor005", "2024-12-07", "16:00", "阿莫西林", "血常规", "物理降温", "多休息");
    QJsonObject json = m_builder->build();

    QJsonObject data = json["data"].toObject();
    QVERIFY(data.contains("advice"));

    QJsonObject advice = data["advice"].toObject();
    QCOMPARE(advice["patientUsername"].toString(), QString("patient005"));
    QCOMPARE(advice["doctorUsername"].toString(), QString("doctor005"));
    QCOMPARE(advice["date"].toString(), QString("2024-12-07"));
    QCOMPARE(advice["time"].toString(), QString("16:00"));
    QCOMPARE(advice["medicine"].toString(), QString("阿莫西林"));
    QCOMPARE(advice["check"].toString(), QString("血常规"));
    QCOMPARE(advice["therapy"].toString(), QString("物理降温"));
    QCOMPARE(advice["care"].toString(), QString("多休息"));
}

void JsonMessageBuilderTest::testAdviceDataStructure()
{
    m_builder->addAdvice("patient006", "doctor006", "2024-12-06", "13:00", "布洛芬", "肝功能", "冷敷", "避免辛辣食物");
    QJsonObject json = m_builder->build();

    QStringList expectedKeys = {"patientUsername", "doctorUsername", "date", "time", "medicine", "check", "therapy", "care"};
    QJsonObject data = json["data"].toObject();
    QJsonObject advice = data["advice"].toObject();

    QVERIFY(validateJsonStructure(advice, expectedKeys));
    QCOMPARE(advice.size(), expectedKeys.size());
}

void JsonMessageBuilderTest::testAddPatientSimple()
{
    m_builder->addPatientSimple("simpleuser", "简单用户");
    QJsonObject json = m_builder->build();

    QJsonObject data = json["data"].toObject();
    QVERIFY(data.contains("patient_0"));

    QJsonObject patientSimple = data["patient_0"].toObject();
    QCOMPARE(patientSimple["username"].toString(), QString("simpleuser"));
    QCOMPARE(patientSimple["name"].toString(), QString("简单用户"));
}

void JsonMessageBuilderTest::testAddDoctorSimple()
{
    m_builder->addDoctorSimple("simpledoctor", "简单医生");
    QJsonObject json = m_builder->build();

    QJsonObject data = json["data"].toObject();
    QVERIFY(data.contains("doctor_0"));

    QJsonObject doctorSimple = data["doctor_0"].toObject();
    QCOMPARE(doctorSimple["username"].toString(), QString("simpledoctor"));
    QCOMPARE(doctorSimple["name"].toString(), QString("简单医生"));
}

void JsonMessageBuilderTest::testSimpleInfoDataStructure()
{
    m_builder->addPatientSimple("user123", "用户123");
    m_builder->addDoctorSimple("doc123", "医生123");
    QJsonObject json = m_builder->build();

    QJsonObject data = json["data"].toObject();

    // 验证患者简略信息
    QJsonObject patientSimple = data["patient_0"].toObject();
    QStringList patientKeys = {"username", "name"};
    QVERIFY(validateJsonStructure(patientSimple, patientKeys));

    // 验证医生简略信息
    QJsonObject doctorSimple = data["doctor_0"].toObject();
    QStringList doctorKeys = {"username", "name"};
    QVERIFY(validateJsonStructure(doctorSimple, doctorKeys));
}

void JsonMessageBuilderTest::testAddNotice()
{
    QString customTime = "2024-12-09 10:30:00";
    m_builder->addNotice("noticeuser", "系统通知", "您有新的预约信息", customTime);
    QJsonObject json = m_builder->build();

    QJsonObject data = json["data"].toObject();
    QVERIFY(data.contains("notice"));

    QJsonObject notice = data["notice"].toObject();
    QCOMPARE(notice["username"].toString(), QString("noticeuser"));
    QCOMPARE(notice["type"].toString(), QString("系统通知"));
    QCOMPARE(notice["message"].toString(), QString("您有新的预约信息"));
    QCOMPARE(notice["time"].toString(), customTime);
}

void JsonMessageBuilderTest::testAddNoticeWithCustomTime()
{
    // 不指定时间，使用默认时间
    m_builder->addNotice("defaultuser", "提醒", "请及时就诊");
    QJsonObject json = m_builder->build();

    QJsonObject data = json["data"].toObject();
    QJsonObject notice = data["notice"].toObject();

    QVERIFY(notice.contains("time"));
    QString timeStr = notice["time"].toString();

    // 验证时间格式 (yyyy-MM-dd hh:mm:ss)
    QRegularExpression timeRegex(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$)");
    QVERIFY(timeRegex.match(timeStr).hasMatch());
}

void JsonMessageBuilderTest::testNoticeDataStructure()
{
    m_builder->addNotice("user123", "预约提醒", "明天上午9点有预约");
    QJsonObject json = m_builder->build();

    QStringList expectedKeys = {"username", "type", "message", "time"};
    QJsonObject data = json["data"].toObject();
    QJsonObject notice = data["notice"].toObject();

    QVERIFY(validateJsonStructure(notice, expectedKeys));
    QCOMPARE(notice.size(), expectedKeys.size());
}

void JsonMessageBuilderTest::testAddQuestionnaire()
{
    m_builder->addQuestionnaire("测试用户", "男", "30", "175", "70", "正常", "120/80", "清晰");
    QJsonObject json = m_builder->build();

    QJsonObject data = json["data"].toObject();
    QVERIFY(data.contains("question"));

    QJsonObject questionnaire = data["question"].toObject();
    QCOMPARE(questionnaire["name"].toString(), QString("测试用户"));
    QCOMPARE(questionnaire["gender"].toString(), QString("男"));
    QCOMPARE(questionnaire["age"].toString(), QString("30"));
    QCOMPARE(questionnaire["height"].toString(), QString("175"));
    QCOMPARE(questionnaire["weight"].toString(), QString("70"));
    QCOMPARE(questionnaire["heart"].toString(), QString("正常"));
    QCOMPARE(questionnaire["pressure"].toString(), QString("120/80"));
    QCOMPARE(questionnaire["lung"].toString(), QString("清晰"));
}

void JsonMessageBuilderTest::testQuestionnaireDataStructure()
{
    m_builder->addQuestionnaire("问卷调查", "女", "25", "165", "55", "稍快", "110/70", "正常");
    QJsonObject json = m_builder->build();

    QStringList expectedKeys = {"name", "gender", "age", "height", "weight", "heart", "pressure", "lung"};
    QJsonObject data = json["data"].toObject();
    QJsonObject questionnaire = data["question"].toObject();

    QVERIFY(validateJsonStructure(questionnaire, expectedKeys));
    QCOMPARE(questionnaire.size(), expectedKeys.size());
}

void JsonMessageBuilderTest::testAddAdditionalData()
{
    m_builder->addAdditionalData("customKey1", "customValue1");
    m_builder->addAdditionalData("customKey2", "customValue2");
    m_builder->addAdditionalData("customKey3", "123");

    QJsonObject json = m_builder->build();
    QJsonObject data = json["data"].toObject();

    QCOMPARE(data["customKey1"].toString(), QString("customValue1"));
    QCOMPARE(data["customKey2"].toString(), QString("customValue2"));
    QCOMPARE(data["customKey3"].toString(), QString("123"));
}

void JsonMessageBuilderTest::testClearAdditionalData()
{
    // 添加各种数据
    m_builder->addPatientInfo("测试患者", "男", "1990-01-01", "123456", "13800138000", "test@test.com");
    m_builder->addAdditionalData("custom1", "value1");
    m_builder->addAdditionalData("custom2", "value2");

    // 验证数据已添加
    QJsonObject jsonBefore = m_builder->build();
    QJsonObject dataBefore = jsonBefore["data"].toObject();
    QVERIFY(dataBefore.contains("patientInfo"));
    QVERIFY(dataBefore.contains("custom1"));
    QVERIFY(dataBefore.contains("custom2"));

    // 清理额外数据
    m_builder->clearAdditionalData();

    // 验证基本数据保留，额外数据被清除
    QJsonObject jsonAfter = m_builder->build();
    QJsonObject dataAfter = jsonAfter["data"].toObject();
    QVERIFY(dataAfter.contains("username"));
    QVERIFY(dataAfter.contains("type"));
    QVERIFY(!dataAfter.contains("patientInfo"));
    QVERIFY(!dataAfter.contains("custom1"));
    QVERIFY(!dataAfter.contains("custom2"));
}

void JsonMessageBuilderTest::testMultipleDataOperations()
{
    // 添加多种类型的数据
    m_builder->addPatientInfo("患者A", "女", "1985-05-15", "111111", "13900139000", "patient@a.com");
    m_builder->addDoctorInfo("医生B", "DOC001", "内科", "100", "08:00", "17:00", "20");
    m_builder->addAppointment("patient001", "doctor001", "2024-12-10", "14:30", "100", "confirmed");
    m_builder->addAdditionalData("operation", "create_appointment");

    QJsonObject json = m_builder->build();
    QJsonObject data = json["data"].toObject();

    // 验证所有数据都存在
    QVERIFY(data.contains("patientInfo"));
    QVERIFY(data.contains("doctorInfo"));
    QVERIFY(data.contains("appointment"));
    QVERIFY(data.contains("operation"));

    // 验证数据内容
    QCOMPARE(data["operation"].toString(), QString("create_appointment"));
    QCOMPARE(data["patientInfo"].toObject()["name"].toString(), QString("患者A"));
    QCOMPARE(data["doctorInfo"].toObject()["name"].toString(), QString("医生B"));
}

void JsonMessageBuilderTest::testJsonStructureValidation()
{
    m_builder->addPatientInfo("测试", "男", "1990-01-01", "123456", "13800138000", "test@test.com");
    QJsonObject json = m_builder->build();

    // 验证顶层结构
    QStringList topLevelKeys = {"command", "data"};
    QVERIFY(validateJsonStructure(json, topLevelKeys));

    // 验证数据对象结构
    QJsonObject data = json["data"].toObject();
    QStringList dataKeys = {"username", "type", "patientInfo"};
    QVERIFY(validateJsonStructure(data, dataKeys));
}

void JsonMessageBuilderTest::testJsonStringConversion()
{
    m_builder->addAdditionalData("test", "测试");
    QString jsonString = m_builder->toString();

    // 验证字符串转换的正确性
    QVERIFY(!jsonString.isEmpty());
    QVERIFY(jsonString.contains("\"test\":\"测试\""));

    // 验证可以解析回JSON对象
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    QVERIFY(error.error == QJsonParseError::NoError);

    QJsonObject parsedJson = doc.object();
    QJsonObject originalJson = m_builder->build();
    QVERIFY(compareJsonObjects(parsedJson, originalJson));
}

void JsonMessageBuilderTest::testJsonParsing()
{
    // 创建复杂的JSON消息
    m_builder->addPatientInfo("解析测试", "女", "1980-08-08", "888888", "13700137000", "parse@test.com");
    m_builder->addAppointment("user123", "doc456", "2024-12-15", "10:00", "200", "pending");

    QJsonObject json = m_builder->build();
    QJsonDocument doc(json);
    QByteArray jsonBytes = doc.toJson(QJsonDocument::Compact);

    // 解析回JSON对象
    QJsonParseError error;
    QJsonDocument parsedDoc = QJsonDocument::fromJson(jsonBytes, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(parsedDoc.isObject());

    QJsonObject parsedJson = parsedDoc.object();
    QVERIFY(compareJsonObjects(parsedJson, json));
}

void JsonMessageBuilderTest::testEmptyParameters()
{
    // 测试空字符串参数
    m_builder->addPatientInfo("", "", "", "", "", "");
    m_builder->addAdditionalData("", "");

    QJsonObject json = m_builder->build();
    QJsonObject data = json["data"].toObject();

    // 验证空参数被正确处理
    QJsonObject patientInfo = data["patientInfo"].toObject();
    QVERIFY(patientInfo.contains("name"));
    QVERIFY(patientInfo["name"].toString().isEmpty());

    QVERIFY(data.contains(""));
}

void JsonMessageBuilderTest::testSpecialCharactersInData()
{
    QString specialString = "特殊字符: 中文\nEnglish\t!@#$%^&*()\"'\\/:;<>?[]{}|";
    m_builder->addPatientInfo(specialString, specialString, specialString, specialString, specialString, specialString);
    m_builder->addAdditionalData("special_key", specialString);

    QJsonObject json = m_builder->build();
    QJsonObject data = json["data"].toObject();

    // 验证特殊字符被正确保存
    QJsonObject patientInfo = data["patientInfo"].toObject();
    QCOMPARE(patientInfo["name"].toString(), specialString);
    QCOMPARE(patientInfo["gender"].toString(), specialString);
    QCOMPARE(data["special_key"].toString(), specialString);

    // 验证JSON字符串转换
    QString jsonString = m_builder->toString();
    QVERIFY(jsonString.contains(specialString));
}

void JsonMessageBuilderTest::testLargeDataValues()
{
    QString largeString = "x".repeated(10000); // 10KB字符串
    m_builder->addPatientInfo(largeString, largeString, largeString, largeString, largeString, largeString);

    QJsonObject json = m_builder->build();
    QJsonObject data = json["data"].toObject();
    QJsonObject patientInfo = data["patientInfo"].toObject();

    // 验证大数据被正确处理
    QCOMPARE(patientInfo["name"].toString().length(), 10000);
    QCOMPARE(patientInfo["gender"].toString().length(), 10000);
    QCOMPARE(patientInfo["name"].toString(), largeString);
}

void JsonMessageBuilderTest::testDataIntegrity()
{
    // 添加多种复杂的数据
    m_builder->addPatientInfo("完整性测试", "男", "1995-12-25", "1234567890", "13800138000", "integrity@test.com");
    m_builder->addCase("case_user", "case_doc", "2024-12-01", "09:00", "头痛", "发热", "无", "CT", "感冒");
    m_builder->addAdditionalData("测试数据", "包含中文和English混合的数据");

    // 多次构建，验证数据一致性
    QJsonObject json1 = m_builder->build();
    QJsonObject json2 = m_builder->build();
    QJsonObject json3 = m_builder->build();

    QVERIFY(compareJsonObjects(json1, json2));
    QVERIFY(compareJsonObjects(json2, json3));

    // 验证数据完整性
    QJsonObject data = json1["data"].toObject();
    QCOMPARE(data["patientInfo"].toObject()["name"].toString(), QString("完整性测试"));
    QCOMPARE(data["case"].toObject()["diagnose"].toString(), QString("感冒"));
    QCOMPARE(data["测试数据"].toString(), QString("包含中文和English混合的数据"));
}

void JsonMessageBuilderTest::testCommandPreservation()
{
    JsonMessageBuilder builder1("login");
    JsonMessageBuilder builder2("getUserInfo");
    JsonMessageBuilder builder3("updateAppointment");

    // 添加一些数据
    builder1.addAdditionalData("test", "value1");
    builder2.addAdditionalData("test", "value2");
    builder3.addAdditionalData("test", "value3");

    QJsonObject json1 = builder1.build();
    QJsonObject json2 = builder2.build();
    QJsonObject json3 = builder3.build();

    QCOMPARE(json1["command"].toString(), QString("login"));
    QCOMPARE(json2["command"].toString(), QString("getUserInfo"));
    QCOMPARE(json3["command"].toString(), QString("updateAppointment"));

    // 验证数据不冲突
    QCOMPARE(json1["data"].toObject()["test"].toString(), QString("value1"));
    QCOMPARE(json2["data"].toObject()["test"].toString(), QString("value2"));
    QCOMPARE(json3["data"].toObject()["test"].toString(), QString("value3"));
}

void JsonMessageBuilderTest::testBasicDataPreservation()
{
    m_builder->setBasicData("preserveuser", "admin");
    m_builder->addAdditionalData("extra", "data");

    // 清理额外数据
    m_builder->clearAdditionalData();

    QJsonObject json = m_builder->build();
    QJsonObject data = json["data"].toObject();

    // 验证基本数据被保留
    QCOMPARE(data["username"].toString(), QString("preserveuser"));
    QCOMPARE(data["type"].toString(), QString("admin"));
    QVERIFY(!data.contains("extra"));
}

void JsonMessageBuilderTest::setupUserSession()
{
    UserSession &session = UserSession::instance();
    QJsonObject userInfo;
    userInfo["username"] = "testuser";
    userInfo["type"] = "patient";
    session.setUserInfo(userInfo, "patient", "testuser");
}

void JsonMessageBuilderTest::cleanupUserSession()
{
    UserSession &session = UserSession::instance();
    session.setUserInfo(QJsonObject(), "test", "test");
}

bool JsonMessageBuilderTest::compareJsonObjects(const QJsonObject &obj1, const QJsonObject &obj2)
{
    if (obj1.size() != obj2.size()) {
        return false;
    }

    for (auto it = obj1.begin(); it != obj1.end(); ++it) {
        QString key = it.key();
        if (!obj2.contains(key)) {
            return false;
        }

        QJsonValue value1 = it.value();
        QJsonValue value2 = obj2[key];

        if (value1 != value2) {
            return false;
        }
    }

    return true;
}

bool JsonMessageBuilderTest::validateJsonStructure(const QJsonObject &json, const QStringList &expectedKeys)
{
    for (const QString &key : expectedKeys) {
        if (!json.contains(key)) {
            return false;
        }
    }
    return true;
}

QTEST_MAIN(JsonMessageBuilderTest)
#include "JsonMessageBuilder_test.moc"