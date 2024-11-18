#include "createdrawingboardwidget.h"
#include "CommonModelAssistant.h"
#include "GlobalAssistant.h"
#include "GraphicsModelingKernel/CanvasContext.h"
#include "GraphicsModelingKernel/GraphicsKernelDefinition.h"
#include "KLWidgets/KColorDialog.h"
#include <QColorDialog>
#include <QGridLayout>
#include <QUuid>
#include <QValidator>

#include "PropertyServerMng.h"

// 属性页服务
#include "server/PropertyManagerServer/IPropertyManagerServer.h"
using namespace Kcc::PropertyManager;
#include "CoreLib/ServerManager.h"

USE_LOGOUT_("RightGraphicsEditWidget")

static const QString BoardName = QObject::tr("Draw Board Name");    // 画板名称
static const QString BoardTypeSTR = QObject::tr("Draw Board Type"); // 画板类型

static const int LabelFixedWidth = 60;
static const int WidgetMinHeight = 360;
static const int BOARDING_SIZE_MAX_LENGTH = 10000;
static const int BOARDING_SIZE_MIN_WIDE = 100;
static const int BOARDING_SIZE_MIN_HIGH = 100;

// 右下角图例允许最大字号
static const int RightGraphicsFontMaxSize = 14;

static const QString ValueRangeInvalide = "invalide";
static const QString ValueRelianceSize = "relianceSize";
// keywords基础
static const QString BoardOrientation = QObject::tr("Arrangement Direction"); // 排列方向
static const QString BoardSize = QObject::tr("Drawing Board Size");           // 画板尺寸
static const QString BoardWidth = QObject::tr("Drawing Board Width(mm)");     // 画板宽(mm)
static const QString BoardHeight = QObject::tr("Drawing Board Height(mm)");   // 画板高(mm)
static const QString BackgroundColor = QObject::tr("Background Color");       // 背景颜色
static const QString BoardScale = QObject::tr("Zoom Ratio%");                 // 缩放比例%
// static const QString BoardActiveStatus = QString("画板激活状态"); 由数据管理器管理
static const QString LegendDisplay = QObject::tr("Show Legend");              // 显示图例
static const QString GridAlignment = QObject::tr("Component Grid Alignment"); // 元件网格对齐
static const QString IdentificationDisplay =
        QObject::tr("Display Bus Single/Three Line Identification"); // 显示母线单线/三线标识
static const QString GridStyle = QObject::tr("Background Style");    // 背景式样

// 连接线
static const QString LineRoutingStyle = QObject::tr("Connection Line Path"); // 连接线路径 连接线布局方式，正交，非正交
static const QString LineStyle = QObject::tr("Connection Line Type");                  // 连接线线型
static const QString LineColor = QObject::tr("Connection Line Color");                 // 连接线颜色
static const QString LineSelectColor = QObject::tr("Connection Line Selection Color"); // 连接线选中颜色
// 图例
static const QString BoardAuthor = QObject::tr("Author");              // 作者
static const QString BoardType_1 = QObject::tr("Type");                // 类型
static const QString BoardDate = QObject::tr("Last Modified Date");    // 最后修改日期
static const QString BoardVersion = QObject::tr("Version");            // 版本
static const QString BoardName_1 = QObject::tr("Name");                // 名称
static const QString BoardDescriptrion = QObject::tr("Explanation");   // 说明信息
static const QString RightGraphicsFont = QObject::tr("Font Settings"); // 字体设置

// 属性枚举
static const QString BoardOrientationH = QObject::tr("Horizontal Arrangement"); // 横向排列
static const QString BoardOrientationV = QObject::tr("Vertical Arrangement");   // 纵向排列
static const QString RoutingOrthogonal = QObject::tr("Orthogonal");             // 正交
static const QString RoutingNonorthogonal = QObject::tr("Non Orthogonal");      // 非正交

static const QString SizeA0 = QString("A0");
static const QString SizeA1 = QString("A1");
static const QString SizeA2 = QString("A2");
static const QString SizeA3 = QString("A3");
static const QString SizeA4 = QString("A4");
static const QString SizeUserDefine = QObject::tr("UserDefine"); // 自定义

static const QString LineStyleSolidLine = QObject::tr("Solid Line");              // 实线
static const QString LineStyleDashLine = QObject::tr("Dashed Line");              // 虚线
static const QString LineStyleDotLine = QObject::tr("Dotted Line");               // 点线
static const QString LineStyleDashDotLine = QObject::tr("Dot Dash");              // 点划线
static const QString LineStyleDashDotDotLine = QObject::tr("Draw a Dotted Line"); // 划点点线

// 网格线属性枚举
static const QString GridStyle_NoLines = QObject::tr("None");  // 无
static const QString GriStyle_GridLines = QObject::tr("Grid"); // 网格
static const QString GriStyle_Dots = QObject::tr("Lattice");   // 点阵
static const QString GridStyleRange = QString("%1=%2,%3=%4,%5=%6")
                                              .arg(GriStyle_Dots)
                                              .arg(QString::number(CanvasContext::kGridShowDotGrid))
                                              .arg(GriStyle_GridLines)
                                              .arg(QString::number(CanvasContext::kGridShowFlag))
                                              .arg(GridStyle_NoLines)
                                              .arg(QString::number(CanvasContext::kGridNoneFlag));
static const QString SizeRange =
        QString("%1,%2,%3,%4,%5,%6").arg(SizeA0).arg(SizeA1).arg(SizeA2).arg(SizeA3).arg(SizeA4).arg(SizeUserDefine);

