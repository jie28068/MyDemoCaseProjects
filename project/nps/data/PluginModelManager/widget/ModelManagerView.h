#ifndef PluginModelManagerView_H
#define PluginModelManagerView_H

#include "CoreLib/Module.h"
#include "KLWidgets/KCustomDialog.h"
#include "ui_ModelManagerView.h"
#include <QAction>
#include <QMainWindow>

class ModelManagerView : public QMainWindow
{
    Q_OBJECT
public:
    ModelManagerView(QWidget *parent = nullptr);
    ~ModelManagerView();

public slots:
    void onTriggerModelManagerView();
    void onTreeViewClicked(QModelIndex index);

private:
    void initUI();
    void initPlatformAction();

    void showDialog(KCustomDialog *dlg);
    void initDialog(KCustomDialog *dlg);

private:
    Ui::ProjectManagerView ui;
    KCustomDialog *m_modelManagerViewDialog; // 外层Dialog，为了统一风格

    QAction *m_actModelManagerView;
};

#endif // PluginModelManagerView_H
