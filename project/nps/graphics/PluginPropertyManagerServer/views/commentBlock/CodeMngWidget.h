#ifndef CODEMNGWIDGET_H
#define CODEMNGWIDGET_H

#include "TypeItemView.h"
#include <QComboBox>
#include <QStackedWidget>
#include <QWidget>

#include "server/CodeManagerServer/ICodeManagerServer.h"

using namespace Kcc::CodeManager;

/// @brief 画板脚本窗口,代码窗口
class CodeMngWidget : public CWidget
{
    Q_OBJECT

public:
    CodeMngWidget(PDrawingBoardClass drawingBoardInfo, QWidget *parent = nullptr);
    ~CodeMngWidget();

    bool isDataChanged();
    virtual bool saveData() override;

private:
    void init();
    void refresh(); // 脚本类型变更，刷新

public slots:
    void onScriptTypeIndexChanged(int index);

private:
    PDrawingBoardClass m_pDrawingBoardInfo; // 当前画板

    PScriptRes m_pScriptResLua;
    PScriptRes m_pScriptResPY;
    ICodeEditWidget *m_pCodeEditorLua;
    ICodeEditWidget *m_pCodeEditorPY;
    QWidget *m_pScripWidget;
    QStackedWidget *m_pStatckWidget;

    // ui
    QComboBox *m_cbScriptType;
};

#endif // CODEMNGWIDGET_H
