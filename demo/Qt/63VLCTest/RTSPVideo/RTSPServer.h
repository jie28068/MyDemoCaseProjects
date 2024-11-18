#ifndef RTSPSERVER_H
#define RTSPSERVER_H

#include <QString>
#include <vlc/vlc.h>

class RTSPServer
{
public:
    RTSPServer();
    ~RTSPServer();

    // 使用VLC实现RTSP推流。将视频文件转码后，推流到指定ip的端口上，协议一般为udp。
    // 在VLC播放器中输入rtsp://xx.xx.xx.xx:端口/，即可播放推流的视频
    // 推流到本地时，如rtsp://127.0.0.1:8554/，即可播放推流的视频
    bool pushVideo(const QString& ip, int port, const QString& filePath);

private:
    libvlc_instance_t *instance;
};

#endif // RTSPSERVER_H
