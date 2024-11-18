#include "DSNFuctionMng.h"
#include "CoreLib/ServerManager.h"
#include "ModelManagerConst.h"

// 1.包含LicenseServer头文件
#include <LicenseServer/ILicenseServer.h>
//  2. 定义授权检测函数
USE_CHECK_LICENSE

DSNFuctionMng &DSNFuctionMng::getInstance()
{
    static DSNFuctionMng instance;
    return instance;
}

DSNFuctionMng::~DSNFuctionMng() { }

void DSNFuctionMng::initFuctionInfo()
{
    KCC_USE_NAMESPACE
    KCC_USE_NAMESPACE_LICENSE
    PILicenseServer server = RequestServer<ILicenseServer>();
    if (!server) {
        return;
    }

    for (auto productInfo : KL_TOOLKIT::DSN_PRODUCT_INFOS) {
        // 1.创建加载器
        Kcc::KccLicense::ProductInfo info;
        info.productId = productInfo.productID;
        info.productName = productInfo.productName;
        const auto loader = server->createLicenseLoader(info);
        if (!loader) {
            continue;
        }
        // 2.检测授权
        m_mapFucInfo[productInfo.productID] = loader->isLicensed(1);
    }
}

bool DSNFuctionMng::isFucLicensed(int fucCode)
{
#ifdef COMPILER_DEVELOPER_MODE
    return true;
#endif // DEBUG

    return m_mapFucInfo.value(fucCode, false);
}

bool DSNFuctionMng::isToolKitsLicensed(const QString &toolkitName)
{
#ifdef COMPILER_DEVELOPER_MODE
    return true;
#endif // DEBUG

    int fucCode = -1;

    // 控制和电气工具箱默认是已授权的
    if (KL_TOOLKIT::CONTROL_TOOLKIT == toolkitName || KL_TOOLKIT::ELECTRICAL_TOOLKIT == toolkitName
        || KL_TOOLKIT::IEECONTROLSYSTEM_TOOLKIT == toolkitName) {
        return true;
    }

    // 扩展工具箱
    if (toolkitName == KL_TOOLKIT::GREENHYDROGEN_TOOLKIT) {
        fucCode = KL_TOOLKIT::NPS_GreenHydrogenSystemToolBox;
    } else if (toolkitName == KL_TOOLKIT::COMPREHENSIVEENERGY_TOOLKIT) {
        fucCode = KL_TOOLKIT::NPS_IntegratedEnergySystemToolBox;
    }
    // 后续增加扩展工具箱和功能码映射表

    return isFucLicensed(fucCode);
}

QString DSNFuctionMng::getDllPathByToolkitName(const QString &toolkitName)
{
    QString dllPath =
            QString("%1%2%3").arg(KL_TOOLKIT::MODEL_DIR).arg(toolkitName).arg(KL_TOOLKIT::DLL_DIR_NAME); // DLL路径

    return dllPath;
}

DSNFuctionMng::DSNFuctionMng() { }