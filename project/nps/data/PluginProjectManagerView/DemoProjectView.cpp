#include "DemoProjectView.h"
#include "KLProject\klproject_global.h"
#include "QPHelper.h"
#include "ToolsLib/Json/json.h"
#include <QDir>
#include <QFileInfo>
#include <QKeyEvent>
#include <QModelIndexList>
#include <QPainter>
#include <QStringListModel>
#include <QSvgRenderer>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>

#define BUILDI_DEMO_PATH (QFileInfo(QCoreApplication::applicationDirPath()).canonicalFilePath() + "/Example")
#define ITEMSIZE QSize(200, 160)
#define ICONSIZE QSize(200, 135)

DemoProjectView::DemoProjectView(QWidget *parent)
    : QWidget(parent), m_treeModel(nullptr), m_bworkspece(false), m_MultiSelect(false)
{
    ui.setupUi(this);
    ui.titleLab->setStyleSheet("color: #003d74;font: bold 16px 'Microsoft YaHei';");
    ui.treeView->setFont(QFont("Microsoft YaHei", 14));
    ui.treeView->setIndentation(20);
    ui.treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_treeModel = new QStandardItemModel(ui.treeView);
    ui.listWidget->setViewMode(QListView::IconMode);
    ui.listWidget->setResizeMode(QListView::Adjust);
    ui.listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui.listWidget->setMovement(QListView::Static);
    ui.listWidget->setSpacing(8);
    ui.listWidget->setContentsMargins(2, 10, 0, 10);
    ui.decLab->setText(tr("No information"));
    connect(ui.treeView, &QTreeView::clicked, this, &DemoProjectView::onTreeItemClick);
}

DemoProjectView::~DemoProjectView() { }

void DemoProjectView::deserializedDemoInfo(const QString &prjpath)
{
    QString prjname = QFileInfo(prjpath).baseName();
    if (m_infoMap.contains(prjname))
        return;

    QFile file(prjpath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    QTextStream text(&file);
    text.setCodec("UTF-8"); // 固定使用UTF-8编码
    QString content = text.readAll();
    file.close();

    DemoInfo demoinfo;
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(content.toStdString(), root)) {
        return;
    }

    // 兼容旧的项目文件
    if (!root["ProjectConfig"].isNull() || !root["All Model path"].isNull() || !root["ProjectProperty"].isNull()) {
        // 项目配置
        if (!root["ProjectConfig"].isNull()) {
            Json::Value::Members nameList = root["ProjectConfig"].getMemberNames();
            for (unsigned int j = 0; j < nameList.size(); j++) {
                QString name = QString::fromStdString(nameList[j]);
                if (name == KL_PRO::BOARD_LABAL) {
                    if (!root["ProjectConfig"][nameList[j]].isNull()) {
                        if (!root["ProjectConfig"][nameList[j]].isObject()) {
                            QString dataStr = root["ProjectConfig"][nameList[j]].asCString();
                            QByteArray buffer;
                            buffer.append(dataStr);
                            // frombase64
                            QByteArray base64Array = QByteArray::fromBase64(buffer.data());
                            ////// 解压缩
                            QByteArray unCompressArray = qUncompress(base64Array);
                            QDataStream inStream(&unCompressArray, QIODevice::ReadWrite);
                            QVariantMap bookMarkMap;
                            inStream >> bookMarkMap;
                            demoinfo.projectConfig[name] = bookMarkMap;
                        }
                    }
                    continue;
                }
                Json::Value::Members List = root["ProjectConfig"][nameList[j]].getMemberNames();
                if (List.size() > 0) {
                    QVariantMap map;
                    for (unsigned int i = 0; i < List.size(); i++) {
                        QString towname = QString::fromStdString(List[i]);
                        map[towname] = QString::fromStdString(root["ProjectConfig"][nameList[j]][List[i]].asString());
                    }
                    demoinfo.projectConfig[name] = map;
                } else {
                    if (root["ProjectConfig"][nameList[j]].size() > 1) {
                        auto num = root["ProjectConfig"][nameList[j]].size();
                        QStringList list;
                        for (unsigned int i = 0; i < num; i++) {
                            list << root["ProjectConfig"][nameList[j]][i].asCString();
                        }
                        demoinfo.projectConfig[name] = QVariant(list);
                        continue;
                    }
                    if (root["ProjectConfig"][nameList[j]].isString()) {
                        demoinfo.projectConfig[name] =
                                QVariant(QString::fromStdString(root["ProjectConfig"][nameList[j]].asString()));
                    }
                }
            }
        }

        // 项目属性
        if (!root["ProjectProperty"].isNull()) {
            Json::Value::Members keyList = root["ProjectProperty"].getMemberNames();
            for (unsigned int i = 0; i < keyList.size(); i++) {
                QString key = QString::fromStdString(keyList[i]);
                demoinfo.projectProperty[key] = QString::fromStdString(root["ProjectProperty"][keyList[i]].asString());
            }
        }
        m_infoMap.insert(QFileInfo(prjpath).baseName(), demoinfo);
        return;
    }

    // QtJson新项目格式
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
    if (!doc.isObject()) {
        return;
    }
    QJsonObject obj = doc.object();

    // 项目属性
    auto mapData = obj.value("Property").toObject().toVariantMap();
    for (auto key : mapData.keys()) {
        demoinfo.projectProperty.insert(key, mapData.value(key));
    }

    // 项目配置
    QJsonObject configObj = obj.value("Config").toObject();
    for (auto iter = configObj.begin(); iter != configObj.end(); iter++) {
        if (iter.key() == KL_PRO::BOARD_ISACTIVATE) {
            demoinfo.projectConfig.insert(iter.key(), iter.value().toObject().toVariantMap());
        } else if (iter.key() == KL_PRO::BOARD_OPEN_STATUS) {
            QStringList boardOpenList;
            for (auto data : iter.value().toArray()) {
                boardOpenList << data.toString();
            }
            demoinfo.projectConfig.insert(iter.key(), boardOpenList);
        } else if (iter.key() == KL_PRO::BOARD_LABAL) {
            QVariantMap labelMap;
            QByteArray labelData = QByteArray::fromBase64(iter.value().toString().toUtf8());
            QDataStream dataStream(labelData);
            dataStream >> labelMap;
            demoinfo.projectConfig.insert(iter.key(), labelMap);
        } else {
            demoinfo.projectConfig.insert(iter.key(), iter.value().toVariant());
        }
    }
    m_infoMap.insert(QFileInfo(prjpath).baseName(), demoinfo);
}

