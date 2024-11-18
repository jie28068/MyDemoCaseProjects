#include <QCoreApplication>
#include <QDebug>
#include "AES.h"
#include "EvpAES.h"
#include "TripleDES.h"
#include "RSAC.h"
#include "HASH.h"

void createTestData(QByteArray& data, int size)
{
    data.resize(size);
    for (int i = 0; i < size; i++)
    {
        data[i] = i % 128;
    }
}

void testAES(const QByteArray& data)
{
    QByteArray plainText = data;
    QByteArray encryptText;
    QByteArray decryptText;

    QByteArray key = QByteArray::fromHex("8cc72b05705d5c46f412af8cbed55aad");
    QByteArray ivec = QByteArray::fromHex("667b02a85c61c786def4521b060265e8");

    // AES ecb模式加密验证
    AES aes;
    aes.ecb_encrypt(plainText, encryptText, key, true);
    aes.ecb_encrypt(encryptText, decryptText, key, false);
    qDebug() << "AES ecb encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // AES cbc模式加密验证
    aes.cbc_encrypt(plainText, encryptText, key, ivec, true);
    aes.cbc_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "AES cbc encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // AES cfb1模式加密验证
    aes.cfb1_encrypt(plainText, encryptText, key, ivec, true);
    aes.cfb1_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "AES cfb1 encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // AES cfb8模式加密验证
    aes.cfb8_encrypt(plainText, encryptText, key, ivec, true);
    aes.cfb8_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "AES cfb8 encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // AES cfb128模式加密验证
    aes.cfb128_encrypt(plainText, encryptText, key, ivec, true);
    aes.cfb128_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "AES cfb128 encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // AES ofb128模式加密验证
    aes.ofb128_encrypt(plainText, encryptText, key, ivec, true);
    aes.ofb128_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "AES ofb128 encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();
}

void testEvpAES(const QByteArray& data)
{
    QByteArray plainText = data;
    QByteArray encryptText;
    QByteArray decryptText;

    QByteArray key = QByteArray::fromHex("8cc72b05705d5c46f412af8cbed55aad");
    QByteArray ivec = QByteArray::fromHex("667b02a85c61c786def4521b060265e8");

    // EvpAES ecb模式加密验证
    EvpAES aes;
    aes.ecb_encrypt(plainText, encryptText, key, true);
    aes.ecb_encrypt(encryptText, decryptText, key, false);
    qDebug() << "EvpAES ecb encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // EvpAES cbc模式加密验证
    aes.cbc_encrypt(plainText, encryptText, key, ivec, true);
    aes.cbc_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "EvpAES cbc encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // EvpAES cfb1模式加密验证
    aes.cfb1_encrypt(plainText, encryptText, key, ivec, true);
    aes.cfb1_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "EvpAES cfb1 encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // EvpAES cfb8模式加密验证
    aes.cfb8_encrypt(plainText, encryptText, key, ivec, true);
    aes.cfb8_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "EvpAES cfb8 encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // EvpAES cfb128模式加密验证
    aes.cfb128_encrypt(plainText, encryptText, key, ivec, true);
    aes.cfb128_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "EvpAES cfb128 encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // EvpAES ofb128模式加密验证
    aes.ofb128_encrypt(plainText, encryptText, key, ivec, true);
    aes.ofb128_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "EvpAES ofb128 encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // EvpAES ctr模式加密验证
    aes.ctr_encrypt(plainText, encryptText, key, ivec, true);
    aes.ctr_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "EvpAES ctr encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // EvpAES gcm模式加密验证
    aes.gcm_encrypt(plainText, encryptText, key, ivec, true);
    aes.gcm_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "EvpAES gcm encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // EvpAES xts模式加密验证
    aes.xts_encrypt(plainText, encryptText, key, ivec, true);
    aes.xts_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "EvpAES xts encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // EvpAES ocb模式加密验证
    aes.ocb_encrypt(plainText, encryptText, key, ivec, true);
    aes.ocb_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "EvpAES ocb encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();
}

