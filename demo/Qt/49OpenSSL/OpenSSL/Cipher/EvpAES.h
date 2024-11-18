#ifndef EVPAES_H
#define EVPAES_H

#include <QByteArray>

struct evp_cipher_ctx_st;
typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;

struct evp_cipher_st;
typedef struct evp_cipher_st EVP_CIPHER;

class EvpAES
{
public:
    EvpAES();
    ~EvpAES();

    bool ecb_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, bool enc);
    bool cbc_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    bool cfb1_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    bool cfb8_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    bool cfb128_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    bool ofb128_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    bool ctr_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    bool gcm_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    bool xts_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);
    bool ocb_encrypt(const QByteArray& in, QByteArray& out, const QByteArray &key, const QByteArray& ivec, bool enc);

private:
    bool encrypt(const QByteArray& in, QByteArray& out, const QByteArray& key, const QByteArray& ivec, const EVP_CIPHER *ciper, bool enc);

private:
    EVP_CIPHER_CTX *ctx;
};

#endif // EVPAES_H
