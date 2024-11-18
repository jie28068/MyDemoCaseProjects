#pragma once
#include "KLModelDefinitionCore/BlockDefinition.h"
#include "KLModelDefinitionCore/VariableGroup.h"
#include "KLProject/KLProject.h"
#include "ModelCopyStrategy.h"
#include "SimuLabConst.h"
#include "server/ProjectManagerServer/IProjectManagerServer.h"
using namespace Kcc::BlockDefinition;
using namespace Kcc::ProjectManager;

class SplitModelTools
{
public:
    static SplitModelTools &getInstance()
    {
        static SplitModelTools instance;
        return instance;
    }

    /// @brief 是否主动拆模型
    /// @param mainBoard 控制主画板
    /// @return
    bool isInitiativeSplitModel(const PDrawingBoardClass &mainBoard);
    /// @brief 检测拆模型规则
    /// @param mainBoard 控制主画板
    /// @return
    bool checkModelRule(const PDrawingBoardClass &mainBoard);
    /// @brief 拆模型
    /// @param curBoard 待拆模型画板
    /// @param mainBoard 控制主画板
    /// @param proj 项目对象
    /// @return
    bool genSplitModel(PDrawingBoardClass &curBoard, const PDrawingBoardClass &mainBoard, PIProjectManagerServer &proj);
    /// @brief 获取仿真步长和子系统之间通信频率
    /// @param pProject 项目对象
    /// @param borad 当前模型
    /// @return 步长/频率
    std::tuple<double, double> getSimulationInfo(const PKLProject &pProject, const PModel &borad);
    /// @brief 获取模型的序号
    /// @param mainBoard 控制主画板
    /// @param model 当前模型
    /// @return 模型在主画板中的序号
    int getModelIndex(const PDrawingBoardClass &mainBoard, const PModel &model);

private:
    struct SplitInfo {
        QList<QString> in;  // 模型里待替换的In模块列表
        QList<QString> out; // 模型里待替换的Out模块列表
        int id;             // 模型ID
        int relId;          // 关联模型ID
        QString name;       // 当前模型名称
        PModel relModel;    // 关联模型
    };

    SplitModelTools() {};

    /// @brief 通过模型端口名称获取关联连线对象
    /// @param curBoard 当前画板
    /// @param block 画板上模块
    /// @param portName 模块上端口
    /// @return
    PBlockConnector getConnectorBySrcPortName(const PDrawingBoardClass &curBoard, const PModel &block,
                                              const QString &portName);
    /// @brief 通过模块端口uuid获取端口信息
    /// @param block 当前模块
    /// @param uuid 模块上某端口UUID
    /// @return
    PVariable getPortInfoByUuid(const PModel &block, const QString &uuid);
    /// @brief 对待拆模型的In/Out按关联子系统分组
    /// @param curModel 待拆模型
    /// @param mainBoard 控制主画板
    /// @return
    QMap<QString, SplitInfo> splitModelGroup(const PDrawingBoardClass &curModel, const PDrawingBoardClass &mainBoard);
    /// @brief 创建接收模块(替换In/SL_Comm)
    /// @param curModel 当前模型
    /// @param proj 项目对象
    /// @param info 拆模型分组信息
    /// @param index 当前创建的recv模块个数
    void createRecvBlock(PDrawingBoardClass &curModel, PIProjectManagerServer &proj, const SplitInfo &info, int index);
    /// @brief 创建发送模块(替换Out模块)
    /// @param curModel 当前模型
    /// @param proj 项目对象
    /// @param info 拆模型分组信息
    /// @param index 当前创建的send模块个数
    void createSendBlock(PDrawingBoardClass &curModel, PIProjectManagerServer &proj, const SplitInfo &info, int index);
};

class CopyBoard2Combine : public ModelCopyStrategy
{
public:
    static PModel getCopy(PModel &model)
    {
        CopyBoard2Combine ts(model);
        return ts.doCopy();
    }

protected:
    using ModelCopyStrategy::ModelCopyStrategy;
    virtual PModel createModel() override
    {
        PModel newModel = ModelHelp::createModel(Model::Combine_Board_Type);
        return newModel;
    }
};