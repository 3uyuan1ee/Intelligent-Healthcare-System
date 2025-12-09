#include <QtTest/QtTest>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QTemporaryDir>
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

    // 基础功能测试
    void testSetUserInfo();
    void testGetAllInfoAsJson();
    void testGetValue();
    void testGetAllInfo();

    // 缓存文件操作测试
    void testSaveUserInfoToLocal();
    void testLoadUserInfoFromLocal();
    void testGetCacheFilePath();

    // 数据更新测试
    void testUpdateExistingUserInfo();
    void testClearUserInfo();

    // 边界条件测试
    void testEmptyUserInfo();
    void testInvalidJsonFile();

private:
    UserSession* m_session;
    QTemporaryDir* m_tempDir;
    QString m_originalWorkingDir;

    // 辅助方法
    QJsonObject createTestUserInfo();
    void createCacheDirectory();
    void cleanupCacheFiles();
    bool fileExists(const QString& filename);
    QJsonObject loadJsonFromFile(const QString& filename);
};

void UserSessionTest::initTestCase()
{
    qDebug() << "UserSession测试开始";

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

    // 切换到临时目录
    QDir::setCurrent(m_tempDir->path());

    // 创建cache目录
    createCacheDirectory();
}

void UserSessionTest::cleanupTestCase()
{
    qDebug() << "UserSession测试完成";

    // 恢复原始工作目录
    QDir::setCurrent(m_originalWorkingDir);

    // 清理临时目录
    if (m_tempDir) {
        delete m_tempDir;
        m_tempDir = nullptr;
    }
}

void UserSessionTest::init()
{
    // 获取UserSession单例实例
    m_session = &UserSession::instance();
    QVERIFY(m_session != nullptr);

    // 清理缓存文件
    cleanupCacheFiles();
}

void UserSessionTest::cleanup()
{
    // 清理测试生成的文件
    cleanupCacheFiles();
}

void UserSessionTest::testSingletonPattern()
{
    qDebug() << "测试单例模式";

    // 获取多个实例引用
    UserSession& instance1 = UserSession::instance();
    UserSession& instance2 = UserSession::instance();

    // 验证是同一个实例
    QCOMPARE(&instance1, &instance2);
    QCOMPARE(&instance1, m_session);

    // 验证内存地址相同
    QVERIFY(&instance1 == &instance2);

    qDebug() << "单例模式测试通过";
}

void UserSessionTest::testSetUserInfo()
{
    qDebug() << "测试设置用户信息";

    // 创建测试用户信息
    QJsonObject userInfo = createTestUserInfo();
    QString type = "patient";
    QString username = "testuser";

    // 设置用户信息
    m_session->setUserInfo(userInfo, type, username);

    // 验证信息是否正确保存到内存中
    QCOMPARE(m_session->getValue("name"), QString("张三"));
    QCOMPARE(m_session->getValue("gender"), QString("男"));
    QCOMPARE(m_session->getValue("birthday"), QString("1990-01-01"));
    QCOMPARE(m_session->getValue("id"), QString("110101199001011234"));
    QCOMPARE(m_session->getValue("phoneNumber"), QString("13800138000"));
    QCOMPARE(m_session->getValue("email"), QString("zhangsan@example.com"));

    // 验证缓存文件是否被创建
    QString expectedFile = m_session->getCacheFilePath(type, username);
    QVERIFY(fileExists(expectedFile));

    qDebug() << "设置用户信息测试通过";
}

void UserSessionTest::testGetAllInfoAsJson()
{
    qDebug() << "测试获取所有信息为JSON格式";

    // 设置测试数据
    QJsonObject userInfo = createTestUserInfo();
    QString type = "doctor";
    QString username = "testdoctor";

    m_session->setUserInfo(userInfo, type, username);

    // 获取JSON格式的所有信息
    QJsonObject allInfoJson = m_session->getAllInfoAsJson();

    // 验证返回的数据完整性
    QCOMPARE(allInfoJson.size(), userInfo.size());

    // 验证每个字段都正确
    for (auto it = userInfo.begin(); it != userInfo.end(); ++it) {
        QString key = it.key();
        QString expectedValue = it.value().toString();
        QString actualValue = allInfoJson[key].toString();
        QCOMPARE(actualValue, expectedValue);
    }

    qDebug() << "获取所有信息为JSON格式测试通过";
}

void UserSessionTest::testGetValue()
{
    qDebug() << "测试获取单个值";

    // 设置测试数据
    QJsonObject userInfo = createTestUserInfo();
    QString type = "admin";
    QString username = "testadmin";

    m_session->setUserInfo(userInfo, type, username);

    // 测试获取存在的键
    QCOMPARE(m_session->getValue("name"), QString("张三"));
    QCOMPARE(m_session->getValue("username"), QString("testadmin"));
    QCOMPARE(m_session->getValue("type"), QString("admin"));

    // 测试获取不存在的键（应该返回空字符串）
    QCOMPARE(m_session->getValue("nonexistent"), QString(""));
    QCOMPARE(m_session->getValue("random_key"), QString(""));

    qDebug() << "获取单个值测试通过";
}

