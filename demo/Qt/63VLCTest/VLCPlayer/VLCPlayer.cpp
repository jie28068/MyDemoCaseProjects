#include "VLCPlayer.h"
#include <QDir>

VLCPlayer::VLCPlayer()
    : media(nullptr)
{
    // 创建VLC实例
    instance = libvlc_new (0, nullptr);

    // 创建VLC媒体播放器
    player = libvlc_media_player_new(instance);
}

VLCPlayer::~VLCPlayer()
{
    // 释放媒体
    if (media)
    {
        libvlc_media_release(media);
        media = nullptr;
    }

    // 释放VLC媒体播放器
    libvlc_media_player_release(player);
    player = nullptr;

    // 释放VLC实例
    libvlc_release(instance);
    instance = nullptr;
}

void VLCPlayer::setMedia(const QString &file)
{
    // 将路径转换为本地系统路径风格
    // win下时，如将路径"a/b/c"转换为"a\\b\\c"形式
    // linux下时，将路径转换为"a/b/c"形式
    QString path = QDir::toNativeSeparators(file);

    // 释放旧的媒体
    if (media)
    {
        libvlc_media_release(media);
        media = nullptr;
    }

    // 创建新的媒体
    media = libvlc_media_new_path(instance, path.toStdString().c_str());
    libvlc_media_player_set_media(player, media); // 把新的媒体设置给播放器
}

void VLCPlayer::setVideoOutput(WId winId)
{
    // 设置播放句柄
#if defined(Q_OS_WIN)
    libvlc_media_player_set_hwnd(player, (void *)winId);
#elif defined(Q_OS_LINUX)
    libvlc_media_player_set_xwindow(player, winId);
#elif defined(Q_OS_MAC)
    libvlc_media_player_set_nsobject(player, (void *)winId);
#endif
}

void VLCPlayer::play()
{
    libvlc_media_player_play(player);
}

void VLCPlayer::pause()
{
    libvlc_media_player_pause(player);
}

void VLCPlayer::resume()
{
    play();
}

void VLCPlayer::stop()
{
    libvlc_media_player_stop(player);
}

VLCPlayer::State VLCPlayer::state()
{
    libvlc_state_t s = libvlc_media_player_get_state(player);
    if (s == libvlc_Ended || s == libvlc_Stopped)
    {
        return Stopped;
    }
    else if (s == libvlc_Paused)
    {
        return Paused;
    }
    else if (s == libvlc_Playing)
    {
        return Playing;
    }
    else
    {
        return Stopped;
    }
}
