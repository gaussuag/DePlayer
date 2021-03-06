#pragma once

#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QList>

namespace Mpv
{
    // filetypes supported by mpv: https://github.com/mpv-player/mpv/blob/master/player/external_files.c
    const QStringList audio_filetypes = {"*.mp3","*.ogg","*.wav","*.wma","*.m4a","*.aac","*.ac3","*.ape","*.flac","*.ra","*.mka","*.dts","*.opus"},
                      video_filetypes = {"*.avi","*.divx","*.mpg","*.mpeg","*.m1v","*.m2v","*.mpv","*.dv","*.3gp","*.mov","*.mp4","*.m4v","*.mqv","*.dat","*.vcd","*.ogm","*.ogv","*.asf","*.wmv","*.vob","*.mkv","*.ram","*.flv","*.rm","*.ts","*.rmvb","*.dvr-ms","*.m2t","*.m2ts","*.rec","*.f4v","*.hdmov","*.webm","*.vp8","*.letv","*.hlv","*.mts"},
                      media_filetypes = audio_filetypes + video_filetypes,
                      subtitle_filetypes = {"*.sub","*.srt","*.ass","*.ssa","*.smi","*.rt","*.txt","*.mks","*.vtt","*.sup"};

    enum PlayState
    {
        // this number scheme is set so we know all playStates greater than 0 mean the video is in play
        Idle = -1,
        StartFile = 1,
        FileLoaded = 2,
        Playing = 3,
        Paused = 4,
        PlayFinished = 5,
        Seeking = 6,
        Stopped = -2
    };
    struct Chapter
    {
        QString title;
        int time;
    };
    struct Track
    {
        int id;
        QString type;
        int src_id;
        QString title;
        QString lang;
        unsigned albumart : 1,
                 _default : 1,
                 external : 1;
        QString external_filename;
        QString codec;

        bool operator==(const Track &t)
        {
            return (id == t.id);
        }
    };
    struct VideoParams
    {
        QString codec;
        int width = 0,
            height = 0,
            dwidth = 0,
            dheight = 0,
            estimated_frames = 0;
        double aspect = 0,
               frameRates = 0;
    };
    struct AudioParams
    {
        QString codec;
        int samplerate,
            channels;
    };

    struct FileInfo
    {
        QString media_title{""};
        QString media_absolutePath{""};
        qreal length = 0;
        QMap<QString, QString> metadata;
        VideoParams video_params;
        AudioParams audio_params;
        QList<Track> tracks; // audio, video, and subs
        QList<Chapter> chapters;
    };

    struct Osd_Dimensions{
        qreal osd_width  = 0;
        qreal osd_height = 0;
        qreal osd_aspect = 0;
        qreal osd_margins_left = 0;
        qreal osd_margins_top = 0;
        qreal osd_margins_right = 0;
        qreal osd_margins_bottom = 0;
    };
}

Q_DECLARE_METATYPE(Mpv::PlayState) // so we can pass it with signals & slots
Q_DECLARE_METATYPE(Mpv::Chapter)
Q_DECLARE_METATYPE(Mpv::Track)
Q_DECLARE_METATYPE(Mpv::VideoParams)
Q_DECLARE_METATYPE(Mpv::AudioParams)
Q_DECLARE_METATYPE(Mpv::FileInfo)
