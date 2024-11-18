#ifndef DRAGDATARESOLVER_H
#define DRAGDATARESOLVER_H
#include <QString>
#include <QStringList>

class DragDataResolver
{

public:
    enum DataType { DataType_Invalid = -1, DataType_Item = 0, DataType_Variable = 1, DataType_MultiVariable = 2 };
    enum VariableType {
        VariableType_Invalid = -1,
        VariableType_Param = 0,
        VariableType_Signal = 1,
    };
    DragDataResolver();
    DragDataResolver(const QString &dragData, const QString &projectName);
    ~DragDataResolver();

    void setDragData(const QString &dragData);
    void setCurrentProjectName(const QString &projectName);
    QString getCurrentProjectName(void) const;

    bool isValid(void) const;

    DataType type(void) const;
    bool isItem(void) const;
    QString itemClassName(void) const;

    bool isVariable(void) const;
    QString variableName(void) const;
    QString variablePath(void) const;
    QString projectName(void) const;
    VariableType variableType(void) const;
    bool isSignal(void) const;
    bool isParam(void) const;

    int variableRows(void) const;
    int variableCols(void) const;

    bool isSameProjectName(void) const;

    ////////////////////////////////////
    bool isMultiVariable(void) const;
    QStringList variableNameList(void) const;
    QStringList variablePathList(void) const;

private:
    void reset(void);

private:
    QString m_dragData;
    DataType m_dataType;
    VariableType m_variableType;

    QString m_itemClassName;
    QString m_variableName;
    QString m_variablePath;
    QString m_projectName;
    int m_variableRows;
    int m_variableCols;

    unsigned long long m_dictionaryId;

    QString m_currentProjectName;

    bool m_isSameProjectName;

    QStringList m_variableNameList;
    QStringList m_variablePathList;
};

#endif // DRAGDATARESOLVER_H
