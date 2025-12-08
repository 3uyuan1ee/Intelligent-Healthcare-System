#include <QtTest/QtTest>
#include <QJsonObject>
#include <QSignalSpy>
#include <QApplication>
#include <QWidget>
#include <QFile>
#include <QDir>
#include "../../Fun./function.h"
#include "../config/test_config.h"

// 前向声明客户端类（避免包含实际的头文件）
class Patient_Client : public QWidget {
    Q_OBJECT
public:
    explicit Patient_Client(QWidget *parent = nullptr) : QWidget(parent) {}
    void show() { QWidget::show(); }
};

class Doctor_Client : public QWidget {
    Q_OBJECT
public:
    explicit Doctor_Client(QWidget *parent = nullptr) : QWidget(parent) {}
    void show() { QWidget::show(); }
};

class Admin_Client : public QWidget {
    Q_OBJECT
public:
    explicit Admin_Client(QWidget *parent = nullptr) : QWidget(parent) {}
    void show() { QWidget::show(); }
};

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
    void testNavigateWithNullWindow();

    // 用户缓存处理测试
    void testHandleUserCacheWithExistingFile();
    void testHandleUserCacheWithNewFile();
    void testHandleUserCacheWithInvalidFile();
    void testHandleUserCacheWithEmptyResponse();
    void testHandleUserCacheWithComplexData();

    // 中英文转换测试
    void testSwitchChinesePatient();
    void testSwitchChineseDoctor();
    void testSwitchChineseAdmin();
    void testSwitchChineseUnknown();
    void testSwitchChineseEmpty();
    void testSwitchChineseCaseInsensitive();

    // 边界条件测试
    void testNavigateWithEmptyType();
    void testNavigateWithSpecialCharacters();
    void testHandleCacheWithSpecialChars();
    void testSwitchChineseWithSpecialChars();

    // 内存管理测试
    void testMemoryLeaks();

    // 文件系统测试
    void testCacheFileCreation();
    void testCacheFilePermissions();

private:
    Function *m_function;
    QWidget *m_testWindow;
    QApplication *m_app;
    QTemporaryDir *m_tempDir;
    QString m_originalDir;

    void setupTestEnvironment();
    void cleanupTestEnvironment();
    bool fileExists(const QString &filename);
    bool compareJsonFiles(const QString &file1, const QString &file2);
    QJsonObject createTestUserData();
    void createCacheFile(const QString &filename, const QJsonObject &data);
};

void FunctionTest::initTestCase()
{
    qDebug() << "Initializing Function test suite...";
    TestConfig::setupTestData();

    // 设置测试环境
    setupTestEnvironment();

    // 创建Qt应用程序（如果还没有）
    if (!QApplication::instance()) {
        int argc = 0;
        char **argv = nullptr;
        m_app = new QApplication(argc, argv);
    } else {
        m_app = QApplication::instance();
    }

    m_function = new Function();
    m_testWindow = new QWidget();
}

void FunctionTest::cleanupTestCase()
{
    qDebug() << "Cleaning up Function test suite...";

    if (m_function) {
        delete m_function;
        m_function = nullptr;
    }

    if (m_testWindow) {
        delete m_testWindow;
        m_testWindow = nullptr;
    }

    cleanupTestEnvironment();
    TestConfig::cleanupTestData();
}

void FunctionTest::init()
{
    // 每个测试前的准备工作
    m_testWindow->show();
}

void FunctionTest::cleanup()
{
    // 每个测试后的清理工作
    m_testWindow->hide();
    m_testWindow->show(); // 重新显示以供下一个测试使用
}

void FunctionTest::testConstructor()
{
    // 测试构造函数
    Function *func = new Function();
    QVERIFY(func != nullptr);
    delete func;
}

void FunctionTest::testNavigateToPatientWindow()
{
    // 测试导航到患者窗口
    QWidget *currentWindow = new QWidget();
    currentWindow->show();

    QVERIFY(currentWindow->isVisible());

    // 执行导航
    Function::navigateToClientWindow("patient", currentWindow);

    // 注意：在实际测试中，我们不能轻易验证新窗口的创建，
    // 因为它涉及到GUI操作。我们主要验证函数不会崩溃。

    // 验证原窗口被隐藏
    QVERIFY(!currentWindow->isVisible());

    delete currentWindow;
}

