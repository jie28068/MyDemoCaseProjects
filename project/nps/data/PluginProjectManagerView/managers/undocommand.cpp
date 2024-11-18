#include "undocommand.h"

TableUndoCommand::TableUndoCommand(TableModelDataModel *ptablemodel, const QList<OldNewDataStruct> &modifylist,
                                   bool bChangedByOut, QUndoCommand *parent /*= nullptr*/)
    : QUndoCommand(parent),
      m_pTableModelDataModel(ptablemodel),
      m_modifyList(modifylist),
      m_bChangedByOut(bChangedByOut),
      m_isFirstRedo(true)
{
}

TableUndoCommand::~TableUndoCommand() { }

void TableUndoCommand::undo()
{
    modifyTable(false);
}

void TableUndoCommand::redo()
{
    if (m_bChangedByOut) {
        if (m_isFirstRedo) {
            m_isFirstRedo = false;
            return;
        } else {
            modifyTable(true);
        }
    } else {
        modifyTable(true);
    }
}

void TableUndoCommand::modifyTable(bool bredo)
{
    if (m_pTableModelDataModel != nullptr) {
        m_pTableModelDataModel->modifyData(m_modifyList, bredo);
    }
}