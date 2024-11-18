#include "BookMark.h"
#include "KLWidgets/KCustomDialog.h"
#include "KLWidgets/KLineEdit.h"

BookMark &BookMark::getInstance()
{
    static BookMark Instance;
    return Instance;
}
BookMark::BookMark(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f),
      isfrist(false),
      m_allaction(new QList<QAction *>()),
      p_mroe(new QMenu(QString(tr("More...")))),
      m_row(0),
      isfristchangeditem(false),
      isdoubleclicked(false),
      ismore(false)
{
    dataPtr.reset(new BookMarkPrivate());
    AddBookMarkWidegt();
    ManageBookMarkWidegt();
}

// 重命名计数，快捷键标志位需要设为全局
QVector<QAction *> BookMark::shortcut_falg(9, nullptr);
QList<QString> *BookMark::m_count = new QList<QString>();

void BookMark::AddBookMarkWidegt()
{
    QWidget *m_widget = new QWidget(nullptr);
    QVBoxLayout *vLayout = new QVBoxLayout;
    QRegExpValidator *nameValidator = new QRegExpValidator(QRegExp("[^&]*"), this);
    m_addDlg = new KCustomDialog(nullptr);
    m_addDlg->setResizeAble(false);
    m_addDlg->setStandardReturnCodeFlag(true);
    m_OK = m_addDlg->addCustomButton(tr("Ok"));
    m_Cancel = m_addDlg->addCustomButton(tr("Cancel"));
    m_line = new KLineEdit(m_addDlg);
    m_line->setMaxLength(16);
    m_line->setValidator(nameValidator);
    m_addDlg->setCentralWidget(m_line);
    m_addDlg->setDefaultButton(m_OK);
    m_addDlg->setFixedSize(250, 150);
    m_addDlg->setTitle(tr("Add Bookmark"));
}

QString BookMark::findRename(QString str)
{
    QString str2 = str;
    int count = 1;
    for (int i = 0; i < m_count->size(); ++i) {
        for (int j = 0; j < m_count->size(); ++j) {
            if (m_count->at(j) == str2) {
                str2 = QString("%1(%2)").arg(str).arg(count);
                ++count;
            }
        }
    }
    return str2;
}

