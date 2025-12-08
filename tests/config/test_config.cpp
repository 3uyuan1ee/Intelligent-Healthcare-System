#include "test_config.h"
#include <QDir>
#include <QStandardPaths>
#include <QJsonArray>
#include <QDebug>

namespace TestConfig {

ServerConfig getServerConfig() {
    return ServerConfig();
}

TestData getTestData() {
    return TestData();
}

TestPaths getTestPaths() {
    TestPaths paths;
    // 确保路径是绝对路径
    QDir currentDir;
    paths.testDataDir = currentDir.absoluteFilePath(paths.testDataDir);
    paths.testCacheDir = currentDir.absoluteFilePath(paths.testCacheDir);
    paths.testTempDir = currentDir.absoluteFilePath(paths.testTempDir);
    return paths;
}

PerformanceConfig getPerformanceConfig() {
    return PerformanceConfig();
}

QJsonObject createMockLoginResponse(bool success) {
    QJsonObject response;
    response["reply"] = success ? "success" : "failed";

    QJsonObject data;
    if (success) {
        TestData testData = getTestData();
        data["username"] = testData.testUsername;
        data["userId"] = testData.testUserId;
        data["type"] = "patient";
        data["loginTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        data["sessionToken"] = "mock_session_token_12345";
    } else {
        data["error"] = "Invalid username or password";
        data["errorCode"] = "AUTH001";
    }

    response["data"] = data;
    return response;
}

QJsonObject createMockUserData() {
    TestData testData = getTestData();

    QJsonObject user;
    user["userId"] = testData.testUserId;
    user["username"] = testData.testUsername;
    user["email"] = testData.testEmail;
    user["phone"] = testData.testPhone;
    user["type"] = "patient";
    user["status"] = "active";
    user["createTime"] = "2024-01-01T00:00:00";
    user["lastLoginTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonObject profile;
    profile["firstName"] = "Test";
    profile["lastName"] = "User";
    profile["gender"] = "male";
    profile["birthDate"] = "1990-01-01";
    profile["address"] = "Test Address, Test City";

    user["profile"] = profile;

    return user;
}

QJsonObject createMockAppointmentData() {
    TestData testData = getTestData();

    QJsonObject appointment;
    appointment["appointmentId"] = "apt_123456";
    appointment["patientId"] = testData.testPatientId;
    appointment["doctorId"] = testData.testDoctorId;
    appointment["status"] = "confirmed";
    appointment["appointmentTime"] = QDateTime::currentDateTime().addDays(1).toString(Qt::ISODate);
    appointment["duration"] = 30; // 分钟

    QJsonObject doctor;
    doctor["doctorId"] = testData.testDoctorId;
    doctor["name"] = "Dr. Test Doctor";
    doctor["specialization"] = "General Practice";
    doctor["department"] = "Internal Medicine";

    appointment["doctor"] = doctor;

    return appointment;
}

QJsonObject createMockMedicalRecordData() {
    TestData testData = getTestData();

    QJsonObject record;
    record["recordId"] = "rec_789012";
    record["patientId"] = testData.testPatientId;
    record["doctorId"] = testData.testDoctorId;
    record["createTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    record["diagnosis"] = "Common Cold";
    record["symptoms"] = "Fever, cough, sore throat";
    record["treatment"] = "Rest, hydration, medication";
    record["notes"] = "Patient should follow up in one week if symptoms persist";

    QJsonArray medications;
    QJsonObject med1;
    med1["name"] = "Paracetamol";
    med1["dosage"] = "500mg";
    med1["frequency"] = "Every 6 hours";
    med1["duration"] = "3 days";
    medications.append(med1);

    record["medications"] = medications;

    return record;
}

QByteArray createMockNetworkResponse(const QString &command, const QJsonObject &data) {
    QJsonObject response;
    response["command"] = command;
    response["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    response["data"] = data;

    QJsonDocument doc(response);
    return doc.toJson(QJsonDocument::Compact);
}

bool compareJsonObjects(const QJsonObject &obj1, const QJsonObject &obj2, const QStringList &ignoreKeys) {
    QJsonDocument doc1(obj1);
    QJsonDocument doc2(obj2);

    // 简单的字符串比较，忽略指定的键
    QString str1 = doc1.toJson(QJsonDocument::Compact);
    QString str2 = doc2.toJson(QJsonDocument::Compact);

    for (const QString &key : ignoreKeys) {
        QRegularExpression regex(QString("\"%1\":[^,}]*").arg(key));
        str1.remove(regex);
        str2.remove(regex);
    }

    return str1 == str2;
}

void cleanupTestData() {
    TestPaths paths = getTestPaths();

    QDir cacheDir(paths.testCacheDir);
    if (cacheDir.exists()) {
        cacheDir.removeRecursively();
    }

    QDir tempDir(paths.testTempDir);
    if (tempDir.exists()) {
        tempDir.removeRecursively();
    }

    // 创建必要的目录
    QDir().mkpath(paths.testCacheDir);
    QDir().mkpath(paths.testTempDir);
}

void setupTestData() {
    TestPaths paths = getTestPaths();

    // 创建测试目录
    QDir().mkpath(paths.testDataDir);
    QDir().mkpath(paths.testCacheDir);
    QDir().mkpath(paths.testTempDir);

    // 创建模拟数据文件
    QJsonObject mockData;
    mockData["login_success"] = createMockLoginResponse(true);
    mockData["login_failed"] = createMockLoginResponse(false);
    mockData["user_data"] = createMockUserData();
    mockData["appointment_data"] = createMockAppointmentData();
    mockData["medical_record_data"] = createMockMedicalRecordData();

    QJsonDocument doc(mockData);

    QFile file(paths.mockDataFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }

    // 创建测试配置文件
    QJsonObject config;
    config["serverHost"] = "127.0.0.1";
    config["serverPort"] = 1437;
    config["timeoutMs"] = 5000;
    config["enableLogging"] = true;
    config["logLevel"] = "debug";

    QJsonDocument configDoc(config);
    QFile configFile(paths.testConfigFile);
    if (configFile.open(QIODevice::WriteOnly)) {
        configFile.write(configDoc.toJson());
        configFile.close();
    }

    qDebug() << "Test data setup completed";
}

} // namespace TestConfig