void DemoProjectView::setMessage(QString message)
{
    ui.label->setText(message);
}
void DemoProjectView::setDemoMap(QMap<QString, QStringList> demoMap)
{
    if (m_treeModel == nullptr) {
        return;
    }
    m_folderList = demoMap;
    m_bworkspece = false;
    if (ui.label != nullptr) {
        ui.label->hide();
    }
    ui.widget->setFixedWidth(196);
    ui.gridwidget->show();
    ui.labwidget->show();
    for (QString namestr : m_folderList.keys()) {
        QStandardItem *foleritem = //
                new QStandardItem(QIcon(BUILDI_DEMO_PATH + "/" + m_folderList[namestr].last()), namestr);
        foleritem->setData(namestr, FolderRole);
        m_treeModel->appendRow(foleritem);
    }

    ui.treeView->setModel(m_treeModel);
    // 默认选中第一行
    QModelIndex modelindex = ui.treeView->model()->index(0, 0);
    ui.treeView->setCurrentIndex(modelindex);
    resetGridView(m_folderList.firstKey());
}

void DemoProjectView::setNameList(QStringList nameList, bool bworkspace)
{
    if (m_treeModel == nullptr) {
        return;
    }
    m_bworkspece = bworkspace;
    m_treeModel->clear();
    m_folderList.clear();
    m_namepathMap.clear();
    m_selectNames.clear();
    if (bworkspace) {
        if (ui.label != nullptr) {
            ui.label->show();
        }
        ui.gridwidget->hide();
        ui.labwidget->hide();
        for (QString namestr : nameList) {
            QStandardItem *tmpitem =
                    new QStandardItem(QIcon(":/PluginProjectManagerView/resource/TreeIcons/user-normal.png"), namestr);
            m_treeModel->appendRow(tmpitem);
        }
    }

    ui.treeView->setModel(m_treeModel);
    // 默认选中第一行
    QModelIndex modelindex = ui.treeView->model()->index(0, 0);
    ui.treeView->setCurrentIndex(modelindex);
    if (bworkspace) {
        m_selectNames << modelindex.data().toString(); // 选中第一个工作空间
    }
}
void DemoProjectView::onTreeItemClick(const QModelIndex &index)
{
    if (m_bworkspece) { // 工作空间只能选中一个
        m_selectNames.clear();
        m_selectNames << index.data().toString();
        return;
    }
    QString foldername = index.data(FolderRole).toString();
    resetGridView(foldername);
}
void DemoProjectView::resetGridView(const QString &folder)
{
    ui.listWidget->clear();
    QStringList list = m_folderList.value(folder);
    for (int i = 0; i < list.size() - 1; i++) { // 最后一个是图标
        QListWidgetItem *witem = new QListWidgetItem(ui.listWidget);
        witem->setSizeHint(ITEMSIZE);
        witem->setFlags(Qt::NoItemFlags);
        ui.listWidget->addItem(witem);
        GridItem *item = new GridItem(ui.listWidget);
        QStringList strlist = list.at(i).split("~");
        item->setFixedSize(ITEMSIZE);
        item->setCurPrj(strlist[0], strlist[1]);
        item->setProperty("RelativePath", strlist[0]);
        item->setProperty("EnglishName", strlist[1]);
        item->setProperty("index", i);
        ui.listWidget->setItemWidget(witem, item);
        connect(item, &GridItem::checkedChanged, this, &DemoProjectView::onGridItemChecked);
        connect(this, &DemoProjectView::selectone, item, &GridItem::onSelectOne);
        deserializedDemoInfo(BUILDI_DEMO_PATH + "/" + strlist[0]);
    }
}
void DemoProjectView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control) {
        m_MultiSelect = true;
    }
    return QWidget::keyPressEvent(event);
}
void DemoProjectView::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Control) {
        m_MultiSelect = false;
    }
    return QWidget::keyReleaseEvent(event);
}
void DemoProjectView::setCurrentInfo(const QString &curprj, const QString &en_name)
{

    QString prjname = QFileInfo(curprj).baseName();
    if ("en_US" == QPHelper::getLanguage()) {
        prjname = en_name;
    }
    ui.titleLab->setText(prjname);
    DemoInfo info = m_infoMap.value(prjname);
    QString text =
            QString(R"(<div style=' font: 14px ; color:#35405C;margin-top:15px;'>%1&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</div>
                              <font style='font: 14px ; color:#333333;'>%5</font>                              
                              <div style=' font: 14px ; color:#35405C;margin-top:15px;'>%2</div>
                              <font style='font: 14px ; color:#333333;'>%6</font>                              
                              <div style=' font: 14px ; color:#35405C;margin-top:15px;'>%3</div>
                              <font style='font: 14px ; color:#333333;'>%7</font>    
                              <div style=' font: 14px ; color:#35405C;margin-top:15px;'>%4</div>
                              <font style='font: 14px ; color:#333333;'>%8</font>     
                              )")
                    .arg(tr("Name"), tr("File Path"), tr("Creation Time"), tr("Modification Time"))
                    .arg(prjname, QFileInfo(curprj).path(), info.projectProperty["createTime"].toString(),
                         info.projectProperty["modifyTime"].toString());
    ui.detailText->setText(text);
}

