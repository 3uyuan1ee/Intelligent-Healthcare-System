#include <QtTest/QtTest>
#include <QApplication>
#include <QSignalSpy>
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

    // 基础功能测试
    void testInitialState();
    void testSetState();
    void testGetCurrentState();

    // 状态枚举测试
    void testAllWidgetStates();
    void testStateTransitions();

    // 边界条件测试
    void testSameStateSet();
    void testRapidStateChanges();

private:
    StateManager* m_stateManager;
};

void StateManagerTest::initTestCase()
{
    qDebug() << "StateManager测试开始";

    // 需要QApplication实例
    if (!QApplication::instance()) {
        int argc = 0;
        char* argv[] = { nullptr };
        new QApplication(argc, argv);
    }
}

void StateManagerTest::cleanupTestCase()
{
    qDebug() << "StateManager测试完成";
}

void StateManagerTest::init()
{
    // 获取StateManager单例实例
    m_stateManager = &StateManager::instance();
    QVERIFY(m_stateManager != nullptr);

    // 重置到初始状态
    m_stateManager->setState(WidgetState::Idle);
}

void StateManagerTest::cleanup()
{
    // 清理：重置到初始状态
    if (m_stateManager) {
        m_stateManager->setState(WidgetState::Idle);
    }
}

void StateManagerTest::testSingletonPattern()
{
    qDebug() << "测试单例模式";

    // 获取多个实例引用
    StateManager& instance1 = StateManager::instance();
    StateManager& instance2 = StateManager::instance();

    // 验证是同一个实例
    QCOMPARE(&instance1, &instance2);
    QCOMPARE(&instance1, m_stateManager);

    // 验证内存地址相同
    QVERIFY(&instance1 == &instance2);

    qDebug() << "单例模式测试通过";
}

void StateManagerTest::testInstanceConsistency()
{
    qDebug() << "测试实例一致性";

    // 多次调用应该返回同一个实例
    for (int i = 0; i < 10; ++i) {
        StateManager& instance = StateManager::instance();
        QCOMPARE(&instance, m_stateManager);
    }

    qDebug() << "实例一致性测试通过";
}

void StateManagerTest::testInitialState()
{
    qDebug() << "测试初始状态";

    // 创建新的StateManager实例来测试初始状态
    // 由于是单例，我们重置到已知状态然后重新检查
    m_stateManager->setState(WidgetState::Idle);
    WidgetState initialState = m_stateManager->currentState();

    // 根据StateManager.cpp的实现，初始状态应该是Idle
    QCOMPARE(initialState, WidgetState::Idle);

    qDebug() << "初始状态测试通过";
}

void StateManagerTest::testSetState()
{
    qDebug() << "测试设置状态";

    // 测试设置不同状态
    m_stateManager->setState(WidgetState::LoggingIn);
    QCOMPARE(m_stateManager->currentState(), WidgetState::LoggingIn);

    m_stateManager->setState(WidgetState::LoggedIn);
    QCOMPARE(m_stateManager->currentState(), WidgetState::LoggedIn);

    m_stateManager->setState(WidgetState::Registing);
    QCOMPARE(m_stateManager->currentState(), WidgetState::Registing);

    m_stateManager->setState(WidgetState::waitRecivePatientInfo);
    QCOMPARE(m_stateManager->currentState(), WidgetState::waitRecivePatientInfo);

    m_stateManager->setState(WidgetState::Error);
    QCOMPARE(m_stateManager->currentState(), WidgetState::Error);

    // 重置到初始状态
    m_stateManager->setState(WidgetState::Idle);

    qDebug() << "设置状态测试通过";
}