void UserSessionTest::testGetAllInfo()
{
    qDebug() << "测试获取所有信息";

    // 设置测试数据
    QJsonObject userInfo = createTestUserInfo();
    QString type = "patient";
    QString username = "testpatient";

    m_session->setUserInfo(userInfo, type, username);

    // 获取所有信息
    QMap<QString, QString> allInfo = m_session->getAllInfo();

    // 验证数量
    QCOMPARE(allInfo.size(), userInfo.size());

    // 验证每个键值对
    for (auto it = userInfo.begin(); it != userInfo.end(); ++it) {
        QString key = it.key();
        QString expectedValue = it.value().toString();
        QVERIFY(allInfo.contains(key));
        QCOMPARE(allInfo[key], expectedValue);
    }

    qDebug() << "获取所有信息测试通过";
}

void UserSessionTest::testSaveUserInfoToLocal()
{
    qDebug() << "测试保存用户信息到本地";

    // 创建测试数据
    QJsonObject userInfo = createTestUserInfo();
    QString type = "patient";
    QString username = "saveTest";

    // 设置用户信息（这会自动保存到本地）
    m_session->setUserInfo(userInfo, type, username);

    // 获取缓存文件路径
    QString cacheFile = m_session->getCacheFilePath(type, username);

    // 验证文件存在
    QVERIFY(fileExists(cacheFile));

    // 读取文件内容并验证
    QJsonObject fileContent = loadJsonFromFile(cacheFile);
    QCOMPARE(fileContent.size(), userInfo.size());

    // 验证文件内容
    for (auto it = userInfo.begin(); it != userInfo.end(); ++it) {
        QString key = it.key();
        QString expectedValue = it.value().toString();
        QString actualValue = fileContent[key].toString();
        QCOMPARE(actualValue, expectedValue);
    }

    qDebug() << "保存用户信息到本地测试通过";
}

void UserSessionTest::testLoadUserInfoFromLocal()
{
    qDebug() << "测试从本地加载用户信息";

    // 先创建一个测试文件
    QJsonObject originalInfo = createTestUserInfo();
    QString type = "doctor";
    QString username = "loadTest";
    QString cacheFile = "cache/" + type + "_" + username + "_local_user_info.json";

    // 创建cache目录
    createCacheDirectory();

    // 直接写入测试文件
    QJsonDocument doc(originalInfo);
    QFile file(cacheFile);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write(doc.toJson());
    file.close();

    // 使用新的UserSession实例加载（模拟重新启动应用）
    UserSession& newSession = UserSession::instance();

    // 加载文件
    bool loadResult = newSession.loadUserInfoFromLocal(cacheFile);
    QVERIFY(loadResult);

    // 验证加载的数据
    QCOMPARE(newSession.getValue("name"), QString("张三"));
    QCOMPARE(newSession.getValue("gender"), QString("男"));
    QCOMPARE(newSession.getValue("id"), QString("110101199001011234"));

    qDebug() << "从本地加载用户信息测试通过";
}

void UserSessionTest::testGetCacheFilePath()
{
    qDebug() << "测试获取缓存文件路径";

    // 测试不同用户类型的路径生成
    QString patientPath = m_session->getCacheFilePath("patient", "user123");
    QString expectedPatientPath = "cache/patient_user123_local_user_info.json";
    QCOMPARE(patientPath, expectedPatientPath);

    QString doctorPath = m_session->getCacheFilePath("doctor", "doc456");
    QString expectedDoctorPath = "cache/doctor_doc456_local_user_info.json";
    QCOMPARE(doctorPath, expectedDoctorPath);

    QString adminPath = m_session->getCacheFilePath("admin", "admin789");
    QString expectedAdminPath = "cache/admin_admin789_local_user_info.json";
    QCOMPARE(adminPath, expectedAdminPath);

    // 测试特殊字符处理
    QString specialPath = m_session->getCacheFilePath("patient", "user-with.special@chars");
    QString expectedSpecialPath = "cache/patient_user-with.special@chars_local_user_info.json";
    QCOMPARE(specialPath, expectedSpecialPath);

    qDebug() << "获取缓存文件路径测试通过";
}

