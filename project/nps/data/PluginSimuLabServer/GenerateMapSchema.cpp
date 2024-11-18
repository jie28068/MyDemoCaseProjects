#include "GenerateMapSchema.h"
#include "CoreLib/ServerManager.h"

USE_LOGOUT_("GenerateMapSchema")

bool GenerateMapSchema::generateMap(SimuLabSchema &mapSchema, const QList<PDrawingBoardClass> &drawingBoardList,
                                    QString &errMsg)
{
    if (drawingBoardList.size() <= 0) {
        errMsg = "画板数据不存在";
        return false;
    }

    PDrawingBoardClass mainDrawboard = drawingBoardList[0];
    if (!checkStandard(mainDrawboard)) {
        errMsg = "控制画板模型命名没有标准化";
        return false;
    }

    QList<PModel> blockList;
    QList<QString> pathList;
    QString boardName = mainDrawboard->getName();
    std::map<std::string, PModel> blockMap;
    translateMap(mainDrawboard->getChildModels(), blockMap);
    std::map<std::string, PModel>::iterator iter = blockMap.begin();
    while (iter != blockMap.end()) {
        PModel block = iter->second;
        iter++;
        QString blockName = block->getName();
        if (!generateOnceModel(blockName, boardName, drawingBoardList, mapSchema, errMsg)) {
            LOGOUT(errMsg, LOG_ERROR);
            return false;
        }
    }
    return true;
}

bool GenerateMapSchema::generateOnceModel(const QString &modelName, const QString &boardName,
                                          const QList<PDrawingBoardClass> &drawingBoardList, SimuLabSchema &mapSchema,
                                          QString &errMsg)
{
    QString splitModelName = QString("%1_%2").arg(boardName).arg(modelName);
    // 获取拆后的模型，没有则报错
    PModel splitBlock = getSplitModelBlock(splitModelName, drawingBoardList);
    if (splitBlock == nullptr) {
        errMsg = QString("拆后模型[%1]不存在").arg(splitModelName);
        return false;
    }

    QList<PModel> blockList;
    QList<QString> pathList;
    QString tmpPath = QString("%1_%2/%1").arg(modelName).arg(boardName).toLower();
    getMinBlockList(splitBlock, blockList, pathList, tmpPath);
    if (blockList.size() <= 0) {
        errMsg = QString("子系统[%1]内部没有模块!").arg(modelName);
        return false;
    }

    PDrawingBoardClass childBoardModel = splitBlock.dynamicCast<DrawingBoardClass>();
    addSimulabMapData(mapSchema, blockList, pathList, childBoardModel);
    return true;
}

void GenerateMapSchema::getMinBlockList(const PModel &block, QList<PModel> &blockList, QList<QString> &pathList,
                                        QString &tmpPath)
{
    if (block != nullptr) {
        if (block->getChildModels().size() <= 0) {
            QString blockName = block->getName();
            blockList << block;
            pathList << tmpPath;
            return;
        }
    }

    QMap<QString, PModel> tempMap = block->getChildModels();
    std::map<std::string, PModel> blockMap;
    translateMap(tempMap, blockMap);
    std::map<std::string, PModel>::iterator iter = blockMap.begin();
    while (iter != blockMap.end()) {
        PModel unit = iter->second;
        iter++;
        QString lastPath = QString("%1/%2").arg(tmpPath).arg(unit->getName());
        getMinBlockList(unit, blockList, pathList, lastPath);
    }
}

void GenerateMapSchema::addSimulabMapData(SimuLabSchema &mapSchema, QList<PModel> &blockList, QList<QString> &pathList,
                                          const PDrawingBoardClass &drawingBoard)
{
    if (pathList.size() == 0)
        return;
    size_t index = 0;
    size_t signCount = 0;
    SimuLabSchema::MapSchema tmpMap;
    tmpMap.mapFileName = pathList[0].split("/")[0];
    tmpMap.drawingBoard = drawingBoard;
    tmpMap.minBlockList = blockList;
    foreach (PModel baseBlock, blockList) {
        if (isVirtualBlock(baseBlock)) {
            index++;
            continue;
        }

        // 参数
        addMapSchemaParam(tmpMap, baseBlock, pathList, index);

        // 信号
        addMapSchemaSignal(tmpMap, baseBlock, pathList, index, signCount);
        index++;
    }
    mapSchema.mapList << tmpMap;
}

void GenerateMapSchema::addMapSchemaParam(SimuLabSchema::MapSchema &tmpMap, const PModel &block,
                                          const QList<QString> &pathList, size_t index)
{
    PVariableGroup paramGroup = block->getVariableGroup(RoleDataDefinition::Parameter);
    if (paramGroup != nullptr) {
        paramGroup->setAutoSortVariable(true);
        QList<PVariable> paramList = paramGroup->getVariableSortByOrder();
        for (auto param : paramList) {
            if (param->getDataType() != "double") {
                continue;
            }
            SimuLabSchema::ParameterUnit paramUnit;
            paramUnit.blkName = block->getPrototypeName() == SL_CONST::ProtoName_SimuLabInput
                    ? QString("%1/%2/SL_Inport").arg(pathList[index]).arg(SL_CONST::ProtoName_SimuLabInput)
                    : pathList[index];
            paramUnit.paramName = param->getName();
            paramUnit.varName = "";
            paramUnit.typeStr = param->getDataType() == "doubleVector" ? "RVector" : "Scalar";
            paramUnit.valueType = getMapParamType(param->getDataType());
            paramUnit.valuesStr = param->getDefaultValue().toString();
            paramUnit.nRows = 1;
            paramUnit.nCols = param->getDataType() == "doubleVector" ? getDoubleVectorSize(paramUnit.valuesStr) : 1;
            paramUnit.blockType = block->getPrototypeName() == SL_CONST::ProtoName_SimuLabInput
                    ? SL_CONST::ProtoName_Constant
                    : block->getPrototypeName();
            tmpMap.parameterList << paramUnit;
        }
    }
}

