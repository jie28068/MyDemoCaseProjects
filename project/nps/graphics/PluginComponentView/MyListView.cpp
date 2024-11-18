#include "MyListView.h"
#include "ContextMenu.h"
#include "CoreLib/ServerManager.h"
#include "GlobalAssistant.h"
#include "GlobalDefinition.h"
#include "server/PluginGraphicsModelingServer/IPluginGraphicModelingServer.h"
#include <QDrag>
#include <QPainter>

using namespace Kcc::PluginGraphicModeling;
using namespace Global;

MyListView::MyListView(const QString &strGroupName, QWidget *parent) : QListView(parent), m_strGroupName(strGroupName)
{
    setFocusPolicy(Qt::ClickFocus);
    setFrameShape(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setUniformItemSizes(true);
    // setSpacing(5);
    setMouseTracking(true);
    setContentsMargins(0, 0, 0, 0);
    // 下面三句实现流式布局
    setViewMode(QListView::IconMode);
    setFlow(QListView::LeftToRight);
    setResizeMode(QListView::Adjust);
}

MyListView::~MyListView() { }

bool MyListView::isEmpty()
{
    return model()->rowCount() == 0;
}

void MyListView::startDrag(Qt::DropActions supportedActions)
{
    QModelIndexList indexes = selectedIndexes();

    // 目前仅支持单行选中
    Q_ASSERT(indexes.size() == 1);
    const QModelIndex &index = indexes[0];

    if ((index.flags() & Qt::ItemIsDragEnabled) == 0) {
        return;
    }
    QMimeData *data = model()->mimeData(indexes);
    if (data == nullptr) {
        return;
    }

    QDrag *drag = new QDrag(this);
    // 取消显示类型关键字，直接用有没有图片资源判断
    if (index.data(Global::UrlRole) != NULL) {
        QImage image = qvariant_cast<QImage>(index.data(Global::UrlRole));
        // png描绘
        if (!image.isNull()) {
            QPixmap pixmap(QPixmap::fromImage(image));
            pixmap = pixmap.scaled(Global::DRAWING_ITEM_WIDTH,DRAWING_ITEM_HEIGHT);
            drag->setPixmap(pixmap);
            drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));
        } else {
            drawText(drag, index);
        }
    } else {
        drawText(drag, index);
    }
    drag->setMimeData(data);

    auto action = drag->exec(Qt::IgnoreAction | Qt::MoveAction | Qt::CopyAction, Qt::MoveAction);
    if (action == Qt::MoveAction) {
        // 列表视图中移动元器件是通过修改GroupName进行筛选的，Item不会新增和删除。
        setCurrentIndex(QModelIndex());
    }
    delete drag;
}

