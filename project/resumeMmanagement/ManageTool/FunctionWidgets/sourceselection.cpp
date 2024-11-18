#include "sourceselection.h"
#include "ui_sourceselection.h"
#include "Model/mttablemodel.h"
#include "ManageTool/managetoolclient.h"

SourceSelection::SourceSelection(QWidget *parent) :
    QWidget(parent),
    ServiceObject(this),
    ui(new Ui::SourceSelection)
{
    ui->setupUi(this);
    btn_add = new QPushButton(ui->lineEdit);
    btn_add->resize(ui->lineEdit->width()/4,20);
    btn_add->setText(">>");
    btn_add->setStyleSheet("QPushButton{border:1px;background-color:#f0f0f0;}");
    btn_add->setVisible(false);
    btn_add->move(ui->lineEdit->width()/*-btn_add->width()*/,0);
    connect(btn_add,&QPushButton::clicked,this,&SourceSelection::btn_add_clicked);
    init();

}

SourceSelection::~SourceSelection()
{
    delete ui;
}

void SourceSelection::showEvent(QShowEvent *event)
{
    display(1);
    ui->btn_internal->setChecked(true);
    this->activateWindow();
    return QWidget::showEvent(event);
}

void SourceSelection::on_btn_internal_clicked()
{
    display(1);

}


void SourceSelection::on_btn_external_clicked()
{
    display(2);

}
//type 1内部 2外部
void SourceSelection::display(int type)
{
    currtype = type;
    ui->comboBox->clear();
    ui->lineEdit->clear();
    ui->label->clear();
    QStringList items;
    switch (type) {
    case 1:
        items<<"招聘平台"<<"内部推荐人";
        requestUsrInfo("type = 'internal' ");
        break;
    case 2:
        items<<"猎头公司"<<"外部推荐人";
        requestUsrInfo("type like 'external%'");
        break;
    default:
        break;
    }
    ui->comboBox->addItems(items);
}

void SourceSelection::requestUsrInfo(QString fileter)
{
    QVariantMap params;
    params["filter"] = fileter;
    params["isAll"] = false;
    ManageToolClient::instance().serviceReply(SERVICE_GROUP_BASE, SERVICE_BASE_QUERYUSRINFO, params,this);
}

void SourceSelection::setUpTabel(QList<QVariantMap> &maplist)
{
    if(currtype==1){
        maplist.append(inter_Channel);
    }
    tableModel->setData(maplist);
    ui->tableView->setModel(filterModel);
    ui->tableView->setColumnHidden(0,true);
    filtering(ui->comboBox->currentText());
}

void SourceSelection::filtering(QString filter)
{
    QString cate = filter;

    if(cate == "招聘平台"){
        filterModel->setFilterKeyColumn(0);
        filterModel->setFilterRegExp(QRegExp("internal_channel",Qt::CaseInsensitive/*,QRegExp::FixedString*/));
    }else if(cate == "内部推荐人"){
        filterModel->setFilterKeyColumn(0);
        filterModel->setFilterRegExp(QRegExp("^((?!channel).)*$",Qt::CaseInsensitive/*,QRegExp::FixedString*/));
    }else if(cate == "猎头公司"){
        filterModel->setFilterKeyColumn(0);
        filterModel->setFilterRegExp(QRegExp("external_channel",Qt::CaseInsensitive/*,QRegExp::FixedString*/));
    }else if(cate == "外部推荐人"){
        filterModel->setFilterKeyColumn(0);
        filterModel->setFilterRegExp(QRegExp("^((?!channel).)*$",Qt::CaseInsensitive/*,QRegExp::FixedString*/));
    }else{
        filterModel->setFilterKeyColumn(2);
        filterModel->setFilterRegExp(QRegExp(cate,Qt::CaseInsensitive,QRegExp::FixedString));
    }

}


void SourceSelection::on_lineEdit_textChanged(const QString &arg1)
{
    QString txt = arg1;
    if(txt.isEmpty()){
        filtering(ui->comboBox->currentText());
        btn_add->setVisible(false);
    }else{
        filtering(txt);
        btn_add->setVisible(true);
    }
}

void SourceSelection::BaseQueryUsrInfo(QVariantMap responseMap)
{
    bool success = responseMap["Success"].toBool();
    if(success){
        QVariantMap params = responseMap["Params"].toMap();
        QVariantList info_list = params["info_list"].toList();
        QList<QVariantMap>maplist;
        foreach (QVariant info, info_list) {
            maplist.append(info.toMap());
        }
        setUpTabel(maplist);
    }
}

void SourceSelection::init()
{
    tableModel = new MTTableModel(this);
    tableModel->setHeaderList({"type","department","name"});
    tableModel->setHeaderMap({{"type","TYPE"},{"department","部门"},{"name","推荐人"}});
    QVariantMap map1 = {{"type","internal_channel"},{"name","前程无忧"}};
    QVariantMap map2 = {{"type","internal_channel"},{"name","智联招聘"}};
    QVariantMap map3 = {{"type","internal_channel"},{"name","猎聘"}};
    QVariantMap map4 = {{"type","internal_channel"},{"name","Boss直聘"}};
    inter_Channel.clear();
    inter_Channel.append(map1);
    inter_Channel.append(map2);
    inter_Channel.append(map3);
    inter_Channel.append(map4);
    tableModel->setDefaultData(1,"无");

    filterModel = new QSortFilterProxyModel(this);
    filterModel->setSourceModel(tableModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}


void SourceSelection::on_comboBox_currentIndexChanged(const QString &arg1)
{
    filtering(arg1);
}


void SourceSelection::on_tableView_clicked(const QModelIndex &index)
{
    QModelIndex sourceIndex = filterModel->mapToSource(index);
    qDebug()<<"SourceSelection::on_tableView_clicked"<<sourceIndex.data();
    if(sourceIndex.column()==2){        
        QList<QVariantMap> data = tableModel->getTableData();
        QList<QVariantMap>::iterator i;
        for (i=data.begin();i!=data.end() ;i++ ) {
            if(i->value("name").toString() == sourceIndex.data().toString()){
                setSrctype(i->value("type").toString());
                break;;
            }
        }
        emit selectSource(sourceIndex.data().toString());
    }

}

void SourceSelection::btn_add_clicked()
{
    QString err;
    if(currtype != 2){
        err = "不可添加内部渠道";
        qDebug()<<"不可添加内部渠道";
        ui->label->setText(err);
        return;
    }
    QString text = ui->lineEdit->text();
    QString type = ui->comboBox->currentText();
    QList<QVariantMap> data = tableModel->getTableData();
    QList<QVariantMap>::iterator i;
    for (i=data.begin();i!=data.end() ;i++ ) {
        if(i->value("name").toString() == text){
            err = "已预添加此用户";
            qDebug()<<"已预添加此用户";
            ui->label->setText(err);
            return;
        }
    }
    QVariantMap newRow;
    if(type == "猎头公司"){
       newRow = {{"type","external_channel"},{"name",text}};
    }else if(type == "外部推荐人"){
        newRow = {{"type","external"},{"name",text}};
    }
    data.append(newRow);
    tableModel->setData(data);
}


const QString &SourceSelection::srctype() const
{
    return m_srctype;
}

void SourceSelection::setSrctype(const QString &newSrctype)
{
    if (m_srctype == newSrctype)
        return;
    m_srctype = newSrctype;
    emit srctypeChanged();
}