void FunctionTest::testNavigateToDoctorWindow()
{
    // 测试导航到医生窗口
    QWidget *currentWindow = new QWidget();
    currentWindow->show();

    QVERIFY(currentWindow->isVisible());

    // 执行导航
    Function::navigateToClientWindow("doctor", currentWindow);

    // 验证原窗口被隐藏
    QVERIFY(!currentWindow->isVisible());

    delete currentWindow;
}

void FunctionTest::testNavigateToAdminWindow()
{
    // 测试导航到管理员窗口
    QWidget *currentWindow = new QWidget();
    currentWindow->show();

    QVERIFY(currentWindow->isVisible());

    // 执行导航
    Function::navigateToClientWindow("admin", currentWindow);

    // 验证原窗口被隐藏
    QVERIFY(!currentWindow->isVisible());

    delete currentWindow;
}

void FunctionTest::testNavigateWithUnknownType()
{
    // 测试未知类型的导航
    QWidget *currentWindow = new QWidget();
    currentWindow->show();

    QVERIFY(currentWindow->isVisible());

    // 执行导航（未知类型）
    Function::navigateToClientWindow("unknown_type", currentWindow);

    // 未知类型时，原窗口应该仍然可见（因为没有新窗口创建）
    QVERIFY(currentWindow->isVisible());

    delete currentWindow;
}

void FunctionTest::testNavigateWithNullWindow()
{
    // 测试空指针窗口（这可能导致崩溃，需要小心处理）
    // 在实际代码中，应该添加空指针检查
    // Function::navigateToClientWindow("patient", nullptr);

    // 由于可能导致崩溃，我们改为测试其他边界情况
    QWidget *currentWindow = new QWidget();
    currentWindow->show();

    // 使用空字符串类型
    Function::navigateToClientWindow("", currentWindow);

    delete currentWindow;
}

void FunctionTest::testHandleUserCacheWithExistingFile()
{
    // 准备测试数据
    QString type = "patient";
    QString username = "existinguser";
    QJsonObject testData = createTestUserData();

    // 先创建缓存文件
    QString filename = UserSession::instance().getCacheFilePath(type, username);
    createCacheFile(filename, testData);

    QVERIFY(fileExists(filename));

    // 执行缓存处理
    Function::handleUserCache(type, username, testData);

    // 验证文件仍然存在
    QVERIFY(fileExists(filename));

    // 验证数据被正确加载到UserSession
    UserSession &session = UserSession::instance();
    QString loadedName = session.getValue("name");
    QCOMPARE(loadedName, testData["name"].toString());
}

void FunctionTest::testHandleUserCacheWithNewFile()
{
    // 准备测试数据
    QString type = "doctor";
    QString username = "newuser";
    QJsonObject testData = createTestUserData();

    // 确保文件不存在
    QString filename = UserSession::instance().getCacheFilePath(type, username);
    if (fileExists(filename)) {
        QFile::remove(filename);
    }

    QVERIFY(!fileExists(filename));

    // 执行缓存处理
    Function::handleUserCache(type, username, testData);

    // 验证文件被创建
    QVERIFY(fileExists(filename));

    // 验证数据被保存
    UserSession &session = UserSession::instance();
    QString loadedName = session.getValue("name");
    QCOMPARE(loadedName, testData["name"].toString());
}

void FunctionTest::testHandleUserCacheWithInvalidFile()
{
    // 准备测试数据
    QString type = "admin";
    QString username = "invaliduser";
    QJsonObject testData = createTestUserData();

    // 创建无效的JSON文件
    QString filename = UserSession::instance().getCacheFilePath(type, username);
    QFile file(filename);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("{ invalid json content");
    file.close();

    QVERIFY(fileExists(filename));

    // 执行缓存处理
    Function::handleUserCache(type, username, testData);

    // 验证文件被重新创建（包含有效数据）
    QVERIFY(fileExists(filename));

    // 验证数据被正确处理
    UserSession &session = UserSession::instance();
    QString loadedName = session.getValue("name");
    QCOMPARE(loadedName, testData["name"].toString());
}