CreateDrawingboardWidget::CreateDrawingboardWidget(PCanvasContext pCanvasCtx, bool isnewboard,
                                                   const QString &initBboardType, QWidget *parent)
    : CWidget(parent),
      m_pCanvasCtx(pCanvasCtx),
      m_bNewBoard(isnewboard),
      m_InitBoardType(initBboardType),
      m_oldBoardName(""),
      m_oldScale(0),
      m_labelBoardName(nullptr),
      m_leditBoardName(nullptr),
      m_labelBoardType(nullptr),
      m_cboxBoardType(nullptr),
      m_tableView(nullptr),
      m_boardTableModel(nullptr),
      m_editBoardName(false),
      dataStatus(false)
{
    initUi();
}

CreateDrawingboardWidget::~CreateDrawingboardWidget() { }

bool CreateDrawingboardWidget::isParameterChanged()
{
    if (!m_leditBoardName || !m_cboxBoardType || !m_boardTableModel || (!m_pCanvasCtx && !m_bNewBoard))
        return false;

    if (m_cboxBoardType->currentText() != getDrawBoardTypeToString() || m_boardTableModel->checkValueChange()) {
        return true;
    }
    if (m_bNewBoard) {
        if (!m_leditBoardName->text().trimmed().isEmpty())
            return true;
    } else {
        if (m_leditBoardName->text().trimmed() != m_pCanvasCtx->name())
            return true;
    }
    return false;
}

bool CreateDrawingboardWidget::checkLegitimacy(QString &errorinfo)
{
    errorinfo = "";
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr || m_boardTableModel == nullptr) {
        errorinfo = tr("the drawing board pointer is empty!"); // 画板指针为空！
        return false;
    }
    QString oldboardname = getOldBoardName();
    QString newboardname = getBoardName();
    if (newboardname.trimmed().isEmpty()) {
        errorinfo = NPS::ERRORINFO_BOARDNAME_EMPTY;
        return false;
    }
    if (oldboardname != newboardname || m_bNewBoard) {
        if (!CMA::checkBoardNameValid(newboardname, errorinfo)) {
            return false;
        }
    }
    if (!m_tableView->isVisible()) {
        return true;
    }

    // CustomModelItem widtitem = m_boardTableModel->getModelItem(BoardWidth);
    // CustomModelItem heightitem = m_boardTableModel->getModelItem(BoardHeight);
    // if ((!widtitem.isValid() || !heightitem.isValid()) || heightitem.value.toInt() <= BOARDING_SIZE_MIN_HIGH
    //     || heightitem.value.toInt() > BOARDING_SIZE_MAX_LENGTH || widtitem.value.toInt() <= BOARDING_SIZE_MIN_WIDE
    //     || widtitem.value.toInt() > BOARDING_SIZE_MAX_LENGTH) {
    //     errorinfo =
    //             tr("%1 is illegal must be greater than %2 x %3 and less than or equal to %4 x %5!") //%1不合法
    //                                                                                                 // 必须大于%2 x
    //                                                                                                 //%3且小于等于%4
    //                                                                                                 x
    //                                                                                                 //%5！
    //                     .arg(QString("%1 x %2").arg(widtitem.value.toString()).arg(heightitem.value.toString()))
    //                     .arg(BOARDING_SIZE_MIN_WIDE)
    //                     .arg(BOARDING_SIZE_MIN_HIGH)
    //                     .arg(BOARDING_SIZE_MAX_LENGTH)
    //                     .arg(BOARDING_SIZE_MAX_LENGTH);
    //     return false;
    // }

    // if (!m_bNewBoard) {
    //     // CustomModelItem BoardOrientItem = m_boardTableModel->getModelItem(BoardOrientation);
    //     //  QString orientationstr = ((m_pCanvasCtx->direction() == 0) ? BoardOrientationH : BoardOrientationV);
    //     if (PropertyServerMng::getInstance().m_pGraphicsModelingServer != nullptr) {
    //         int sceneWidth = widtitem.value.toInt();
    //         int sceneHeight = heightitem.value.toInt();
    //         // if (BoardOrientItem.value.toString() == BoardOrientationH) {
    //         //     sceneWidth = widtitem.value.toInt();
    //         //     sceneHeight = heightitem.value.toInt();
    //         // } else {
    //         //     sceneHeight = widtitem.value.toInt();
    //         //     sceneWidth = heightitem.value.toInt();
    //         // }
    //         if (sceneWidth != m_pCanvasCtx->size().width() || sceneHeight != m_pCanvasCtx->size().height()) {
    //             QRectF rect = QRectF(0, 0, 3.7795 * sceneWidth, 3.7795 * sceneHeight);
    //             QRectF fitRect =
    //                     PropertyServerMng::getInstance().m_pGraphicsModelingServer->getMinRect(m_pCanvasCtx->uuid());
    //             if (fitRect.width() >= rect.width() || fitRect.height() >= rect.height() && !fitRect.isEmpty()) {
    //                 errorinfo = tr("The content goes beyond the artboard boundary"); // 内容超出画板边界
    //                 return false;
    //             }
    //         }
    //     }
    // }

    CustomModelItem scaleitem = m_boardTableModel->getModelItem(BoardScale);
    if (scaleitem.value.toInt() > 500 || scaleitem.value.toInt() < 20) {
        errorinfo = tr("%1 is illegal must between[20,500]").arg(BoardScale); //%1不合法 必须在[20,500]区间内
        return false;
    }
    return true;
}

