#include "RTSPServer.h"
#include <QDir>

#define VIDEO_MEDIA_NAME        "Video"

RTSPServer::RTSPServer()
{
    // 创建VLC实例
    instance = libvlc_new (0, nullptr);
}

RTSPServer::~RTSPServer()
{
    // 停止推流
    libvlc_vlm_stop_media(instance, VIDEO_MEDIA_NAME);

    // 释放VLC实例
    libvlc_vlm_release(instance);
    instance = nullptr;
}

bool RTSPServer::pushVideo(const QString &ip, int port, const QString &filePath)
{
    // 转码参数：#transcode{vcodec=h264,acodec=mpga,ab=128,channels=2,samplerate=44100,scodec=none}
    QString convertPara = "#transcode{vcodec=h264,acodec=mpga,ab=128,channels=2,samplerate=44100,scodec=none}";

    // 网络参数：rtp{sdp=rtsp://xx.xx.xx.xx:yyyy/}
    // 表示本机ip时，可省略ip，只写端口，如rtp{sdp=rtsp://:8554/}
    QString netPara = "rtp{sdp=rtsp://" + ip + ":" + QString::number(port) + "/}";

    // 如sout = "#transcode{vcodec=h264,acodec=mpga,ab=128,channels=2,samplerate=44100,scodec=none}:rtp{sdp=rtsp://127.0.0.1:8554/}"
    QString sout = convertPara + ":" + netPara;

    // 将推流视频路径转换为本地系统风格，win下"a\\b\\c"，linux下"a/b/c"
    QString path = QDir::toNativeSeparators(filePath);

    // 添加名为VIDEO_MEDIA_NAME的广播
    int ret = libvlc_vlm_add_broadcast(instance, VIDEO_MEDIA_NAME,
                                       path.toStdString().c_str(),
                                       sout.toStdString().c_str(),
                                       0, nullptr, true, false);
    if (ret != 0)
    {
        return false;
    }

    // 播放该广播
    ret = libvlc_vlm_play_media(instance, VIDEO_MEDIA_NAME);
    return (ret == 0);
}