void DemoProjectView::onGridItemChecked(bool checked)
{
    QObject *obj = sender();
    GridItem *item = qobject_cast<GridItem *>(obj);
    QString selectname = item->property("RelativePath").toString();
    QString englishname = item->property("EnglishName").toString();
    setCurrentInfo(selectname, englishname);
    QString data = selectname + "~" + englishname;
    if (m_MultiSelect) { // 多选
        if (checked) {
            if (!m_selectNames.contains(data)) {
                m_selectNames.append(data);
            }
        } else {
            m_selectNames.removeOne(data);
        }
    } else { // 单选
        if (checked) {
            m_selectNames.clear();
            m_selectNames.append(data);
            emit selectone(item->property("index").toInt());
        }
    }
}

void DemoProjectView::setSelectionMode(QAbstractItemView::SelectionMode mode)
{
    ui.treeView->setSelectionMode(mode);
}

QStringList DemoProjectView::getSelectNames()
{
    return m_selectNames;
}

QIcon DemoProjectView::getFolderIcon(const QString &foldername)
{
    if (foldername == QString("IEEE标准电网模型")) {
        return QIcon(":/PluginProjectManagerView/resource/SampleProjectIcons/IEEEStandardGridModel_normal.png");
    } else if (foldername == QString("简单电路模型")) {
        return QIcon(":/PluginProjectManagerView/resource/SampleProjectIcons/SimpleCircuitModel_normal.png");
    } else if (foldername == QString("测试模型")) {
        return QIcon(":/PluginProjectManagerView/resource/SampleProjectIcons/TestModel_normal.png");
    } else if (foldername == QString("控制系统模型")) {
        return QIcon(":/PluginProjectManagerView/resource/SampleProjectIcons/ControlSystemsModel_normal.png");
    } else if (foldername == QString("系统构造型模块库")) {
        return QIcon(
                ":/PluginProjectManagerView/resource/SampleProjectIcons/SystemConstructiveModuleLibrary_normal.png");
    } else if (foldername == QString("新型电力系统模型")) {
        return QIcon(":/PluginProjectManagerView/resource/SampleProjectIcons/NewPowerSystemModel_normal.png");
    } else if (foldername == QString("新能源模型")) {
        return QIcon(":/PluginProjectManagerView/resource/SampleProjectIcons/IntegratedEnergyModel_normal.png");
    } else if (foldername == QString("航空航天模型")) {
        return QIcon(":/PluginProjectManagerView/resource/SampleProjectIcons/AerospaceModel_normal.png");
    }

    return QIcon();
}