void BookMark::updateListWidget()
{
    m_mangertablewidget->clearContents();
    m_mangertablewidget->setRowCount(m_allaction->size() + p_mroe->actions().size());
    int row_count = -1;
    for (int i = 0; i < shortcut_falg.size(); ++i) {
        if (shortcut_falg.at(i) == nullptr) {
            continue;
        }
        row_count++;
        QVariantMap map = shortcut_falg.at(i)->data().toMap();
        QString name = map.find("name").value().toString();
        QString shortcut = map.find("shortcut").value().toString();
        QString time = map.find("time").value().toString();
        QString i_type = map.find("type").value().toString();
        QString type = readBroadName(i_type);
        QTableWidgetItem *m_name = new QTableWidgetItem(name);
        m_name->setFlags(m_name->flags() & (~Qt::ItemIsEditable)); // 设置不可编辑
        QTableWidgetItem *m_type = new QTableWidgetItem(type);
        m_type->setFlags(m_type->flags() & (~Qt::ItemIsEditable)); // 设置不可编辑
        QTableWidgetItem *m_time = new QTableWidgetItem(time);
        m_time->setFlags(m_time->flags() & (~Qt::ItemIsEditable)); // 设置不可编辑
        m_mangertablewidget->setItem(row_count, 0, new QTableWidgetItem(shortcut_falg.at(i)->text()));
        m_mangertablewidget->setItem(row_count, 1, new QTableWidgetItem(shortcut));
        m_mangertablewidget->setItem(row_count, 2, m_name);
        m_mangertablewidget->setItem(row_count, 3, m_type);
        m_mangertablewidget->setItem(row_count, 4, m_time);
    }

    for (int i = 0; i < p_mroe->actions().size(); ++i) {
        QString a = p_mroe->actions().at(i)->text();
        QVariantMap map = p_mroe->actions().at(i)->data().toMap();
        QString name = map.find("name").value().toString();
        QString shortcut = map.find("shortcut").value().toString();
        QString time = map.find("time").value().toString();
        QString i_type = map.find("type").value().toString();
        QString type = readBroadName(i_type);
        QTableWidgetItem *m_name = new QTableWidgetItem(name);
        m_name->setFlags(m_name->flags() & (~Qt::ItemIsEditable)); // 设置不可编辑
        QTableWidgetItem *m_type = new QTableWidgetItem(type);
        m_type->setFlags(m_type->flags() & (~Qt::ItemIsEditable)); // 设置不可编辑
        QTableWidgetItem *m_time = new QTableWidgetItem(time);
        m_time->setFlags(m_time->flags() & (~Qt::ItemIsEditable)); // 设置不可编辑
        m_mangertablewidget->setItem(i + m_allaction->size(), 0, new QTableWidgetItem(p_mroe->actions().at(i)->text()));
        m_mangertablewidget->setItem(i + m_allaction->size(), 1, new QTableWidgetItem("None"));
        m_mangertablewidget->setItem(i + m_allaction->size(), 2, m_name);
        m_mangertablewidget->setItem(i + m_allaction->size(), 3, m_type);
        m_mangertablewidget->setItem(i + m_allaction->size(), 4, m_time);
    }

    // 内容居中显示
    for (int i = 0; i < m_mangertablewidget->rowCount(); i++) {
        for (int j = 0; j < m_mangertablewidget->columnCount(); j++) {
            if (m_mangertablewidget->item(i, j) == nullptr) {
                continue;
            }
            m_mangertablewidget->item(i, j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
    }
}

QString BookMark::readBroadName(QString i_type)
{
    switch (i_type.toInt()) {
    case 0:
        i_type = tr("Invalid Canvas");
        break;
    case 1:
        i_type = tr("Network Model");
        break;
    case 2:
        i_type = tr("Network Model");
        break;
    case 3:
        i_type = tr("Stereotype Templates");
        break;
    case 4:
        i_type = tr("Composite Templates");
        break;
    case 5:
        i_type = tr("Monitoring Panel");
        break;
    case 6:
        i_type = tr("Network Model");
        break;
    default:
        i_type = tr("Unknown Template");
        break;
    }
    return i_type;
}

void BookMark::setCanvasContext(QSharedPointer<CanvasContext> context)
{
    data = context;
}

void BookMark::actionCreateAddBookmark()
{
    m_addDlg->exec();
    ismore = false;
    auto p_btn = m_addDlg->clickedButton();
    if (p_btn == m_OK && !m_line->text().isEmpty() && m_line->text().trimmed() != ""
        && m_line->text().trimmed() != tr("More...")) {
        QAction *new_action; // 每个新增的action
        QString str = findRename(m_line->text());
        new_action = new QAction(str, this);
        QString s_shortcut = "";
        // 快捷键设置
        for (int i = 0; i < 9; ++i) {
            if (shortcut_falg[i] == nullptr) { // 标志数组，有空就插入到次处
                QKeySequence *keyshortcut = new QKeySequence(Qt::CTRL + (48 + i + 1));
                s_shortcut = keyshortcut->toString();
                new_action->setShortcut(*keyshortcut);
                shortcut_falg[i] = new_action;
                break;
            }
        }

        // 刷新重排
        realignment();
        // 各个容器添加新数据
        m_count->push_back(str);
        if (m_allaction->size() < 9) {
            m_allaction->push_back(new_action);
        } else { // 菜单数大于9个时,插入”更多“二级菜单
            if (!isfrist) {
                p_mroe->addAction(new_action);
                dataPtr->getBookMarkMenu()->addMenu(p_mroe);
                isfrist = true;
                ismore = true;
            }
            if (p_mroe->actions().size() > 0) {
                p_mroe->addAction(new_action);
                ismore = true;
            }
        }
        m_line->clear();
        // 设置数据
        new_action->setData(savedata(s_shortcut, str));
        connect(new_action, SIGNAL(triggered()), this, SLOT(onUseBookmark()));
    }
    serialize();
}

void BookMark::actionCreateManageBookmark()
{
    updateListWidget();
    m_manageDlg->exec();
}

void BookMark::ManageBookMarkWidegt()
{
    QWidget *m_widget = new QWidget(nullptr);
    QVBoxLayout *vLayout = new QVBoxLayout;
    m_mangertablewidget = new BookMarkTableWidget(this, nullptr);
    // 设置列数
    m_mangertablewidget->setColumnCount(5);
    m_mangertablewidget->setObjectName("BookMark_Mangertablewidget");
    // 设置表头
    m_mangertablewidget->setHorizontalHeaderLabels(QStringList()
                                                   << tr("Bookmark Name") << tr("Shortcut Key") << tr("Canvas Name")
                                                   << tr("Canvas Type") << tr("Last Modified Time"));
    // 设置可伸缩
    m_mangertablewidget->horizontalHeader()->setStretchLastSection(true);
    QLabel *helpTextLabel = new QLabel(tr("Tip: Book signature and shortcut keys can be modified by double - "
                                          "clicking!")); // 提示：书签名与快捷键可通过双击进行修改！
    vLayout->addWidget(helpTextLabel);
    vLayout->addWidget(m_mangertablewidget);
    m_widget->setLayout(vLayout);
    m_manageDlg = new KCustomDialog(tr("Manager bookmark"), m_widget, KBaseDlgBox::StandardButton::Ok);
    m_manageDlg->setStandardReturnCodeFlag(false);
    m_delete = m_manageDlg->addCustomButton(tr("Delete"));
    m_delete->setEnabled(false);
    m_mangertablewidget->setMinimumSize(800, 300);
    m_manageDlg->setDefaultButton(m_delete);
    connect(m_mangertablewidget, SIGNAL(itemClicked(QTableWidgetItem *)), this,
            SLOT(deleteBookmark(QTableWidgetItem *)));
    connect(m_delete, SIGNAL(clicked()), this, SLOT(deleteItemBookmark()));
    connect(m_mangertablewidget, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this,
            SLOT(changeItemBookmark(QTableWidgetItem *)));
    connect(m_mangertablewidget, SIGNAL(itemChanged(QTableWidgetItem *)), this,
            SLOT(changeBookmark(QTableWidgetItem *)));
    connect(m_manageDlg, SIGNAL(closed(int)), this, SLOT(undeletePtn(int)));
}

void BookMark::onUseBookmark()
{
    QAction *ac = (QAction *)(sender());
    QVariantMap map = ac->data().toMap();
    emit previewUUIDChanged(map.find("uuid").value().toString());                                             // 画板
    emit previewScaleChanged(map.find("uuid").value().toString(), map.find("scale").value().toDouble());      // 缩放
    emit previewSceneChanged(map.find("uuid").value().toString(), map.find("centerPoint").value().toPoint()); // 位置
}

void BookMark::deleteBookmark(QTableWidgetItem *item)
{
    m_row = m_mangertablewidget->row(item);
    m_delete->setEnabled(true);

    itemsRow.clear(); // 刷新保存的行
    auto selectItems = m_mangertablewidget->selectedItems();
    foreach (auto items, selectItems) {
        if (alreadyExisted(items->row())) {
            itemsRow.push_back(items->row());
        }
    }
}

void BookMark::deleteItemBookmark()
{
    if (!itemsAllRow.isEmpty())
        itemsRow = itemsAllRow;
    qSort(itemsRow.begin(), itemsRow.end()); // 保存的行需要排序
    for (int j = itemsRow.size() - 1; j >= 0; --j) {
        if (m_mangertablewidget->item(itemsRow[j], 0) == nullptr) {
            continue;
        }
        QString text = m_mangertablewidget->item(itemsRow[j], 0)->text();
        // 有快捷键的删除后标志数组更新
        for (int i = 0; i < 9; i++) {
            if (shortcut_falg[i] == nullptr) {
                continue;
            } else if (text == shortcut_falg[i]->text()) {
                shortcut_falg[i] = nullptr;
            }
        }
        // 如果更多存在时，每删除都要删除p_mroe中的数据，计数器更新
        foreach (auto act, p_mroe->actions()) {
            if (act->text() == text) {
                p_mroe->removeAction(act);
                for (int i = 0; i < m_count->size(); ++i) {
                    if (m_count->at(i) == act->text()) {
                        m_count->removeAt(i);
                    }
                }
            }
        }
        // 当m_more中的数据为空时，且getBookMarkMenu找到更多时
        if (p_mroe->actions().size() == 0) {
            foreach (QAction *act, dataPtr->getBookMarkMenu()->actions()) {
                if (act->text() == tr("More...")) {
                    dataPtr->getBookMarkMenu()->removeAction(act);
                    isfrist = false;
                }
            }
        }

        // 相应容器去除对应数据
        for (int i = 0; i < m_allaction->size(); ++i) {
            if (m_allaction->at(i)->text() == text) {
                dataPtr->getBookMarkMenu()->removeAction(m_allaction->at(i));
                m_allaction->removeAt(i);
            }
        }
        for (int i = 0; i < m_count->size(); ++i) {
            if (m_count->at(i) == text) {
                m_count->removeAt(i);
            }
        }
        m_mangertablewidget->removeRow(itemsRow[j]);
    }
    m_delete->setEnabled(false);
    itemsAllRow.clear();
    serialize();
}

void BookMark::changeItemBookmark(QTableWidgetItem *item)
{
    if (item == nullptr) {
        return;
    }
    if (m_mangertablewidget->column(item) == 0) {
        m_lastchangeText = item->text();
    }
    if (m_mangertablewidget->column(item) == 1) {
        m_shortcutlastchangeText = item->text();
    }
    isfristchangeditem = false;
    isdoubleclicked = true;
}

void BookMark::changeBookmark(QTableWidgetItem *item)
{
    // 命名修改
    if (m_mangertablewidget->column(item) == 0) {
        if (!isfristchangeditem && isdoubleclicked) {
            isdoubleclicked = false;
            isfristchangeditem = true;
            if (item->text().trimmed() == "" || item->text().trimmed() == tr("More...")
                || item->text() == m_lastchangeText || item->text().trimmed().size() >= 15
                || item->text().contains('&')) {
                item->setText(m_lastchangeText);
                return;
            }
            QString str = findRename(item->text()); // 查找修改时是否已有重命名
            m_count->push_back(str);
            item->setText(str);

            for (int i = 0; i < m_count->size(); ++i) { // 修改的同时删除计数的
                if (m_count->at(i) == m_lastchangeText) {
                    m_count->removeAt(i);
                }
            }

            foreach (auto act, p_mroe->actions()) {
                if (act->text() == m_lastchangeText) {
                    act->setText(str);
                    // 实时修改时间与命名
                    QString currentTime = getLocalTime();
                    QVariantMap map = act->data().toMap();
                    map.find("time").value().setValue(currentTime);
                    map.find("bookmarkname").value().setValue(str);
                    act->setData(map);
                }
            }

            for (int i = 0; i < m_allaction->size(); ++i) {
                if (m_allaction->at(i)->text() == m_lastchangeText) {
                    // 实时修改时间与命名
                    QString currentTime = getLocalTime();
                    QVariantMap map = m_allaction->at(i)->data().toMap();
                    map.find("time").value().setValue(currentTime);
                    map.find("bookmarkname").value().setValue(str);
                    m_allaction->at(i)->setData(map);
                    if (m_mangertablewidget->item(m_row, 4) != nullptr) {
                        m_mangertablewidget->item(m_row, 4)->setText(currentTime);
                    }
                    m_allaction->at(i)->setText(str);
                }
            }
            m_lastchangeText = ""; // 记录字符串置空
        }
    }

    // 快捷键修改
    if (m_mangertablewidget->column(item) == 1) {
        if (!isfristchangeditem && isdoubleclicked) {
            isdoubleclicked = false;
            isfristchangeditem = true;
            QString newshortcut = QString("Ctrl+%1").arg(item->text());
            QString last_none = m_shortcutlastchangeText;
            // 符合快捷键的字符才做修改
            if (!newshortcut.contains(QRegExp("^(Ctrl\\+[1-9])$")) || newshortcut == m_shortcutlastchangeText) {
                item->setText(m_shortcutlastchangeText);
                return;
            }
            auto last_char = m_shortcutlastchangeText.at(m_shortcutlastchangeText.size() - 1);
            item->setText(newshortcut);
            int last_index_shortcut = ((QString)last_char).toInt() - 1;
            int index_shortcut = ((QString)newshortcut.at(newshortcut.size() - 1)).toInt() - 1;
            QKeySequence keyshortcut;
            if (last_none != "None") { // 如果不是原修改不是更多
                // 如果新的快捷键位置没有被占用
                if (shortcut_falg[index_shortcut] == nullptr) {
                    shortcut_falg[index_shortcut] = shortcut_falg[last_index_shortcut]; // 原action赋值给新action
                    shortcut_falg[last_index_shortcut] = nullptr;                       // 原action置空
                    shortcut_falg[index_shortcut]->setShortcut(keyshortcut.fromString(newshortcut)); // 设置新快捷键
                                                                                                     // 快捷键保存
                    QVariantMap map = shortcut_falg[index_shortcut]->data().toMap();
                    QString currentTime = getLocalTime();
                    map.find("time").value().setValue(currentTime);
                    map.find("shortcut").value().setValue(newshortcut);
                    shortcut_falg[index_shortcut]->setData(map);
                    m_mangertablewidget->item(m_row, 4)->setText(currentTime);
                } else { // 被占用了新快捷键位置的原有快捷键就要添加到更多里面
                    for (int i = 0; i < m_allaction->size(); ++i) {
                        if (shortcut_falg[index_shortcut] == m_allaction->at(i)) {
                            m_allaction->removeAt(i);
                        }
                    }
                    for (int i = 0; i < dataPtr->getBookMarkMenu()->actions().size(); ++i) {
                        if (i <= 2) {
                            continue;
                        }
                        if (shortcut_falg[index_shortcut] == dataPtr->getBookMarkMenu()->actions().at(i)) {
                            QVariantMap map = shortcut_falg[index_shortcut]->data().toMap();
                            QString currentTime = getLocalTime();
                            map.find("time").value().setValue(currentTime);
                            map.find("shortcut").value().setValue(QString(""));
                            map.find("ismore").value().setValue(QString("true"));
                            shortcut_falg[index_shortcut]->setShortcut(QKeySequence(QString("")));
                            shortcut_falg[index_shortcut]->setData(map);
                            p_mroe->addAction(shortcut_falg[index_shortcut]);
                            m_mangertablewidget->item(i - 3, 4)->setText(currentTime);
                            m_mangertablewidget->item(i - 3, 1)->setText("None");
                        }
                    }
                    shortcut_falg[index_shortcut] = shortcut_falg[last_index_shortcut]; // 原action赋值给新action
                    shortcut_falg[last_index_shortcut] = nullptr;                       // 原action置空
                    shortcut_falg[index_shortcut]->setShortcut(keyshortcut.fromString(newshortcut)); // 设置新快捷键
                    QVariantMap map = shortcut_falg[index_shortcut]->data().toMap();
                    QString currentTime = getLocalTime();
                    map.find("time").value().setValue(currentTime);
                    map.find("shortcut").value().setValue(newshortcut);
                    m_mangertablewidget->item(m_row, 4)->setText(currentTime);
                    shortcut_falg[index_shortcut]->setData(map);
                }
            } else {
                if (shortcut_falg[index_shortcut] == nullptr) {
                    foreach (auto action, p_mroe->actions()) {
                        if (m_mangertablewidget->item(m_row, 0)->text() == action->text()) {
                            shortcut_falg[index_shortcut] = action;
                            QVariantMap map = action->data().toMap();
                            QString currentTime = getLocalTime();
                            map.find("time").value().setValue(currentTime);
                            map.find("shortcut").value().setValue(QString(newshortcut));
                            map.find("ismore").value().setValue(QString("false"));
                            shortcut_falg[index_shortcut]->setShortcut(keyshortcut.fromString(newshortcut));
                            m_mangertablewidget->item(m_row, 4)->setText(currentTime);
                            shortcut_falg[index_shortcut]->setData(map);
                            m_allaction->append(action);
                            p_mroe->removeAction(action);
                        }
                    }
                } else {
                    for (int i = 0; i < m_allaction->size(); ++i) {
                        if (shortcut_falg[index_shortcut] == m_allaction->at(i)) {
                            m_allaction->removeAt(i);
                        }
                    }
                    for (int i = 0; i < dataPtr->getBookMarkMenu()->actions().size(); ++i) {
                        if (i <= 2) {
                            continue;
                        }
                        if (shortcut_falg[index_shortcut] == dataPtr->getBookMarkMenu()->actions().at(i)) {
                            QVariantMap map = shortcut_falg[index_shortcut]->data().toMap();
                            QString currentTime = getLocalTime();
                            map.find("time").value().setValue(currentTime);
                            map.find("shortcut").value().setValue(QString(""));
                            map.find("ismore").value().setValue(QString("true"));
                            shortcut_falg[index_shortcut]->setShortcut(QKeySequence(QString("")));
                            shortcut_falg[index_shortcut]->setData(map);
                            p_mroe->addAction(shortcut_falg[index_shortcut]);
                            m_mangertablewidget->item(i - 3, 4)->setText(currentTime);
                            m_mangertablewidget->item(i - 3, 1)->setText("None");
                        }
                    }
                    foreach (auto action, p_mroe->actions()) {
                        if (m_mangertablewidget->item(m_row, 0)->text() == action->text()) {
                            shortcut_falg[index_shortcut] = action;
                            QVariantMap map = action->data().toMap();
                            QString currentTime = getLocalTime();
                            map.find("time").value().setValue(currentTime);
                            map.find("shortcut").value().setValue(QString(newshortcut));
                            map.find("ismore").value().setValue(QString("false"));
                            shortcut_falg[index_shortcut]->setShortcut(keyshortcut.fromString(newshortcut));
                            m_mangertablewidget->item(m_row, 4)->setText(currentTime);
                            shortcut_falg[index_shortcut]->setData(map);
                            m_allaction->append(action);
                            p_mroe->removeAction(action);
                        }
                    }
                }
            }
            realignment();
            updateListWidget();
        }
        m_shortcutlastchangeText = "";
    }
    serialize();
}

void BookMark::undeletePtn(int value)
{
    m_delete->setEnabled(false);
}

QMenu *BookMarkPrivate::getBookMarkMenu()
{
    return BookMarkList;
}

BookMarkPrivate::BookMarkPrivate() : BookMarkList(new QMenu(nullptr))
{
    BookMarkList->setWindowFlag(Qt::NoDropShadowWindowHint);
    BookMarkList->addAction(ActionManager::getInstance().getAction(ActionManager::AddBookmark));
    BookMarkList->addAction(ActionManager::getInstance().getAction(ActionManager::ManageBookmark));
    BookMarkList->addSeparator();
}

QMenu *BookMark::getBookMarkMenu()
{
    if (this == nullptr) {
        return NULL;
    }
    return dataPtr->BookMarkList;
}

void BookMark::deleteBookMarkAction(QString uid)
{
    QVariantMap map;
    QString m_uid;
    // 删除快捷键
    for (int i = 0; i < dataPtr->getBookMarkMenu()->actions().size(); ++i) {
        map = dataPtr->getBookMarkMenu()->actions().at(i)->data().toMap();
        if (map.contains("uuid")) {
            m_uid = map.find("uuid").value().toString();
            if (m_uid == uid) {
                shortcut_falg[i - 3] = nullptr;
            }
        }
    }
    // 删除更多里面数据
    foreach (auto action, p_mroe->actions()) {
        map = action->data().toMap();
        if (!map.isEmpty()) {
            m_uid = map.find("uuid").value().toString();
            if (m_uid == uid) {
                p_mroe->removeAction(action);
                for (int i = 0; i < m_count->size(); ++i) {
                    if (m_count->at(i) == action->text()) {
                        m_count->removeAt(i);
                    }
                }
            }
        }
    }
    // 当p_more中的数据为空时，且getBookMarkMenu找到更多时
    if (p_mroe->actions().size() == 0) {
        foreach (QAction *act, dataPtr->getBookMarkMenu()->actions()) {
            if (act->text() == tr("More...")) {
                dataPtr->getBookMarkMenu()->removeAction(act);
                isfrist = false;
            }
        }
    }
    // 删除action
    foreach (QAction *act, dataPtr->getBookMarkMenu()->actions()) {
        map = act->data().toMap();
        if (!map.isEmpty()) {
            m_uid = map.find("uuid").value().toString();
            if (m_uid == uid) {
                for (int i = 0; i < m_allaction->size(); ++i) {
                    if (m_allaction->at(i)->text() == act->text()) {
                        dataPtr->getBookMarkMenu()->removeAction(m_allaction->at(i));
                        m_allaction->removeAt(i);
                    }
                }
                for (int i = 0; i < m_count->size(); ++i) {
                    if (m_count->at(i) == act->text()) {
                        m_count->removeAt(i);
                    }
                }
            }
        }
    }
    serialize();
}

QVariantMap BookMark::savedata(QString ls, QString rs)
{
    QVariantMap temp; // 书签保存数据
    // 获取当前时间
    QString currentTime = getLocalTime();
    // 保存画板数据
    temp.insert("bookmarkname", rs);               // 书签名
    temp.insert("uuid", data->uuid());             // uuid
    temp.insert("scale", data->scale());           // 缩放
    temp.insert("centerPoint", data->centerPos()); // 位置
    temp.insert("name", data->name());             // 画板名
    temp.insert("time", currentTime);              // 修改时间
    temp.insert("type", data->type());             // 画板类型
    temp.insert("ismore", ismore);                 // 是否是更多...里面的数据
    temp.insert("shortcut", ls);                   // 快捷键
    return temp;
}

void BookMark::realignment()
{
    dataPtr->getBookMarkMenu()->clear();
    dataPtr->getBookMarkMenu()->addAction(ActionManager::getInstance().getAction(ActionManager::AddBookmark));
    dataPtr->getBookMarkMenu()->addAction(ActionManager::getInstance().getAction(ActionManager::ManageBookmark));
    dataPtr->getBookMarkMenu()->addSeparator();
    for (int i = 0; i < 9; ++i) {
        if (shortcut_falg[i] != nullptr) { // 有快捷键的action插入
            dataPtr->getBookMarkMenu()->addAction(shortcut_falg[i]);
        }
    }
    if (p_mroe->actions().size() > 0) {
        dataPtr->getBookMarkMenu()->addMenu(p_mroe);
    }
}

void BookMarkTableWidget::mousePressEvent(QMouseEvent *e)
{
    QPoint pos = e->pos();
    QTableWidgetItem *item = itemAt(pos);
    if (item == nullptr) {
        m_bookMark->m_delete->setEnabled(false);
        this->clearFocus();
        this->clearSelection();
    }
    m_bookMark->isfristchangeditem = false;
    QTableWidget::mousePressEvent(e);
}

void BookMarkTableWidget::keyPressEvent(QKeyEvent *event)
{
    // 全选
    if (event->key() == Qt::Key_A && event->modifiers() == Qt::ControlModifier) {
        m_bookMark->m_delete->setEnabled(true);
        m_bookMark->itemsAllRow.clear();
        for (int i = 0; i < m_bookMark->m_mangertablewidget->rowCount(); ++i) {
            m_bookMark->m_mangertablewidget->selectRow(i);
            m_bookMark->itemsAllRow.push_back(i);
        }
    }

    // 删除
    if (event->matches(QKeySequence::Delete)) {
        m_bookMark->deleteItemBookmark();
    }
}

void BookMarkTableWidget::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList selectedRanges = selected.indexes();
    QVector<int> rows;
    // 遍历所有选中区域
    for (auto &range : selectedRanges) {
        // 获取区域行号
        int topRow = range.row();
        // 添加所有行号到列表中
        if (!rows.contains(topRow)) {
            rows.append(topRow);
        }
    }
    if (rows.count() == 0) {
        m_bookMark->setButtonDelete(false);
    } else {
        m_bookMark->setDeleteRow(rows);
        m_bookMark->setButtonDelete(true);
    }
    update();
}

void BookMark::serialize()
{
    QVariantList listdata;
    // 有快捷键的action
    int count = 0;
    foreach (QAction *act, dataPtr->getBookMarkMenu()->actions()) {
        if (count <= 2) {
            count++;
            continue;
        }
        QVariantMap map = act->data().toMap();
        listdata.append((QVariant)map);
    }
    // 更多...的action
    foreach (auto action, p_mroe->actions()) {
        QVariantMap map = action->data().toMap();
        listdata.append((QVariant)map);
    }
    QVariantMap map;
    map.insert("bookMark", listdata);
    emit bookMarkSerialize(map);
}

void BookMark::deserialize(QVariantMap data)
{
    // 先刷新数据
    for (int i = 0; i < 9; ++i) {
        shortcut_falg[i] = nullptr;
    }
    dataPtr->getBookMarkMenu()->clear();
    p_mroe->clear();
    m_count->clear();
    m_allaction->clear();
    m_mangertablewidget->clearContents();
    dataPtr->getBookMarkMenu()->addAction(ActionManager::getInstance().getAction(ActionManager::AddBookmark));
    dataPtr->getBookMarkMenu()->addAction(ActionManager::getInstance().getAction(ActionManager::ManageBookmark));
    dataPtr->getBookMarkMenu()->addSeparator();
    if (data.isEmpty()) {
        return;
    }
    // 没找到就返回
    if (data.find("bookMark") == data.end()) {
        return;
    }
    QVariantList list = data.find("bookMark").value().toList(); // 获取保存的数据链表
    if (list.isEmpty()) {
        return;
    }
    for (int i = 0; i < list.size(); ++i) {
        QVariantMap map = list.at(i).toMap();
        if (map.isEmpty() || map.find("bookmarkname").value().toString() == "false") { // 空名时不做处理
            continue;
        }
        // 书签名
        QAction *action = new QAction(map.find("bookmarkname").value().toString(), this);
        // 重命名容器
        m_count->push_back(map.find("bookmarkname").value().toString());
        // 快捷键容器
        QKeySequence keyshortcut;
        QString shortcut = map.find("shortcut").value().toString();
        if (shortcut.size() == 6) { // 有快捷键时才做以下处理
            // 获取快捷键数字，根据快捷键数字确定排序位置
            int key_shortcut = ((QString)shortcut.at(shortcut.size() - 1)).toInt() - 1;
            action->setShortcut(keyshortcut.fromString(shortcut));
            shortcut_falg[key_shortcut] = action;
        }
        action->setData(map);
        // 有更多...时，生成二级菜单，没有就一般处理
        bool m_more = map.find("ismore").value().toBool();
        if (!m_more) {
            m_allaction->push_back(action);
            dataPtr->getBookMarkMenu()->addAction(action);
        } else {
            dataPtr->getBookMarkMenu()->addMenu(p_mroe);
            p_mroe->addAction(action);
        }
        // 反序列化的action使能
        connect(action, SIGNAL(triggered()), this, SLOT(onUseBookmark()));
    }
    // 更新管理表
    updateListWidget();
}

bool BookMark::alreadyExisted(int vule)
{
    for (int i = 0; i < itemsRow.size(); ++i) {
        if (itemsRow[i] == vule) {
            return false;
        }
    }
    return true;
}

QString BookMark::getLocalTime()
{
    QDateTime current_time = QDateTime::currentDateTime();
    QString currentTime = current_time.toString("yyyy-MM-dd hh:mm:ss");
    return currentTime;
}

void BookMark::changeBoardName(const QString oldname, const QString newname)
{
    if (oldname == newname) {
        return;
    }
    for (int i = 0; i < m_allaction->size(); ++i) {
        if (m_allaction->at(i) == nullptr) {
            continue;
        }
        QVariantMap map = m_allaction->at(i)->data().toMap();
        QString name = map.find("name").value().toString();
        if (name == oldname) {
            QString currentTime = getLocalTime();
            map.find("time").value().setValue(currentTime);
            map.find("name").value().setValue(newname);
            m_allaction->at(i)->setData(map);
        }
    }

    for (int i = 0; i < p_mroe->actions().size(); ++i) {
        QVariantMap map = p_mroe->actions().at(i)->data().toMap();
        QString name = map.find("name").value().toString();
        if (name == oldname) {
            QString currentTime = getLocalTime();
            map.find("time").value().setValue(currentTime);
            map.find("name").value().setValue(newname);
            p_mroe->actions().at(i)->setData(map);
        }
    }
    serialize();
    updateListWidget();
}

int BookMark::getBookMarkNumbee()
{
    return p_mroe->actions().size() + m_allaction->size();
}

int BookMark::getBookMarkMaxLimit()
{
    return BOOKMARK_MAXNUMBER;
}

QList<QAction *> *BookMark::getBookMarks()
{
    return m_allaction;
}

void BookMark::setDeleteRow(QVector<int> row)
{
    itemsRow = row;
}

void BookMark::setButtonDelete(bool falg)
{
    m_delete->setEnabled(falg);
}
