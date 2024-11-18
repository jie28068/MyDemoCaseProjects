#ifndef DDXTARGETADPTERFORBLOCK_H
#define DDXTARGETADPTERFORBLOCK_H
#include "server/DataDictionary/IDataDictionaryDDXServer.h"

/*
        ddx适配器：
        建立 DDX 服务和 SimuNPS 数据输入 之间的 关联通道
*/
using namespace Kcc::DataDictionary;

class DDXTargetAdpterForBlock : public IDDXTargetAdpter
{
public:
    DDXTargetAdpterForBlock(const PIDataDictionaryDDXServer &pddxServer);
    ~DDXTargetAdpterForBlock(void);

    // IDDXTargetAdpter
    // 当DDX服务即将开始使用这个终端适配器时，会先行调用ReadyToRun，以便完成适配器的初始化工作。
    virtual bool ReadyToRun();
    // 当DDX服务即将停止使用这个终端适配器时，会先行调用ReadyToStop，以便完成适配器的反初始化工作。
    virtual bool ReadyToStop();
    // 当DDX服务开始关注/监视字典元素时，会先行调用BeforeAddInMonitor，以便完成关注/监视字典元素前的一些特殊的“初始化”、“定制化”工作。
    virtual bool BeforeAddInMonitor(PIElementBase element);
    // 当DDX服务开始不再关注/监视字典元素时，会先行调用AfterRemoveFromMonitor，以便完成不再关注/监视字典元素后的一些特殊的“反初始化”、“定制化”工作。
    virtual bool AfterRemoveFromMonitor(PIElementBase element);
    // 适配器的类型
    virtual QString AdpterType() const;
    /// 根据通道配置生成通道ID信息，这个信息会在终端有数据输入，或者需要输出到终端是会带上它
    virtual void *CreateChannelID(const DDXChannelInfoStruct &configs);
    // 发送通道数据
    virtual bool OutputChannelData(const void *channelID, unsigned char *data, unsigned int len, DataInfo &dinfo);

    virtual bool ChannelIsLocal(const DDXChannelInfoStruct &configs) { return true; }

private:
    PIDataDictionaryDDXServer _ddxServer;
};

#endif
