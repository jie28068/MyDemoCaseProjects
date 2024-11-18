#pragma once
#include "KLModelDefinitionCore/PublicDefine.h"
#include "KLModelDefinitionCore/Variable.h"
#include "KLModelDefinitionCore/VariableGroup.h"
#include "KMessageBox.h"
#include "WizardPageBase.h"
#include "WizardPageParamNew.h"
#include <QBuffer>
#include <QComboBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QMenu>
#include <QPainter>
#include <QSvgRenderer>
#include <QTableWidget>
#include <QToolTip>

#define LABLE_WIDTH 150
#define LABLE_HEIGHT 150
#define CELL_SIZE 60

class MyWidget;
class MyLable;
class TableWidget;
class IamgeSourceInfoPage : public WizardPageBase
{
    Q_OBJECT
    friend class MyWidget;

public:
    /// @brief 控制
    /// @param contrlBlock
    /// @param parnt
    IamgeSourceInfoPage(PControlBlock contrlBlock, PControlBlock ncontrlBlock, QWidget *parnt = nullptr);
    /// @brief 电气
    /// @param elecBlock
    /// @param parnt
    IamgeSourceInfoPage(PElectricalBlock elecBlock, PElectricalBlock nelecBlock, QWidget *parnt = nullptr);
    /// @brief 初始化界面
    /// @param picA A类
    /// @param picB B类
    /// @param isElec 是否为电气
    void inIt(QVariantMap pic, bool isElec = false);

    /// @brief 初始化变量
    void inItVariable();
    /// @brief 获取所有图片资源
    /// @return
    QVariantMap getSource();

    /// @brief 获取label的资源
    /// @param label
    /// @return
    QVariant getSource(MyLable *label, bool isSvg = false);
    /// @brief 加载已有图片
    /// @param var
    void setInitLabelImage(const QVariant &var, MyLable *label, bool isSvg = false);
    /// @brief 获取是否为电气
    /// @return
    bool getIsElec();

    //***更多图片****//
    /// @brief 设置提示消息
    /// @param str
    void setMessageLabel(const QString &str);
    /// @brief 设置添加按钮
    /// @param fale
    void setButtonEnable(bool fale);
    /// @brief 设置确定按钮
    /// @param falg
    void setDialogEndble(bool falg);
    /// @brief 保存图片
    /// @param Block
    void saveMoreImage(PModel block);
    /// @brief 初始化界面
    void inItMoreImage();
    //***end***//

protected:
    /// @brief 初始化页面
    virtual void initializePage() override;
    /// @brief 完成页面
    /// @return
    virtual bool validatePage() override;

private slots:
    /// 创建更多图片窗口
    void creatMoreImageWidget();
    /// 添加行
    void addRowTable();
    /// @brief 表格菜单变化
    /// @param fagle
    void onMenuEnableChanged(bool fagle);

private:
    bool isElec; // 是否为电气

    //***PNG控件组***//
    MyLable *pngNormalLabel;
    MyLable *pngDisableLabel;
    MyLable *pngWarningLabel;
    MyLable *pngErrorLabel;
    QGroupBox *pngGroupBox;
    //***end***//

    //***SVG控件组***//
    MyLable *svgNormalLabel;
    MyLable *svgDisableLabel;
    MyLable *svgWarningLabel;
    MyLable *svgErrorLabel;
    QGroupBox *svgGroupBox;
    //***end***//

    //***更多图片控件组***//
    TableWidget *table1;       // 表格1
    TableWidget *table2;       // 表格2
    QLabel *messagelabel;      // 提示信息
    QStringList otherNames;    // 固定的八张图片
    QPushButton *addButton;    // 添加行
    KCustomDialog *dialog;     // 更多图片窗口
    QPushButton *moreImageBtn; // 添加更多图片按钮
    //***end***//

public:
    QList<QString> names;
};

class MyWidget : public QWidget
{
    Q_OBJECT
public:
    MyWidget(MyLable *label, QWidget *parnt = 0);
    /// @brief 设置二级弹窗
    /// @param dialog
    void setKCustomDialog(KCustomDialog *dialog);

public slots:
    void onSoureButton();
    void onDrawButton();

private:
    KLineEdit *edit;
    QPushButton *soureButton;
    QPushButton *drawButton;
    MyLable *mLabel;
    KCustomDialog *mDialog;
};

class MyLable : public QLabel
{
    Q_OBJECT
public:
    MyLable(QString str, IamgeSourceInfoPage *imagePage, QWidget *parnt = nullptr);
    /// @brief 设置图片
    void switchFileImage();
    /// @brief 设置SVG图片数据
    /// @param data
    void setSvgData(QByteArray data);
    /// @brief 获取SVG图片数据
    /// @return
    QByteArray getSvgData();

protected:
    void paintEvent(QPaintEvent *) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void changeWheelValue(QPoint event, int value);
    bool event(QEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *ev) override;

private slots:
    void inputImage(bool checked = false);

private:
    double m_scaleValue;  // 图片缩放倍数
    QPointF m_drawPoint;  // 绘图起点
    QPointF m_mousePoint; // 鼠标当前位置点
    QRect m_rectPixmap;   // 被绘图片的矩形范围
    bool m_isMousePress;  // 鼠标是否按下

    const double SCALE_MAX_VALUE; // 最大放大到原来的10倍
    const double SCALE_MIN_VALUE; // 最小缩小到原来的0.5倍
    QString strs;                 // 图片分类
    IamgeSourceInfoPage *imagePage;
    QByteArray svgData; // SVG的QByteArray数据
};

class IamgeSourceInfoPageOnModify : public IamgeSourceInfoPage
{
    Q_OBJECT
public:
    IamgeSourceInfoPageOnModify(PControlBlock pBlock, PControlBlock npBlock, QWidget *parent = nullptr)
        : IamgeSourceInfoPage(pBlock, npBlock, parent), isElec(false)
    {
    }
    IamgeSourceInfoPageOnModify(PElectricalBlock pElecBlock, PElectricalBlock npElecBlock, QWidget *parent = nullptr)
        : IamgeSourceInfoPage(pElecBlock, npElecBlock, parent), isElec(true)
    {
    }
    virtual void initializePage() override;

private:
    bool isElec;
};

class TableWidget : public QTableWidget
{
    Q_OBJECT
public:
    TableWidget(IamgeSourceInfoPage *page, QWidget *parent = 0);
    /// @brief 初始化已有的图片
    /// @param str key值
    /// @param value value值
    /// @param order 表序号
    void InitImage(const QString str, QVariant &value, int order);

public slots:
    /// 双击添加图片
    void addImage(int row, int column);
    /// @brief 添加表行
    void addRow();
    /// @brief 删除表行
    void deleteRow();
    /// @brief 右键删除菜单
    /// @param event
    void contextMenuEvent(QContextMenuEvent *event);
    /// @brief 单元格变化时
    /// @param row
    /// @param column
    void onCellChanged(int row, int column);

signals:
    void menuEnableChanged(bool fagle);

private:
    // 要删除的行
    int row;
    IamgeSourceInfoPage *m_page;
};

class MyMoreLabel : public QLabel
{
public:
    MyMoreLabel(QWidget *parnt = nullptr) : QLabel(parnt)
    {
        setAcceptDrops(true);
        setFixedSize(60, 60);
        setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    void setSvgData(QByteArray data) { svgData = data; }

    QByteArray getSvgData() { return svgData; }

private:
    // 图片二进制数据
    QByteArray svgData;
};