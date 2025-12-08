#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QThread>
#include <QMetaEnum>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>
#include "../../Instance/StateManager.h"
#include "../config/test_config.h"

class StateManagerTest : public QObject
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

    // 基础状态管理测试
    void testInitialState();
    void testSetState();
    void testGetCurrentState();

    // 状态枚举测试
    void testStateEnumValues();
    void testStateEnumCount();

    // 状态转换测试
    void testValidStateTransitions();
    void testStatePersistence();
    void testRapidStateChanges();

    // 边界状态测试
    void testIdleState();
    void testErrorState();
    void testWaitStates();

    // 线程安全测试
    void testConcurrentStateAccess();
    void testThreadSafety();

    // 内存管理测试
    void testMemoryManagement();

    // 状态语义测试
    void testLoginRelatedStates();
    void testPatientRelatedStates();
    void testDoctorRelatedStates();
    void testAdminRelatedStates();

    // 特殊状态测试
    void testSpecialWaitStates();

private:
    StateManager *m_stateManager;
    QString stateToString(WidgetState state);
    bool isValidState(WidgetState state);
    int getStateCount();
    QList<WidgetState> getAllStates();

    // 状态分组辅助方法
    QList<WidgetState> getLoginStates();
    QList<WidgetState> getPatientStates();
    QList<WidgetState> getDoctorStates();
    QList<WidgetState> getAdminStates();
    QList<WidgetState> getWaitStates();
    QList<WidgetState> getSpecialStates();
};

void StateManagerTest::initTestCase()
{
    qDebug() << "Initializing StateManager test suite...";
    TestConfig::setupTestData();

    m_stateManager = &StateManager::instance();
    QVERIFY(m_stateManager != nullptr);
}

void StateManagerTest::cleanupTestCase()
{
    qDebug() << "Cleaning up StateManager test suite...";
    TestConfig::cleanupTestData();

    // 重置状态到初始状态
    m_stateManager->setState(WidgetState::Idle);
}

void StateManagerTest::init()
{
    // 每个测试前重置状态
    m_stateManager->setState(WidgetState::Idle);
}

void StateManagerTest::cleanup()
{
    // 每个测试后清理状态
    m_stateManager->setState(WidgetState::Idle);
}

void StateManagerTest::testSingletonPattern()
{
    // 测试单例模式
    StateManager &instance1 = StateManager::instance();
    StateManager &instance2 = StateManager::instance();

    QCOMPARE(&instance1, &instance2);
    QCOMPARE(&instance1, m_stateManager);

    // 验证内存地址相同
    QVERIFY(&instance1 == &instance2);
}

void StateManagerTest::testInstanceConsistency()
{
    // 多次获取实例应该返回同一个对象
    for (int i = 0; i < 100; ++i) {
        StateManager *instance = &StateManager::instance();
        QCOMPARE(instance, m_stateManager);
    }
}

void StateManagerTest::testInitialState()
{
    // 验证初始状态
    WidgetState initialState = m_stateManager->currentState();
    QCOMPARE(initialState, WidgetState::Idle);
}

void StateManagerTest::testSetState()
{
    // 测试设置不同的状态
    WidgetState testStates[] = {
        WidgetState::LoggingIn,
        WidgetState::LoggedIn,
        WidgetState::Registing,
        WidgetState::waitRecivePatientInfo,
        WidgetState::Error
    };

    for (WidgetState state : testStates) {
        m_stateManager->setState(state);
        QCOMPARE(m_stateManager->currentState(), state);
    }
}

void StateManagerTest::testGetCurrentState()
{
    // 测试获取当前状态
    m_stateManager->setState(WidgetState::LoggingIn);
    QCOMPARE(m_stateManager->currentState(), WidgetState::LoggingIn);

    m_stateManager->setState(WidgetState::waitRecivePatientList);
    QCOMPARE(m_stateManager->currentState(), WidgetState::waitRecivePatientList);

    m_stateManager->setState(WidgetState::Error);
    QCOMPARE(m_stateManager->currentState(), WidgetState::Error);
}

void StateManagerTest::testStateEnumValues()
{
    // 测试状态枚举值
    QVERIFY(isValidState(WidgetState::Idle));
    QVERIFY(isValidState(WidgetState::LoggingIn));
    QVERIFY(isValidState(WidgetState::LoggedIn));
    QVERIFY(isValidState(WidgetState::Error));

    // 验证状态值不重复
    QList<WidgetState> allStates = getAllStates();
    QSet<int> stateValues;
    for (WidgetState state : allStates) {
        int stateValue = static_cast<int>(state);
        QVERIFY(!stateValues.contains(stateValue));
        stateValues.insert(stateValue);
    }
}

