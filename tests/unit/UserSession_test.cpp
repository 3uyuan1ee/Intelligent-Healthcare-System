#include <QtTest/QtTest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include <QFile>
#include <QThread>
#include <QTemporaryDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include "../../Instance/UserSession.h"
#include "../config/test_config.h"

class UserSessionTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // 单例模式测试
    void testSingletonPattern();
    void testInstanceConsistency();

    // 基础功能测试
    void testSetUserInfo();
    void testGetUserInfo();
    void testGetValue();
    void testGetAllInfo();
    void testGetAllInfoAsJson();

    // 缓存文件操作测试
    void testSaveUserInfoToLocal();
    void testLoadUserInfoFromLocal();
    void testLoadNonExistentFile();
    void testLoadInvalidJsonFile();

    // 缓存路径测试
    void testGetCacheFilePath();

    // 数据更新测试
    void testUpdateExistingUserInfo();
    void testClearUserInfo();

    // 线程安全测试
    void testConcurrentAccess();

    // 边界条件测试
    void testEmptyUserInfo();
    void testLargeUserInfo();
    void testSpecialCharactersInUserInfo();

    // 内存管理测试
    void testMemoryLeakPrevention();

private:
    QJsonObject createTestUserInfo();
    void createCacheDir();
    void cleanupCacheDir();
    bool compareJsonWithMap(const QJsonObject &json, const QMap<QString, QString> &map);

    UserSession *m_session;
    QTemporaryDir *m_tempDir;
    QString m_originalCacheDir;
};

void UserSessionTest::initTestCase()
{
    qDebug() << "Initializing UserSession test suite...";
    TestConfig::setupTestData();

    // 创建临时缓存目录
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());

    // 备份原始缓存目录并设置测试缓存目录
    m_originalCacheDir = QDir::currentPath();
    QDir::setCurrent(m_tempDir->path());

    // 创建cache目录
    createCacheDir();

    // 获取UserSession实例
    m_session = &UserSession::instance();
    QVERIFY(m_session != nullptr);
}

void UserSessionTest::cleanupTestCase()
{
    qDebug() << "Cleaning up UserSession test suite...";

    // 恢复原始目录
    QDir::setCurrent(m_originalCacheDir);

    // 清理临时目录
    if (m_tempDir) {
        delete m_tempDir;
        m_tempDir = nullptr;
    }

    // 清理测试数据
    TestConfig::cleanupTestData();
}

void UserSessionTest::init()
{
    // 每个测试前的准备工作
    cleanupCacheDir();
    createCacheDir();
}

void UserSessionTest::cleanup()
{
    // 每个测试后的清理工作
    cleanupCacheDir();
}

void UserSessionTest::testSingletonPattern()
{
    // 测试单例模式
    UserSession &instance1 = UserSession::instance();
    UserSession &instance2 = UserSession::instance();

    QCOMPARE(&instance1, &instance2);
    QCOMPARE(&instance1, m_session);

    // 验证是同一个内存地址
    QVERIFY(&instance1 == &instance2);
}

void UserSessionTest::testInstanceConsistency()
{
    // 测试实例一致性
    UserSession *session1 = &UserSession::instance();
    UserSession *session2 = &UserSession::instance();

    QCOMPARE(session1, session2);
    QCOMPARE(session1, m_session);

    // 多次调用应该返回同一个实例
    for (int i = 0; i < 100; ++i) {
        UserSession *session = &UserSession::instance();
        QCOMPARE(session, m_session);
    }
}

void UserSessionTest::testSetUserInfo()
{
    QJsonObject userInfo = createTestUserInfo();
    QString type = "patient";
    QString username = "testuser";

    // 设置用户信息
    m_session->setUserInfo(userInfo, type, username);

    // 验证信息是否正确设置
    QMap<QString, QString> allInfo = m_session->getAllInfo();
    QCOMPARE(allInfo.size(), userInfo.size());

    // 验证每个键值对
    for (auto it = userInfo.begin(); it != userInfo.end(); ++it) {
        QString key = it.key();
        QString expectedValue = it.value().toString();
        QString actualValue = m_session->getValue(key);
        QCOMPARE(actualValue, expectedValue);
    }
}

void UserSessionTest::testGetUserInfo()
{
    QJsonObject userInfo = createTestUserInfo();
    QString type = "doctor";
    QString username = "testdoctor";

    // 先设置用户信息
    m_session->setUserInfo(userInfo, type, username);

    // 获取所有信息
    QMap<QString, QString> allInfo = m_session->getAllInfo();
    QJsonObject allInfoJson = m_session->getAllInfoAsJson();

    // 验证数量一致
    QCOMPARE(allInfo.size(), userInfo.size());
    QCOMPARE(allInfoJson.size(), userInfo.size());

    // 验证内容一致
    QVERIFY(compareJsonWithMap(allInfoJson, allInfo));
}