void MyListView::contextMenuEvent(QContextMenuEvent *e)
{
    QModelIndex index = indexAt(e->pos());
    if (!index.isValid()) {
        QListView::contextMenuEvent(e);
        return;
    }

    int contextType = index.data(ContextTypeRole).toInt();
    if (contextType == ContextMenu::Ignore || !index.data(Global::DragEnabledRole).toBool()) {
        QListView::contextMenuEvent(e);
        return;
    }

    QString strGroupName = index.data(GroupNameRole).toString();
    QString strName = index.data(NameRole).toString();
    QString strModuleName = index.data(ModuleNameRole).toString();
    auto pMenu = ContextMenuManager::instance(strModuleName)->createMenu();
#ifdef COMPILER_SUPPORT_BUILTIN_MODIFY
    if (contextType & ContextMenu::ModifyControlComponent) {
        pMenu->setModifyCodeEnable(strName);
    }
    if (contextType & ContextMenu::ModifyBoardComponent) {
        pMenu->setModifyBoardEnable(strName);
    }
//     if (contextType & ContextMenu::RemoveCodeComponent) {
//         pMenu->setRemoveCodeEnable(strName);
//     }

//     if (contextType & ContextMenu::RemoveBoardComponent) {
//         pMenu->setRemoveBoardEnable(strName);
//     }
    if (contextType & ContextMenu::elecModifyCodeComponent) {
        pMenu->setChangeElcBuildInEnable(strName);
    }
//     if (contextType & ContextMenu::elecRemoveCodeComponent) {
//         pMenu->removeElcBuildInEnable(strName);
//     }
#else
    if (QObject::tr(strGroupName.toUtf8()) == ControllerGroupCustomCode
        || QObject::tr(strGroupName.toUtf8()) == ControllerGroupCustomBoard) {
        if (contextType & ContextMenu::ModifyControlComponent) {
            pMenu->setModifyCodeEnable(strName);
        }
        if (contextType & ContextMenu::ModifyBoardComponent) {
            pMenu->setModifyBoardEnable(strName);
        }
        if (contextType & ContextMenu::RemoveCodeComponent) {
            pMenu->setRemoveCodeEnable(strName);
        }
        if (contextType & ContextMenu::RemoveBoardComponent) {
            pMenu->setRemoveBoardEnable(strName);
        }
    }
#endif

    if (contextType & ContextMenu::ShowProperty) {
        pMenu->setShowPropertyEnable(strName);
    }
    if (contextType & ContextMenu::AddComponentToBoard) {
        // 根据是否打开画板、打开画板的类型是否和模块类型匹配决定是否显示 添加模块到画板的action
        PIPluginGraphicModelingServer m_pDrawingBoardServer = RequestServer<IPluginGraphicModelingServer>();
        if (m_pDrawingBoardServer) {
            QString boardUuid = m_pDrawingBoardServer->getActiveDrawingBoardUUID();
            if (!boardUuid.isEmpty()) {
                PDrawingBoardClass board = m_pDrawingBoardServer->getDrawingBoardInfoById(boardUuid);
                if (board) {
                    int boardType = board->getModelType();
                    if (((boardType == Model::Ele_Board_Type || boardType == Model::Ele_CombineBoard_Type)
                         && index.data(CategoryRole).toInt() == Device)
                        || ((boardType == Model::Control_Board_Type || boardType == Model::Combine_Board_Type
                             || boardType == Model::Complex_Board_Type)
                            && index.data(CategoryRole).toInt() == Controller)) {
                        if (boardType == Model::Ele_Board_Type) {
                            if (strName != NPS::PROTOTYPENAME_EXTERNAL && strName != NPS::PROTOTYPENAME_SINGAL) {
                                pMenu->setAddComponentToBoardEnable(strName);
                            }
                        }
                        if (boardType == Model::Ele_CombineBoard_Type) {
                            pMenu->setAddComponentToBoardEnable(strName);
                        }
                        if (boardType == Model::Control_Board_Type) {
                            if (strName != NPS::PROTOTYPENAME_CTRLIN && strName != NPS::PROTOTYPENAME_CTRLOUT
                                && strName != NPS::PROTOTYPENAME_SLOT) {
                                pMenu->setAddComponentToBoardEnable(strName);
                            }
                        }
                        if (boardType == Model::Combine_Board_Type) {
                            if (strName != NPS::PROTOTYPENAME_SLOT && strName != NPS::SPECIAL_BLOCK_ELECINTERFACE) {
                                pMenu->setAddComponentToBoardEnable(strName);
                            }
                        }
                        if (boardType == Model::Complex_Board_Type) {
                            if (strName == NPS::PROTOTYPENAME_SLOT) {
                                pMenu->setAddComponentToBoardEnable(strName);
                            }
                        }
                    }
                }
            }
        }
    }

    if (contextType & ContextMenu::CreateComponentInstance) {
        pMenu->setCreateComponentInstanceEnable(strName);
    }
    if (contextType & ContextMenu::Help) {
        pMenu->showHelpEnable(strName);
    }
    if (pMenu->isEmpty()) {
        QListView::contextMenuEvent(e);
        return;
    }
    pMenu->setObjectName("uniformStyleMenu_cV");
    pMenu->exec(mapToGlobal(e->pos()));
    e->accept();
}

void MyListView::mouseDoubleClickEvent(QMouseEvent *e)
{
    QModelIndex i = indexAt(e->pos());
    if (i.isValid() && e->button() == Qt::LeftButton) {
        emit leftMouseButtonDoubleClicked(i.data(NameRole).toString());
        e->accept();
    } else {
        QListView::mouseDoubleClickEvent(e);
    }
}

void MyListView::mouseMoveEvent(QMouseEvent *event)
{
    QListView::mouseMoveEvent(event);
}

