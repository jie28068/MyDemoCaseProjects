#ifndef SimuLabServer_H
#define SimuLabServer_H

#include "CoreLib/ServerBase.h"
#include "FileManager.h"
#include "KLModelDefinitionCore/BlockDefinition.h"
#include "KLProject/KLProject.h"
#include "SimuLabConst.h"
#include "server/CtrlSys/ICtrlSysCodeMgr.h"
#include "server/ProjectManagerServer/IProjectManagerServer.h"
#include "server/SimuLabServer/ISimuLabServer.h"

using namespace Kcc;
using namespace Kcc::BlockDefinition;
using namespace Kcc::SimuLab;
using namespace Kcc::CtrlSys;
using namespace Kcc::ProjectManager;

class SimuLabServer : public ISimuLabServer, public ServerBase
{
public:
    SimuLabServer();
    ~SimuLabServer();

    virtual void Init();
    virtual void UnInit();

    /// @brief 获取配置信息
    /// @return
    virtual QString getConfig();
    /// @brief 拆模型
    /// @param dirPath 项目文件[.npsproj]所在路径
    /// @param errMsg 错误信息
    /// @return
    virtual bool splitModel(const QString &dirPath, QString &errMsg);
    /// @brief 生成代码
    /// @param dirPath 项目文件[.npsproj]所在路径
    /// @param errMsg 错误信息
    /// @return
    virtual bool generateCode(const QString &dirPath, QString &errMsg);

private:
    /// @brief 加载nps项目
    /// @param projPath .npsprj路径
    /// @return
    PKLProject loadProject(const QString &projPath);
    /// @brief 解析工程文件
    /// @param dirPath 项目文件[.npsproj]所在路径
    /// @param project 项目对象
    /// @param drawingBoardList 画板列表
    /// @param isUpdate 是否强制更新，默认重新加载本地画板文件
    /// @return
    bool getProjectInfo(const QString &dirPath, PKLProject &project, QList<PDrawingBoardClass> &drawingBoardList,
                        bool isUpdate = true);
    /// @brief 创建代码生成目录
    /// @param rootPath 项目根目录
    /// @param drawingBoard 待生成代码的画板
    /// @param isRule 模型名是否规范化
    /// @return
    bool generateDir(const QString &rootPath, const PDrawingBoardClass &drawingBoard, bool isRule);
    /// @brief 将列表中的控制画板插入列表头(控制画板有且仅有一个)
    /// @param drawingBoardList 项目所有画板列表
    /// @return
    bool swapMainDrawingBoard(QList<PDrawingBoardClass> &drawingBoardList);
    /// @brief 如果画板上有导入的第三方c/c++模型，则通过这个接口拷贝模型源码到项目工作空间指定目录下。
    /// @param dstCodePath xx_Linux所在路径
    /// @param minBlockList 画板上所有最小模块列表
    /// @return
    bool splitCopySourceCode(const QString &dstCodePath, const QList<PModel> &minBlockList);
    /// @brief 生成代码(写map/subsys文件、生成控制引擎源码、拷贝固定模板代码、拷贝用户模块源码)
    /// @param workPath 项目根路径
    /// @param mapSchema map信息
    /// @param pProject nps项目反序列化对象
    /// @param errMsg 错误日志
    /// @return
    bool generateSubSysCode(const QString &workPath, const SimuLabSchema &mapSchema, const PKLProject &pProject,
                            QString &errMsg);
    /// @brief 设置子系统步长
    /// @param workPath 项目工作路径
    /// @param mainBoard 主控制画板
    /// @param project 项目对象
    void setSubModelSteps(const QString &workPath, const PDrawingBoardClass &mainBoard, const PKLProject &project);
    /// @brief 拷贝文件模块配置的文件到固定目录datafiles下
    /// @param mapUnit 当前画板解析后结构
    /// @param filesDir 拷贝的目标文件目录
    /// @param errMsg 错误日志
    /// @return
    bool copyFileBlockCnf(const SimuLabSchema::MapSchema &mapUnit, const QString &filesDir, QString &errMsg);

private:
    QMap<QString, double> subStepMap;
    PICtrlSysCodeMgr pCtrlSysCodeMgr;
    PIProjectManagerServer pPrjMgr;
};

#endif // SimuLabServer_H