GridItem::GridItem(QWidget *parent) : QWidget(parent)
{
    m_svgLab = new QLabel(this);
    m_svgLab->setFixedSize(ICONSIZE);
    m_textLab = new QLabel(this);
    m_textLab->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    m_svgLab->setProperty("checked", "false");
    m_checked = false;
    m_svgLab->setStyleSheet("QLabel[checked=false]{border:1px solid #cecece;}"
                            "QLabel[checked=true]{border:1px solid #2ca6e0;}");
    m_textLab->setStyleSheet("font: 14px 'Microsoft YaHei'; color:#333333;padding: 0px; margin: 0px; ");
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setMargin(0);
    vlayout->addWidget(m_svgLab);
    vlayout->addSpacing(4);
    vlayout->addWidget(m_textLab);
    setVisible(true);
}

GridItem::~GridItem() { }

void GridItem::setItemImage() { }

void GridItem::setCurPrj(const QString &prjpath, const QString &englishname)
{
    QString prjName = QFileInfo(prjpath).baseName();
    m_textLab->setText(prjName);
    if ("en_US" == QPHelper::getLanguage()) {
        m_textLab->setText(englishname);
    }
    QString svgpath = QFileInfo(prjpath).dir().path();
    QString file = BUILDI_DEMO_PATH + "/" + svgpath + "/" + "preview.png";
    QPixmap pix;
    if (QFileInfo(file).isFile()) {
        pix.load(file);
        pix = pix.scaled(ICONSIZE, Qt::KeepAspectRatio);
    } else {
        QString svgname = BUILDI_DEMO_PATH + "/" + svgpath + "/" + "preview.svg";
        QSvgRenderer render;
        render.load(svgname);
        QSize size = render.defaultSize();
        pix = QPixmap(size);
        pix = pix.scaled(ICONSIZE, Qt::KeepAspectRatio);
        pix.fill(Qt::transparent);
        QPainter painter(&pix);
        painter.setRenderHints(QPainter::Antialiasing);
        render.render(&painter);
    }
    m_svgLab->setPixmap(pix);
    m_svgLab->setAlignment(Qt::AlignCenter);
}

void GridItem::mousePressEvent(QMouseEvent *event)
{
    setChecked(!m_checked);
    return QWidget::mousePressEvent(event);
}
bool GridItem::isChecked() const
{
    return m_checked;
}
void GridItem::setChecked(bool checked)
{
    if (checked == m_checked)
        return;
    m_checked = checked;
    m_svgLab->setProperty("checked", checked);
    m_svgLab->style()->unpolish(m_svgLab);
    m_svgLab->style()->polish(m_svgLab);
    emit checkedChanged(checked);
}
void GridItem::onSelectOne(int index)
{
    if (index != property("index").toInt()) {
        m_svgLab->setProperty("checked", false);
        m_checked = false;
        m_svgLab->style()->unpolish(m_svgLab);
        m_svgLab->style()->polish(m_svgLab);
    }
}