#include "archiveinfo.h"
#include "ui_archiveinfo.h"
#include <QTimer>
#include "sourceselection.h"
#include <QWidgetAction>
#include <QMenu>
#include <QDate>

ArchiveInfo::ArchiveInfo(QWidget *parent) :
    QWidget(parent),
    MapObject(this),
    ui(new Ui::ArchiveInfo)
{
    ui->setupUi(this);

    ui->post->lineEdit()->setPlaceholderText("请输入岗位信息");
    ui->post->installEventFilter(this);

    headerList<<QString("name")
             <<QString("birthday")
            <<QString("tel_no")
           <<QString("post")
          <<QString("level")
         <<QString("levelname")
         <<QString("type")
        <<QString("src")
       <<QString("srctype")
      <<QString("age");
    model.setHeaderList(headerList);

    QMenu * menu = new QMenu(ui->src);
    QWidgetAction * action = new QWidgetAction(menu);
    SourceSelection *selection = new SourceSelection(this);
    connect(selection,&SourceSelection::selectSource,ui->src,&QPushButton::setText);
    connect(selection,&SourceSelection::srctypeChanged,this,[this,selection](){
        if(-1 == dataMapper.currentIndex())return ;
        model.setTableData(dataMapper.currentIndex(),"srctype",selection->srctype());
    });
    action->setDefaultWidget(selection);
    menu->addAction(action);
    ui->src->setMenu(menu);
    level = new QComboBox(this);
    level->addItems({"0","10"});
    level->setVisible(false);
    ui->level->addItem("一般人才",0);
    ui->level->addItem("优质人才",10);

    dataMapper.setModel(&model);
    dataMapper.addMapping(ui->name, model.headerSection("name"));
    dataMapper.addMapping(ui->birthday, model.headerSection("birthday"), "text");
    dataMapper.addMapping(ui->age, model.headerSection("age"), "text");
    dataMapper.addMapping(ui->tel_no, model.headerSection("tel_no"));
    dataMapper.addMapping(ui->post, model.headerSection("post"), "currentText");

    dataMapper.addMapping(ui->level, model.headerSection("levelname"), "currentText");
    dataMapper.addMapping(level, model.headerSection("level"), "currentText");
    dataMapper.addMapping(ui->type, model.headerSection("type"), "currentText");
    dataMapper.addMapping(ui->src, model.headerSection("src"),"text");
    dataMapper.addMapping(selection, model.headerSection("srctype"),"srctype");
    ui->group_opt->hide();

}

ArchiveInfo::~ArchiveInfo()
{
    delete ui;
}

void ArchiveInfo::setCurIndex(int index)
{
    dataMapper.setCurrentIndex(index);
}

void ArchiveInfo::recvResult(bool success, QString msg)
{
    Q_UNUSED(success)
    ui->lab_msg->setText(msg);
    QTimer::singleShot(3000, ui->lab_msg, &QLabel::clear);
}

void ArchiveInfo::propertyInit(QString sign)
{
    QVariantMap map;
    map["name"]="**";
    QString age =ComputationTime::MTime(UploadResume::birthday);
    if(age != NULL)
    {
        //map["birthday"]=UploadResume::birthday;
        map["age"] = age;
    }
    else
    {
        map["birthday"]="**********";
        map["age"] = "**";
    }
    if(GetTextTel::GetTel(UploadResume::Text) == NULL)
    {
        map["tel_no"]="************";
    }
    else
    {
        map["tel_no"]=GetTextTel::GetTel(UploadResume::Text);
    }
    map["level"]="0";
    map["levelname"]="一般人才";
    map["type"]="储备入库";
    map["src"]="--";
    map["sign"]=sign;

    QList<QVariantMap> dataList;
    dataList<<map;
    model.setData(dataList);

    model.setHeaderList(headerList);
    dataMapper.toFirst();
}

void ArchiveInfo::setModel(MTTableModel &archiveModel)
{
    model.setData(archiveModel.getTableData());
    model.setHeaderList(headerList);
    dataMapper.setModel(&model);
}

void ArchiveInfo::setData(QList<QVariantMap> listData)
{
    model.setData(listData);
    dataMapper.toFirst();
}

void ArchiveInfo::setDate(QString birthday)
{
    model.setTableData(dataMapper.currentIndex(),"birthday", birthday);
}

void ArchiveInfo::setAge(QString birthday)
{
    QDate borndate;
    if(birthday.length()==7)                   // xxxx-xx
       borndate=QDate::fromString(birthday,"yyyy-MM");
    else if(birthday.length()==10)              // xxxx-xx-xx
       borndate=QDate::fromString(birthday,"yyyy-MM-dd");
    QDate curredate=QDate::currentDate();
    int days=borndate.daysTo(curredate);
    ui->age->setText(QString::number(days/365));
}

/**
 * @brief ArchiveInfo::getArchiveInfo
    + name：姓名 默认**
    + birthday：生日 默认********
    + tel_no：电话号码  默认***********
    + post：岗位[QT][C++][产品经理]
    + level：级别[未定：0][初级：10][中级：20][高级：30]
    + type：档案类型
    + src：档案来源（用户ID）
    + mark：档案标记 默认无标记
    + sign：档案签名，对应的简历文件MD5
    + timestamp：变更时间
    + opt_id：操作人
 * @return
 */
QVariantMap ArchiveInfo::getArchiveInfo()
{

    if(-1 == dataMapper.currentIndex())
        return QVariantMap();

    return model.getCurTableData(dataMapper.currentIndex());
}

const ArchiveInfo::OptFlag &ArchiveInfo::optFlag() const
{
    return m_optFlag;
}

void ArchiveInfo::on_btn_archive_update_clicked()
{
    QVariantMap params = getArchiveInfo();
    emit archiveInfoUpdate(params);
}


void ArchiveInfo::on_btn_archive_commit_clicked()
{
    QVariantMap params = getArchiveInfo();
    emit archiveInfoCommit(params);
}

void ArchiveInfo::setOptFlag(const OptFlag &newOptFlag)
{
    if (m_optFlag == newOptFlag)
        return;

    ui->btn_archive_commit->setVisible(newOptFlag&ArchiveInfo::OPT_COMMIT);
    ui->btn_archive_update->setVisible(newOptFlag&ArchiveInfo::OPT_UPDATE);

    m_optFlag = newOptFlag;
    emit optFlagChanged();
}

void ArchiveInfo::on_src_clicked()
{

}

void ArchiveInfo::on_level_currentIndexChanged(int index)
{
    level->setCurrentIndex(index);
    dataMapper.submit();
    qDebug()<<"ArchiveInfo::on_level_currentIndexChanged"<<index;
}


void ArchiveInfo::on_birthday_editingFinished()
{
    QString iage = ComputationTime::MTime(ui->birthday->text());
    ui->age->setText(iage);
}

