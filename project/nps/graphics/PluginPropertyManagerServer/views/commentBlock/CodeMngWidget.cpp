#include "CodeMngWidget.h"
#include "PropertyServerMng.h"

#include "qgridlayout.h"
#include <QLabel>

static const ushort SCRIPT_TYPE_OFFSET = 1; // 脚本类型Lua为1

CodeMngWidget::CodeMngWidget(PDrawingBoardClass drawingBoardInfo, QWidget *parent)
    : CWidget(parent), m_pDrawingBoardInfo(drawingBoardInfo), m_pCodeEditorLua(nullptr), m_pCodeEditorPY(nullptr)
{
    init();
}

CodeMngWidget::~CodeMngWidget() { }

bool CodeMngWidget::isDataChanged()
{
    bool isChanged = false;

    if (m_pDrawingBoardInfo->getScriptType() != m_cbScriptType->currentIndex() + SCRIPT_TYPE_OFFSET)
        return true;

    if (m_cbScriptType->currentIndex() == 0 && m_pCodeEditorLua) { // 界面选中Lua
        if (m_pDrawingBoardInfo->getScript() != m_pCodeEditorLua->getText())
            isChanged = true;
    } else if (m_cbScriptType->currentIndex() == 1 && m_pCodeEditorPY) {
        if (m_pDrawingBoardInfo->getScript() != m_pCodeEditorPY->getText())
            isChanged = true;
    }

    return isChanged;
}

bool CodeMngWidget::saveData()
{
    if (m_pDrawingBoardInfo == nullptr) {
        return false;
    }

    m_pDrawingBoardInfo->setScriptType(m_cbScriptType->currentIndex() + SCRIPT_TYPE_OFFSET);

    if (m_pDrawingBoardInfo->getScriptType() == CODE_LANG_LUA && m_pCodeEditorLua) { // Lua
        m_pDrawingBoardInfo->setScript(m_pCodeEditorLua->getText());
    } else if (m_pDrawingBoardInfo->getScriptType() == CODE_LANG_PYTHON && m_pCodeEditorPY) { // python
        m_pDrawingBoardInfo->setScript(m_pCodeEditorPY->getText());
    }
    return true;
}

void CodeMngWidget::init()
{
    // 创建运行脚本所需的资源
    if (PropertyServerMng::getInstance().m_pCodeManagerSvr == nullptr || m_pDrawingBoardInfo == nullptr) {
        return;
    }

    m_pScriptResLua = PropertyServerMng::getInstance().m_pCodeManagerSvr->CreateModelScriptRes(CODE_LANG_LUA,
                                                                                               m_pDrawingBoardInfo);
    m_pCodeEditorLua = PropertyServerMng::getInstance().m_pCodeManagerSvr->GetScriptEditor(m_pScriptResLua);

    m_pScriptResPY = PropertyServerMng::getInstance().m_pCodeManagerSvr->CreateModelScriptRes(CODE_LANG_PYTHON,
                                                                                              m_pDrawingBoardInfo);
    m_pCodeEditorPY = PropertyServerMng::getInstance().m_pCodeManagerSvr->GetScriptEditor(m_pScriptResPY);

    QWidget *pScripWidgetLua = PropertyServerMng::getInstance().m_pCodeManagerSvr->GetScriptWidget(m_pScriptResLua);
    QWidget *pScripWidgetPY = PropertyServerMng::getInstance().m_pCodeManagerSvr->GetScriptWidget(m_pScriptResPY);

    int curIndex = m_pDrawingBoardInfo->getScriptType() - SCRIPT_TYPE_OFFSET; // combox当前索引

    m_cbScriptType = new QComboBox(this);
    m_cbScriptType->setView(new QListView());
    QStringList typeList;
    if (pScripWidgetLua) {
        typeList << "Lua";
    }
    if (pScripWidgetPY) {
        typeList << "Python";
    }
    m_cbScriptType->addItems(typeList);
    m_cbScriptType->setCurrentIndex(curIndex);
    connect(m_cbScriptType, SIGNAL(currentIndexChanged(int)), this, SLOT(onScriptTypeIndexChanged(int)));
    m_cbScriptType->hide();

    m_pStatckWidget = new QStackedWidget(this);
    if (pScripWidgetLua) {
        m_pStatckWidget->addWidget(pScripWidgetLua);
    }
    if (pScripWidgetPY) {
        m_pStatckWidget->addWidget(pScripWidgetPY);
    }
    if (m_pDrawingBoardInfo->getScriptType() == 1 && m_pCodeEditorLua) { // Lua
        m_pCodeEditorLua->setText(m_pDrawingBoardInfo->getScript());
    } else if (m_pDrawingBoardInfo->getScriptType() == 2 && m_pCodeEditorPY) { // python
        m_pCodeEditorPY->setText(m_pDrawingBoardInfo->getScript());
    }

    refresh();

    QLabel *lab = new QLabel(this);
    lab->setText(tr("script type:")); // 脚本类型
    lab->hide();

    QGridLayout *pGridLayout = new QGridLayout(this);
    pGridLayout->addWidget(lab, 0, 0, 1, 1);
    pGridLayout->addWidget(m_cbScriptType, 0, 1, 1, 3);
    pGridLayout->addWidget(m_pStatckWidget, 1, 0, 1, 4);

    this->setLayout(pGridLayout);
}

void CodeMngWidget::refresh()
{
    int scriptType = m_cbScriptType->currentIndex() + SCRIPT_TYPE_OFFSET; // 脚本类型

    m_pStatckWidget->setCurrentIndex(m_cbScriptType->currentIndex()); // 切换窗口

    PScriptRes tmpScriptRes = nullptr;
    ICodeEditWidget *curEditWidget = nullptr;

    if (scriptType == CODE_LANG_LUA && m_pScriptResLua) {
        tmpScriptRes = m_pScriptResLua;
        curEditWidget = m_pCodeEditorLua;
    } else if (scriptType == CODE_LANG_PYTHON && m_pScriptResPY) {
        tmpScriptRes = m_pScriptResPY;
        curEditWidget = m_pCodeEditorPY;
    }

    if (!tmpScriptRes || !curEditWidget)
        return;

    // if (curEditWidget && !m_pDrawingBoardInfo->script.isEmpty()) {
    //     curEditWidget->setText(m_pDrawingBoardInfo->script);
    // }

    // 设置全局变量 fixme
    // QMap<QString, QVariant> globalVars;
    // QString errInfo;
    // QStringList apiInfo;
    // for (auto iter = m_pDrawingBoardInfo->blockMap.begin(); iter != m_pDrawingBoardInfo->blockMap.end(); iter++) {
    //     if (typeid(*iter.value()) == typeid(ControlBlock)) {
    //         PControlBlock pCtrlBlock = iter.value().dynamicCast<ControlBlock>();
    //         if (pCtrlBlock) {
    //             QList<QString> varKeys = pCtrlBlock->getVarKeys();
    //             for (auto iter = varKeys.begin(); iter != varKeys.end(); iter++) {
    //                 globalVars[*iter] = 0;
    //                 apiInfo << *iter;
    //             }
    //         }
    //     }
    // }
    // PropertyServerMng::getInstance().m_pCodeManagerSvr->setGlobalVars(tmpScriptRes, globalVars, errInfo);

    // // 将各模块的变量加到代码编辑框的提示中
    // if (curEditWidget && !apiInfo.isEmpty()) {
    //     curEditWidget->clearApiInfo();
    //     curEditWidget->addApiInfo(apiInfo);
    // }
}

void CodeMngWidget::onScriptTypeIndexChanged(int index)
{
    refresh();
}