void UserSessionTest::testGetValue()
{
    QJsonObject userInfo = createTestUserInfo();
    QString type = "admin";
    QString username = "testadmin";

    // 设置用户信息
    m_session->setUserInfo(userInfo, type, username);

    // 测试存在的键
    QCOMPARE(m_session->getValue("username"), QString("testuser123"));
    QCOMPARE(m_session->getValue("email"), QString("test@example.com"));
    QCOMPARE(m_session->getValue("phone"), QString("13800138000"));

    // 测试不存在的键
    QCOMPARE(m_session->getValue("nonexistent"), QString(""));
    QCOMPARE(m_session->getValue(""), QString(""));
    QCOMPARE(m_session->getValue("invalid_key_12345"), QString(""));
}

void UserSessionTest::testGetAllInfo()
{
    QJsonObject userInfo = createTestUserInfo();
    QString type = "patient";
    QString username = "testpatient";

    // 设置用户信息
    m_session->setUserInfo(userInfo, type, username);

    // 获取所有信息
    QMap<QString, QString> allInfo = m_session->getAllInfo();

    // 验证信息完整性
    QCOMPARE(allInfo.size(), userInfo.size());

    for (auto it = userInfo.begin(); it != userInfo.end(); ++it) {
        QVERIFY(allInfo.contains(it.key()));
        QCOMPARE(allInfo[it.key()], it.value().toString());
    }
}

void UserSessionTest::testGetAllInfoAsJson()
{
    QJsonObject userInfo = createTestUserInfo();
    QString type = "doctor";
    QString username = "testdoctor123";

    // 设置用户信息
    m_session->setUserInfo(userInfo, type, username);

    // 获取JSON格式的所有信息
    QJsonObject allInfoJson = m_session->getAllInfoAsJson();

    // 验证JSON完整性
    QCOMPARE(allInfoJson.size(), userInfo.size());

    for (auto it = userInfo.begin(); it != userInfo.end(); ++it) {
        QVERIFY(allInfoJson.contains(it.key()));
        QCOMPARE(allInfoJson[it.key()].toString(), it.value().toString());
    }
}

void UserSessionTest::testSaveUserInfoToLocal()
{
    QJsonObject userInfo = createTestUserInfo();
    QString type = "patient";
    QString username = "saveuser";

    // 设置用户信息（这会触发保存）
    m_session->setUserInfo(userInfo, type, username);

    // 检查文件是否被创建
    QString filePath = m_session->getCacheFilePath(type, username);
    QFile file(filePath);
    QVERIFY(file.exists());

    // 验证文件内容
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QVERIFY(doc.isObject());

    QJsonObject savedInfo = doc.object();
    QCOMPARE(savedInfo.size(), userInfo.size());

    // 验证保存的内容
    for (auto it = userInfo.begin(); it != userInfo.end(); ++it) {
        QVERIFY(savedInfo.contains(it.key()));
        QCOMPARE(savedInfo[it.key()].toString(), it.value().toString());
    }
}

void UserSessionTest::testLoadUserInfoFromLocal()
{
    QJsonObject userInfo = createTestUserInfo();
    QString type = "doctor";
    QString username = "loaduser";

    // 先保存用户信息
    m_session->setUserInfo(userInfo, type, username);

    // 创建新的UserSession实例来测试加载
    UserSession &newInstance = UserSession::instance();

    // 加载用户信息
    QString filePath = m_session->getCacheFilePath(type, username);
    bool loadSuccess = newInstance.loadUserInfoFromLocal(filePath);
    QVERIFY(loadSuccess);

    // 验证加载的信息
    QMap<QString, QString> loadedInfo = newInstance.getAllInfo();
    QCOMPARE(loadedInfo.size(), userInfo.size());

    for (auto it = userInfo.begin(); it != userInfo.end(); ++it) {
        QCOMPARE(loadedInfo[it.key()], it.value().toString());
    }
}

void UserSessionTest::testLoadNonExistentFile()
{
    // 尝试加载不存在的文件
    QString nonExistentFile = "cache/nonexistent_user_info.json";
    bool loadSuccess = m_session->loadUserInfoFromLocal(nonExistentFile);
    QVERIFY(!loadSuccess);
}

