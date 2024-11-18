#ifndef RANDOM_H
#define RANDOM_H

#include <QtGlobal>

#ifdef WIN32 // windows
#include <stdlib.h>
#else // linux
#define _rotl64(value, bits) ((value << bits) | (value >> (64 - bits))) // 64位变量value循环左移bits位
#endif

/**
 * @brief The Random class
 * 高效伪随机数生成器
 */
class Random
{
public:
    Random(quint64 ulSeed = 0)
    {
        _ulState[0] = 0xf1ea5eed;
        _ulState[1] = ulSeed;
        _ulState[2] = ulSeed;
        _ulState[3] = ulSeed;

        for (quint32 i = 0; i < 20; i++)
        {
            rand64();
        }
    }

    inline quint64 rand64()
    {
        quint64 e = _ulState[0] - _rotl64(_ulState[1], 7);
        _ulState[0] = _ulState[1] ^ _rotl64(_ulState[2], 13);
        _ulState[1] = _ulState[2] + _rotl64(_ulState[3], 37);
        _ulState[2] = _ulState[3] + e;
        _ulState[3] = e + _ulState[0];

        return _ulState[3];
    }

    inline quint32 rand32()
    {
        return (quint32)rand64();
    }

private:
    quint64 _ulState[4];
};

#endif // RANDOM_H
