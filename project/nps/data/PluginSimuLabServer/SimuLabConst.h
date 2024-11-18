#pragma once

namespace SL_CONST {
// 常量
constexpr auto NormalSMName = "sm";                       // 默认sm模型名称
constexpr auto NormalSSName = "ss";                       // 默认ss模型名称
constexpr auto SubModelSuffix = "subsys";                 // SimuLab标识子模型的文件后缀名称
constexpr auto GenCodeDirName = "simucad";                // 固定代码管理的目录名称
constexpr auto EngineDllPath = "/usr/local/designer_lib"; // 传给控制引擎模块所在so目录路径
constexpr auto StepJsonKeyName = "Step";                  // SimuLab获取模型步长json中key的名称
constexpr auto ModelIdJsonKeyName = "ModelId";            // SimuLab获取模型IDjson中key的名称
constexpr auto SysTransferSendParam = "bsend";            // SL_SysTransfer模块参数名称
constexpr auto SysTransferSrcIdParam = "srcNodeID";       // SL_SysTransfer模块参数名称
constexpr auto SysTransferIdParam = "NodeID";             // SL_SysTransfer模块参数名称
constexpr auto SysTransferSTParam = "SampleTime";         // SL_SysTransfer模块参数名称
constexpr auto ComSTParam = "sampletime";                 // SL_Comm模块参数名称
constexpr auto ComStepParam = "step";                     // SL_Comm模块参数名称
constexpr auto FMUPATH = "fmuPath";                       // FMU模块路径
constexpr auto FromFilePATH = "fileName";                 // FromFile模块路径

// 模块原型
constexpr auto ProtoName_SimuLabOutput = "SL_Output";
constexpr auto ProtoName_SimuLabInput = "SL_Input";
constexpr auto ProtoName_SimuLabCom = "SL_Comm";
constexpr auto ProtoName_SysTransfer = "SL_SysTransfer";
constexpr auto ProtoName_Constant = "Constant";
constexpr auto ProtoName_FromFile = "FromFile";
constexpr auto ProtoName_FMU = "FMU";
// 虚拟模块
constexpr auto ProtoName_In = "In";
constexpr auto ProtoName_Out = "Out";
constexpr auto ProtoName_From = "From";
constexpr auto ProtoName_Goto = "Goto";
constexpr auto ProtoName_BC = "BusCreator";
constexpr auto ProtoName_BS = "BusSelector";
constexpr auto ProtoName_Null = "Null";
constexpr auto ProtoName_Mux = "Mux";
constexpr auto ProtoName_DeMux = "DeMux";
}
