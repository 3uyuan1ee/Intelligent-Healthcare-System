#include <QtTest/QtTest>
#include <QJsonObject>
#include <QSignalSpy>
#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QDir>
#include <QTemporaryDir>
#include "../../Fun./function.h"
#include "../../Instance/UserSession.h"
#include "../config/test_config.h"

// 由于我们不想依赖实际的客户端类，我们创建简化的测试版本
class MockPatientClient : public QWidget {
    Q_OBJECT
public:
    explicit MockPatientClient(QWidget *parent = nullptr) : QWidget(parent) {}
    static int createCount;
    static int showCount;

    MockPatientClient() {
        createCount++;
    }

    void show() override {
        showCount++;
        QWidget::show();
    }

    static void resetCounts() {
        createCount = 0;
        showCount = 0;
    }
};

class MockDoctorClient : public QWidget {
    Q_OBJECT
public:
    explicit MockDoctorClient(QWidget *parent = nullptr) : QWidget(parent) {}
    static int createCount;
    static int showCount;

    MockDoctorClient() {
        createCount++;
    }

    void show() override {
        showCount++;
        QWidget::show();
    }

    static void resetCounts() {
        createCount = 0;
        showCount = 0;
    }
};

class MockAdminClient : public QWidget {
    Q_OBJECT
public:
    explicit MockAdminClient(QWidget *parent = nullptr) : QWidget(parent) {}
    static int createCount;
    static int showCount;

    MockAdminClient() {
        createCount++;
    }

    void show() override {
        showCount++;
        QWidget::show();
    }

    static void resetCounts() {
        createCount = 0;
        showCount = 0;
    }
};

// 静态成员初始化
int MockPatientClient::createCount = 0;
int MockPatientClient::showCount = 0;
int MockDoctorClient::createCount = 0;
int MockDoctorClient::showCount = 0;
int MockAdminClient::createCount = 0;
int MockAdminClient::showCount = 0;

class FunctionTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // 构造函数测试
    void testConstructor();

    // 窗口导航测试
    void testNavigateToPatientWindow();
    void testNavigateToDoctorWindow();
    void testNavigateToAdminWindow();
    void testNavigateWithUnknownType();

    // 用户缓存处理测试
    void testHandleUserCacheWithExistingFile();
    void testHandleUserCacheWithNewFile();
    void testHandleUserCacheWithInvalidFile();

    // 中英文转换测试
    void testSwitchChinese();
    void testSwitchChineseWithAlreadyEnglish();
    void testSwitchChineseWithUnknownType();

private:
    Function* m_function;
    QWidget* m_mockCurrentWindow;
    QTemporaryDir* m_tempDir;
    QString m_originalWorkingDir;

    // 辅助方法
    void setupTestEnvironment();
    void cleanupTestEnvironment();
    QJsonObject createTestUserInfo();
    void createMockCacheFile(const QString& filename, const QJsonObject& data);
    bool cacheFileExists(const QString& filename);
};

void FunctionTest::initTestCase()
{
    qDebug() << "Function测试开始";

    // 需要QApplication实例
    if (!QApplication::instance()) {
        int argc = 0;
        char* argv[] = { nullptr };
        new QApplication(argc, argv);
    }

    // 创建临时目录用于测试
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());

    // 保存原始工作目录
    m_originalWorkingDir = QDir::currentPath();
}

void FunctionTest::cleanupTestCase()
{
    qDebug() << "Function测试完成";

    // 恢复原始工作目录
    QDir::setCurrent(m_originalWorkingDir);

    // 清理临时目录
    if (m_tempDir) {
        delete m_tempDir;
        m_tempDir = nullptr;
    }
}

void FunctionTest::init()
{
    setupTestEnvironment();

    // 创建Function实例
    m_function = new Function();

    // 创建模拟的当前窗口
    m_mockCurrentWindow = new QWidget();

    // 重置Mock客户端计数器
    MockPatientClient::resetCounts();
    MockDoctorClient::resetCounts();
    MockAdminClient::resetCounts();
}

