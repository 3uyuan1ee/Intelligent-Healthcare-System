#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

namespace TestConfig {

// 测试服务器配置
struct ServerConfig {
    QString host = "127.0.0.1";
    int port = 1437;
    bool useSSL = false;
    int timeout = 5000; // 毫秒
};

// 测试数据配置
struct TestData {
    QString testUsername = "testuser";
    QString testPassword = "testpass123";
    QString testEmail = "test@example.com";
    QString testPhone = "13800138000";
    QString testUserId = "user123";
    QString testDoctorId = "doc456";
    QString testPatientId = "pat789";
    QString testAdminId = "admin001";
};

// 测试路径配置
struct TestPaths {
    QString testDataDir = "tests/data";
    QString testCacheDir = "tests/cache";
    QString testTempDir = "tests/temp";
    QString mockDataFile = "tests/data/mock_responses.json";
    QString testConfigFile = "tests/config/test_settings.json";
};

// 性能测试配置
struct PerformanceConfig {
    int maxNetworkLatency = 1000; // 毫秒
    int maxResponseTime = 2000;   // 毫秒
    int maxMemoryUsage = 100 * 1024 * 1024; // 100MB
    int concurrentConnections = 10;
    int testIterations = 1000;
};

// 获取测试配置
ServerConfig getServerConfig();
TestData getTestData();
TestPaths getTestPaths();
PerformanceConfig getPerformanceConfig();

// 辅助函数
QJsonObject createMockLoginResponse(bool success = true);
QJsonObject createMockUserData();
QJsonObject createMockAppointmentData();
QJsonObject createMockMedicalRecordData();
QByteArray createMockNetworkResponse(const QString &command, const QJsonObject &data);

// 测试工具函数
bool compareJsonObjects(const QJsonObject &obj1, const QJsonObject &obj2, const QStringList &ignoreKeys = {});
void cleanupTestData();
void setupTestData();

} // namespace TestConfig

#endif // TEST_CONFIG_H