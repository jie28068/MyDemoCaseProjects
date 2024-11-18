#ifndef DRAWBOARDPROPERTY_H
#define DRAWBOARDPROPERTY_H

#include "CodeMngWidget.h"
#include "CommonWrapper.h"
#include "CoverWidget.h"
#include "createdrawingboardwidget.h"
#include <QSharedPointer>
namespace Kcc {
namespace BlockDefinition {
class DrawingBoardClass;
}
}
class DrawBoardProperty : public CommonWrapper
{
    Q_OBJECT
public:
    DrawBoardProperty(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> pDrawboard, bool isNewBoard = false,
                      const QString &initBoardType = "", bool readOnly = false);
    ~DrawBoardProperty();

    void init();

    // CommonWrapper
    QString getTitle() override;
    void onDialogExecuteResult(QDialog::DialogCode code) override;

    QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> getNewDrawingBoard();
    QString getOldBoardName();
    double getOldScale();
    bool isCodeChanged();
    bool getDataStatus(); // 只有画板名或者缩放比例修改返回false,有其他属性变化返回true

private:
    void updateBoardDataDictionary();

private:
    PDrawingBoardClass m_pDrawingBoardInfo; // 当前画板
    PCanvasContext m_pCanvasCtx;

    CreateDrawingboardWidget *m_pCreateBoardWidget;  // 画板信息（基础）
    ConnectorWidget *m_pConnectorWidget;             // 连接线
    CodeMngWidget *m_pCodeMngWidget;                 // 代码管理
    RightGraphicsEditWidget *m_pRightGraphicsWidget; // 右下角图例信息修改
    CoverWidget *m_coverWidget;                      // 封面

    PScriptRes m_pScriptRes;
    bool m_bNewBoard;
    QString m_newBoardInitType;
    bool m_bIsCodeChanged;
};

#endif // DRAWBOARDPROPERTY_H