void StateManagerTest::testStateEnumCount()
{
    // 验证状态枚举的数量
    int expectedCount = 48; // 根据头文件中的枚举
    int actualCount = getStateCount();

    QCOMPARE(actualCount, expectedCount);

    // 验证所有状态都是有效的
    QList<WidgetState> allStates = getAllStates();
    QCOMPARE(allStates.size(), expectedCount);

    for (WidgetState state : allStates) {
        QVERIFY(isValidState(state));
    }
}

void StateManagerTest::testValidStateTransitions()
{
    // 测试有效的状态转换
    QList<WidgetState> validTransitions[] = {
        {WidgetState::Idle, WidgetState::LoggingIn},           // 空闲到登录
        {WidgetState::LoggingIn, WidgetState::LoggedIn},       // 登录中到已登录
        {WidgetState::LoggedIn, WidgetState::Idle},            // 已登录到空闲
        {WidgetState::Idle, WidgetState::Error},              // 任何时候都可以到错误状态
        {WidgetState::Error, WidgetState::Idle},              // 错误状态到空闲
    };

    for (const QList<WidgetState> &transition : validTransitions) {
        WidgetState fromState = transition[0];
        WidgetState toState = transition[1];

        m_stateManager->setState(fromState);
        QCOMPARE(m_stateManager->currentState(), fromState);

        m_stateManager->setState(toState);
        QCOMPARE(m_stateManager->currentState(), toState);
    }
}

void StateManagerTest::testStatePersistence()
{
    // 测试状态持久性
    m_stateManager->setState(WidgetState::waitReciveDoctorList);

    // 多次获取状态应该返回相同值
    for (int i = 0; i < 100; ++i) {
        QCOMPARE(m_stateManager->currentState(), WidgetState::waitReciveDoctorList);
    }
}

void StateManagerTest::testRapidStateChanges()
{
    // 测试快速状态切换
    WidgetState states[] = {
        WidgetState::Idle,
        WidgetState::LoggingIn,
        WidgetState::LoggedIn,
        WidgetState::waitRecivePatientInfo,
        WidgetState::waitReciveDoctorInfo,
        WidgetState::Error,
        WidgetState::Idle
    };

    for (int i = 0; i < 1000; ++i) {
        WidgetState state = states[i % 7];
        m_stateManager->setState(state);
        QCOMPARE(m_stateManager->currentState(), state);
    }
}

void StateManagerTest::testIdleState()
{
    // 测试空闲状态
    m_stateManager->setState(WidgetState::Idle);
    QCOMPARE(m_stateManager->currentState(), WidgetState::Idle);

    // 空闲状态应该可以从任何其他状态恢复
    WidgetState testStates[] = {
        WidgetState::LoggingIn,
        WidgetState::LoggedIn,
        WidgetState::waitRecivePatientList,
        WidgetState::Error
    };

    for (WidgetState state : testStates) {
        m_stateManager->setState(state);
        m_stateManager->setState(WidgetState::Idle);
        QCOMPARE(m_stateManager->currentState(), WidgetState::Idle);
    }
}

void StateManagerTest::testErrorState()
{
    // 测试错误状态
    m_stateManager->setState(WidgetState::Error);
    QCOMPARE(m_stateManager->currentState(), WidgetState::Error);

    // 从错误状态可以转换到其他状态
    m_stateManager->setState(WidgetState::Idle);
    QCOMPARE(m_stateManager->currentState(), WidgetState::Idle);
}

void StateManagerTest::testWaitStates()
{
    // 测试所有等待状态
    QList<WidgetState> waitStates = getWaitStates();

    for (WidgetState waitState : waitStates) {
        m_stateManager->setState(waitState);
        QCOMPARE(m_stateManager->currentState(), waitState);

        // 从等待状态可以转换到其他状态
        m_stateManager->setState(WidgetState::Idle);
        QCOMPARE(m_stateManager->currentState(), WidgetState::Idle);

        m_stateManager->setState(waitState);
        QCOMPARE(m_stateManager->currentState(), waitState);
    }
}