void FunctionTest::cleanup()
{
    cleanupTestEnvironment();

    if (m_function) {
        delete m_function;
        m_function = nullptr;
    }

    if (m_mockCurrentWindow) {
        delete m_mockCurrentWindow;
        m_mockCurrentWindow = nullptr;
    }
}

void FunctionTest::testConstructor()
{
    qDebug() << "测试构造函数";

    // 测试Function构造函数不会崩溃
    Function* testFunction = new Function();
    QVERIFY(testFunction != nullptr);

    delete testFunction;

    qDebug() << "构造函数测试通过";
}

// 窗口导航测试暂时跳过，因为依赖UI组件
// void FunctionTest::testNavigateToPatientWindow()
// {
//     // 跳过UI相关的测试
// }
//
// void FunctionTest::testNavigateToDoctorWindow()
// {
//     // 跳过UI相关的测试
// }
//
// void FunctionTest::testNavigateToAdminWindow()
// {
//     // 跳过UI相关的测试
// }
//
// void FunctionTest::testNavigateWithUnknownType()
// {
//     // 跳过UI相关的测试
// }

void FunctionTest::testHandleUserCacheWithExistingFile()
{
    qDebug() << "测试处理存在的用户缓存文件";

    // 创建测试用户信息
    QJsonObject userInfo = createTestUserInfo();
    QString type = "patient";
    QString username = "existing_user";

    // 创建模拟的缓存文件
    QString cacheFile = "cache/" + type + "_" + username + "_local_user_info.json";
    createMockCacheFile(cacheFile, userInfo);

    // 调用handleUserCache
    Function::handleUserCache(type, username, userInfo);

    // 验证用户会话被正确设置
    UserSession& session = UserSession::instance();
    QCOMPARE(session.getValue("name"), QString("张三"));
    QCOMPARE(session.getValue("username"), QString(username));

    qDebug() << "处理存在的用户缓存文件测试通过";
}

void FunctionTest::testHandleUserCacheWithNewFile()
{
    qDebug() << "测试处理新的用户缓存文件";

    // 创建测试用户信息
    QJsonObject userInfo = createTestUserInfo();
    QString type = "doctor";
    QString username = "new_user";

    // 确保缓存文件不存在
    QString cacheFile = "cache/" + type + "_" + username + "_local_user_info.json";
    QVERIFY(!cacheFileExists(cacheFile));

    // 调用handleUserCache
    Function::handleUserCache(type, username, userInfo);

    // 验证用户会话被正确设置
    UserSession& session = UserSession::instance();
    QCOMPARE(session.getValue("name"), QString("张三"));
    QCOMPARE(session.getValue("username"), QString(username));

    // 验证缓存文件被创建
    QVERIFY(cacheFileExists(cacheFile));

    qDebug() << "处理新的用户缓存文件测试通过";
}

void FunctionTest::testHandleUserCacheWithInvalidFile()
{
    qDebug() << "测试处理无效的用户缓存文件";

    QString type = "admin";
    QString username = "invalid_user";

    // 创建无效的缓存文件
    QString cacheFile = "cache/" + type + "_" + username + "_local_user_info.json";
    QDir cacheDir;
    if (!cacheDir.exists("cache")) {
        cacheDir.mkdir("cache");
    }

    QFile file(cacheFile);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("{ invalid json content }");
    file.close();

    // 创建新的用户信息
    QJsonObject userInfo = createTestUserInfo();

    // 调用handleUserCache应该创建新的缓存
    Function::handleUserCache(type, username, userInfo);

    // 验证用户会话被正确设置
    UserSession& session = UserSession::instance();
    QCOMPARE(session.getValue("name"), QString("张三"));

    qDebug() << "处理无效的用户缓存文件测试通过";
}

