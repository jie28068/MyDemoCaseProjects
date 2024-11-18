#ifndef SOURCESELECTION_H
#define SOURCESELECTION_H

#include <QWidget>
#include "serviceobject.h"
#include <QSortFilterProxyModel>

namespace Ui {
class SourceSelection;
}
class QPushButton;
class MTTableModel;

class SourceSelection : public QWidget , public ServiceObject
{
    Q_OBJECT
    Q_PROPERTY(QString srctype READ srctype WRITE setSrctype NOTIFY srctypeChanged)
public:
    explicit SourceSelection(QWidget *parent = nullptr);
    ~SourceSelection();
    void showEvent(QShowEvent *event);

    const QString &srctype() const;
    void setSrctype(const QString &newSrctype);

private slots:
    void on_btn_internal_clicked();

    void on_btn_external_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

    Q_INVOKABLE void BaseQueryUsrInfo(QVariantMap responseMap);
    void on_comboBox_currentIndexChanged(const QString &arg1);
    void on_tableView_clicked(const QModelIndex &index);
    void btn_add_clicked();
signals:
    void selectSource(QString);
    void srctypeChanged();

private:
    Ui::SourceSelection *ui;
    QPushButton *btn_add;
    MTTableModel *tableModel;
    QSortFilterProxyModel* filterModel;
    QList<QVariantMap> inter_Channel;
    int currtype;//当前类型 1内部 2外部

    void init();
    void display(int type);
    void requestUsrInfo(QString fileter);
    void setUpTabel(QList<QVariantMap> &maplist);
    void filtering(QString filter);
    QString m_srctype;
};

#endif // SOURCESELECTION_H