bool CreateDrawingboardWidget::saveDrawingBoardData(PCanvasContext pCanvasCtx)
{
    CanvasContext *canvasCtx = pCanvasCtx.data();
    if (!m_boardTableModel || !pCanvasCtx || !canvasCtx) {
        return false;
    }

    pCanvasCtx->setName(getBoardName());
    // 保存数据
    int oldorientation = pCanvasCtx->direction(); // 0横向，1纵向
    // QSize oldboardSize = pCanvasCtx->size();
    QString oldcolorstr = pCanvasCtx->name();
    int oldscalevalue = pCanvasCtx->scale() * 100;

    bool oldlegenddisplayvalue = canvasCtx->getCanvasProperty().isLegendVisible();
    bool oldidentificationDisplay = canvasCtx->getCanvasProperty().isBusbarPhapseVisible();
    bool oldSimuSortDisplay = canvasCtx->isShowModelRunningSort();
    int oldGridStyle = pCanvasCtx->gridFlag();
    QList<CustomModelItem> newlist = m_boardTableModel->getNewListData();
    CustomModelItem boardwidthitem = m_boardTableModel->getModelItem(BoardWidth);
    CustomModelItem boardheightitem = m_boardTableModel->getModelItem(BoardHeight);
    CustomModelItem coloritem = m_boardTableModel->getModelItem(BackgroundColor);
    foreach (CustomModelItem item, newlist) {
        // if (item.keywords == BoardOrientation) {
        //     int neworientation = ((BoardOrientationH == item.value.toString()) ? 0 : 1);
        //     if (neworientation != oldorientation) {
        //         pCanvasCtx->setDirection((QBoxLayout::Direction)neworientation);
        //     }
        // } else
        // if (item.keywords == BoardSize) {
        //     QSize newsize = QSize(boardwidthitem.value.toInt(), boardheightitem.value.toInt());
        //     if (newsize != oldboardSize) {
        //         pCanvasCtx->setSize(newsize);
        //     }
        // } else
        if (item.keywords == BackgroundColor) {
            if (coloritem.value.toString() != oldcolorstr) {
                pCanvasCtx->setBackgroundColor(QColor(coloritem.value.toString()));
            }
        } else if (item.keywords == BoardScale) {
            int newscale = item.value.toInt();
            if (newscale != oldscalevalue) {
                pCanvasCtx->setScale((qreal)newscale / 100);
            }
        } else if (item.keywords == LegendDisplay) {
            bool bdisplay = item.value.toBool();
            if (bdisplay != oldlegenddisplayvalue) {
                canvasCtx->getCanvasProperty().setLegendVisible(bdisplay);
            }
        } else if (item.keywords == IdentificationDisplay) {
            bool bdisplay = item.value.toBool();
            if (bdisplay != oldidentificationDisplay) {
                canvasCtx->getCanvasProperty().setBusbarPhapseVisible(bdisplay);
            }
        } else if (item.keywords == GridStyle) {
            int itemValue = item.value.toInt();
            if (itemValue != oldGridStyle) {
                canvasCtx->setGridFlag(CanvasContext::GridFlag(itemValue));
            }
        } /*else if (item.keywords == ShowSimuSort) {
            bool bdisplay = item.value.toBool();
            if (bdisplay != oldSimuSortDisplay) {
                canvasCtx->enableShowModelRunningSort(bdisplay);
            }
        }*/
    }

    return true;
}

QString CreateDrawingboardWidget::getOldBoardName()
{
    return m_oldBoardName;
}

double CreateDrawingboardWidget::getOldScale()
{
    return m_oldScale;
}

QString CreateDrawingboardWidget::getBoardName()
{
    if (m_leditBoardName != nullptr) {
        return m_leditBoardName->text().trimmed();
    }
    return "";
}

int CreateDrawingboardWidget::getBoardType()
{
    if (m_cboxBoardType != nullptr) {
        return getStringToDrawBoardType(m_cboxBoardType->currentText());
    }
    return ElecBoardModel::Type;
}

bool CreateDrawingboardWidget::getDataStatus()
{
    if (!m_bNewBoard) // 修改画板时
    {
        return dataStatus;
    }
    return false;
}

