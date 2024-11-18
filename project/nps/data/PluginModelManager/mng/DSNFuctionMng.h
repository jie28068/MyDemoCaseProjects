#ifndef DSNFUCTIONMNG_H
#define DSNFUCTIONMNG_H

#include <QMap>

// Designer授权功能管理
class DSNFuctionMng
{
public:
    static DSNFuctionMng &getInstance();
    ~DSNFuctionMng();

    void initFuctionInfo();                                      // 初始化授权功能信息
    bool isFucLicensed(int fucCode);                             //
    bool isToolKitsLicensed(const QString &toolkitName);         // 工具箱是否授权
    QString getDllPathByToolkitName(const QString &toolkitName); // 获取工具箱dll路径

private:
    DSNFuctionMng();

private:
    QMap<int, bool> m_mapFucInfo; // 授权信息
};

#endif