void testTripleDES(const QByteArray& data)
{
    QByteArray plainText = data;
    QByteArray encryptText;
    QByteArray decryptText;

    QByteArray key = QByteArray::fromHex("8cc72b05705d5c46f412af8cbed55aad8cc72b05705d5c46");
    QByteArray ivec = QByteArray::fromHex("667b02a85c61c786");

    // TripleDES ecb模式加密验证
    TripleDES tripleDES;
    tripleDES.ecb_encrypt(plainText, encryptText, key, true);
    tripleDES.ecb_encrypt(encryptText, decryptText, key, false);
    qDebug() << "TripleDES ecb encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // TripleDES cbc模式加密验证
    tripleDES.cbc_encrypt(plainText, encryptText, key, ivec, true);
    tripleDES.cbc_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "TripleDES cbc encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // TripleDES cfb1模式加密验证
    tripleDES.cfb1_encrypt(plainText, encryptText, key, ivec, true);
    tripleDES.cfb1_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "TripleDES cfb1 encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // TripleDES cfb64模式加密验证
    tripleDES.cfb64_encrypt(plainText, encryptText, key, ivec, true);
    tripleDES.cfb64_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "TripleDES cfb64 encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // TripleDES ofb64模式加密验证
    tripleDES.ofb64_encrypt(plainText, encryptText, key, ivec, true);
    tripleDES.ofb64_encrypt(encryptText, decryptText, key, ivec, false);
    qDebug() << "TripleDES ofb64 encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();
}

void testRSA(const QByteArray& data)
{
    // RSA密钥对生成验证
    RSAC rsac;
    rsac.generateKeyPair("./prikey.pem", "./pubkey.pem", 1024);

    // RSA加解密验证
    QByteArray priKey, pubKey;
    QByteArray plainText = data;
    QByteArray encryptText;
    QByteArray decryptText;
    rsac.generateKeyPair(priKey, pubKey, 1024);
    rsac.encrypt(plainText, encryptText, pubKey);       // 加密
    rsac.decrypt(encryptText, decryptText, priKey);     // 解密
    qDebug() << "RSA encrypt verify" << ((decryptText == plainText) ? "succeeded" : "failed");
    encryptText.clear();
    decryptText.clear();

    // RSA签名、验签
    HASH hash(HASH::SHA256); // 生成文件SHA256摘要
    hash.addData("life's a struggle");
    QByteArray digest = hash.result();

    QByteArray sign;
    rsac.sign(digest, sign, priKey);                // 签名
    bool ret = rsac.verify(digest, sign, pubKey);   // 验签
    qDebug() << "RSA sign verify" << (ret ? "succeeded" : "failed");
}

void testHash(const QByteArray& data)
{
    // Sha1
    HASH hash1(HASH::SHA1);
    hash1.addData(data);
    qDebug() << "SHA1:" << hash1.result().toHex();

    // Sha224
    HASH hash2(HASH::SHA224);
    hash2.addData(data);
    qDebug() << "SHA224:" << hash2.result().toHex();

    // Sha256
    HASH hash3(HASH::SHA256);
    hash3.addData(data);
    qDebug() << "SHA256:" << hash3.result().toHex();

    // Sha384
    HASH hash4(HASH::SHA384);
    hash4.addData(data);
    qDebug() << "SHA384:" << hash4.result().toHex();

    // Sha512
    HASH hash5(HASH::SHA512);
    hash5.addData(data);
    qDebug() << "SHA512:" << hash5.result().toHex();

    // Ripemd160
    HASH hash6(HASH::RIPEMD160);
    hash6.addData(data);
    qDebug() << "RIPEMD160:" << hash6.result().toHex();

    // Md5
    HASH hash7(HASH::MD5);
    hash7.addData(data);
    qDebug() << "MD5:" << hash7.result().toHex();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 产生1MB+3B的测试数据，为了使该测试数据长度，不为8或16的整数倍
    QByteArray data;
    createTestData(data, 1*1024*1024+3);

    // 测试AES
    testAES(data);     // 测试，直接调用OpenSSL中AES算法函数
    testEvpAES(data);  // 测试，通过EVP框架调用AES算法

    // 测试TripleDES
    testTripleDES(data);

    // 测试RSA
    testRSA(data);

    // 测试HASH
    testHash(data);

    return a.exec();
}