void StateManagerTest::testGetCurrentState()
{
    qDebug() << "测试获取当前状态";

    // 设置一个已知状态
    m_stateManager->setState(WidgetState::LoggedIn);

    // 获取当前状态
    WidgetState currentState = m_stateManager->currentState();

    // 验证状态正确
    QCOMPARE(currentState, WidgetState::LoggedIn);

    // 测试多个不同的状态
    QList<WidgetState> testStates = {
        WidgetState::Idle,
        WidgetState::LoggingIn,
        WidgetState::LoggedIn,
        WidgetState::Registing,
        WidgetState::waitRecivePatientInfo,
        WidgetState::waitReciveDoctorInfo,
        WidgetState::waitRecivePatientList,
        WidgetState::waitReciveDoctorList,
        WidgetState::waitRecivePatientInfoEdit,
        WidgetState::waitReciveDoctorInfoEdit,
        WidgetState::waitRecivePatientAppointment,
        WidgetState::waitReciveDoctorAppointment,
        WidgetState::waitRecivePatientAppointmentJudge,
        WidgetState::waitRecivePatientAppointmentList,
        WidgetState::waitReciveDoctorAppointmentList,
        WidgetState::waitReciveAvailableDoctortList,
        WidgetState::waitRecivePatientCaseList,
        WidgetState::waitReciveDoctorCaseList,
        WidgetState::waitRecivePatientAdviceList,
        WidgetState::waitReciveDoctorAdviceList,
        WidgetState::waitRecivePatientQuestionResult,
        WidgetState::waitReciveDoctorCheckin,
        WidgetState::waitReciveDoctorAbcenceJudge,
        WidgetState::waitReciveDoctorAttendance,
        WidgetState::waitReciveDoctorCase,
        WidgetState::waitReciveDoctorCaseJudge,
        WidgetState::waitReciveDoctorAdvice,
        WidgetState::waitReciveDoctorAdviceJudge,
        WidgetState::waitReciveAdminChart,
        WidgetState::waitAdminReciveDoctorList,
        WidgetState::waitAdminReciveCheckin,
        WidgetState::Error
    };

    for (WidgetState state : testStates) {
        m_stateManager->setState(state);
        QCOMPARE(m_stateManager->currentState(), state);
    }

    // 重置到初始状态
    m_stateManager->setState(WidgetState::Idle);

    qDebug() << "获取当前状态测试通过";
}

void StateManagerTest::testAllWidgetStates()
{
    qDebug() << "测试所有WidgetState枚举值";

    // 确保所有状态枚举值都可以正确设置和获取
    QList<WidgetState> allStates = {
        WidgetState::Idle,
        WidgetState::LoggingIn,
        WidgetState::LoggedIn,
        WidgetState::Registing,
        WidgetState::waitRecivePatientInfo,
        WidgetState::waitReciveDoctorInfo,
        WidgetState::waitRecivePatientList,
        WidgetState::waitReciveDoctorList,
        WidgetState::waitRecivePatientInfoEdit,
        WidgetState::waitReciveDoctorInfoEdit,
        WidgetState::waitRecivePatientAppointment,
        WidgetState::waitReciveDoctorAppointment,
        WidgetState::waitRecivePatientAppointmentJudge,
        WidgetState::waitRecivePatientAppointmentList,
        WidgetState::waitReciveDoctorAppointmentList,
        WidgetState::waitReciveAvailableDoctortList,
        WidgetState::waitRecivePatientCaseList,
        WidgetState::waitReciveDoctorCaseList,
        WidgetState::waitRecivePatientAdviceList,
        WidgetState::waitReciveDoctorAdviceList,
        WidgetState::waitRecivePatientQuestionResult,
        WidgetState::waitReciveDoctorCheckin,
        WidgetState::waitReciveDoctorAbcenceJudge,
        WidgetState::waitReciveDoctorAttendance,
        WidgetState::waitReciveDoctorCase,
        WidgetState::waitReciveDoctorCaseJudge,
        WidgetState::waitReciveDoctorAdvice,
        WidgetState::waitReciveDoctorAdviceJudge,
        WidgetState::waitReciveAdminChart,
        WidgetState::waitAdminReciveDoctorList,
        WidgetState::waitAdminReciveCheckin,
        WidgetState::waitdie,
        WidgetState::waitdie_2,
        WidgetState::waitdie_3,
        WidgetState::waitdie_4,
        WidgetState::waitdie_5,
        WidgetState::waitdie_6,
        WidgetState::waitdie_7,
        WidgetState::waitdie_8,
        WidgetState::waitdie_9,
        WidgetState::Error
    };

    // 验证每个状态都可以设置
    for (WidgetState state : allStates) {
        m_stateManager->setState(state);
        QCOMPARE(m_stateManager->currentState(), state);

        // 验证状态是唯一的（每个状态值应该不同）
        // 注意：这里我们主要验证状态设置功能正常工作
    }

    // 重置到初始状态
    m_stateManager->setState(WidgetState::Idle);

    qDebug() << "所有WidgetState枚举值测试通过";
}