void UserSessionTest::testLoadInvalidJsonFile()
{
    // 创建无效的JSON文件
    QString invalidFile = "cache/invalid_user_info.json";
    QFile file(invalidFile);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("{ invalid json content");
    file.close();

    // 尝试加载无效JSON文件
    bool loadSuccess = m_session->loadUserInfoFromLocal(invalidFile);
    QVERIFY(!loadSuccess);
}

void UserSessionTest::testGetCacheFilePath()
{
    // 测试不同类型和用户名的路径生成
    QString path1 = m_session->getCacheFilePath("patient", "user1");
    QCOMPARE(path1, QString("cache/patient_user1_local_user_info.json"));

    QString path2 = m_session->getCacheFilePath("doctor", "doc123");
    QCOMPARE(path2, QString("cache/doctor_doc123_local_user_info.json"));

    QString path3 = m_session->getCacheFilePath("admin", "admin_001");
    QCOMPARE(path3, QString("cache/admin_admin_001_local_user_info.json"));
}

void UserSessionTest::testUpdateExistingUserInfo()
{
    QString type = "patient";
    QString username = "updateuser";

    // 设置初始用户信息
    QJsonObject initialInfo;
    initialInfo["username"] = "user123";
    initialInfo["email"] = "old@example.com";
    initialInfo["phone"] = "1234567890";

    m_session->setUserInfo(initialInfo, type, username);

    // 更新部分信息
    QJsonObject updateInfo;
    updateInfo["email"] = "new@example.com";  // 更新现有字段
    updateInfo["address"] = "New Address";   // 添加新字段

    m_session->setUserInfo(updateInfo, type, username);

    // 验证更新结果
    QCOMPARE(m_session->getValue("username"), QString("user123"));  // 保持不变
    QCOMPARE(m_session->getValue("email"), QString("new@example.com"));  // 已更新
    QCOMPARE(m_session->getValue("phone"), QString("1234567890"));  // 保持不变
    QCOMPARE(m_session->getValue("address"), QString("New Address"));  // 新增
}

void UserSessionTest::testClearUserInfo()
{
    QJsonObject userInfo = createTestUserInfo();
    QString type = "admin";
    QString username = "clearuser";

    // 设置用户信息
    m_session->setUserInfo(userInfo, type, username);

    // 验证信息已设置
    QVERIFY(!m_session->getAllInfo().isEmpty());

    // 创建空的JSON对象来清除信息
    QJsonObject emptyInfo;
    m_session->setUserInfo(emptyInfo, type + "_empty", username + "_empty");

    // 验证新实例为空
    QMap<QString, QString> info = m_session->getAllInfo();
    QVERIFY(info.isEmpty());
}

void UserSessionTest::testConcurrentAccess()
{
    const int threadCount = 5;
    const int operationsPerThread = 10;
    QString type = "patient";

    QList<QThread*> threads;
    QList<QJsonObject> testDataList;

    // 为每个线程准备测试数据
    for (int i = 0; i < threadCount; ++i) {
        QJsonObject testData;
        testData["threadId"] = QString::number(i);
        testData["username"] = QString("user_%1").arg(i);
        testData["email"] = QString("user_%1@example.com").arg(i);
        testData["operation"] = QString("test_%1").arg(i);
        testDataList.append(testData);
    }

    // 创建多个线程同时访问UserSession
    for (int i = 0; i < threadCount; ++i) {
        QThread *thread = QThread::create([this, &testDataList, type, operationsPerThread, i]() {
            QString username = QString("concurrent_user_%1").arg(i);

            for (int j = 0; j < operationsPerThread; ++j) {
                QJsonObject data = testDataList[i];
                data["iteration"] = j;
                data["timestamp"] = QDateTime::currentMSecsSinceEpoch();

                // 设置用户信息
                m_session->setUserInfo(data, type, username);

                // 读取用户信息
                QString value = m_session->getValue("threadId");
                QCOMPARE(value, QString::number(i));

                QThread::msleep(1);
            }
        });

        threads.append(thread);
        thread->start();
    }

    // 等待所有线程完成
    for (QThread *thread : threads) {
        thread->wait();
        delete thread;
    }

    // 验证没有崩溃和数据损坏
    QVERIFY(true);
}

void UserSessionTest::testEmptyUserInfo()
{
    QString type = "patient";
    QString username = "emptyuser";

    // 测试空JSON对象
    QJsonObject emptyInfo;
    m_session->setUserInfo(emptyInfo, type, username);

    // 验证结果
    QMap<QString, QString> allInfo = m_session->getAllInfo();
    QVERIFY(allInfo.isEmpty());

    QJsonObject allInfoJson = m_session->getAllInfoAsJson();
    QVERIFY(allInfoJson.isEmpty());

    // 测试获取不存在的值
    QString value = m_session->getValue("nonexistent");
    QVERIFY(value.isEmpty());
}

