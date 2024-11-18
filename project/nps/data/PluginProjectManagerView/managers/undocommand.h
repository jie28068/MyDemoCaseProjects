#ifndef UNDOCOMMAND_H
#define UNDOCOMMAND_H

#include "TableModelDataModel.h"
#include <QUndoCommand>

class TableUndoCommand : public QUndoCommand
{
public:
    TableUndoCommand(TableModelDataModel *ptablemodel, const QList<OldNewDataStruct> &modifylist, bool bChangedByOut,
                     QUndoCommand *parent = nullptr);
    ~TableUndoCommand();

    virtual void undo();
    virtual void redo();

private:
    void modifyTable(bool bredo);

private:
    TableModelDataModel *m_pTableModelDataModel;
    QList<OldNewDataStruct> m_modifyList;
    bool m_bChangedByOut; // 是否外部修改。
    bool m_isFirstRedo;   // 如果是外部修改，初次redo，不处理，已被外部已经修改。
};

#endif // UNDOCOMMAND_H
