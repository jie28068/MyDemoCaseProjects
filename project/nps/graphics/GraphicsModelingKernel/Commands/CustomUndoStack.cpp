#include "CustomUndoStack.h"

CustomUndoStack::CustomUndoStack(QObject *parent) : QUndoStack(parent)
{
    isIgnoreCommand = false;
}

void CustomUndoStack::push(QUndoCommand *cmd)
{
    if (isIgnoreCommand) {
        return;
    }
    QUndoStack::push(cmd);
}

void CustomUndoStack::setIgnoreCommandFlag(bool ignore)
{
    isIgnoreCommand = ignore;
}

bool CustomUndoStack::getIsIgnoreCommand()
{
    return isIgnoreCommand;
}
