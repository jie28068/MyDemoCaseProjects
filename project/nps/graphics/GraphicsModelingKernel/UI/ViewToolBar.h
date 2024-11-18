#ifndef VIEWTOOLBAR_H
#define VIEWTOOLBAR_H

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSharedPointer>
#include <QWidget>

class QToolBar;
class CustomComboBox;
class CanvasContext;
class ViewToolBar : public QWidget
{
    Q_OBJECT

public:
    enum ComboBoxType {
        ZoomLevlel,               // 缩放比例
        DefaultVoltageLevel,      // 节点和母线的默认电压等级
        DefaultPhaseTechnologies, // 节点的默认相技术
    };

    ViewToolBar(QWidget *parent, QSharedPointer<CanvasContext> ctx, QString title = "");
    ~ViewToolBar();
    void addAction(QAction *action);
    void addPushButton(QWidget *action);
    void insertComboBox(QAction *before, ComboBoxType BoxType);
    void insertSeparator(QAction *before);
    void setScaleBoxIndexFitIn();
    void setComboBoxEnableState(ComboBoxType BoxType, bool isEnable);
    void setComboBoxItemsList(QStringList list, ComboBoxType BoxType);
    void setComboBoxVisibleState(bool isVisible);
    void setAddSeparator();
    void setScaleComboboxText(double zoomSize); // 缩放比例显示
    void setReadOnly(bool readOnly);
    QStringList getScaleCoefficient();
    QString getToolBarBoxValue(ComboBoxType boxtype);

    QPushButton *getBookMarkButton() { return bookMarkBtn; }
    void setInsertWidget(QAction *before, QWidget *widget);

protected:
    bool eventFilter(QObject *, QEvent *) override;
    void paintEvent(QPaintEvent *) override;

    void initActions();

signals:
    void scaleBoxIndexChanged(const QString &text);
    void volLevelBoxIndexChanged(const QString &text);
    void phaseTechBoxIndexChanged(const QString &text);

public slots:
    void onScaleBoxIndexChanged(const QString &text);
private slots:
    void onVolLevelBoxIndexChanged(const QString &text);
    void onPhaseTechBoxIndexChanged(const QString &text);

public:
    QToolBar *_toolbar;
    CustomComboBox *_scaleCombobox;
    QComboBox *_volLevelCombobox;
    QComboBox *_phaseTechCombobox;
    QPushButton *bookMarkBtn;
    QStringList strScaleText;
    QSharedPointer<CanvasContext> canvasCtx;
    QMap<QAction *, QToolButton *> actionToolButtonMap;
};

// 自定义ComboBox
class CustomComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit CustomComboBox(QWidget *parent = 0);
    void setZoomSize(double size); // 设置显示比例
    ~CustomComboBox();

protected:
    // void enterEvent(QEvent *);
    // void onCurrentIndexChanged(const QString &);
private:
    // QLineEdit* lineEdit;
    // QString data;
};

#endif // VIEWTOOLBAR_H
