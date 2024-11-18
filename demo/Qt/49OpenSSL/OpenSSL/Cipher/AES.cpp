#include "AES.h"
#include <openssl/modes.h>
#include <openssl/aes.h>
#include "Padding.h"

/**
 * @brief AES::ecb_encrypt
 * ECB模式加解密，填充模式采用PKCS7Padding，
 * 支持对任意长度明文进行加解密。
 * @param in 输入数据
 * @param out 输出结果
 * @param key 密钥，长度必须是16/24/32字节，否则加密失败
 * @param enc true-加密，false-解密
 * @return 执行结果
 */
bool AES::ecb_encrypt(const QByteArray &in, QByteArray &out, const QByteArray &key, bool enc)
{
    // 检查密钥合法性(只能是16、24、32字节)
    Q_ASSERT(key.size() == 16 || key.size() == 24 || key.size() == 32);

    if (enc)
    {
        // 生成加密key
        AES_KEY aes_key;
        if (AES_set_encrypt_key((const unsigned char*)key.data(), key.size() * 8, &aes_key) != 0)
        {
            return false;
        }

        // 进行PKCS7Padding填充
        QByteArray inTemp = Padding::PKCS7Padding(in, AES_BLOCK_SIZE);

        // 执行ECB模式加密
        out.resize(inTemp.size()); // 调整输出buf大小
        for (int i = 0; i < inTemp.size() / AES_BLOCK_SIZE; i++)
        {
            AES_ecb_encrypt((const unsigned char*)inTemp.data() + AES_BLOCK_SIZE * i,
                            (unsigned char*)out.data() + AES_BLOCK_SIZE * i,
                            &aes_key,
                            AES_ENCRYPT);
        }
        return true;
    }
    else
    {
        // 生成解密key
        AES_KEY aes_key;
        if (AES_set_decrypt_key((const unsigned char*)key.data(), key.size() * 8, &aes_key) != 0)
        {
            return false;
        }

        // 执行ECB模式解密
        out.resize(in.size()); // 调整输出buf大小
        for (int i = 0; i < in.size() / AES_BLOCK_SIZE; i++)
        {
            AES_ecb_encrypt((const unsigned char*)in.data() + AES_BLOCK_SIZE * i,
                            (unsigned char*)out.data() + AES_BLOCK_SIZE * i,
                            &aes_key,
                            AES_DECRYPT);
        }

        // 解除PKCS7Padding填充
        out = Padding::PKCS7UnPadding(out);
        return true;
    }
}

/**
 * @brief AES::cbc_encrypt
 * CBC模式加解密，填充模式采用PKCS7Padding，
 * 支持对任意长度明文进行加解密。
 * @param in 输入数据
 * @param out 输出结果
 * @param key 密钥，长度必须是16/24/32字节，否则加密失败
 * @param ivec 初始向量，长度必须是16字节
 * @param enc true-加密，false-解密
 * @return 执行结果
 */
bool AES::cbc_encrypt(const QByteArray &in, QByteArray &out, const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是16、24、32字节)
    Q_ASSERT(key.size() == 16 || key.size() == 24 || key.size() == 32);
    Q_ASSERT(ivec.size() == 16); // 初始向量为16字节

    if (enc)
    {
        // 生成加密key
        AES_KEY aes_key;
        if (AES_set_encrypt_key((const unsigned char*)key.data(), key.size() * 8, &aes_key) != 0)
        {
            return false;
        }

        // 进行PKCS7Padding填充
        QByteArray inTemp = Padding::PKCS7Padding(in, AES_BLOCK_SIZE);

        // 执行CBC模式加密
        QByteArray ivecTemp = ivec; // ivec会被修改，故需要临时变量来暂存
        out.resize(inTemp.size()); // 调整输出buf大小
        AES_cbc_encrypt((const unsigned char*)inTemp.data(),
                        (unsigned char*)out.data(),
                        inTemp.size(),
                        &aes_key,
                        (unsigned char*)ivecTemp.data(),
                        AES_ENCRYPT);
        return true;
    }
    else
    {
        // 生成解密key
        AES_KEY aes_key;
        if (AES_set_decrypt_key((const unsigned char*)key.data(), key.size() * 8, &aes_key) != 0)
        {
            return false;
        }

        // 执行CBC模式解密
        QByteArray ivecTemp = ivec; // ivec会被修改，故需要临时变量来暂存
        out.resize(in.size()); // 调整输出buf大小
        AES_cbc_encrypt((const unsigned char*)in.data(),
                        (unsigned char*)out.data(),
                        in.size(),
                        &aes_key,
                        (unsigned char*)ivecTemp.data(),
                        AES_DECRYPT);

        // 解除PKCS7Padding填充
        out = Padding::PKCS7UnPadding(out);
        return true;
    }
}

/**
 * @brief AES::cfb1_encrypt
 * CFB1模式加解密，支持对任意长度明文进行加解密。
 * @param in 输入数据
 * @param out 输出结果
 * @param key 密钥，长度必须是16/24/32字节，否则加密失败
 * @param ivec 初始向量，长度必须是16字节
 * @param enc true-加密，false-解密
 * @return 执行结果
 */
