#ifndef RESUMESELECTION_H
#define RESUMESELECTION_H

#include <QWidget>
#include <QMap>
#include <QButtonGroup>

namespace Ui {
class ResumeSelection;
}
class QPushButton;

class ResumeSelection : public QWidget
{
    Q_OBJECT

public:
    explicit ResumeSelection(QWidget *parent = nullptr);
    enum Department{
        DP_PERSON,//人事部
        DP_OTHER,//其他
    };
    ~ResumeSelection();
    /**
     * @brief display 按照部门显示相应菜单
     * @param strDepartment 部门
     */
    void display(QString strDepartment);
    void display(Department department);
private slots:
    void on_btn_cur_selection_clicked();
    void on_btn_new_selection_clicked();
    void on_btn_complete_selection_clicked();
    void on_stackedWidget_currentChanged(int arg1);

public slots:
    void createFilterPlan(QStringList idList);

private:
    Ui::ResumeSelection *ui;

    Department departMentMap(QString);
    QMap<QPushButton*,QWidget*>mapBtnWidget;
    QButtonGroup* btnGroup;
    void initDisp();

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
};

#endif // RESUMESELECTION_H