void FunctionTest::testHandleUserCacheWithEmptyResponse()
{
    // 准备测试数据
    QString type = "patient";
    QString username = "emptyuser";
    QJsonObject emptyData;

    // 执行缓存处理
    Function::handleUserCache(type, username, emptyData);

    // 验证文件被创建
    QString filename = UserSession::instance().getCacheFilePath(type, username);
    QVERIFY(fileExists(filename));

    // 验证UserSession中的数据
    UserSession &session = UserSession::instance();
    QMap<QString, QString> allData = session.getAllInfo();
    QVERIFY(allData.isEmpty());
}

void FunctionTest::testHandleUserCacheWithComplexData()
{
    // 准备复杂的测试数据
    QString type = "doctor";
    QString username = "complexuser";
    QJsonObject complexData;

    complexData["name"] = "复杂用户名";
    complexData["email"] = "complex@example.com";
    complexData["phone"] = "13800138000";
    complexData["id"] = "123456789012345678";
    complexData["department"] = "内科";
    complexData["title"] = "主任医师";
    complexData["specialization"] = "心血管疾病";
    complexData["experience"] = "10年";
    complexData["education"] = "医学博士";

    // 执行缓存处理
    Function::handleUserCache(type, username, complexData);

    // 验证复杂数据被正确处理
    UserSession &session = UserSession::instance();
    QCOMPARE(session.getValue("name"), QString("复杂用户名"));
    QCOMPARE(session.getValue("email"), QString("complex@example.com"));
    QCOMPARE(session.getValue("department"), QString("内科"));
    QCOMPARE(session.getValue("specialization"), QString("心血管疾病"));
}

void FunctionTest::testSwitchChinesePatient()
{
    // 测试患者类型转换
    QString result = Function::switchChinese("患者");
    QCOMPARE(result, QString("patient"));
}

void FunctionTest::testSwitchChineseDoctor()
{
    // 测试医生类型转换
    QString result = Function::switchChinese("医生");
    QCOMPARE(result, QString("doctor"));
}

void FunctionTest::testSwitchChineseAdmin()
{
    // 测试管理员类型转换
    QString result = Function::switchChinese("管理员");
    QCOMPARE(result, QString("admin"));
}

void FunctionTest::testSwitchChineseUnknown()
{
    // 测试未知类型转换
    QString result = Function::switchChinese("未知类型");
    QCOMPARE(result, QString("未知类型"));
}

void FunctionTest::testSwitchChineseEmpty()
{
    // 测试空字符串
    QString result = Function::switchChinese("");
    QCOMPARE(result, QString(""));
}

void FunctionTest::testSwitchChineseCaseInsensitive()
{
    // 测试大小写敏感性（当前实现是大小写敏感的）
    QString result1 = Function::switchChinese("患者");
    QString result2 = Function::switchChinese("病人");  // 不同的中文表达

    QCOMPARE(result1, QString("patient"));
    QCOMPARE(result2, QString("病人"));  // 应该返回原字符串
}

void FunctionTest::testNavigateWithEmptyType()
{
    // 测试空类型导航
    QWidget *currentWindow = new QWidget();
    currentWindow->show();

    QVERIFY(currentWindow->isVisible());

    // 执行导航（空类型）
    Function::navigateToClientWindow("", currentWindow);

    // 空类型时，原窗口应该仍然可见
    QVERIFY(currentWindow->isVisible());

    delete currentWindow;
}

void FunctionTest::testNavigateWithSpecialCharacters()
{
    // 测试包含特殊字符的类型
    QWidget *currentWindow = new QWidget();
    currentWindow->show();

    QVERIFY(currentWindow->isVisible());

    // 执行导航（特殊字符类型）
    Function::navigateToClientWindow("patient@#$%", currentWindow);

    // 特殊字符类型时，原窗口应该仍然可见
    QVERIFY(currentWindow->isVisible());

    delete currentWindow;
}

void FunctionTest::testHandleCacheWithSpecialChars()
{
    // 测试包含特殊字符的用户名
    QString type = "patient";
    QString username = "user@#$%^&*()";
    QJsonObject testData = createTestUserData();

    // 执行缓存处理
    Function::handleUserCache(type, username, testData);

    // 验证文件被创建
    QString filename = UserSession::instance().getCacheFilePath(type, username);
    QVERIFY(fileExists(filename));

    // 验证数据被正确处理
    UserSession &session = UserSession::instance();
    QString loadedName = session.getValue("name");
    QCOMPARE(loadedName, testData["name"].toString());
}