bool AES::cfb1_encrypt(const QByteArray &in, QByteArray &out, const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是16、24、32字节)
    Q_ASSERT(key.size() == 16 || key.size() == 24 || key.size() == 32);
    Q_ASSERT(ivec.size() == 16); // 初始向量为16字节

    // 特别注意：CFB模式加密和解密均使用加密key。
    // 生成加密key
    AES_KEY aes_key;
    if (AES_set_encrypt_key((const unsigned char*)key.data(), key.size() * 8, &aes_key) != 0)
    {
        return false;
    }

    // 执行CFB1模式加密或解密
    int num = 0;
    QByteArray ivecTemp = ivec; // ivec会被修改，故需要临时变量来暂存
    int encVal = enc ? AES_ENCRYPT : AES_DECRYPT;
    out.resize(in.size()); // 调整输出buf大小
    AES_cfb1_encrypt((const unsigned char*)in.data(),
                    (unsigned char*)out.data(),
                    in.size() * 8,
                    &aes_key,
                    (unsigned char*)ivecTemp.data(),
                    &num,
                    encVal);
    return true;
}

/**
 * @brief AES::cfb8_encrypt
 * CFB8模式加解密，支持对任意长度明文进行加解密。
 * @param in 输入数据
 * @param out 输出结果
 * @param key 密钥，长度必须是16/24/32字节，否则加密失败
 * @param ivec 初始向量，长度必须是16字节
 * @param enc true-加密，false-解密
 * @return 执行结果
 */
bool AES::cfb8_encrypt(const QByteArray &in, QByteArray &out, const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是16、24、32字节)
    Q_ASSERT(key.size() == 16 || key.size() == 24 || key.size() == 32);
    Q_ASSERT(ivec.size() == 16); // 初始向量为16字节

    // 特别注意：CFB模式加密和解密均使用加密key。
    // 生成加密key
    AES_KEY aes_key;
    if (AES_set_encrypt_key((const unsigned char*)key.data(), key.size() * 8, &aes_key) != 0)
    {
        return false;
    }

    // 执行CFB8模式加密或解密
    int num = 0;
    QByteArray ivecTemp = ivec; // ivec会被修改，故需要临时变量来暂存
    int encVal = enc ? AES_ENCRYPT : AES_DECRYPT;
    out.resize(in.size()); // 调整输出buf大小
    AES_cfb8_encrypt((const unsigned char*)in.data(),
                    (unsigned char*)out.data(),
                    in.size(),
                    &aes_key,
                    (unsigned char*)ivecTemp.data(),
                    &num,
                    encVal);
    return true;
}

/**
 * @brief AES::cfb128_encrypt
 * CFB128模式加解密，支持对任意长度明文进行加解密。
 * @param in 输入数据
 * @param out 输出结果
 * @param key 密钥，长度必须是16/24/32字节，否则加密失败
 * @param ivec 初始向量，长度必须是16字节
 * @param enc true-加密，false-解密
 * @return 执行结果
 */
bool AES::cfb128_encrypt(const QByteArray &in, QByteArray &out, const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是16、24、32字节)
    Q_ASSERT(key.size() == 16 || key.size() == 24 || key.size() == 32);
    Q_ASSERT(ivec.size() == 16); // 初始向量为16字节

    // 特别注意：CFB模式加密和解密均使用加密key。
    // 生成加密key
    AES_KEY aes_key;
    if (AES_set_encrypt_key((const unsigned char*)key.data(), key.size() * 8, &aes_key) != 0)
    {
        return false;
    }

    // 执行CFB128模式加密或解密
    int num = 0;
    QByteArray ivecTemp = ivec; // ivec会被修改，故需要临时变量来暂存
    int encVal = enc ? AES_ENCRYPT : AES_DECRYPT;
    out.resize(in.size()); // 调整输出buf大小
    AES_cfb128_encrypt((const unsigned char*)in.data(),
                    (unsigned char*)out.data(),
                    in.size(),
                    &aes_key,
                    (unsigned char*)ivecTemp.data(),
                    &num,
                    encVal);
    return true;
}

/**
 * @brief AES::ofb128_encrypt
 * OFB128模式加解密，支持对任意长度明文进行加解密。
 * @param in 输入数据
 * @param out 输出结果
 * @param key 密钥，长度必须是16/24/32字节，否则加密失败
 * @param ivec 初始向量，长度必须是16字节
 * @param enc true-加密，false-解密
 * @return 执行结果
 */
bool AES::ofb128_encrypt(const QByteArray &in, QByteArray &out, const QByteArray &key, const QByteArray &ivec, bool enc)
{
    // 检查密钥合法性(只能是16、24、32字节)
    Q_ASSERT(key.size() == 16 || key.size() == 24 || key.size() == 32);
    Q_ASSERT(ivec.size() == 16); // 初始向量为16字节

    // 特别注意：OFB模式加密和解密均使用加密key。
    // 生成加密key
    AES_KEY aes_key;
    if (AES_set_encrypt_key((const unsigned char*)key.data(), key.size() * 8, &aes_key) != 0)
    {
        return false;
    }

    Q_UNUSED(enc);

    // 特别注意：加密与解密执行相同的操作
    // 执行OFB128模式加密或解密
    int num = 0;
    QByteArray ivecTemp = ivec; // ivec会被修改，故需要临时变量来暂存
    out.resize(in.size()); // 调整输出buf大小
    AES_ofb128_encrypt((const unsigned char*)in.data(),
                    (unsigned char*)out.data(),
                    in.size(),
                    &aes_key,
                    (unsigned char*)ivecTemp.data(),
                    &num);
    return true;
}
