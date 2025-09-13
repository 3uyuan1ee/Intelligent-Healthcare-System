
#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include <QObject>
//状态栏
enum class WidgetState {
    Idle,
    LoggingIn,
    LoggedIn,
    Registing,
    waitRecivePatientInfo,
    waitReciveDoctorInfo,
    waitRecivePatientList,
    waitReciveDoctorList,
    waitRecivePatientInfoEdit,
    waitReciveDoctorInfoEdit,
    waitRecivePatientAppointment,
    waitReciveDoctorAppointment,
    waitRecivePatientAppointmentJudge,
    waitRecivePatientAppointmentList,
    waitReciveDoctorAppointmentList,
    waitReciveAvailableDoctortList,
    waitRecivePatientCaseList,
    waitReciveDoctorCaseList,
    waitRecivePatientAdviceList,
    waitReciveDoctorAdviceList,
    waitRecivePatientQuestionResult,
    waitReciveDoctorCheckin,
    waitReciveDoctorAbcenceJudge,
    waitReciveDoctorAttendance,
    waitReciveDoctorCase,
    waitReciveDoctorCaseJudge,
    waitReciveDoctorAdvice,
    waitReciveDoctorAdviceJudge,
    waitReciveAdminChart,
    waitAdminReciveDoctorList,
    waitAdminReciveCheckin,
    waitdie,
    waitdie_2,
    waitdie_3,
    waitdie_4,
    waitdie_5,
    waitdie_6,
    waitdie_7,
    waitdie_8,
    waitdie_9,
    Error
};

class StateManager : public QObject
{
    Q_OBJECT

public:
    // 获取单例实例
    static StateManager& instance();

    // 获取当前状态
    WidgetState currentState() const;

    // 设置当前状态
    void setState(WidgetState state);


private:
    explicit StateManager(QObject *parent = nullptr);  // 私有构造函数
    StateManager(const StateManager&) = delete;        // 禁止拷贝构造
    StateManager& operator=(const StateManager&) = delete; // 禁止赋值构造

    WidgetState m_currentState;  // 当前状态
};

#endif // STATEMANAGER_H