QList<CustomModelItem> CreateDrawingboardWidget::getDrawboardModelList()
{
    QList<CustomModelItem> modellistdata;

    if (!m_pCanvasCtx && !m_bNewBoard) {
        return modellistdata;
    }
    // QString orientationrange = QString("%1,%2").arg(BoardOrientationH).arg(BoardOrientationV);
    if (m_bNewBoard) {
        // 初始化数据
        // QString orientationstr = BoardOrientationH;
        // modellistdata.append(CustomModelItem(BoardOrientation, BoardOrientation, orientationstr,
        //                                      RoleDataDefinition::ControlTypeComboBox, true, orientationrange));
        // QSize boardSize = getSize(SizeA2);
        // modellistdata.append(CustomModelItem(BoardSize, BoardSize, SizeA2, RoleDataDefinition::ControlTypeComboBox,
        //                                      true, SizeRange));
        // modellistdata.append(CustomModelItem(BoardWidth, BoardWidth, boardSize.width(),
        //                                      RoleDataDefinition::ControlTypeInt, false, ValueRangeInvalide));
        // modellistdata.append(CustomModelItem(BoardHeight, BoardHeight, boardSize.height(),
        //                                      RoleDataDefinition::ControlTypeInt, false, ValueRangeInvalide));
        QString colorstr = QString("#ffffff");
        modellistdata.append(CustomModelItem(BackgroundColor, BackgroundColor, colorstr,
                                             RoleDataDefinition::ControlTypeColor, false, ValueRangeInvalide));
        int scalevalue = 100;
        modellistdata.append(CustomModelItem(BoardScale, BoardScale, scalevalue, RoleDataDefinition::ControlTypeInt,
                                             true, ValueRangeInvalide));
        modellistdata.append(CustomModelItem(LegendDisplay, LegendDisplay, true,
                                             RoleDataDefinition::ControlTypeCheckbox, true, ValueRangeInvalide));
        modellistdata.append(CustomModelItem(GridAlignment, GridAlignment, true,
                                             RoleDataDefinition::ControlTypeCheckbox, false, ValueRangeInvalide));
        modellistdata.append(CustomModelItem(GridStyle, GridStyle, CanvasContext::kGridShowDotGrid,
                                             RoleDataDefinition::ControlTypeComboBox, true, GridStyleRange));
    } else {
        CanvasContext *defaultCanvasCtx = dynamic_cast<CanvasContext *>(m_pCanvasCtx.data());
        if (!defaultCanvasCtx) {
            return modellistdata;
        }
        // int orientation = m_pCanvasCtx->direction(); // 0横向，1纵向
        // QString orientationstr = ((orientation == 0) ? BoardOrientationH : BoardOrientationV);
        // modellistdata.append(CustomModelItem(BoardOrientation, BoardOrientation, orientationstr,
        //                                      RoleDataDefinition::ControlTypeComboBox, true, orientationrange));
        // QSize boardSize = m_pCanvasCtx->size();

        // QString boardsizestr = getDrawingBoardSizeString(boardSize);
        // modellistdata.append(CustomModelItem(BoardSize, BoardSize, boardsizestr,
        //                                      RoleDataDefinition::ControlTypeComboBox, true, SizeRange));
        // modellistdata.append(CustomModelItem(BoardWidth, BoardWidth, boardSize.width(),
        //                                      RoleDataDefinition::ControlTypeInt,
        //                                      boardsizestr == SizeUserDefine ? true : false, ValueRangeInvalide));
        // modellistdata.append(CustomModelItem(BoardHeight, BoardHeight, boardSize.height(),
        //                                      RoleDataDefinition::ControlTypeInt,
        //                                      boardsizestr == SizeUserDefine ? true : false, ValueRangeInvalide));
        QString colorstr = m_pCanvasCtx->backgroundColor().name();
        modellistdata.append(CustomModelItem(BackgroundColor, BackgroundColor, colorstr,
                                             RoleDataDefinition::ControlTypeColor, false, ValueRangeInvalide));
        // int scalevalue = pDrawboard->externalData[RealTimeDataDefine::percentage].toInt();
        int scalevalue = qRound(m_pCanvasCtx->scale() * 100);
        modellistdata.append(CustomModelItem(BoardScale, BoardScale, scalevalue, RoleDataDefinition::ControlTypeInt,
                                             true, ValueRangeInvalide));
        /*if (!isNew && ElecBoardModel::Type == pDrawboard->getModelType())
        {
                bool bactive = ((DrawingBoardClass::activated ==
                        pDrawboard->getParameter(DrawingBoardClass::boardStatus).toInt()) ? true:false);
                m_data.append(BoardTableItem(BoardActiveStatus,bactive,RoleDataDefinition::ControlTypeCheckbox,true,ValueRangeInvalide));
        }*/

        modellistdata.append(CustomModelItem(LegendDisplay, LegendDisplay,
                                             defaultCanvasCtx->getCanvasProperty().isLegendVisible(),
                                             RoleDataDefinition::ControlTypeCheckbox, true, ValueRangeInvalide));
        modellistdata.append(CustomModelItem(GridAlignment, GridAlignment, true,
                                             RoleDataDefinition::ControlTypeCheckbox, false, ValueRangeInvalide));
        if (CanvasContext::kElectricalType == m_pCanvasCtx->type()) {
            bool displayidentification = defaultCanvasCtx->getCanvasProperty().isBusbarPhapseVisible();
            modellistdata.append(CustomModelItem(IdentificationDisplay, IdentificationDisplay, displayidentification,
                                                 RoleDataDefinition::ControlTypeCheckbox, true, ValueRangeInvalide));
        }
        // if (CanvasContext::kControlSystemType == m_pCanvasCtx->type()
        //     || CanvasContext::kUserDefinedType == m_pCanvasCtx->type()) {
        //     bool displaySimuSort = defaultCanvasCtx->isShowModelRunningSort();
        //     modellistdata.append(CustomModelItem(ShowSimuSort, ShowSimuSort, displaySimuSort,
        //                                          RoleDataDefinition::ControlTypeCheckbox, true, ValueRangeInvalide));
        // }
        modellistdata.append(CustomModelItem(GridStyle, GridStyle, defaultCanvasCtx->gridFlag(),
                                             RoleDataDefinition::ControlTypeComboBox,
                                             m_pCanvasCtx->getLocked() ? false : true, GridStyleRange));
    }
    return modellistdata;
}

QString CreateDrawingboardWidget::getBoardInitName(const QString &boardType)
{
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr) {
        return QString();
    }
    QString initName = "";
    if (PMKEY::BoardTypeElectrical == boardType) {
        initName = "Grid";
    } else if (PMKEY::BoardTypeControl == boardType) {
        initName = "ControlSystem";
    } else if (PMKEY::BoardTypeCombine == boardType) {
        initName = "Combine";
    } else if (PMKEY::BoardTypeComplex == boardType) {
        initName = "Complex";
    } else if (PMKEY::BoardTypeElecCombine == boardType) {
        initName = "ElectricalCombine";
    }
    int number = 1;
    QString realName = QString("%1_1").arg(initName);
    while (PropertyServerMng::getInstance().m_projectManagerServer->IsExistModel(realName)) {
        realName = QString("%1_%2").arg(initName).arg(QString::number(++number));
    }
    return realName;
}

