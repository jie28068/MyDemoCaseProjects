#ifndef DATADICTIONARYDIALOG_H
#define DATADICTIONARYDIALOG_H

#include "KLWidgets/KCustomDialog.h"
#include "server/DataDictionary/IDataDictionaryDDXServer.h"
#include "server/DataDictionary/IDataDictionaryServer.h"
#include "server/DataDictionary/IDataDictionaryViewServer.h"
#include <QDialog>

class DataDictionaryDialog : public KCustomDialog
{
    Q_OBJECT

public:
    DataDictionaryDialog(QWidget *parent = nullptr);
    ~DataDictionaryDialog();

    static bool isShow(void) { return m_isShow; }
    void refreshNodeState();

private:
    QWidget *createDataDictionaryView();

    void setTreeExpandState(QTreeView *varTree);

private:
    static bool m_isShow;
    PIElementTreeViewController m_dataDictionaryViewController;
    QTreeView *mTree = nullptr;
};

#endif // DATADICTIONARYDIALOG_H
