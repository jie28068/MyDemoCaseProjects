#ifndef VLCPLAYER_H
#define VLCPLAYER_H

#include <QString>
#include <qwindowdefs.h>
#include <vlc/vlc.h>

class VLCPlayer
{
public:
    enum State
    {
        Stopped,	// 停止
        Playing,	// 正在播放
        Paused		// 暂停
    };

    VLCPlayer();
    ~VLCPlayer();

    void setMedia(const QString& file); // 设置视频文件路径
    void setVideoOutput(WId winId); // 设置视频播放窗口id，视频在该窗口进行播放

    void play();	// 播放
    void pause();   // 暂停
    void resume();	// 继续
    void stop();	// 停止

    State state();	// 获取当前播放器状态

private:
    libvlc_instance_t* instance;    ///>VLC实例
    libvlc_media_player_t* player;  ///>VLC媒体播放器
    libvlc_media_t* media;          ///>可以播放的媒体
};

#endif // VLCPLAYER_H