void CreateDrawingboardWidget::initUi()
{
    if (!m_pCanvasCtx && !m_bNewBoard) {
        return;
    }
    m_boardTypeMapInitName.clear();
    QGridLayout *pGridLayout = new QGridLayout(this);
    pGridLayout->setMargin(0);
    pGridLayout->setVerticalSpacing(8);
    m_labelBoardName = new QLabel(this);
    // m_labelBoardName->setFixedWidth(LabelFixedWidth);
    m_labelBoardName->setText(BoardName);
    m_leditBoardName = new KLineEdit(this);
    m_leditBoardName->setValidator(new QRegExpValidator(QRegExp(NPS::REG_FILE_NAME)));

    m_labelBoardType = new QLabel(this);
    // m_labelBoardType->setFixedWidth(LabelFixedWidth);
    m_labelBoardType->setText(BoardTypeSTR);
    m_cboxBoardType = new QComboBox(this);
    m_cboxBoardType->setView(new QListView());
    QStringList boxtypelist = QStringList()
            << PMKEY::BoardTypeControl << PMKEY::BoardTypeElectrical << PMKEY::BoardTypeCombine
            << PMKEY::BoardTypeComplex << PMKEY::BoardTypeElecCombine;
    m_cboxBoardType->addItems(boxtypelist);

    if (m_bNewBoard) {
        m_leditBoardName->setEnabled(true);
        if (!m_InitBoardType.isEmpty()) {
            m_leditBoardName->setText(getBoardInitName(m_InitBoardType));
            m_cboxBoardType->setCurrentIndex(m_cboxBoardType->findText(m_InitBoardType));
            m_cboxBoardType->setEnabled(false);
        } else {
            m_leditBoardName->setText(getBoardInitName(PMKEY::BoardTypeControl));
            m_cboxBoardType->setCurrentIndex(0);
            m_cboxBoardType->setEnabled(true);
        }
        m_oldBoardName = "";
        m_boardTypeMapInitName.insert(PMKEY::BoardTypeElectrical, getBoardInitName(PMKEY::BoardTypeElectrical));
        m_boardTypeMapInitName.insert(PMKEY::BoardTypeControl, getBoardInitName(PMKEY::BoardTypeControl));
        m_boardTypeMapInitName.insert(PMKEY::BoardTypeComplex, getBoardInitName(PMKEY::BoardTypeComplex));
        m_boardTypeMapInitName.insert(PMKEY::BoardTypeCombine, getBoardInitName(PMKEY::BoardTypeCombine));
        m_boardTypeMapInitName.insert(PMKEY::BoardTypeElecCombine, getBoardInitName(PMKEY::BoardTypeElecCombine));
    } else {
        m_oldBoardName = m_pCanvasCtx->name();
        m_oldScale = m_pCanvasCtx->scale();

        m_leditBoardName->setEnabled(true);
        m_cboxBoardType->setEnabled(false);
        m_leditBoardName->setText(m_oldBoardName);
        int indexno = m_cboxBoardType->findText(getDrawBoardTypeToString());
        m_cboxBoardType->setCurrentIndex(indexno);
    }

    m_tableView = new TableView(this);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // m_tableView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    m_tableView->setItemDelegate(new CustomTableItemDelegate(this));
    m_boardTableModel = new PropertyTableModel(m_tableView);
    m_boardTableModel->setModelData(getDrawboardModelList(),
                                    QStringList() << CMA::ATTRIBUTE_NAME << CMA::ATTRIBUTE_VALUE); // 属性名 属性值
    m_tableView->setModel(m_boardTableModel);

    pGridLayout->addWidget(m_labelBoardName, 0, 0, 1, 1);
    pGridLayout->addWidget(m_leditBoardName, 0, 1, 1, 3);
    pGridLayout->addWidget(m_labelBoardType, 1, 0, 1, 1);
    pGridLayout->addWidget(m_cboxBoardType, 1, 1, 1, 3);
    pGridLayout->addWidget(m_tableView, 2, 0, 1, 4);

    connect(m_tableView, SIGNAL(doubleClicked(const QModelIndex &)), this,
            SLOT(onTableItemDoubleClicked(const QModelIndex &)));
    connect(m_boardTableModel, SIGNAL(modelDataItemChanged(const CustomModelItem &, const CustomModelItem &)), this,
            SLOT(onModelDataItemChanged(const CustomModelItem &, const CustomModelItem &)));
    connect(m_cboxBoardType, SIGNAL(currentTextChanged(const QString &)), this,
            SLOT(onCurrentTextChanged(const QString &)));
    connect(m_leditBoardName, SIGNAL(textEdited(const QString &)), this, SLOT(onTextEdited(const QString &)));
    // if (m_pDrawingBoard && m_pDrawingBoard->getParameter(DrawingBoardClass::createdFromFrame).toBool()) {
    //     m_tableView->setVisible(false);
    //     m_labelBoardType->setVisible(false);
    //     m_cboxBoardType->setVisible(false);
    //     QSpacerItem *vspacer = new QSpacerItem(20, 80, QSizePolicy::Minimum, QSizePolicy::Expanding);
    //     pGridLayout->addItem(vspacer, 3, 0, 1, 4);
    // }
}

