#ifndef EXPORTIMAGE_H
#define EXPORTIMAGE_H
#define checkStateChecked 2
#define PdfTotalGraphScaleHeight 0.4875

#include "ActionManager.h"
#include "CanvasContext.h"
#include "CanvasViewDefaultImpl.h"
#include "ICanvasScene.h"
#include "KLWidgets/KCustomDialog.h"
#include <QCheckBox>
#include <QComboBox>
#include <QPdfWriter>
#include <QPrinter>
#include <QPushButton>
#include <QQueue>
#include <QSharedPointer>
#include <QWidget>
#include <QWidgetAction>
#include <qfiledialog.h>
#include <qlabel.h>

class CanvasViewDefaultImplPrivate;
class CanvasContext;
class SearchLineEdit;
class SourceProxy;
class ActionManager;
class CanvasViewDefaultImpl;
class MyLable;

class ExportImage : public QWidget
{
    Q_OBJECT

public:
    ExportImage(QSharedPointer<CanvasContext> canvasContext, CanvasViewDefaultImpl *m_exportimage, QWidget *parent = 0);
    ~ExportImage();
    // 设置图片
    void setImage(QPixmap pixmap);
    // 建立弹窗
    void CreatSaveImage();
    /// @brief 设置边框
    void setBroad();
    /// @brief 渲染图片
    /// @param falg 全图
    /// @param falgLine 网格
    void rendererImage(bool falg = false, bool falgLine = false);
    /// @brief 初始化界面
    void InitUi();
    /// @brief 保存文件路径
    /// @param filepath
    void saveFilePath(QString filepath);

public slots:
    /// @brief 导出为pdf
    void pdfPrint();
    /// @brief 导出图片
    void ButtonExportImage();
    // 整图
    void changefull(int checkState);
    // 网格
    void changeLine(int checkState);
    // 边框
    void changeBorad(int checkState);
    // 还原网格
    void falgclose(int checkState);
    /// @brief 隐藏复选框
    /// @param value
    void onLabelIsHide(int value);

private:
    QPixmap pix; // 图片
    CanvasViewDefaultImpl *p_CanvasViewDefaultImpl;
    KCustomDialog *my_saveimage;        // 弹窗
    QPushButton *m_pushbutton_export;   // 导出按钮
    QSharedPointer<CanvasContext> data; // 画板数据
    CanvasContext::GridFlag falg;       // 网格标志位
    MyLable *m_label;                   // 显示图片
    QCheckBox *m_isfull;                // 整图
    QCheckBox *m_isborad;               // 边框
    QCheckBox *m_isline;                // 网格
    QWidget *m_ui;                      // UI
    static QString lastpath;            // 上一次路径
    static QQueue<QString> *paths;      // 前俩次路径
    QComboBox *mComboBox;               // 选择导出格式
};

class MyLable : public QLabel
{
public:
    MyLable(QWidget *parnt = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void changeWheelValue(QPoint event, int value);

private:
    double m_scaleValue;  // 图片缩放倍数
    QPointF m_drawPoint;  // 绘图起点
    QPointF m_mousePoint; // 鼠标当前位置点
    QRect m_rectPixmap;   // 被绘图片的矩形范围
    bool m_isMousePress;  // 鼠标是否按下

    const double SCALE_MAX_VALUE; // 最大放大到原来的10倍
    const double SCALE_MIN_VALUE; // 最小缩小到原来的0.5倍
};

#endif // EXPORTIMAGE_H
