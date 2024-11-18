#ifndef UNDOSTACKMANAGER_H
#define UNDOSTACKMANAGER_H

#include <QObject>

/// @brief 撤销重做堆栈管理
class UndoStackManager : public QObject
{
    Q_OBJECT

public:
    UndoStackManager(QObject *parent);
    ~UndoStackManager();

private:
};

#endif // UNDOSTACKMANAGER_H