QString CreateDrawingboardWidget::getDrawBoardTypeToString()
{
    if (m_bNewBoard) {
        return PMKEY::BoardTypeElectrical;
    }
    if (m_pCanvasCtx == nullptr)
        return "";
    int boardtype = m_pCanvasCtx->type();
    switch (boardtype) {
    case CanvasContext::kElectricalType:
        return PMKEY::BoardTypeElectrical;
    case CanvasContext::kControlSystemType:
        return PMKEY::BoardTypeControl;
    case CanvasContext::kUserDefinedType:
        return PMKEY::BoardTypeCombine;
    case CanvasContext::kUserDefinedFrameType:
        return PMKEY::BoardTypeComplex;
    case CanvasContext::kElecUserDefinedType:
        return PMKEY::BoardTypeElecCombine;
    default:
        break;
    }
    return "";
}

int CreateDrawingboardWidget::getStringToDrawBoardType(const QString &boardtypestr)
{
    if (boardtypestr.isEmpty())
        return ElecBoardModel::Type;
    if (PMKEY::BoardTypeElectrical == boardtypestr) {
        return ElecBoardModel::Type;
    } else if (PMKEY::BoardTypeControl == boardtypestr) {
        return ControlBoardModel::Type;
    } else if (PMKEY::BoardTypeCombine == boardtypestr) {
        return CombineBoardModel::Type;
    } else if (PMKEY::BoardTypeComplex == boardtypestr) {
        return ComplexBoardModel::Type;
    } else if (PMKEY::BoardTypeElecCombine == boardtypestr) {
        return ElecCombineBoardModel::Type;
    }
    return ElecBoardModel::Type;
}

QString CreateDrawingboardWidget::getDrawingBoardSizeString(QSize boardSize)
{
    if (boardSize == QSize(1189, 841)) {
        return SizeA0;
    } else if (boardSize == QSize(841, 594)) {
        return SizeA1;
    } else if (boardSize == QSize(594, 420)) {
        return SizeA2;
    } else if (boardSize == QSize(420, 297)) {
        return SizeA3;
    } else if (boardSize == QSize(297, 210)) {
        return SizeA4;
    } else
        return SizeUserDefine;
}

QSize CreateDrawingboardWidget::getSize(const QString &sizestr)
{
    if (sizestr == SizeA0) {
        return QSize(1189, 841);
    } else if (sizestr == SizeA1) {
        return QSize(841, 594);
    } else if (sizestr == SizeA2) {
        return QSize(594, 420);
    } else if (sizestr == SizeA3) {
        return QSize(420, 297);
    } else if (sizestr == SizeA4) {
        return QSize(297, 210);
    } else
        return QSize(510, 240);
}

void CreateDrawingboardWidget::onTableItemDoubleClicked(const QModelIndex &index)
{
    if (index.isValid() && m_boardTableModel != nullptr
        && RoleDataDefinition::ControlTypeColor == index.data(NPS::ModelDataTypeRole).toString()) {
        KColorDialog dlg(tr("Select Color"), this);
        QColor curColor;
        QString keyword = index.data(NPS::ModelDataKeywordsRole).toString();
        QString strColor = index.data(PropertyTableModel::CustomRole_ColorRole).toString();
        curColor.setNamedColor(strColor);
        dlg.setCurrentColor(curColor);
        if (dlg.exec() == KColorDialog::Ok) {
            QColor color = dlg.currentColor();
            m_boardTableModel->updateValue(keyword, color.name());
        }
    }
}

void CreateDrawingboardWidget::onModelDataItemChanged(const CustomModelItem &olditem, const CustomModelItem &newitem)
{
    // if (!olditem.isValid() || !newitem.isValid() || olditem.keywords != newitem.keywords
    //     || m_boardTableModel == nullptr) {
    //     return;
    // }
    // QString sizestr;
    // QModelIndex index;
    // index.isValid();
    // if (BoardSize == newitem.keywords) {
    //     QSize boardsize = getSize(newitem.value.toString());
    //     QString sizestr = getDrawingBoardSizeString(boardsize);
    //     CustomModelItem widtitem = m_boardTableModel->getModelItem(BoardWidth);
    //     CustomModelItem heightitem = m_boardTableModel->getModelItem(BoardHeight);
    //     if (SizeUserDefine == sizestr) {
    //         widtitem.bcanEdit = true;
    //         heightitem.bcanEdit = true;
    //     } else {
    //         widtitem.bcanEdit = false;
    //         heightitem.bcanEdit = false;
    //         widtitem.value = boardsize.width();
    //         heightitem.value = boardsize.height();
    //     }
    //     m_boardTableModel->updateValue(BoardWidth, widtitem);
    //     m_boardTableModel->updateValue(BoardHeight, heightitem);
    // }
}

void CreateDrawingboardWidget::onCurrentTextChanged(const QString &text)
{
    if (m_bNewBoard && m_leditBoardName != nullptr && !m_editBoardName) {
        m_leditBoardName->setText(m_boardTypeMapInitName[text]);
    }
}

void CreateDrawingboardWidget::onTextEdited(const QString &textstr)
{
    m_editBoardName = true;
}

// 连接线信息
ConnectorWidget::ConnectorWidget(PCanvasContext pCanvasCtx, QWidget *parent /*= nullptr*/)
    : CWidget(parent), m_pCanvasCtx(pCanvasCtx), m_tableView(nullptr), m_connectorTableModel(nullptr)
{
    initUi();
}

ConnectorWidget::~ConnectorWidget() { }

