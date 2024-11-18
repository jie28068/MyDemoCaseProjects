#ifndef MODELVIEWDEFINE_H
#define MODELVIEWDEFINE_H

#include <QColor>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QSize>
#include <QString>

namespace ModelViewDefine {
static const QString VIEW_TITLE_NAME = QObject::tr("Model Manager View");

// 配置树节点名称
static const QString NODE_THE_PRJ = QObject::tr("Project");                          // 本项目
static const QString NODE_LOCAL_CODE_MODEL = QObject::tr("Code Model");              // 代码型
static const QString NODE_CONTROL_COMB_MODEL = QObject::tr("Control Combine Model"); // 控制构造型
static const QString NODE_ELEC_COMB_MODEL = QObject::tr("Electrical Combine Model"); // 电气构造型

// 原型管理视图树配置文件（.json
static const QString JSON_TOOLKIT_TYPE = "toolkitType";       // 工具箱类型
static const QString JSON_GOURP_OBJ_NAME = "groupName";       // 分组名称
static const QString JSON_GOURP_OBJ_ORDER = "groupOrder";     // 分组顺序
static const QString JSON_GOURP_OBJ_Expand = "groupIsExpand"; // 分组是否展开
static const QString JSON_GOURP_OBJ = "GroupList";            // 分组对象

static const QString JSON_MODEL_OBJ = "ModelList";          // 原型对象j
static const QString JSON_MODEL_OBJ_NAME = "prototypeName"; // 名称
static const QString JSON_MODEL_OBJ_SHOW = "display";       // 是否显示
static const QString JSON_MODEL_OBJ_ORDER = "displayorder"; // 模块顺序

static const QString IS_DEL_ITEM = QObject::tr("Are you sure you want to delete the item?unable to restore "
                                               "after deletion!"); // 确定要删除该选项吗？删除后无法恢复！

// Model Role
enum { ImageRole = Qt::UserRole + 1, TypeRole, ModelType };

static const QSize ListItemSize = QSize(104, 74); // 列表元素大小
static const QSize TreeIconSize = QSize(35, 25);  // 树节点图标大小
static const QSize InnerBoxSize = QSize(65, 45);

// 画元件icon的一些颜色及大小
static const QColor TEXTCOLOR_NORMAL = QColor("#252d3f");  // 文字颜色
static const QColor TEXTCOLOR_DISABLE = QColor("#BFBFBF"); // 禁用文字颜色

static const QColor BGCOLOR_SELECTED = QColor("#DCE6F0");  // 选中时背景颜色
static const QColor BGCOLOR_MOUSEOVER = QColor("#EEF5FB"); // 滑过时背景颜色
static const QColor BGCOLOR_DISABLE = QColor("#F5F5F5");   // 禁用背景颜色
}

#endif // end MODELVIEWDEFINE_H