void StateManagerTest::testConcurrentStateAccess()
{
    const int threadCount = 20;
    const int operationsPerThread = 100;

    QList<QThread*> threads;

    // 创建多个线程同时访问StateManager
    for (int i = 0; i < threadCount; ++i) {
        QThread *thread = QThread::create([this, operationsPerThread, i]() {
            for (int j = 0; j < operationsPerThread; ++j) {
                // 设置状态
                WidgetState state = static_cast<WidgetState>((i * operationsPerThread + j) % getStateCount());
                m_stateManager->setState(state);

                // 读取状态
                WidgetState currentState = m_stateManager->currentState();
                QVERIFY(isValidState(currentState));

                // 短暂延迟
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

    // 验证没有崩溃
    QVERIFY(true);
}

void StateManagerTest::testThreadSafety()
{
    // 测试多线程环境下的安全性
    const int iterationCount = 1000;

    // 使用QtConcurrent进行多线程测试
    QThreadPool::globalInstance()->setMaxThreadCount(8);

    QList<QFuture<void>> futures;

    for (int i = 0; i < 8; ++i) {
        QFuture<void> future = QtConcurrent::run([this, iterationCount, i]() {
            for (int j = 0; j < iterationCount; ++j) {
                // 设置不同状态
                WidgetState state = static_cast<WidgetState>(j % getStateCount());
                m_stateManager->setState(state);

                // 验证状态有效性
                WidgetState current = m_stateManager->currentState();
                if (!isValidState(current)) {
                    QFAIL(QString("Invalid state detected in thread %1: %2")
                          .arg(i).arg(static_cast<int>(current)).toLocal8Bit());
                }

                QThread::msleep(1);
            }
        });

        futures.append(future);
    }

    // 等待所有操作完成
    for (QFuture<void> &future : futures) {
        future.waitForFinished();
    }

    // 验证最终状态有效
    WidgetState finalState = m_stateManager->currentState();
    QVERIFY(isValidState(finalState));
}

void StateManagerTest::testMemoryManagement()
{
    // 测试内存管理
    StateManager &instance = StateManager::instance();

    // 重复获取实例不应该创建新对象
    StateManager *ptr1 = &instance;
    StateManager *ptr2 = &StateManager::instance();
    StateManager *ptr3 = &StateManager::instance();

    QCOMPARE(ptr1, ptr2);
    QCOMPARE(ptr2, ptr3);

    // 测试拷贝构造和赋值操作被禁用
    // 这部分在编译时就会检查，这里我们只验证单例行为
    QVERIFY(ptr1 == m_stateManager);
}

void StateManagerTest::testLoginRelatedStates()
{
    // 测试登录相关状态
    QList<WidgetState> loginStates = getLoginStates();

    for (WidgetState state : loginStates) {
        m_stateManager->setState(state);
        QCOMPARE(m_stateManager->currentState(), state);

        // 验证状态名称（通过调试输出）
        qDebug() << "Login state:" << stateToString(state);
    }

    // 测试登录流程：Idle -> LoggingIn -> LoggedIn -> Idle
    m_stateManager->setState(WidgetState::Idle);
    QCOMPARE(m_stateManager->currentState(), WidgetState::Idle);

    m_stateManager->setState(WidgetState::LoggingIn);
    QCOMPARE(m_stateManager->currentState(), WidgetState::LoggingIn);

    m_stateManager->setState(WidgetState::LoggedIn);
    QCOMPARE(m_stateManager->currentState(), WidgetState::LoggedIn);

    m_stateManager->setState(WidgetState::Idle);
    QCOMPARE(m_stateManager->currentState(), WidgetState::Idle);
}

void StateManagerTest::testPatientRelatedStates()
{
    // 测试患者相关状态
    QList<WidgetState> patientStates = getPatientStates();

    for (WidgetState state : patientStates) {
        m_stateManager->setState(state);
        QCOMPARE(m_stateManager->currentState(), state);

        qDebug() << "Patient state:" << stateToString(state);
    }
}

void StateManagerTest::testDoctorRelatedStates()
{
    // 测试医生相关状态
    QList<WidgetState> doctorStates = getDoctorStates();

    for (WidgetState state : doctorStates) {
        m_stateManager->setState(state);
        QCOMPARE(m_stateManager->currentState(), state);

        qDebug() << "Doctor state:" << stateToString(state);
    }
}

void StateManagerTest::testAdminRelatedStates()
{
    // 测试管理员相关状态
    QList<WidgetState> adminStates = getAdminStates();

    for (WidgetState state : adminStates) {
        m_stateManager->setState(state);
        QCOMPARE(m_stateManager->currentState(), state);

        qDebug() << "Admin state:" << stateToString(state);
    }
}

void StateManagerTest::testSpecialWaitStates()
{
    // 测试特殊的等待状态（waitdie系列）
    QList<WidgetState> specialStates = getSpecialStates();

    for (WidgetState state : specialStates) {
        m_stateManager->setState(state);
        QCOMPARE(m_stateManager->currentState(), state);

        qDebug() << "Special state:" << stateToString(state);

        // 验证可以从特殊状态恢复
        m_stateManager->setState(WidgetState::Idle);
        QCOMPARE(m_stateManager->currentState(), WidgetState::Idle);
    }
}

QString StateManagerTest::stateToString(WidgetState state)
{
    // 将状态枚举转换为字符串表示
    switch (state) {
        case WidgetState::Idle: return "Idle";
        case WidgetState::LoggingIn: return "LoggingIn";
        case WidgetState::LoggedIn: return "LoggedIn";
        case WidgetState::Registing: return "Registing";
        case WidgetState::waitRecivePatientInfo: return "waitRecivePatientInfo";
        case WidgetState::waitReciveDoctorInfo: return "waitReciveDoctorInfo";
        case WidgetState::waitRecivePatientList: return "waitRecivePatientList";
        case WidgetState::waitReciveDoctorList: return "waitReciveDoctorList";
        case WidgetState::waitRecivePatientInfoEdit: return "waitRecivePatientInfoEdit";
        case WidgetState::waitReciveDoctorInfoEdit: return "waitReciveDoctorInfoEdit";
        case WidgetState::waitRecivePatientAppointment: return "waitRecivePatientAppointment";
        case WidgetState::waitReciveDoctorAppointment: return "waitReciveDoctorAppointment";
        case WidgetState::waitRecivePatientAppointmentJudge: return "waitRecivePatientAppointmentJudge";
        case WidgetState::waitRecivePatientAppointmentList: return "waitRecivePatientAppointmentList";
        case WidgetState::waitReciveDoctorAppointmentList: return "waitReciveDoctorAppointmentList";
        case WidgetState::waitReciveAvailableDoctortList: return "waitReciveAvailableDoctortList";
        case WidgetState::waitRecivePatientCaseList: return "waitRecivePatientCaseList";
        case WidgetState::waitReciveDoctorCaseList: return "waitReciveDoctorCaseList";
        case WidgetState::waitRecivePatientAdviceList: return "waitRecivePatientAdviceList";
        case WidgetState::waitReciveDoctorAdviceList: return "waitReciveDoctorAdviceList";
        case WidgetState::waitRecivePatientQuestionResult: return "waitRecivePatientQuestionResult";
        case WidgetState::waitReciveDoctorCheckin: return "waitReciveDoctorCheckin";
        case WidgetState::waitReciveDoctorAbcenceJudge: return "waitReciveDoctorAbcenceJudge";
        case WidgetState::waitReciveDoctorAttendance: return "waitReciveDoctorAttendance";
        case WidgetState::waitReciveDoctorCase: return "waitReciveDoctorCase";
        case WidgetState::waitReciveDoctorCaseJudge: return "waitReciveDoctorCaseJudge";
        case WidgetState::waitReciveDoctorAdvice: return "waitReciveDoctorAdvice";
        case WidgetState::waitReciveDoctorAdviceJudge: return "waitReciveDoctorAdviceJudge";
        case WidgetState::waitReciveAdminChart: return "waitReciveAdminChart";
        case WidgetState::waitAdminReciveDoctorList: return "waitAdminReciveDoctorList";
        case WidgetState::waitAdminReciveCheckin: return "waitAdminReciveCheckin";
        case WidgetState::waitdie: return "waitdie";
        case WidgetState::waitdie_2: return "waitdie_2";
        case WidgetState::waitdie_3: return "waitdie_3";
        case WidgetState::waitdie_4: return "waitdie_4";
        case WidgetState::waitdie_5: return "waitdie_5";
        case WidgetState::waitdie_6: return "waitdie_6";
        case WidgetState::waitdie_7: return "waitdie_7";
        case WidgetState::waitdie_8: return "waitdie_8";
        case WidgetState::waitdie_9: return "waitdie_9";
        case WidgetState::Error: return "Error";
        default: return QString("Unknown(%1)").arg(static_cast<int>(state));
    }
}

bool StateManagerTest::isValidState(WidgetState state)
{
    // 检查状态是否在有效范围内
    int stateValue = static_cast<int>(state);
    return stateValue >= 0 && stateValue < getStateCount();
}

int StateManagerTest::getStateCount()
{
    // 返回状态枚举的总数
    return 48; // 根据头文件定义
}

QList<WidgetState> StateManagerTest::getAllStates()
{
    QList<WidgetState> states;

    // 添加所有已知状态
    states << WidgetState::Idle
           << WidgetState::LoggingIn
           << WidgetState::LoggedIn
           << WidgetState::Registing
           << WidgetState::waitRecivePatientInfo
           << WidgetState::waitReciveDoctorInfo
           << WidgetState::waitRecivePatientList
           << WidgetState::waitReciveDoctorList
           << WidgetState::waitRecivePatientInfoEdit
           << WidgetState::waitReciveDoctorInfoEdit
           << WidgetState::waitRecivePatientAppointment
           << WidgetState::waitReciveDoctorAppointment
           << WidgetState::waitRecivePatientAppointmentJudge
           << WidgetState::waitRecivePatientAppointmentList
           << WidgetState::waitReciveDoctorAppointmentList
           << WidgetState::waitReciveAvailableDoctortList
           << WidgetState::waitRecivePatientCaseList
           << WidgetState::waitReciveDoctorCaseList
           << WidgetState::waitRecivePatientAdviceList
           << WidgetState::waitReciveDoctorAdviceList
           << WidgetState::waitRecivePatientQuestionResult
           << WidgetState::waitReciveDoctorCheckin
           << WidgetState::waitReciveDoctorAbcenceJudge
           << WidgetState::waitReciveDoctorAttendance
           << WidgetState::waitReciveDoctorCase
           << WidgetState::waitReciveDoctorCaseJudge
           << WidgetState::waitReciveDoctorAdvice
           << WidgetState::waitReciveDoctorAdviceJudge
           << WidgetState::waitReciveAdminChart
           << WidgetState::waitAdminReciveDoctorList
           << WidgetState::waitAdminReciveCheckin
           << WidgetState::waitdie
           << WidgetState::waitdie_2
           << WidgetState::waitdie_3
           << WidgetState::waitdie_4
           << WidgetState::waitdie_5
           << WidgetState::waitdie_6
           << WidgetState::waitdie_7
           << WidgetState::waitdie_8
           << WidgetState::waitdie_9
           << WidgetState::Error;

    return states;
}

QList<WidgetState> StateManagerTest::getLoginStates()
{
    return QList<WidgetState>() << WidgetState::Idle
                               << WidgetState::LoggingIn
                               << WidgetState::LoggedIn
                               << WidgetState::Registing;
}

QList<WidgetState> StateManagerTest::getPatientStates()
{
    return QList<WidgetState>() << WidgetState::waitRecivePatientInfo
                               << WidgetState::waitRecivePatientInfoEdit
                               << WidgetState::waitRecivePatientAppointment
                               << WidgetState::waitRecivePatientAppointmentJudge
                               << WidgetState::waitRecivePatientAppointmentList
                               << WidgetState::waitRecivePatientCaseList
                               << WidgetState::waitRecivePatientAdviceList
                               << WidgetState::waitRecivePatientQuestionResult;
}

QList<WidgetState> StateManagerTest::getDoctorStates()
{
    return QList<WidgetState>() << WidgetState::waitReciveDoctorInfo
                               << WidgetState::waitReciveDoctorInfoEdit
                               << WidgetState::waitReciveDoctorList
                               << WidgetState::waitReciveDoctorAppointment
                               << WidgetState::waitReciveDoctorAppointmentList
                               << WidgetState::waitReciveAvailableDoctortList
                               << WidgetState::waitReciveDoctorCaseList
                               << WidgetState::waitReciveDoctorAdviceList
                               << WidgetState::waitReciveDoctorCheckin
                               << WidgetState::waitReciveDoctorAbcenceJudge
                               << WidgetState::waitReciveDoctorAttendance
                               << WidgetState::waitReciveDoctorCase
                               << WidgetState::waitReciveDoctorCaseJudge
                               << WidgetState::waitReciveDoctorAdvice
                               << WidgetState::waitReciveDoctorAdviceJudge;
}

QList<WidgetState> StateManagerTest::getAdminStates()
{
    return QList<WidgetState>() << WidgetState::waitReciveAdminChart
                               << WidgetState::waitAdminReciveDoctorList
                               << WidgetState::waitAdminReciveCheckin;
}

QList<WidgetState> StateManagerTest::getWaitStates()
{
    QList<WidgetState> waitStates;
    QList<WidgetState> allStates = getAllStates();

    for (WidgetState state : allStates) {
        QString stateStr = stateToString(state);
        if (stateStr.startsWith("wait")) {
            waitStates << state;
        }
    }

    return waitStates;
}

QList<WidgetState> StateManagerTest::getSpecialStates()
{
    return QList<WidgetState>() << WidgetState::waitdie
                               << WidgetState::waitdie_2
                               << WidgetState::waitdie_3
                               << WidgetState::waitdie_4
                               << WidgetState::waitdie_5
                               << WidgetState::waitdie_6
                               << WidgetState::waitdie_7
                               << WidgetState::waitdie_8
                               << WidgetState::waitdie_9;
}

QTEST_MAIN(StateManagerTest)
#include "StateManager_test.moc"