#ifndef OPENBOARDWIDGET_H
#define OPENBOARDWIDGET_H

#include "AssistantDefine.h"
#include "ui_openboardwidget.h"
#include <QMap>
#include <QSharedPointer>
#include <QStringList>
#include <QWidget>

namespace Kcc {
namespace BlockDefinition {
class Model;
class DrawingBoardClass;
}
}

class OpenBoardWidget : public QWidget
{
    Q_OBJECT

public:
    OpenBoardWidget(QWidget *parent = 0);
    ~OpenBoardWidget();

    enum ActionType {
        OpenDrawingboard,  // 打开
        DeleteDrawingboard // 删除
    };
    void setActionType(ActionType actionType);
    ActionType getActionType() { return m_actionType; }
    QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> getDrawboard();

private slots:
    void onBoardTypeChanged(const QString &text);

private:
    void initCombox(QMap<QString, QStringList> cboxData);
    QMap<QString, QStringList> getInitComboxData();
    QString getBoardName();

private:
    Ui::OpenBoardWidget ui;
    QMap<QString, QStringList> m_cboxData;

    ActionType m_actionType;
};

#endif // OPENBOARDWIDGET_H
