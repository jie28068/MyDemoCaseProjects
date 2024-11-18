#include "undostackmanager.h"

UndoStackManager::UndoStackManager() { }

UndoStackManager::~UndoStackManager() { }

UndoStackManager &UndoStackManager::instance()
{
    static UndoStackManager stack;
    return stack;
}

PUndoStack UndoStackManager::getUndoStack(const QString &stackname)
{
    if (!m_mapUndoStack.contains(stackname)) {
        QUndoStack *stack = new QUndoStack();
        stack->setUndoLimit(100);
        m_mapUndoStack[stackname] = PUndoStack(stack);
    }
    return m_mapUndoStack[stackname];
}

void UndoStackManager::releaseUndoStack(const QString &stackname)
{
    m_mapUndoStack.remove(stackname);
}

void UndoStackManager::clearAllUndoStack()
{
    m_mapUndoStack.clear();
}
