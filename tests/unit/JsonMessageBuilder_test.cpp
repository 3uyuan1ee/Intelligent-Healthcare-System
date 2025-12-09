#include <QtTest/QtTest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QApplication>
#include "../../Fun./JsonMessageBuilder.h"
#include "../../Instance/UserSession.h"
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

    // 医生信息测试
    void testAddDoctorInfo();
    void testAddDoctorInfoWithCustomUsername();

    // 预约信息测试
    void testAddAppointment();

    // 病例信息测试
    void testAddCase();

    // 医嘱信息测试
    void testAddAdvice();

    // 简略信息测试
    void testAddPatientSimple();
    void testAddDoctorSimple();

    // 通知信息测试
    void testAddNotice();
    void testAddNoticeWithCustomTime();

    // 问卷信息测试
    void testAddQuestionnaire();

    // 数据管理测试
    void testAddAdditionalData();
    void testClearAdditionalData();
    void testMultipleDataOperations();

    // JSON格式验证测试
    void testJsonStructureValidation();
    void testJsonStringConversion();

    // 边界条件测试
    void testEmptyParameters();
    void testSpecialCharactersInData();

private:
    void setupUserSession(const QString& username, const QString& type);
    void clearUserSession();
    bool validateJsonStructure(const QJsonObject& json, const QString& expectedCommand);
};

void JsonMessageBuilderTest::initTestCase()
{
    qDebug() << "JsonMessageBuilder测试开始";

    // 需要QApplication实例
    if (!QApplication::instance()) {
        int argc = 0;
        char* argv[] = { nullptr };
        new QApplication(argc, argv);
    }
}

void JsonMessageBuilderTest::cleanupTestCase()
{
    qDebug() << "JsonMessageBuilder测试完成";
}

void JsonMessageBuilderTest::init()
{
    // 清理用户会话
    clearUserSession();
}

void JsonMessageBuilderTest::cleanup()
{
    // 清理用户会话
    clearUserSession();
}

void JsonMessageBuilderTest::testConstructorWithCommand()
{
    qDebug() << "测试带命令的构造函数";

    JsonMessageBuilder builder("LOGIN");

    QJsonObject result = builder.build();

    // 验证基本结构
    QVERIFY(result.contains("command"));
    QVERIFY(result.contains("data"));

    QCOMPARE(result["command"].toString(), QString("LOGIN"));
    QVERIFY(result["data"].isObject());

    // 验证data对象的基本字段
    QJsonObject data = result["data"].toObject();
    QVERIFY(data.contains("username"));
    QVERIFY(data.contains("type"));

    qDebug() << "带命令的构造函数测试通过";
}

void JsonMessageBuilderTest::testConstructorWithAllParameters()
{
    qDebug() << "测试带所有参数的构造函数";

    setupUserSession("testuser", "patient");

    JsonMessageBuilder builder("TEST_COMMAND", "customuser", "doctor");

    QJsonObject result = builder.build();

    // 验证基本结构
    QCOMPARE(result["command"].toString(), QString("TEST_COMMAND"));

    QJsonObject data = result["data"].toObject();
    QCOMPARE(data["username"].toString(), QString("customuser"));
    QCOMPARE(data["type"].toString(), QString("doctor"));

    qDebug() << "带所有参数的构造函数测试通过";
}

void JsonMessageBuilderTest::testSetBasicData()
{
    qDebug() << "测试设置基础数据";

    JsonMessageBuilder builder("COMMAND");
    builder.setBasicData("newuser", "admin");

    QJsonObject result = builder.build();

    QJsonObject data = result["data"].toObject();
    QCOMPARE(data["username"].toString(), QString("newuser"));
    QCOMPARE(data["type"].toString(), QString("admin"));

    qDebug() << "设置基础数据测试通过";
}