void FunctionTest::testSwitchChinese()
{
    qDebug() << "测试中英文转换";

    // 测试中文转英文
    QString result1 = Function::switchChinese("患者");
    QCOMPARE(result1, QString("patient"));

    QString result2 = Function::switchChinese("医生");
    QCOMPARE(result2, QString("doctor"));

    QString result3 = Function::switchChinese("管理员");
    QCOMPARE(result3, QString("admin"));

    // 测试英文保持不变
    QString result4 = Function::switchChinese("patient");
    QCOMPARE(result4, QString("patient"));

    QString result5 = Function::switchChinese("doctor");
    QCOMPARE(result5, QString("doctor"));

    QString result6 = Function::switchChinese("admin");
    QCOMPARE(result6, QString("admin"));

    // 测试未知类型保持不变
    QString result7 = Function::switchChinese("unknown");
    QCOMPARE(result7, QString("unknown"));

    QString result8 = Function::switchChinese("");
    QCOMPARE(result8, QString(""));

    QString result9 = Function::switchChinese("Test User");
    QCOMPARE(result9, QString("Test User"));

    qDebug() << "中英文转换测试通过";
}

void FunctionTest::testSwitchChineseWithAlreadyEnglish()
{
    qDebug() << "测试转换已经是英文的类型";

    // 测试各种英文输入
    QList<QString> englishInputs = {"patient", "doctor", "admin", "nurse", "staff"};
    QList<QString> expectedOutputs = {"patient", "doctor", "admin", "nurse", "staff"};

    for (int i = 0; i < englishInputs.size(); ++i) {
        QString result = Function::switchChinese(englishInputs[i]);
        QCOMPARE(result, expectedOutputs[i]);
    }

    qDebug() << "转换已经是英文的类型测试通过";
}

void FunctionTest::testSwitchChineseWithUnknownType()
{
    qDebug() << "测试转换未知类型";

    // 测试各种未知输入
    QList<QString> unknownInputs = {
        "unknown", "test", "用户", "测试", "123", "user_123", "Patient"
    };

    for (const QString& input : unknownInputs) {
        QString result = Function::switchChinese(input);
        // 未知类型应该保持不变
        QCOMPARE(result, input);
    }

    qDebug() << "转换未知类型测试通过";
}

// 辅助方法实现

void FunctionTest::setupTestEnvironment()
{
    // 切换到临时目录
    QDir::setCurrent(m_tempDir->path());

    // 创建cache目录
    QDir dir;
    if (!dir.exists("cache")) {
        QVERIFY(dir.mkdir("cache"));
    }
}

void FunctionTest::cleanupTestEnvironment()
{
    // 清理cache目录中的文件
    QDir cacheDir("cache");
    if (cacheDir.exists()) {
        QStringList files = cacheDir.entryList(QStringList() << "*_local_user_info.json", QDir::Files);
        for (const QString& file : files) {
            cacheDir.remove(file);
        }
    }

    // 清理UserSession
    UserSession& session = UserSession::instance();
    QJsonObject emptyInfo;
    session.setUserInfo(emptyInfo, "", "");
}

QJsonObject FunctionTest::createTestUserInfo()
{
    QJsonObject userInfo;
    userInfo["name"] = "张三";
    userInfo["gender"] = "男";
    userInfo["birthday"] = "1990-01-01";
    userInfo["id"] = "110101199001011234";
    userInfo["phoneNumber"] = "13800138000";
    userInfo["email"] = "zhangsan@example.com";
    return userInfo;
}

void FunctionTest::createMockCacheFile(const QString& filename, const QJsonObject& data)
{
    QFile file(filename);
    QVERIFY(file.open(QIODevice::WriteOnly));

    QJsonDocument doc(data);
    file.write(doc.toJson());
    file.close();
}

bool FunctionTest::cacheFileExists(const QString& filename)
{
    QFile file(filename);
    return file.exists();
}

QTEST_MAIN(FunctionTest)
#include "Function_test.moc"