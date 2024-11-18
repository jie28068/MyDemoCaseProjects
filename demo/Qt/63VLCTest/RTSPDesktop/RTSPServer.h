#ifndef RTSPSERVER_H
#define RTSPSERVER_H

#include <QString>
#include <vlc/vlc.h>

/**
 * @brief The RTSPServer class
 * 建议不要使用vlc-2.2.4，该版本推流桌面，会发生崩溃。
 * 在实际使用vlc-2.2.6下测试时，可以正常推流桌面。
 */
class RTSPServer
{
public:
    RTSPServer();
    ~RTSPServer();

    // 使用VLC实现RTSP推流桌面（共享桌面）
    // 将桌面录制的视频数据，推流到指定ip的端口上
    bool pushDesktop(const QString& ip, int port);

private:
    libvlc_instance_t *instance;
};

#endif // RTSPSERVER_H
