#ifndef UNDOSTACKMANAGER_H
#define UNDOSTACKMANAGER_H

#include <QMap>
#include <QSharedPointer>
#include <QUndoStack>

typedef QSharedPointer<QUndoStack> PUndoStack;

class UndoStackManager : public QObject
{
    Q_OBJECT

private:
    UndoStackManager();
    ~UndoStackManager();

public:
    static UndoStackManager &instance();
    PUndoStack getUndoStack(const QString &stackname);
    void releaseUndoStack(const QString &stackname);
    void clearAllUndoStack();

private:
    QMap<QString, PUndoStack> m_mapUndoStack;
};

#endif // UNDOSTACKMANAGER_H
