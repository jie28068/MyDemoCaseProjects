#include "TripleDES.h"
#include "Padding.h"

/**
 * @brief TripleDES::ecb_encrypt
 * ECB模式加解密，支持对任意长度明文进行加解密。
 * @param in 输入数据
 * @param out 输出结果
 * @param key 密钥，长度必须是24字节，否则加密失败
 * @param enc true-加密，false-解密
 */
void TripleDES::ecb_encrypt(const QByteArray &in, QByteArray &out, const QByteArray &key, bool enc)
{
    // 检查密钥合法性(只能是24字节)
    Q_ASSERT(key.size() == 24);

    // 设置key
    DES_key_schedule sch1, sch2, sch3;
    setKey(key, sch1, sch2, sch3);

    // 进行加解密
    if (enc)
    {
        // 进行PKCS7Padding填充
        QByteArray inTemp = Padding::PKCS7Padding(in, DES_BLOCK_SIZE);

        // 执行加密
        out.resize(inTemp.size()); // 调整输出buf大小
        for (int i = 0; i < inTemp.size() / DES_BLOCK_SIZE; i++)
        {
            DES_ecb3_encrypt((const_DES_cblock*)(inTemp.constData() + i * DES_BLOCK_SIZE),
                             (DES_cblock *)(out.data() + i * DES_BLOCK_SIZE),
                             &sch1, &sch2, &sch3, DES_ENCRYPT);
        }
    }
    else
    {
        // 执行解密
        out.resize(in.size()); // 调整输出buf大小
        for (int i = 0; i < in.size() / DES_BLOCK_SIZE; i++)
        {
            DES_ecb3_encrypt((const_DES_cblock*)(in.constData() + i * DES_BLOCK_SIZE),
                             (DES_cblock *)(out.data() + i * DES_BLOCK_SIZE),
                             &sch1, &sch2, &sch3, DES_DECRYPT);
        }

        // 解除PKCS7Padding填充
        out = Padding::PKCS7UnPadding(out);
    }
}

/**
 * @brief TripleDES::cbc_encrypt
 * CBC模式加解密，支持对任意长度明文进行加解密。
 * @param in 输入数据
 * @param out 输出结果
 * @param key 密钥，长度必须是24字节，否则加密失败
 * @param ivec 初始向量，长度必须是8字节
 * @param enc true-加密，false-解密
 */
void TripleDES::cbc_encrypt(const QByteArray &in, QByteArray &out, const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是24字节)
    Q_ASSERT(key.size() == 24);
    Q_ASSERT(ivec.size() == 8); // 初始向量为8字节

    // 设置key
    DES_key_schedule sch1, sch2, sch3;
    setKey(key, sch1, sch2, sch3);

    // 进行加解密
    QByteArray ivecTemp = ivec; // 此参数被自动修改，故使用局部变量传递
    if (enc)
    {
        // 进行PKCS7Padding填充
        QByteArray inTemp = Padding::PKCS7Padding(in, DES_BLOCK_SIZE);

        // 执行加密
        out.resize(inTemp.size()); // 调整输出buf大小
        DES_ede3_cbc_encrypt((const unsigned char *)inTemp.constData(),
                             (unsigned char *)out.data(),
                             inTemp.size(), &sch1, &sch2, &sch3,
                             (DES_cblock *)ivecTemp.data(), DES_ENCRYPT);
    }
    else
    {
        // 执行解密
        out.resize(in.size()); // 调整输出buf大小
        DES_ede3_cbc_encrypt((const unsigned char *)in.constData(),
                             (unsigned char *)out.data(),
                             in.size(), &sch1, &sch2, &sch3,
                             (DES_cblock *)ivecTemp.data(), DES_DECRYPT);

        // 解除PKCS7Padding填充
        out = Padding::PKCS7UnPadding(out);
    }
}

/**
 * @brief TripleDES::cfb1_encrypt
 * CFB1模式加解密，支持对任意长度明文进行加解密。
 * @param in 输入数据
 * @param out 输出结果
 * @param key 密钥，长度必须是24字节，否则加密失败
 * @param ivec 初始向量，长度必须是8字节
 * @param enc true-加密，false-解密
 */
