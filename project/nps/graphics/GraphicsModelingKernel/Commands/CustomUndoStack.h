#pragma once

#include <QUndoStack>

class CustomUndoStack : public QUndoStack
{
    Q_OBJECT
public:
    CustomUndoStack(QObject *parent = nullptr);

    void push(QUndoCommand *cmd);

    void setIgnoreCommandFlag(bool ignore);

    bool getIsIgnoreCommand();

private:
    bool isIgnoreCommand;
};