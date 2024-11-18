#ifndef AES_H
#define AES_H

#include <QByteArray>

/**
 * @brief The AES class
 * AES加密算法，包括AES下多种加密模式的实现，底层调用openssl。
 */
class AES
{
public:
    bool ecb_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, bool enc);
    bool cbc_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    bool cfb1_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    bool cfb8_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    bool cfb128_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    bool ofb128_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
};

#endif // AES_H
