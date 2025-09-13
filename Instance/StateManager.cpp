
#include "StateManager.h"

StateManager::StateManager(QObject *parent)
    : QObject(parent),
    m_currentState(WidgetState::Idle)  // 默认状态为 Idle
{
}

// 获取单例实例
StateManager& StateManager::instance()
{
    static StateManager instance;
    return instance;
}

WidgetState StateManager::currentState() const
{
    return m_currentState;
}

void StateManager::setState(WidgetState state)
{
    m_currentState = state;
}