bool ConnectorWidget::isValueChanged()
{
    if (m_connectorTableModel == nullptr)
        return false;
    QString errorinfo = "";
    return m_connectorTableModel->checkValueChange();
}

void ConnectorWidget::savePropertyToBoard()
{
    if (m_connectorTableModel == nullptr || m_pCanvasCtx == nullptr) {
        return;
    }
    // 保存数据
    int oldlinestyle = m_pCanvasCtx->lineStyle();
    QString oldlinecolorstr = m_pCanvasCtx->lineColor().name();
    QString oldlineselectcolorstr = m_pCanvasCtx->lineSelectColor().name();
    QList<CustomModelItem> newlist = m_connectorTableModel->getNewListData();
    foreach (CustomModelItem item, newlist) {
        if (item.keywords == LineStyle) {
            int linestyle = item.value.toInt();
            if (linestyle != oldlinestyle) {
                m_pCanvasCtx->setLineStyle(linestyle);
            }
        }
        if (item.keywords == LineColor) {
            QString linecolor = item.value.toString();
            if (linecolor != oldlinecolorstr) {
                m_pCanvasCtx->setLineColor(linecolor);
            }
        }
        if (item.keywords == LineSelectColor) {
            QString lineselectcolor = item.value.toString();
            if (lineselectcolor != oldlineselectcolorstr) {
                m_pCanvasCtx->setLineSelectColor(lineselectcolor);
            }
        }
    }
}

void ConnectorWidget::initUi()
{
    QGridLayout *pGridLayout = new QGridLayout(this);
    m_tableView = new TableView(this);
    // Qt5 fix
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableView->setItemDelegate(new CustomTableItemDelegate(this));
    m_connectorTableModel = new PropertyTableModel(m_tableView);
    m_connectorTableModel->setModelData(getModelList(),
                                        QStringList() << CMA::ATTRIBUTE_NAME << CMA::ATTRIBUTE_VALUE); // 属性名 属性值
    m_tableView->setModel(m_connectorTableModel);

    pGridLayout->addWidget(m_tableView, 0, 0, 1, 1);
    connect(m_tableView, SIGNAL(doubleClicked(const QModelIndex &)), this,
            SLOT(onTableItemDoubleClicked(const QModelIndex &)));
}

QList<CustomModelItem> ConnectorWidget::getModelList()
{
    QList<CustomModelItem> listdata;
    if (!m_pCanvasCtx) {
        return listdata;
    }
    QString stylerange = QString("1,2,3,4,5");
    // Qt::PenStyle lineStyle = (Qt::PenStyle)pDrawboard->externalData.value(GKD::CANVAS_LINE_STYLE,
    // Qt::SolidLine).toInt();
    int lineStyleInt = m_pCanvasCtx->lineStyle();
    listdata.append(CustomModelItem(LineStyle, LineStyle, lineStyleInt, NPS::DataType_LineStyle, true, stylerange));
    QString colorstr = m_pCanvasCtx->lineColor().name();
    listdata.append(CustomModelItem(LineColor, LineColor, colorstr, RoleDataDefinition::ControlTypeColor, true,
                                    ValueRangeInvalide));
    QString selectColorstr = m_pCanvasCtx->lineSelectColor().name();
    listdata.append(CustomModelItem(LineSelectColor, LineSelectColor, selectColorstr,
                                    RoleDataDefinition::ControlTypeColor, true, ValueRangeInvalide));
    QString linestyerange = QString("%1,%2").arg(RoutingOrthogonal).arg(RoutingNonorthogonal);
    listdata.append(CustomModelItem(LineRoutingStyle, LineRoutingStyle, RoutingOrthogonal,
                                    RoleDataDefinition::ControlTypeComboBox, false, linestyerange));
    return listdata;
}

void ConnectorWidget::onTableItemDoubleClicked(const QModelIndex &index)
{
    if (index.isValid() && m_connectorTableModel != nullptr
        && RoleDataDefinition::ControlTypeColor == index.data(NPS::ModelDataTypeRole).toString()) {
        KColorDialog dlg(tr("Select Color"), this);
        QColor curColor;
        QString keyword = index.data(NPS::ModelDataKeywordsRole).toString();
        QString strColor = index.data(PropertyTableModel::CustomRole_ColorRole).toString();
        curColor.setNamedColor(strColor);
        dlg.setCurrentColor(curColor);
        if (dlg.exec() == KColorDialog::Ok) {
            QColor color = dlg.currentColor();
            m_connectorTableModel->updateValue(keyword, color.name());
        }
    }
}

// 画板右下角图例信息编辑画面
RightGraphicsEditWidget::RightGraphicsEditWidget(PCanvasContext pCanvasCtx, QWidget *parent)
    : CWidget(parent), m_pCanvasCtx(pCanvasCtx), m_tableView(nullptr), m_graphicsTableModel(nullptr)
{
    initUi();
}

RightGraphicsEditWidget::~RightGraphicsEditWidget() { }

bool RightGraphicsEditWidget::isValueChanged()
{
    if (m_graphicsTableModel == nullptr)
        return false;
    QString errorinfo = "";
    return m_graphicsTableModel->checkValueChange();
}

void RightGraphicsEditWidget::savePropertyToBoard()
{
    if (m_pCanvasCtx == nullptr || m_graphicsTableModel == nullptr) {
        return;
    }

    // 保存数据
    QString olddescprtionstr = m_pCanvasCtx->description();
    QString oldauthorstr = m_pCanvasCtx->author();

    QList<CustomModelItem> newlist = m_graphicsTableModel->getNewListData();
    foreach (CustomModelItem item, newlist) {
        if (item.keywords == BoardDescriptrion) {
            QString newdesc = item.value.toString();
            if (newdesc != olddescprtionstr) {
                m_pCanvasCtx->setDescription(newdesc);
            }
        }
        if (item.keywords == BoardAuthor) {
            QString newAuthor = item.value.toString();
            if (newAuthor != oldauthorstr) {
                m_pCanvasCtx->setAuthor(newAuthor);
            }
        }
    }
}