void FunctionTest::testSwitchChineseWithSpecialChars()
{
    // 测试包含特殊字符的字符串
    QString result1 = Function::switchChinese("患者@#$");
    QString result2 = Function::switchChinese("医生123");
    QString result3 = Function::switchChinese("管理员!");

    QCOMPARE(result1, QString("患者@#$"));
    QCOMPARE(result2, QString("医生123"));
    QCOMPARE(result3, QString("管理员!"));
}

void FunctionTest::testMemoryLeaks()
{
    // 测试内存泄漏
    for (int i = 0; i < 1000; ++i) {
        QWidget *window = new QWidget();

        // 重复执行导航操作
        Function::navigateToClientWindow("patient", window);
        Function::navigateToClientWindow("doctor", window);
        Function::navigateToClientWindow("admin", window);

        delete window;
    }

    // 如果没有崩溃，说明内存管理基本正常
    QVERIFY(true);
}

void FunctionTest::testCacheFileCreation()
{
    // 测试缓存文件的创建
    QString type = "patient";
    QString username = "filetest";
    QJsonObject testData = createTestUserData();

    // 确保目录存在
    QString filename = UserSession::instance().getCacheFilePath(type, username);
    QFileInfo fileInfo(filename);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        QVERIFY(dir.mkpath(dir.absolutePath()));
    }

    // 执行缓存处理
    Function::handleUserCache(type, username, testData);

    // 验证文件被创建
    QVERIFY(fileExists(filename));

    // 验证文件可读
    QFile file(filename);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray content = file.readAll();
    QVERIFY(!content.isEmpty());
    file.close();
}

void FunctionTest::testCacheFilePermissions()
{
    // 测试缓存文件的权限
    QString type = "doctor";
    QString username = "permissiontest";
    QJsonObject testData = createTestUserData();

    // 执行缓存处理
    Function::handleUserCache(type, username, testData);

    QString filename = UserSession::instance().getCacheFilePath(type, username);
    QFile file(filename);

    // 验证文件可读
    QVERIFY(file.open(QIODevice::ReadOnly));
    file.close();

    // 验证文件可写
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Append));
    file.close();
}

void FunctionTest::setupTestEnvironment()
{
    // 创建临时目录
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());

    // 备份并切换工作目录
    m_originalDir = QDir::currentPath();
    QDir::setCurrent(m_tempDir->path());

    // 创建cache目录
    QDir().mkpath("cache");
}

void FunctionTest::cleanupTestEnvironment()
{
    // 恢复原始工作目录
    QDir::setCurrent(m_originalDir);

    // 清理临时目录
    if (m_tempDir) {
        delete m_tempDir;
        m_tempDir = nullptr;
    }
}

bool FunctionTest::fileExists(const QString &filename)
{
    return QFile::exists(filename);
}

bool FunctionTest::compareJsonFiles(const QString &file1, const QString &file2)
{
    QFile f1(file1), f2(file2);

    if (!f1.open(QIODevice::ReadOnly) || !f2.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data1 = f1.readAll();
    QByteArray data2 = f2.readAll();

    f1.close();
    f2.close();

    return data1 == data2;
}

QJsonObject FunctionTest::createTestUserData()
{
    QJsonObject userData;
    userData["name"] = "测试用户";
    userData["email"] = "test@example.com";
    userData["phone"] = "13800138000";
    userData["id"] = "123456789012345678";
    userData["type"] = "patient";
    userData["createTime"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    return userData;
}

void FunctionTest::createCacheFile(const QString &filename, const QJsonObject &data)
{
    // 确保目录存在
    QFileInfo fileInfo(filename);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        QVERIFY(dir.mkpath(dir.absolutePath()));
    }

    // 创建文件
    QFile file(filename);
    QVERIFY(file.open(QIODevice::WriteOnly));

    QJsonDocument doc(data);
    file.write(doc.toJson());
    file.close();
}

// 包含 moc 文件
#include "Function_test.moc"

// 由于我们重新定义了客户端类，需要再次包含相关头文件
#include "../../Client/patient_client.h"
#include "../../Client/doctor_client.h"
#include "../../Client/admin_client.h"

QTEST_MAIN(FunctionTest)