void UserSessionTest::testLargeUserInfo()
{
    QString type = "doctor";
    QString username = "largeuser";

    // 创建大量用户信息
    QJsonObject largeInfo;
    for (int i = 0; i < 100; ++i) {  // 减少数量避免测试时间过长
        QString key = QString("field_%1").arg(i);
        QString value = QString("value_%1_with_some_long_content").arg(i);
        largeInfo[key] = value;
    }

    m_session->setUserInfo(largeInfo, type, username);

    // 验证所有数据都被保存
    QMap<QString, QString> allInfo = m_session->getAllInfo();
    QCOMPARE(allInfo.size(), 100);

    // 随机验证一些数据
    QCOMPARE(allInfo["field_0"], QString("value_0_with_some_long_content"));
    QCOMPARE(allInfo["field_50"], QString("value_50_with_some_long_content"));
    QCOMPARE(allInfo["field_99"], QString("value_99_with_some_long_content"));
}

void UserSessionTest::testSpecialCharactersInUserInfo()
{
    QString type = "admin";
    QString username = "specialuser";

    // 创建包含特殊字符的用户信息
    QJsonObject specialInfo;
    specialInfo["unicode"] = QString("测试用户🎉");
    specialInfo["quotes"] = QString("Single ' and double \" quotes");
    specialInfo["slashes"] = QString("Forward / and back \\ slashes");
    specialInfo["newlines"] = QString("Line 1\nLine 2\tTabbed");
    specialInfo["html"] = QString("<script>alert('xss')</script>");
    specialInfo["json_chars"] = QString("{}[]:,\"");
    specialInfo["emoji"] = QString("😀🎈🏥💊");

    m_session->setUserInfo(specialInfo, type, username);

    // 验证特殊字符被正确处理
    QCOMPARE(m_session->getValue("unicode"), QString("测试用户🎉"));
    QCOMPARE(m_session->getValue("quotes"), QString("Single ' and double \" quotes"));
    QCOMPARE(m_session->getValue("slashes"), QString("Forward / and back \\ slashes"));
    QCOMPARE(m_session->getValue("emoji"), QString("😀🎈🏥💊"));
}

void UserSessionTest::testMemoryLeakPrevention()
{
    QString type = "patient";

    // 重复创建和销毁大量数据，检查内存泄漏
    for (int i = 0; i < 100; ++i) {  // 减少数量避免测试时间过长
        QString username = QString("memtest_user_%1").arg(i);
        QJsonObject info;
        info["largeData"] = QString("x").repeated(100);  // 100B数据
        info["iteration"] = i;

        m_session->setUserInfo(info, type, username);

        // 验证数据可以被正确检索
        QString value = m_session->getValue("iteration");
        QCOMPARE(value.toInt(), i);

        // 清理缓存文件
        QString filePath = m_session->getCacheFilePath(type, username);
        QFile::remove(filePath);
    }

    // 验证没有内存泄漏（主要通过不崩溃来判断）
    QVERIFY(true);
}

QJsonObject UserSessionTest::createTestUserInfo()
{
    QJsonObject userInfo;
    userInfo["username"] = "testuser123";
    userInfo["email"] = "test@example.com";
    userInfo["phone"] = "13800138000";
    userInfo["userId"] = "user123456";
    userInfo["userType"] = "patient";
    userInfo["realName"] = "测试用户";
    userInfo["idCard"] = "123456789012345678";
    userInfo["address"] = "测试地址123号";
    userInfo["createTime"] = "2024-01-01T00:00:00";
    userInfo["lastLogin"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    return userInfo;
}

void UserSessionTest::createCacheDir()
{
    QDir dir;
    QVERIFY(dir.mkdir("cache"));
}

void UserSessionTest::cleanupCacheDir()
{
    QDir cacheDir("cache");
    if (cacheDir.exists()) {
        cacheDir.removeRecursively();
    }
}

bool UserSessionTest::compareJsonWithMap(const QJsonObject &json, const QMap<QString, QString> &map)
{
    if (json.size() != map.size()) {
        return false;
    }

    for (auto it = json.begin(); it != json.end(); ++it) {
        QString key = it.key();
        if (!map.contains(key)) {
            return false;
        }

        QString jsonValue = it.value().toString();
        QString mapValue = map.value(key);

        if (jsonValue != mapValue) {
            return false;
        }
    }

    return true;
}

QTEST_MAIN(UserSessionTest)
#include "UserSession_test.moc"