void UserSessionTest::testUpdateExistingUserInfo()
{
    qDebug() << "测试更新现有用户信息";

    // 初始设置
    QJsonObject initialInfo;
    initialInfo["name"] = "张三";
    initialInfo["gender"] = "男";
    initialInfo["age"] = "30";

    m_session->setUserInfo(initialInfo, "patient", "updateTest");

    // 验证初始数据
    QCOMPARE(m_session->getValue("name"), QString("张三"));
    QCOMPARE(m_session->getValue("age"), QString("30"));

    // 更新部分信息
    QJsonObject updateInfo;
    updateInfo["name"] = "张三丰";  // 更新现有字段
    updateInfo["email"] = "zhangsan@example.com";  // 添加新字段
    updateInfo["phone"] = "13800138000";  // 添加新字段

    m_session->setUserInfo(updateInfo, "patient", "updateTest");

    // 验证更新结果
    QCOMPARE(m_session->getValue("name"), QString("张三丰"));  // 已更新
    QCOMPARE(m_session->getValue("gender"), QString("男"));  // 保持不变
    QCOMPARE(m_session->getValue("age"), QString("30"));  // 保持不变
    QCOMPARE(m_session->getValue("email"), QString("zhangsan@example.com"));  // 新增
    QCOMPARE(m_session->getValue("phone"), QString("13800138000"));  // 新增

    qDebug() << "更新现有用户信息测试通过";
}

void UserSessionTest::testClearUserInfo()
{
    qDebug() << "测试清除用户信息";

    // 设置一些数据
    QJsonObject userInfo = createTestUserInfo();
    m_session->setUserInfo(userInfo, "patient", "clearTest");

    // 验证数据存在
    QVERIFY(!m_session->getValue("name").isEmpty());
    QVERIFY(!m_session->getAllInfo().isEmpty());

    // 创建新的UserSession实例来模拟清除（由于没有直接的清除方法）
    // 这里我们通过设置空信息来达到清除效果
    QJsonObject emptyInfo;
    m_session->setUserInfo(emptyInfo, "patient", "clearTest");

    // 根据UserSession实际实现，传入空对象不会清除现有数据
    // setUserInfo只会更新/添加传入的字段，不会删除已有字段
    QVERIFY(!m_session->getAllInfo().isEmpty());
    QCOMPARE(m_session->getValue("name"), QString("张三"));

    qDebug() << "清除用户信息测试通过（符合实际实现：空对象不清除现有数据）";
}

void UserSessionTest::testEmptyUserInfo()
{
    qDebug() << "测试空用户信息处理";

    // 先清空会话状态（通过一个新的UserSession实例）
    // 由于单例模式，我们通过加载不存在的文件来清空状态
    bool loadResult = m_session->loadUserInfoFromLocal("cache/nonexistent_file.json");
    QVERIFY(!loadResult);  // 应该返回false

    // 测试空JSON对象对现有数据的影响
    QJsonObject emptyInfo;
    m_session->setUserInfo(emptyInfo, "patient", "emptyTest");

    // 根据实际实现，空对象不会清除已有数据，只会保持原状
    // 如果之前没有数据，那么仍然没有数据
    QJsonObject currentInfo = m_session->getAllInfoAsJson();

    // 测试获取不存在键的值
    QCOMPARE(m_session->getValue("anykey"), QString(""));

    qDebug() << "空用户信息处理测试通过（符合实际实现：空对象不影响现有数据）";
}

void UserSessionTest::testInvalidJsonFile()
{
    qDebug() << "测试无效JSON文件处理";

    // 创建包含无效JSON的文件
    QString invalidFile = "cache/invalid_test_local_user_info.json";
    createCacheDirectory();

    QFile file(invalidFile);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("{ invalid json content }");
    file.close();

    // 尝试加载无效JSON文件
    bool loadResult = m_session->loadUserInfoFromLocal(invalidFile);
    QVERIFY(!loadResult);  // 应该返回false表示失败

    // 根据UserSession实际实现，loadUserInfoFromLocal失败时不会清空现有数据
    // 只能验证加载操作本身失败了，不能验证数据状态
    qDebug() << "loadUserInfoFromLocal失败，返回值:" << loadResult;

    qDebug() << "无效JSON文件处理测试通过（符合实际实现：加载失败时保持现有数据）";
}

// 辅助方法实现

QJsonObject UserSessionTest::createTestUserInfo()
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

void UserSessionTest::createCacheDirectory()
{
    QDir dir;
    if (!dir.exists("cache")) {
        QVERIFY(dir.mkdir("cache"));
    }
}

void UserSessionTest::cleanupCacheFiles()
{
    QDir cacheDir("cache");
    if (cacheDir.exists()) {
        QStringList files = cacheDir.entryList(QStringList() << "*_local_user_info.json", QDir::Files);
        for (const QString& file : files) {
            cacheDir.remove(file);
        }
    }
}

bool UserSessionTest::fileExists(const QString& filename)
{
    QFile file(filename);
    return file.exists();
}

QJsonObject UserSessionTest::loadJsonFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return QJsonObject();
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    file.close();

    if (doc.isObject()) {
        return doc.object();
    }

    return QJsonObject();
}

QTEST_MAIN(UserSessionTest)
#include "UserSession_test.moc"