void JsonMessageBuilderTest::testBuild()
{
    qDebug() << "测试构建JSON对象";

    JsonMessageBuilder builder("TEST");
    builder.addAdditionalData("testKey", "testValue");

    QJsonObject result = builder.build();

    // 验证返回的是有效的JSON对象
    QVERIFY(result.isEmpty() == false);
    QVERIFY(result.contains("command"));
    QVERIFY(result.contains("data"));

    // 验证data字段包含我们添加的数据
    QJsonObject data = result["data"].toObject();
    QCOMPARE(data["testKey"].toString(), QString("testValue"));

    qDebug() << "构建JSON对象测试通过";
}

void JsonMessageBuilderTest::testToString()
{
    qDebug() << "测试转换为字符串";

    JsonMessageBuilder builder("STRING_TEST");
    builder.addAdditionalData("message", "Hello World");

    QString jsonString = builder.toString();

    // 验证返回的是非空字符串
    QVERIFY(!jsonString.isEmpty());

    // 验证可以解析为有效的JSON
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    QCOMPARE(error.error, QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    // 验证解析后的对象内容
    QJsonObject obj = doc.object();
    QCOMPARE(obj["command"].toString(), QString("STRING_TEST"));
    QCOMPARE(obj["data"].toObject()["message"].toString(), QString("Hello World"));

    qDebug() << "转换为字符串测试通过";
}

void JsonMessageBuilderTest::testAddPatientInfo()
{
    qDebug() << "测试添加患者信息";

    setupUserSession("patient001", "patient");

    JsonMessageBuilder builder("ADD_PATIENT");
    builder.addPatientInfo("张三", "男", "1990-01-01", "110101199001011234",
                           "13800138000", "zhangsan@example.com");

    QJsonObject result = builder.build();
    QJsonObject data = result["data"].toObject();

    // 验证患者信息结构
    QVERIFY(data.contains("patientInfo"));
    QJsonObject patientInfo = data["patientInfo"].toObject();

    QCOMPARE(patientInfo["username"].toString(), QString("patient001")); // 从UserSession获取
    QCOMPARE(patientInfo["name"].toString(), QString("张三"));
    QCOMPARE(patientInfo["gender"].toString(), QString("男"));
    QCOMPARE(patientInfo["birthday"].toString(), QString("1990-01-01"));
    QCOMPARE(patientInfo["id"].toString(), QString("110101199001011234"));
    QCOMPARE(patientInfo["phoneNumber"].toString(), QString("13800138000"));
    QCOMPARE(patientInfo["email"].toString(), QString("zhangsan@example.com"));

    qDebug() << "添加患者信息测试通过";
}

void JsonMessageBuilderTest::testAddPatientInfoWithCustomUsername()
{
    qDebug() << "测试添加患者信息（自定义用户名）";

    JsonMessageBuilder builder("ADD_PATIENT");
    builder.addPatientInfo("李四", "女", "1985-05-15", "110101198505154321",
                           "13900139000", "lisi@example.com", "custompatient");

    QJsonObject result = builder.build();
    QJsonObject patientInfo = result["data"].toObject()["patientInfo"].toObject();

    QCOMPARE(patientInfo["username"].toString(), QString("custompatient"));
    QCOMPARE(patientInfo["name"].toString(), QString("李四"));

    qDebug() << "添加患者信息（自定义用户名）测试通过";
}

void JsonMessageBuilderTest::testAddDoctorInfo()
{
    qDebug() << "测试添加医生信息";

    setupUserSession("doctor001", "doctor");

    JsonMessageBuilder builder("ADD_DOCTOR");
    builder.addDoctorInfo("王医生", "D001", "内科", "50", "8", "17", "20");

    QJsonObject result = builder.build();
    QJsonObject data = result["data"].toObject();

    // 验证医生信息结构
    QVERIFY(data.contains("doctorInfo"));
    QJsonObject doctorInfo = data["doctorInfo"].toObject();

    QCOMPARE(doctorInfo["username"].toString(), QString("doctor001")); // 从UserSession获取
    QCOMPARE(doctorInfo["name"].toString(), QString("王医生"));
    QCOMPARE(doctorInfo["id"].toString(), QString("D001"));
    QCOMPARE(doctorInfo["department"].toString(), QString("内科"));
    QCOMPARE(doctorInfo["cost"].toString(), QString("50"));
    QCOMPARE(doctorInfo["begin"].toString(), QString("8"));
    QCOMPARE(doctorInfo["end"].toString(), QString("17"));
    QCOMPARE(doctorInfo["limit"].toString(), QString("20"));

    qDebug() << "添加医生信息测试通过";
}

void JsonMessageBuilderTest::testAddDoctorInfoWithCustomUsername()
{
    qDebug() << "测试添加医生信息（自定义用户名）";

    JsonMessageBuilder builder("ADD_DOCTOR");
    builder.addDoctorInfo("赵医生", "D002", "外科", "80", "9", "18", "15", "customdoctor");

    QJsonObject result = builder.build();
    QJsonObject doctorInfo = result["data"].toObject()["doctorInfo"].toObject();

    QCOMPARE(doctorInfo["username"].toString(), QString("customdoctor"));
    QCOMPARE(doctorInfo["name"].toString(), QString("赵医生"));

    qDebug() << "添加医生信息（自定义用户名）测试通过";
}

void JsonMessageBuilderTest::testAddAppointment()
{
    qDebug() << "测试添加预约信息";

    JsonMessageBuilder builder("MAKE_APPOINTMENT");
    builder.addAppointment("patient001", "doctor001", "2024-01-20", "10:00", "50", "pending");

    QJsonObject result = builder.build();
    QJsonObject data = result["data"].toObject();

    // 验证预约信息结构
    QVERIFY(data.contains("appointment"));
    QJsonObject appointment = data["appointment"].toObject();

    QCOMPARE(appointment["patientUsername"].toString(), QString("patient001"));
    QCOMPARE(appointment["doctorUsername"].toString(), QString("doctor001"));
    QCOMPARE(appointment["date"].toString(), QString("2024-01-20"));
    QCOMPARE(appointment["time"].toString(), QString("10:00"));
    QCOMPARE(appointment["cost"].toString(), QString("50"));
    QCOMPARE(appointment["status"].toString(), QString("pending"));

    qDebug() << "添加预约信息测试通过";
}

void JsonMessageBuilderTest::testAddCase()
{
    qDebug() << "测试添加病例信息";

    JsonMessageBuilder builder("ADD_CASE");
    builder.addCase("patient001", "doctor001", "2024-01-20", "11:00",
                    "头痛发热", "体温38.5℃", "无特殊病史",
                    "血常规检查", "上呼吸道感染");

    QJsonObject result = builder.build();
    QJsonObject data = result["data"].toObject();

    // 验证病例信息结构
    QVERIFY(data.contains("case"));
    QJsonObject caseInfo = data["case"].toObject();

    QCOMPARE(caseInfo["patientUsername"].toString(), QString("patient001"));
    QCOMPARE(caseInfo["doctorUsername"].toString(), QString("doctor001"));
    QCOMPARE(caseInfo["date"].toString(), QString("2024-01-20"));
    QCOMPARE(caseInfo["time"].toString(), QString("11:00"));
    QCOMPARE(caseInfo["main"].toString(), QString("头痛发热"));
    QCOMPARE(caseInfo["now"].toString(), QString("体温38.5℃"));
    QCOMPARE(caseInfo["past"].toString(), QString("无特殊病史"));
    QCOMPARE(caseInfo["check"].toString(), QString("血常规检查"));
    QCOMPARE(caseInfo["diagnose"].toString(), QString("上呼吸道感染"));

    qDebug() << "添加病例信息测试通过";
}

void JsonMessageBuilderTest::testAddAdvice()
{
    qDebug() << "测试添加医嘱信息";

    JsonMessageBuilder builder("ADD_ADVICE");
    builder.addAdvice("patient001", "doctor001", "2024-01-20", "12:00",
                     "阿司匹林", "血常规检查", "物理降温", "多休息");

    QJsonObject result = builder.build();
    QJsonObject data = result["data"].toObject();

    // 验证医嘱信息结构
    QVERIFY(data.contains("advice"));
    QJsonObject advice = data["advice"].toObject();

    QCOMPARE(advice["patientUsername"].toString(), QString("patient001"));
    QCOMPARE(advice["doctorUsername"].toString(), QString("doctor001"));
    QCOMPARE(advice["date"].toString(), QString("2024-01-20"));
    QCOMPARE(advice["time"].toString(), QString("12:00"));
    QCOMPARE(advice["medicine"].toString(), QString("阿司匹林"));
    QCOMPARE(advice["check"].toString(), QString("血常规检查"));
    QCOMPARE(advice["therapy"].toString(), QString("物理降温"));
    QCOMPARE(advice["care"].toString(), QString("多休息"));

    qDebug() << "添加医嘱信息测试通过";
}

void JsonMessageBuilderTest::testAddPatientSimple()
{
    qDebug() << "测试添加患者简略信息";

    JsonMessageBuilder builder("PATIENT_SIMPLE");
    builder.addPatientSimple("patient001", "张三");

    QJsonObject result = builder.build();
    QJsonObject data = result["data"].toObject();

    // 验证患者简略信息结构
    QVERIFY(data.contains("patient_0"));
    QJsonObject patientSimple = data["patient_0"].toObject();

    QCOMPARE(patientSimple["username"].toString(), QString("patient001"));
    QCOMPARE(patientSimple["name"].toString(), QString("张三"));

    qDebug() << "添加患者简略信息测试通过";
}

void JsonMessageBuilderTest::testAddDoctorSimple()
{
    qDebug() << "测试添加医生简略信息";

    JsonMessageBuilder builder("DOCTOR_SIMPLE");
    builder.addDoctorSimple("doctor001", "李医生");

    QJsonObject result = builder.build();
    QJsonObject data = result["data"].toObject();

    // 验证医生简略信息结构
    QVERIFY(data.contains("doctor_0"));
    QJsonObject doctorSimple = data["doctor_0"].toObject();

    QCOMPARE(doctorSimple["username"].toString(), QString("doctor001"));
    QCOMPARE(doctorSimple["name"].toString(), QString("李医生"));

    qDebug() << "添加医生简略信息测试通过";
}

void JsonMessageBuilderTest::testAddNotice()
{
    qDebug() << "测试添加通知信息";

    QString customTime = "2024-01-20 15:30:00";

    JsonMessageBuilder builder("SEND_NOTICE");
    builder.addNotice("user001", "patient", "您的预约已确认", customTime);

    QJsonObject result = builder.build();
    QJsonObject data = result["data"].toObject();

    // 验证通知信息结构
    QVERIFY(data.contains("notice"));
    QJsonObject notice = data["notice"].toObject();

    QCOMPARE(notice["username"].toString(), QString("user001"));
    QCOMPARE(notice["type"].toString(), QString("patient"));
    QCOMPARE(notice["message"].toString(), QString("您的预约已确认"));
    QCOMPARE(notice["time"].toString(), customTime);

    qDebug() << "添加通知信息测试通过";
}

void JsonMessageBuilderTest::testAddNoticeWithCustomTime()
{
    qDebug() << "测试添加通知信息（自定义时间）";

    JsonMessageBuilder builder("SEND_NOTICE");
    // 使用默认时间参数
    builder.addNotice("user002", "doctor", "有新的预约请求");

    QJsonObject result = builder.build();
    QJsonObject notice = result["data"].toObject()["notice"].toObject();

    QCOMPARE(notice["username"].toString(), QString("user002"));
    QCOMPARE(notice["type"].toString(), QString("doctor"));
    QCOMPARE(notice["message"].toString(), QString("有新的预约请求"));

    // 验证时间格式（默认使用当前时间）
    QString timeStr = notice["time"].toString();
    QVERIFY(!timeStr.isEmpty());

    qDebug() << "添加通知信息（自定义时间）测试通过";
}

void JsonMessageBuilderTest::testAddQuestionnaire()
{
    qDebug() << "测试添加健康评估问卷";

    JsonMessageBuilder builder("SUBMIT_QUESTIONNAIRE");
    builder.addQuestionnaire("张三", "男", "30", "175", "70", "75", "120/80", "正常");

    QJsonObject result = builder.build();
    QJsonObject data = result["data"].toObject();

    // 验证问卷信息结构
    QVERIFY(data.contains("question"));
    QJsonObject question = data["question"].toObject();

    QCOMPARE(question["name"].toString(), QString("张三"));
    QCOMPARE(question["gender"].toString(), QString("男"));
    QCOMPARE(question["age"].toString(), QString("30"));
    QCOMPARE(question["height"].toString(), QString("175"));
    QCOMPARE(question["weight"].toString(), QString("70"));
    QCOMPARE(question["heart"].toString(), QString("75"));
    QCOMPARE(question["pressure"].toString(), QString("120/80"));
    QCOMPARE(question["lung"].toString(), QString("正常"));

    qDebug() << "添加健康评估问卷测试通过";
}

void JsonMessageBuilderTest::testAddAdditionalData()
{
    qDebug() << "测试添加自定义数据";

    JsonMessageBuilder builder("CUSTOM_DATA");
    builder.addAdditionalData("customKey1", "customValue1");
    builder.addAdditionalData("customKey2", "123");
    builder.addAdditionalData("customKey3", "true");

    QJsonObject result = builder.build();
    QJsonObject data = result["data"].toObject();

    QCOMPARE(data["customKey1"].toString(), QString("customValue1"));
    QCOMPARE(data["customKey2"].toString(), QString("123"));
    QCOMPARE(data["customKey3"].toString(), QString("true"));

    qDebug() << "添加自定义数据测试通过";
}

void JsonMessageBuilderTest::testClearAdditionalData()
{
    qDebug() << "测试清除额外数据";

    setupUserSession("testuser", "patient");

    JsonMessageBuilder builder("CLEAR_TEST");

    // 先添加一些数据
    builder.addAdditionalData("key1", "value1");
    builder.addPatientSimple("patient1", "患者1");

    // 清除额外数据
    builder.clearAdditionalData();

    QJsonObject result = builder.build();
    QJsonObject data = result["data"].toObject();

    // 验证基本数据仍然存在
    QCOMPARE(data["username"].toString(), QString("testuser"));
    QCOMPARE(data["type"].toString(), QString("patient"));

    // 验证额外数据被清除
    QVERIFY(!data.contains("key1"));
    QVERIFY(!data.contains("patient_0"));

    qDebug() << "清除额外数据测试通过";
}

void JsonMessageBuilderTest::testMultipleDataOperations()
{
    qDebug() << "测试多个数据操作";

    JsonMessageBuilder builder("MULTI_TEST");

    // 添加多种类型的数据
    builder.addAdditionalData("operation", "multiple");
    builder.addPatientSimple("patient001", "张三");
    builder.addDoctorSimple("doctor001", "李医生");
    builder.addNotice("user001", "patient", "测试通知");

    QJsonObject result = builder.build();
    QJsonObject data = result["data"].toObject();

    // 验证所有数据都存在
    QCOMPARE(data["operation"].toString(), QString("multiple"));
    QCOMPARE(data["patient_0"].toObject()["name"].toString(), QString("张三"));
    QCOMPARE(data["doctor_0"].toObject()["name"].toString(), QString("李医生"));
    QCOMPARE(data["notice"].toObject()["message"].toString(), QString("测试通知"));

    qDebug() << "多个数据操作测试通过";
}

void JsonMessageBuilderTest::testJsonStructureValidation()
{
    qDebug() << "测试JSON结构验证";

    JsonMessageBuilder builder("STRUCTURE_TEST");
    builder.addPatientInfo("测试用户", "男", "1990-01-01", "1234567890", "13800138000", "test@example.com");

    QJsonObject result = builder.build();

    // 验证顶层结构
    QVERIFY(result.contains("command"));
    QVERIFY(result.contains("data"));
    QVERIFY(result["command"].isString());
    QVERIFY(result["data"].isObject());

    // 验证data对象结构
    QJsonObject data = result["data"].toObject();
    QVERIFY(data.contains("username"));
    QVERIFY(data.contains("type"));
    QVERIFY(data.contains("patientInfo"));

    // 验证嵌套结构
    QJsonObject patientInfo = data["patientInfo"].toObject();
    QVERIFY(patientInfo.contains("name"));
    QVERIFY(patientInfo.contains("gender"));
    QVERIFY(patientInfo.contains("birthday"));

    qDebug() << "JSON结构验证测试通过";
}

void JsonMessageBuilderTest::testJsonStringConversion()
{
    qDebug() << "测试JSON字符串转换";

    JsonMessageBuilder builder("STRING_TEST");
    builder.addAdditionalData("测试键", "测试值");

    QString jsonString = builder.toString();

    // 验证字符串有效性
    QVERIFY(!jsonString.isEmpty());

    // 验证可以重新解析
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    QCOMPARE(error.error, QJsonParseError::NoError);

    // 验证重新解析后内容一致
    QJsonObject parsed = doc.object();
    QCOMPARE(parsed["command"].toString(), QString("STRING_TEST"));
    QCOMPARE(parsed["data"].toObject()["测试键"].toString(), QString("测试值"));

    qDebug() << "JSON字符串转换测试通过";
}

void JsonMessageBuilderTest::testEmptyParameters()
{
    qDebug() << "测试空参数处理";

    // 测试空命令
    JsonMessageBuilder builder1("");
    QJsonObject result1 = builder1.build();
    QCOMPARE(result1["command"].toString(), QString(""));

    // 测试空字符串参数
    JsonMessageBuilder builder2("EMPTY_TEST");
    builder2.addPatientInfo("", "", "", "", "", "", "");
    QJsonObject result2 = builder2.build();
    QJsonObject patientInfo = result2["data"].toObject()["patientInfo"].toObject();
    QCOMPARE(patientInfo["name"].toString(), QString(""));
    QCOMPARE(patientInfo["gender"].toString(), QString(""));

    qDebug() << "空参数处理测试通过";
}

void JsonMessageBuilderTest::testSpecialCharactersInData()
{
    qDebug() << "测试特殊字符数据处理";

    JsonMessageBuilder builder("SPECIAL_CHARS");

    // 测试包含特殊字符的数据
    QString specialText = "特殊字符测试：中文、English、123、!@#$%^&*()_+-=[]{}|;':\",./<>?";
    builder.addAdditionalData("specialText", specialText);
    builder.addPatientInfo("用户名-包含.特殊@字符", "性\\别", "生\\日", "身\"份\"证", "电'话", "邮\"箱\"");

    QJsonObject result = builder.build();
    QString jsonString = builder.toString();

    // 验证特殊字符不会破坏JSON结构
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &error);
    QCOMPARE(error.error, QJsonParseError::NoError);

    // 验证特殊字符被正确保存
    QJsonObject data = result["data"].toObject();
    QCOMPARE(data["specialText"].toString(), specialText);

    qDebug() << "特殊字符数据处理测试通过";
}

// 辅助方法实现

void JsonMessageBuilderTest::setupUserSession(const QString& username, const QString& type)
{
    UserSession& session = UserSession::instance();
    QJsonObject userInfo;
    userInfo["username"] = username;
    userInfo["name"] = "测试用户";
    session.setUserInfo(userInfo, type, username);
}

void JsonMessageBuilderTest::clearUserSession()
{
    UserSession& session = UserSession::instance();
    QJsonObject emptyInfo;
    session.setUserInfo(emptyInfo, "", "");
}

bool JsonMessageBuilderTest::validateJsonStructure(const QJsonObject& json, const QString& expectedCommand)
{
    if (!json.contains("command") || !json.contains("data")) {
        return false;
    }

    if (json["command"].toString() != expectedCommand) {
        return false;
    }

    if (!json["data"].isObject()) {
        return false;
    }

    return true;
}

QTEST_MAIN(JsonMessageBuilderTest)
#include "JsonMessageBuilder_test.moc"