void RightGraphicsEditWidget::initUi()
{
    QGridLayout *pGridLayout = new QGridLayout(this);
    m_tableView = new TableView(this);
    // Qt5 fix
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableView->setItemDelegate(new CustomTableItemDelegate(this));
    m_graphicsTableModel = new PropertyTableModel(m_tableView);
    m_graphicsTableModel->setModelData(getModelList(),
                                       QStringList() << CMA::ATTRIBUTE_NAME << CMA::ATTRIBUTE_VALUE); // 属性名 属性值
    m_tableView->setModel(m_graphicsTableModel);

    pGridLayout->addWidget(m_tableView, 0, 0, 1, 1);

    connect(m_tableView, SIGNAL(doubleClicked(const QModelIndex &)), this,
            SLOT(onRgTableItemDoubleClicked(const QModelIndex &)));
}

QList<CustomModelItem> RightGraphicsEditWidget::getModelList()
{
    QList<CustomModelItem> listdata;
    // 设置属性页右下角图例信息
    if (!m_pCanvasCtx) {
        return listdata;
    }
    CanvasProperty &canvasProperty = m_pCanvasCtx->getCanvasProperty();
    // QString namestr = pDrawboard->getName();
    QString namestr = m_pCanvasCtx->name();
    listdata.append(CustomModelItem(BoardName_1, BoardName_1, namestr, RoleDataDefinition::ControlTypeTextbox, false,
                                    ValueRangeInvalide));
    // QString boardVersionstr = pDrawboard->getParameter(DrawingBoardClass::verID).toString();
    QString boardVersionstr = m_pCanvasCtx->version();
    listdata.append(CustomModelItem(BoardVersion, BoardVersion, boardVersionstr, RoleDataDefinition::ControlTypeTextbox,
                                    false, ValueRangeInvalide));
    int boardType = m_pCanvasCtx->type();
    QString boardTypestr;
    if (boardType == CanvasContext::kElectricalType) {
        boardTypestr = PMKEY::BoardTypeElectrical;
    } else if (boardType == CanvasContext::kControlSystemType) {
        boardTypestr = PMKEY::BoardTypeControl;
    } else if (boardType == CanvasContext::kUserDefinedType) {
        boardTypestr = PMKEY::BoardTypeCombine;
    } else if (boardType == CanvasContext::kUserDefinedFrameType) {
        boardTypestr = PMKEY::BoardTypeComplex;
    } else if (boardType == CanvasContext::kElecUserDefinedType) {
        boardTypestr = PMKEY::BoardTypeElecCombine;
    }
    listdata.append(
            CustomModelItem(BoardType_1, BoardType_1, boardTypestr, RoleDataDefinition::ControlTypeTextbox, false));
    QString boardAuthorstr = m_pCanvasCtx->author();
    listdata.append(
            CustomModelItem(BoardAuthor, BoardAuthor, boardAuthorstr, RoleDataDefinition::ControlTypeTextbox, true));
    QString descprtionstr = m_pCanvasCtx->description();
    listdata.append(CustomModelItem(BoardDescriptrion, BoardDescriptrion, descprtionstr,
                                    RoleDataDefinition::ControlTypeTextbox, true));
    QString boardDatestr = m_pCanvasCtx->getLastModifyTime();
    listdata.append(CustomModelItem(BoardDate, BoardDate, boardDatestr, RoleDataDefinition::ControlTypeTextbox, false));
    QStringList fontList = canvasProperty.getCopyRightFont().toString().split(",", QString::SkipEmptyParts);
    QString font;
    if (fontList.size() >= 2) {
        font = fontList[0] + "," + fontList[1];
    }
    listdata.append(
            CustomModelItem(RightGraphicsFont, RightGraphicsFont, font, RoleDataDefinition::ControlTypeFont, false));
    return listdata;
}

void RightGraphicsEditWidget::onRgTableItemDoubleClicked(const QModelIndex &index)
{
    if (index.isValid() && m_graphicsTableModel != nullptr
        && RoleDataDefinition::ControlTypeFont == index.data(NPS::ModelDataTypeRole).toString()) {

        PIPropertyManagerServer pPropertyServer = RequestServer<IPropertyManagerServer>();

        if (pPropertyServer != nullptr) {
            CanvasProperty &canvasProperty = m_pCanvasCtx->getCanvasProperty();
            m_pTextFont = canvasProperty.getCopyRightFont();
            setfont = pPropertyServer->ShowFontSetDialog(m_pTextFont);
            if (m_pTextFont == setfont) {
                return;
            } else if (setfont.toString().split(",", QString::SkipEmptyParts)[1].toInt() > RightGraphicsFontMaxSize) {
                LOGOUT(tr("The maximum font size allowed for legends is [%1]").arg(RightGraphicsFontMaxSize),
                       LOG_ERROR);
                return;
            } else {
                canvasProperty.setCopyRightFont(setfont);
            }
        } else {
            return;
        }
        QStringList fontList = setfont.toString().split(",", QString::SkipEmptyParts);
        QString font = fontList[0] + "," + fontList[1];
        m_graphicsTableModel->setData(index, font);
    }
}
