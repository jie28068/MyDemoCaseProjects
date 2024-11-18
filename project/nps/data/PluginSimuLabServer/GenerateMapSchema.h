#pragma once
#include "KLModelDefinitionCore/BlockDefinition.h"
#include "KLModelDefinitionCore/VariableGroup.h"
#include "SimuLabConst.h"
using namespace Kcc::BlockDefinition;

class SimuLabSchema
{
public:
    // map参数信息
    struct ParameterUnit {
        QString blkName;
        QString paramName;
        QString varName;
        QString typeStr;
        int valueType;
        int nRows;
        int nCols;
        QString valuesStr;
        QString blockType;
    };
    // map信号信息
    struct SignalUnit {
        QString blkName;
        QString sigName;
        int boIdx;
        int sigWidth;
        int row;
        int col;
        QString typeStr;
        QString blockType;
    };
    // map结构
    struct MapSchema {
        QList<ParameterUnit> parameterList;
        QList<SignalUnit> signalList;
        QList<PModel> minBlockList;
        QString mapFileName;
        PDrawingBoardClass drawingBoard;
    };

public:
    QList<MapSchema> mapList;
};

class GenerateMapSchema
{
public:
    static GenerateMapSchema &getInstance()
    {
        static GenerateMapSchema instance;
        return instance;
    }

    /// @brief 生成map信息，顶层画板上只能有子系统且命名要是SM/SS
    /// @param mapSchema map信息
    /// @param drawingBoardList 所有画板列表
    /// @param errMsg 错误日志
    /// @return
    bool generateMap(SimuLabSchema &mapSchema, const QList<PDrawingBoardClass> &drawingBoardList, QString &errMsg);
    /// @brief 检查控制画板上模型是否标准化
    /// @param board 控制画板
    /// @return
    bool checkStandard(const PDrawingBoardClass &board);
    /// @brief 获取模型map结构
    /// @param modelName 模型名称
    /// @param boardName 画板名称
    /// @param drawingBoardList 工程画板列表
    /// @param mapSchema map结构
    /// @param errMsg 错误日志
    bool generateOnceModel(const QString &modelName, const QString &boardName,
                           const QList<PDrawingBoardClass> &drawingBoardList, SimuLabSchema &mapSchema,
                           QString &errMsg);

private:
    GenerateMapSchema() {};

    /// @brief 获取画板上所有最小模块和路径
    /// @param block 当前模型对象(子系统或模块)
    /// @param blockList 模块列表
    /// @param pathList 模块对应路径列表
    /// @param tmpPath 临时路径，递归时记录使用
    void getMinBlockList(const PModel &block, QList<PModel> &blockList, QList<QString> &pathList, QString &tmpPath);
    /// @brief 添加map结构信息
    /// @param mapSchema map结构信息
    /// @param blockList 模块列表
    /// @param pathList 路径列表
    /// @param drawingBoard 对应画板
    void addSimulabMapData(SimuLabSchema &mapSchema, QList<PModel> &blockList, QList<QString> &pathList,
                           const PDrawingBoardClass &drawingBoard);
    /// @brief 添加map的参数信息
    /// @param tmpMap map结构
    /// @param block 当前模型
    /// @param pathList 模型路径列表
    /// @param index 当前模型路径列表所在位置
    void addMapSchemaParam(SimuLabSchema::MapSchema &tmpMap, const PModel &block, const QList<QString> &pathList,
                           size_t index);
    /// @brief 添加map信号信息
    /// @param tmpMap map结构
    /// @param block 当前模型
    /// @param pathList 模型路径列表
    /// @param index 当前模型路径列表所在位置
    /// @param signCount 当前信号数量
    void addMapSchemaSignal(SimuLabSchema::MapSchema &tmpMap, const PModel &block, const QList<QString> &pathList,
                            size_t index, size_t &signCount);
    /// @brief 根据参数类型获取map中对应的枚举值
    /// @param paramType 参数类型
    /// @return
    int getMapParamType(const QString &paramType);
    /// @brief qmap转换为std::map同时用模块名作为键值(与控制引擎保持一致)
    /// @param blockMap 画板上所有模型/模块
    /// @param newMap std::map
    void translateMap(const QMap<QString, PModel> &blockMap, std::map<std::string, PModel> &newMap);
    /// @brief 获取doubleVector长度
    /// @param doubleVector doubleVector的QString值
    /// @return
    int getDoubleVectorSize(const QString &doubleVector);
    /// @brief 是否是虚模块
    /// @param block 模块
    /// @return
    bool isVirtualBlock(const PModel &block);
    /// @brief 获取拆模型后的模型对象
    /// @param blockName 拆后模型名称
    /// @param drawingBoardList 项目下所有画板列表
    /// @return
    PModel getSplitModelBlock(const QString &blockName, const QList<PDrawingBoardClass> &drawingBoardList);
};