void TripleDES::cfb1_encrypt(const QByteArray &in, QByteArray &out, const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是24字节)
    Q_ASSERT(key.size() == 24);
    Q_ASSERT(ivec.size() == 8); // 初始向量为8字节

    // 设置key
    DES_key_schedule sch1, sch2, sch3;
    setKey(key, sch1, sch2, sch3);

    // 进行加解密
    QByteArray ivecTemp = ivec; // 此参数被自动修改，故使用局部变量传递
    int encVal = enc ? DES_ENCRYPT : DES_DECRYPT;
    out.resize(in.size()); // 调整输出buf大小
    DES_ede3_cfb_encrypt((const unsigned char *)in.constData(),
                         (unsigned char *)out.data(),
                         8, in.size(), &sch1, &sch2, &sch3,
                         (DES_cblock *)ivecTemp.data(), encVal);
}

/**
 * @brief TripleDES::cfb64_encrypt
 * CFB64模式加解密，支持对任意长度明文进行加解密。
 * @param in 输入数据
 * @param out 输出结果
 * @param key 密钥，长度必须是24字节，否则加密失败
 * @param ivec 初始向量，长度必须是8字节
 * @param enc true-加密，false-解密
 */
void TripleDES::cfb64_encrypt(const QByteArray &in, QByteArray &out, const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是24字节)
    Q_ASSERT(key.size() == 24);
    Q_ASSERT(ivec.size() == 8); // 初始向量为8字节

    // 设置key
    DES_key_schedule sch1, sch2, sch3;
    setKey(key, sch1, sch2, sch3);

    // 进行加解密
    int num = 0;
    QByteArray ivecTemp = ivec; // 此参数被自动修改，故使用局部变量传递
    int encVal = enc ? DES_ENCRYPT : DES_DECRYPT;
    out.resize(in.size()); // 调整输出buf大小
    DES_ede3_cfb64_encrypt((const unsigned char *)in.constData(),
                         (unsigned char *)out.data(),
                         in.size(), &sch1, &sch2, &sch3,
                         (DES_cblock *)ivecTemp.data(), &num, encVal);
}

/**
 * @brief TripleDES::ofb64_encrypt
 * OFB64模式加解密，支持对任意长度明文进行加解密。
 * @param in 输入数据
 * @param out 输出结果
 * @param key 密钥，长度必须是24字节，否则加密失败
 * @param ivec 初始向量，长度必须是8字节
 * @param enc true-加密，false-解密
 */
void TripleDES::ofb64_encrypt(const QByteArray &in, QByteArray &out, const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是24字节)
    Q_ASSERT(key.size() == 24);
    Q_ASSERT(ivec.size() == 8); // 初始向量为8字节

    Q_UNUSED(enc)

    // 设置key
    DES_key_schedule sch1, sch2, sch3;
    setKey(key, sch1, sch2, sch3);

    // 进行加解密
    int num = 0;
    QByteArray ivecTemp = ivec; // 此参数被自动修改，故使用局部变量传递
    out.resize(in.size()); // 调整输出buf大小
    DES_ede3_ofb64_encrypt((const unsigned char *)in.constData(),
                         (unsigned char *)out.data(),
                         in.size(), &sch1, &sch2, &sch3,
                         (DES_cblock *)ivecTemp.data(), &num);
}

void TripleDES::setKey(const QByteArray &key, DES_key_schedule &sch1, DES_key_schedule &sch2, DES_key_schedule &sch3)
{
    const_DES_cblock key1, key2, key3;
    memcpy(key1, key.data(), 8);
    memcpy(key2, key.data() + 8, 8);
    memcpy(key3, key.data() + 16, 8);

    DES_set_key_unchecked(&key1, &sch1);
    DES_set_key_unchecked(&key2, &sch2);
    DES_set_key_unchecked(&key3, &sch3);
}