void GenerateMapSchema::addMapSchemaSignal(SimuLabSchema::MapSchema &tmpMap, const PModel &block,
                                           const QList<QString> &pathList, size_t index, size_t &signCount)
{
    PVariableGroup outputGroup = block->getVariableGroup(RoleDataDefinition::OutputSignal);
    if (outputGroup != nullptr) {
        outputGroup->setAutoSortVariable(true);
        QList<PVariable> outputList = outputGroup->getVariableSortByOrder();
        for (auto output : outputList) {
            if (output->getDataType() != "double") {
                continue;
            }
            SimuLabSchema::SignalUnit signalUnit;
            QString valuesStr = output->getDefaultValue().toString();
            signalUnit.blkName = block->getPrototypeName() == SL_CONST::ProtoName_SimuLabOutput
                    ? QString("%1/%2/SL_Outport").arg(pathList[index]).arg(SL_CONST::ProtoName_SimuLabOutput)
                    : pathList[index];
            signalUnit.sigName = output->getName();
            signalUnit.boIdx = signCount;
            signalUnit.row = 1;
            signalUnit.col = output->getDataType() == "doubleVector" ? getDoubleVectorSize(valuesStr) : 1;
            signalUnit.sigWidth = signalUnit.row * signalUnit.col;
            signalUnit.typeStr = output->getDataType() == "doubleVector" ? "V" : "S";
            signalUnit.blockType = (block->getPrototypeName() == SL_CONST::ProtoName_SimuLabOutput
                                    || block->getPrototypeName() == SL_CONST::ProtoName_SimuLabInput)
                    ? "DataTypeConversion"
                    : block->getPrototypeName();
            tmpMap.signalList << signalUnit;
            signCount++;
        }
    }
}

int GenerateMapSchema::getMapParamType(const QString &paramType)
{
    int valueType = 0;
    if (paramType == "int") {

    } else if (paramType == "bool") {
        valueType = 8;
    } else if (paramType == "double") {
        valueType = 0;
    } else if (paramType == "KccString") {

    } else if (paramType == "doubleVector") {
    }
    return valueType;
}

void GenerateMapSchema::translateMap(const QMap<QString, PModel> &blockMap, std::map<std::string, PModel> &newMap)
{
    QMapIterator<QString, PModel> iter(blockMap);
    while (iter.hasNext()) {
        iter.next();
        PModel block = iter.value();
        QString key = block->getName();
        newMap[key.toStdString()] = block;
    }
}

int GenerateMapSchema::getDoubleVectorSize(const QString &doubleVector)
{
    QStringList vector = doubleVector.split(",");
    return vector.size();
}

bool GenerateMapSchema::isVirtualBlock(const PModel &block)
{
    if (block->getPrototypeName() == SL_CONST::ProtoName_In || block->getPrototypeName() == SL_CONST::ProtoName_Out
        || block->getPrototypeName() == SL_CONST::ProtoName_From
        || block->getPrototypeName() == SL_CONST::ProtoName_Goto || block->getPrototypeName() == SL_CONST::ProtoName_BC
        || block->getPrototypeName() == SL_CONST::ProtoName_BS || block->getPrototypeName() == SL_CONST::ProtoName_Null
        || block->getPrototypeName() == SL_CONST::ProtoName_Mux
        || block->getPrototypeName() == SL_CONST::ProtoName_DeMux) {
        return true;
    }
    return false;
}

bool GenerateMapSchema::checkStandard(const PDrawingBoardClass &board)
{
    size_t smCount = 0;
    for (auto block : board->getChildModels()) {
        QString blockName = block->getName();
        // 顶层画板模块必须都是子系统
        if (block->getChildModels().size() <= 0) {
            // LOGOUT(QString("控制画板上[%1]不是子系统!").arg(blockName), LOG_ERROR);
            return false;
        }

        // 名称规则判断
        QString bfFlag = blockName.split("_")[0].toLower();
        if (bfFlag != SL_CONST::NormalSMName && bfFlag != SL_CONST::NormalSSName) {
            // LOGOUT(QString("控制画板上[%1]模型名称必须是SM_/SS_开始!").arg(blockName), LOG_ERROR);
            return false;
        }

        // 有且仅能有一个SM
        if (bfFlag == SL_CONST::NormalSMName) {
            smCount++;
            if (smCount > 1) {
                // LOGOUT("控制画板上SM模型有且只能存在一个!", LOG_ERROR);
                return false;
            }
        }
    }

    if (smCount <= 0) {
        // LOGOUT("控制画板上不存在SM模型!", LOG_ERROR);
        return false;
    }
    return true;
}

PModel GenerateMapSchema::getSplitModelBlock(const QString &blockName,
                                             const QList<PDrawingBoardClass> &drawingBoardList)
{
    for (auto board : drawingBoardList) {
        if (board->getName() == blockName) {
            return board;
        }
    }
    return nullptr;
}