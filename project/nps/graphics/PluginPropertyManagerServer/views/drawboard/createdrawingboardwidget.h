#ifndef CREATEDRAWINGBOARDWIDGET_H
#define CREATEDRAWINGBOARDWIDGET_H

#include "GraphicsModelingKernel/CanvasContext.h"
#include "KLineEdit.h"
#include "PropertyTableModel.h"
#include "ServerManager.h"
#include "TypeItemView.h"
#include <QComboBox>
#include <QFontDialog>
#include <QLabel>
#include <QSharedPointer>
#include <QSize>

class CreateDrawingboardWidget : public CWidget
{
    Q_OBJECT

public:
    CreateDrawingboardWidget(PCanvasContext pCanvasCtx, bool isnewboard, const QString &initBboardType = "",
                             QWidget *parent = nullptr);
    ~CreateDrawingboardWidget();

    bool isParameterChanged();
    virtual bool checkLegitimacy(QString &errorinfo) override;
    bool saveDrawingBoardData(PCanvasContext pCanvasCtx);
    QString getOldBoardName();
    double getOldScale();
    QString getBoardName();
    int getBoardType();
    bool getDataStatus();

private slots:
    void onTableItemDoubleClicked(const QModelIndex &index);
    void onModelDataItemChanged(const CustomModelItem &olditem, const CustomModelItem &newitem);
    void onCurrentTextChanged(const QString &text);
    void onTextEdited(const QString &textstr);

private:
    void initUi();
    QString getDrawBoardTypeToString();
    int getStringToDrawBoardType(const QString &boardtypestr);
    QString getDrawingBoardSizeString(QSize boardSize);
    QSize CreateDrawingboardWidget::getSize(const QString &sizestr);
    QList<CustomModelItem> getDrawboardModelList();
    /// @brief 新建画板返回初值名
    /// @param boardType 目前电气画板才有
    /// @return
    QString getBoardInitName(const QString &boardType);

private:
    PCanvasContext m_pCanvasCtx;

    bool m_bNewBoard;
    QString m_InitBoardType;
    QString m_oldBoardName;
    double m_oldScale;
    QLabel *m_labelBoardName;
    KLineEdit *m_leditBoardName;
    QLabel *m_labelBoardType;
    QComboBox *m_cboxBoardType;
    TableView *m_tableView;
    PropertyTableModel *m_boardTableModel;
    QMap<QString, QString> m_boardTypeMapInitName; // 初始化名字
    bool m_editBoardName;                          // 编辑了名字
    bool dataStatus; // 默认为false,若画板名、缩放比例以外的数据发生改变 状态置为true
};

// 连接线页面
class ConnectorWidget : public CWidget
{
    Q_OBJECT
public:
    ConnectorWidget(PCanvasContext pCanvasCtx, QWidget *parent = nullptr);
    ~ConnectorWidget();
    bool isValueChanged();
    void savePropertyToBoard();

private:
    void initUi();
    QList<CustomModelItem> getModelList();
private slots:
    void onTableItemDoubleClicked(const QModelIndex &index);

private:
    PCanvasContext m_pCanvasCtx;

    TableView *m_tableView;
    PropertyTableModel *m_connectorTableModel;
};

// 画板右下角图例信息
class RightGraphicsEditWidget : public CWidget
{
    Q_OBJECT
public:
    RightGraphicsEditWidget(PCanvasContext pCanvasCtx, QWidget *parent = nullptr);
    ~RightGraphicsEditWidget();

    bool isValueChanged();
    void savePropertyToBoard();

private:
    void initUi();
    QList<CustomModelItem> getModelList();

private slots:
    void onRgTableItemDoubleClicked(const QModelIndex &index);

private:
    PCanvasContext m_pCanvasCtx;

    TableView *m_tableView;
    PropertyTableModel *m_graphicsTableModel;

    QMap<QString, QVariant> m_extraInfoMap;
    QFont m_pTextFont;
    QFont setfont;
    QFontDialog m_pRightGraphicsFont;
};

#endif // CREATEDRAWINGBOARDWIDGET_H