void StateManagerTest::testStateTransitions()
{
    qDebug() << "测试状态转换";

    // 测试一些常见的状态转换序列

    // 登录流程
    m_stateManager->setState(WidgetState::Idle);
    QCOMPARE(m_stateManager->currentState(), WidgetState::Idle);

    m_stateManager->setState(WidgetState::LoggingIn);
    QCOMPARE(m_stateManager->currentState(), WidgetState::LoggingIn);

    m_stateManager->setState(WidgetState::LoggedIn);
    QCOMPARE(m_stateManager->currentState(), WidgetState::LoggedIn);

    // 患者信息查看流程
    m_stateManager->setState(WidgetState::waitRecivePatientInfo);
    QCOMPARE(m_stateManager->currentState(), WidgetState::waitRecivePatientInfo);

    // 预约流程
    m_stateManager->setState(WidgetState::waitRecivePatientAppointment);
    QCOMPARE(m_stateManager->currentState(), WidgetState::waitRecivePatientAppointment);

    m_stateManager->setState(WidgetState::waitRecivePatientAppointmentJudge);
    QCOMPARE(m_stateManager->currentState(), WidgetState::waitRecivePatientAppointmentJudge);

    // 错误状态
    m_stateManager->setState(WidgetState::Error);
    QCOMPARE(m_stateManager->currentState(), WidgetState::Error);

    // 恢复到空闲状态
    m_stateManager->setState(WidgetState::Idle);
    QCOMPARE(m_stateManager->currentState(), WidgetState::Idle);

    qDebug() << "状态转换测试通过";
}

void StateManagerTest::testSameStateSet()
{
    qDebug() << "测试设置相同状态";

    // 设置初始状态
    m_stateManager->setState(WidgetState::LoggedIn);
    QCOMPARE(m_stateManager->currentState(), WidgetState::LoggedIn);

    // 多次设置相同状态
    m_stateManager->setState(WidgetState::LoggedIn);
    QCOMPARE(m_stateManager->currentState(), WidgetState::LoggedIn);

    m_stateManager->setState(WidgetState::LoggedIn);
    QCOMPARE(m_stateManager->currentState(), WidgetState::LoggedIn);

    // 验证状态没有改变
    QCOMPARE(m_stateManager->currentState(), WidgetState::LoggedIn);

    qDebug() << "设置相同状态测试通过";
}

void StateManagerTest::testRapidStateChanges()
{
    qDebug() << "测试快速状态变化";

    // 快速连续设置不同状态
    QList<WidgetState> rapidStates = {
        WidgetState::Idle,
        WidgetState::LoggingIn,
        WidgetState::LoggedIn,
        WidgetState::waitRecivePatientInfo,
        WidgetState::waitRecivePatientAppointment,
        WidgetState::Error,
        WidgetState::Idle
    };

    for (WidgetState state : rapidStates) {
        m_stateManager->setState(state);
        QCOMPARE(m_stateManager->currentState(), state);
    }

    // 再次快速循环设置状态
    for (int i = 0; i < 10; ++i) {
        m_stateManager->setState(WidgetState::LoggedIn);
        QCOMPARE(m_stateManager->currentState(), WidgetState::LoggedIn);

        m_stateManager->setState(WidgetState::waitRecivePatientList);
        QCOMPARE(m_stateManager->currentState(), WidgetState::waitRecivePatientList);

        m_stateManager->setState(WidgetState::Error);
        QCOMPARE(m_stateManager->currentState(), WidgetState::Error);
    }

    // 重置到初始状态
    m_stateManager->setState(WidgetState::Idle);

    qDebug() << "快速状态变化测试通过";
}

QTEST_MAIN(StateManagerTest)
#include "StateManager_test.moc"