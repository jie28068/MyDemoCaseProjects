#include "HASH.h"
#include <openssl/evp.h>

/**
 * @brief HASH::HASH
 * @param mode HASH算法类型
 */
HASH::HASH(Mode mode)
    :_mode(mode)
{
}

/**
 * @brief HASH::addData
 * 添加待加密数据
 * @param data 数据
 */
void HASH::addData(const QByteArray &data)
{
    _data = data;
}

/**
 * @brief HASH::result
 * 获取加密结果
 * @return 加密结果
 */
QByteArray HASH::result()
{
    OpenSSL_add_all_digests();

    const EVP_MD *md = nullptr;
    md = EVP_get_digestbyname(modeToString(_mode).toStdString().c_str());
    if(!md) // 不支持的格式
    {
        return QByteArray();
    }

    unsigned char mdValue[EVP_MAX_MD_SIZE] = {0};
    unsigned int mdLen = 0;

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    EVP_MD_CTX_init(mdctx);
    EVP_DigestInit_ex(mdctx, md, nullptr);
    EVP_DigestUpdate(mdctx, _data.data(), _data.size());
    EVP_DigestFinal_ex(mdctx, mdValue, &mdLen);
    EVP_MD_CTX_free(mdctx);

    QByteArray out((const char *)mdValue, mdLen);
    return out;
}

/**
 * @brief HASH::modeToString
 * Mode型枚举值转字符串
 * @param value 枚举值
 * @return 字符串
 */
QString HASH::modeToString(HASH::Mode value)
{
    QMetaEnum metaEnum = QMetaEnum::fromType<Mode>();
    const char* key = metaEnum.valueToKey(value);
    Q_ASSERT(key != nullptr);
    return QString(key);
}
