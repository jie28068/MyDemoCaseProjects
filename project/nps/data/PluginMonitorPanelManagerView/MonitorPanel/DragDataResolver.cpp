#include "DragDataResolver.h"
#include <QStringList>

DragDataResolver::DragDataResolver(const QString &dragData, const QString &projectName)
    : m_dataType(DataType_Invalid),
      m_variableType(VariableType_Param),
      m_dictionaryId(0),
      m_currentProjectName(projectName),
      m_isSameProjectName(true),
      m_variableRows(1),
      m_variableCols(1)
{
    setDragData(dragData);
}

DragDataResolver::DragDataResolver()
    : m_dataType(DataType_Invalid),
      m_variableType(VariableType_Param),
      m_dictionaryId(0),
      m_isSameProjectName(true),
      m_variableRows(1),
      m_variableCols(1)
{
}

DragDataResolver::~DragDataResolver() { }

void DragDataResolver::setDragData(const QString &dragData)
{
    QString strTemp;
    QStringList strList;

    reset();
    m_dragData = dragData;
    strList = m_dragData.split("|||");
    if (strList.size() < 2) {
        return;
    }
    QString strFlag = strList[0];
    if (strFlag == "KLGraphicsItem") {
        if (strList.size() != 2) {
            return;
        }
        m_itemClassName = strList[1];
        m_dataType = DataType_Item;
    } else if (strFlag == "DictionaryDrag") {
        if (strList.size() != 3) {
            return;
        }
        m_dictionaryId = strList[1].toULongLong();
        m_variablePath = strList[2];
        m_variableName = m_variablePath;
        m_variableName.replace("Dictionary.", "");
        m_dataType = DataType_Variable;
    } else if (strFlag == "DictionaryDrags") {
        if (strList.size() < 5) {
            return;
        }
        m_dataType = DataType_MultiVariable;
        m_dictionaryId = strList[1].toULongLong();
        for (int i = 0; i < (strList.size() - 1) / 2; i++) {
            QString strTemp = strList[2 + i * 2];
            m_variablePathList << strTemp;
            strTemp.replace("Dictionary.", "");
            m_variableNameList << strTemp;
        }
    } else if (strFlag == "ProjectManagerDrag") {
        if (strList.size() != 7) {
            return;
        }
        QStringList tempList = strList[6].split(";");
        if (tempList.size() != 2)
            return;

        QStringList subList = tempList[0].split("=");
        if (subList.size() != 2)
            return;
        m_variableRows = subList[1].toInt();
        if (m_variableRows < 1)
            m_variableRows = 1;

        subList = tempList[1].split("=");
        if (subList.size() != 2)
            return;
        m_variableCols = subList[1].toInt();
        if (m_variableCols < 1)
            m_variableCols = 1;

        if (strList[3] != m_currentProjectName /* && !m_currentProjectName.isEmpty()*/) {
            // return;
            m_isSameProjectName = false;
        }
        m_variableName = strList[1];
        m_variablePath = strList[2];
        m_projectName = strList[3];
        if (strList[4] == "信号") {
            m_variableType = VariableType_Signal;
        }

        m_dataType = DataType_Variable;
    }
}

void DragDataResolver::setCurrentProjectName(const QString &projectName)
{
    m_currentProjectName = projectName;
}

QString DragDataResolver::getCurrentProjectName(void) const
{
    return m_currentProjectName;
}

bool DragDataResolver::isValid(void) const
{
    return (m_dataType != DataType_Invalid);
}

DragDataResolver::DataType DragDataResolver::type(void) const
{
    return m_dataType;
}

bool DragDataResolver::isItem(void) const
{
    return (m_dataType == DataType_Item);
}

QString DragDataResolver::itemClassName(void) const
{
    return m_itemClassName;
}

bool DragDataResolver::isVariable(void) const
{
    return (m_dataType == DataType_Variable);
}

QString DragDataResolver::variableName(void) const
{
    return m_variableName;
}

QString DragDataResolver::variablePath(void) const
{
    return m_variablePath;
}

QString DragDataResolver::projectName(void) const
{
    return m_projectName;
}

DragDataResolver::VariableType DragDataResolver::variableType(void) const
{
    return m_variableType;
}

bool DragDataResolver::isSignal(void) const
{
    return (m_variableType == VariableType_Signal);
}

bool DragDataResolver::isParam(void) const
{
    return (m_variableType == VariableType_Param);
}

int DragDataResolver::variableRows(void) const
{
    return m_variableRows;
}

int DragDataResolver::variableCols(void) const
{
    return m_variableCols;
}

bool DragDataResolver::isSameProjectName(void) const
{
    return m_isSameProjectName;
}

bool DragDataResolver::isMultiVariable(void) const
{
    return (m_dataType == DataType_MultiVariable);
}

QStringList DragDataResolver::variableNameList(void) const
{
    return m_variableNameList;
}

QStringList DragDataResolver::variablePathList(void) const
{
    return m_variablePathList;
}

void DragDataResolver::reset(void)
{
    m_dataType = DataType_Invalid;
    m_variableType = VariableType_Param;
    m_itemClassName.clear();
    m_variableName.clear();
    m_variablePath.clear();
    m_projectName.clear();
    m_dictionaryId = 0;
    m_isSameProjectName = true;
    m_variableRows = 1;
    m_variableCols = 1;
}
