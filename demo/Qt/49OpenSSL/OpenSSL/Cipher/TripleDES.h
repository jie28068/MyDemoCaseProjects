#ifndef TRIPLEDES_H
#define TRIPLEDES_H

#include <QByteArray>
#include <openssl/des.h>

/**
 * @brief The TripleDES class
 * TripleDES算法相关实现
 * key必须是24字节
 * ivec必须是8字节
 */
class TripleDES
{
public:
    void ecb_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, bool enc);
    void cbc_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    void cfb1_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    void cfb64_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    void ofb64_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);

private:
    void setKey(const QByteArray& key, DES_key_schedule& sch1, DES_key_schedule& sch2, DES_key_schedule& sch3);

private:
    const int DES_BLOCK_SIZE = 8;
};

#endif // TRIPLEDES_H