void MyListView::mouseReleaseEvent(QMouseEvent *e)
{
    QListView::mouseReleaseEvent(e);
    QModelIndex index = this->indexAt(e->pos());
    if (!index.isValid()) {
        this->setCurrentIndex(this->currentIndex().sibling(-1, -1));
    }
    update();
}

void MyListView::resizeEvent(QResizeEvent *event)
{
    calculateHeight();
    QListView::resizeEvent(event);
}

void MyListView::paintEvent(QPaintEvent *e)
{
    calculateHeight();
    QListView::paintEvent(e);
}

void MyListView::focusOutEvent(QFocusEvent *e)
{
    QListView::focusOutEvent(e);
    if (this->selectionModel() != nullptr) {
        this->selectionModel()->clearSelection();
    }
    this->setCurrentIndex(this->currentIndex().sibling(-1, -1));
    update();
}

void MyListView::calculateHeight()
{
    int itemCount = model()->rowCount();
    if (itemCount <= 0) {
        hide();
        return;
    }

    // 计算listview每行的图标个数，并计算出合理的高度
    // IconMode模式绘制item，需要的宽度为 spacing + item + spacing + 1
    // ListMode模式只需计算行高
    int shfr = sizeHintForRow(0);
    int shfc = sizeHintForColumn(0);
    int width = this->width() - 2; // 这里左右两边各减去一个像素
    int spacing = this->spacing();

    if (viewMode() == IconMode) {
        int countPerLine = 1;
        if ((width - spacing) % (shfc + spacing) == 0) {
            countPerLine = (width - spacing) / (shfc + spacing) - 1;
        } else {
            countPerLine = (width - spacing) / (shfc + spacing);
        }
        if (countPerLine <= 0) {
            return;
        }

        int rowNeed = (itemCount % countPerLine == 0) ? (itemCount / countPerLine) : (itemCount / countPerLine + 1);
        int height = qMax((shfr + spacing) * rowNeed, 1) + spacing + 6; // 上下各加一个像素，否则会有滚动效果
        setFixedHeight(height);
    } else {
        int height = qMax((shfr + spacing) * itemCount, 1) + spacing + 6; // 上下各加一个像素，否则会有滚动效果
        setFixedHeight(height);
    }
}

void MyListView::drawText(QDrag *drag, const QModelIndex &index)
{
    QPixmap pixmapController(Global::DRAWING_ITEM_WIDTH, Global::DRAWING_ITEM_HEIGHT);
    pixmapController.fill(QColor("#FFFFFF"));
    QPainter painter(&pixmapController);
    QPen pen(QColor("#252d3f"));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setFont(QFont("Arial", 8));
    QRectF textRect(5, 5, Global::DRAWING_ITEM_WIDTH - 10, Global::DRAWING_ITEM_HEIGHT - 10);
    QRectF drawRect(0, 0, Global::DRAWING_ITEM_WIDTH - 1, Global::DRAWING_ITEM_HEIGHT - 1);
    // QRectF fillRect(2, 2, DRAWING_ITEM_WIDTH - 4, DRAWING_ITEM_HEIGHT - 4);
    QString text = index.data(ReadableNameRole).toString();
    QFontMetrics fontWidth(painter.font());
    QString elidnote =
            fontWidth.elidedText(index.data(Qt::DisplayRole).toString(), Qt::ElideRight, 2 * textRect.width() - 6);
    int textW = painter.fontMetrics().width(text);
    int textH = painter.fontMetrics().height();
    // painter.fillRect(fillRect, QColor("#E6ECF5"));
    painter.drawText(textRect, Qt::TextWrapAnywhere | Qt::AlignCenter, elidnote);
    painter.drawRect(drawRect);
    if (index.data(Global::GroupNameRole).toString() == Global::ControllerGroupCustomBoard) {
        QRectF inRect(3, 3, Global::DRAWING_ITEM_WIDTH - 7, Global::DRAWING_ITEM_HEIGHT - 7);
        painter.drawRect(inRect);
    }

    if (!pixmapController.isNull()) {
        drag->setPixmap(pixmapController);
        drag->setHotSpot(QPoint(pixmapController.width() / 2, pixmapController.height() / 2));
    }
}
