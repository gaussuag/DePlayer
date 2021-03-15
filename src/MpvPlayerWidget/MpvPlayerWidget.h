#ifndef MPVPLAYERWIDGET_H
#define MPVPLAYERWIDGET_H

#include <QtWidgets/QOpenGLWidget>
#include <client.h>
#include <render_gl.h>
#include <QPointer>
#include "qthelper.hpp"
#include "MpvTypes.h"

class MpvEventString
{
public:
    static QString EventString(mpv_event_id eventId);
};


class MpvPlayerWidget Q_DECL_FINAL: public QOpenGLWidget
{
    Q_OBJECT
public:
    MpvPlayerWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~MpvPlayerWidget();


    QSize sizeHint() const { return QSize(480, 270);}

    QVariant getProperty(const QString& name) const;

    Mpv::PlayState getPlayState() {return playState;}
    Mpv::FileInfo getMpvFileInfo() {return fileInfo;}
    Mpv::Osd_Dimensions getOsdDimensionsInfo() { return osdDimensionsInfo;}

    QString getPlayingIndex();

    double getSpeed() { return speed;}
    double getPosition(){ return position;}
    double getDuration(){ return duration;}
    double getPercent(){ return percent;}

    int getFrameNumber() { return currentFrameNumber;}
    int getFrameCount()  { return estimatedFrames;}


    int getVolume(){ return volume;}

    bool isLoaded(){ return loadedFlag;}
    bool isHardwareDecode(){ return hwdec;}
    bool isMute(){ return mute;}
    bool isSeeking();
    bool isPause();
    bool isPlaying();

    QSize getVideoSize(){ return QSize(videoWidth,videoHeight);}



    void playFile(const QString &AbsoluteFilePath);
    void openFile(const QString &AbsoluteFilePath);
    void appendFile(const QString &AbsoluteFilePath);

    void playFileOnPlaylistIndex(int index);

    void seek(qreal pos);
    void frameStep();
    void frameBackStep();

    void play();
    void pause();
    void playOrPause();
    void stop();
    void restart();
    void clearPlaylist();

    void setVolume(int level);
    void setMute(bool flag);

    void setSpeed(double value);

    void hardwareDecode（(bool flag);

    void setPreProperties();

    void playNext();


    void test();
public slots:
    void setProperty(const QString& name, const QVariant& value);
    void command(const QVariant& params);
    void setOption(const QString& name, const QVariant& value);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

protected slots:
    void loadFileInfo();
    void loadVideoParams();
    void loadAudioParams();
    void loadMetadata();
    void unloadFileInfo();
private Q_SLOTS:
    void on_mpv_events();
    void maybeUpdate();
private:
    mpv_handle *mpv{nullptr};
    mpv_render_context *mpv_gl{nullptr};

    // player parameter
    Mpv::PlayState playState = Mpv::Idle;
    Mpv::FileInfo fileInfo;
    Mpv::Osd_Dimensions osdDimensionsInfo;
    QString     msgLevel,
                playingIndex;
    double      speed{1},
                position{0},
                duration{0},
                percent{0};

    bool        loadedFlag{false},
                hwdec{true},
                mute{false};
    int         videoWidth{0},
                videoHeight{0},
                estimatedFrames{0},
                currentFrameNumber{-1},
                volume{50};


    void handle_mpv_event(mpv_event *event);
    static void on_update(void *ctx);

    void initMpvHandle();

    void updatePlayState(Mpv::PlayState s)     { Q_EMIT playStateChanged(playState = s); }

    void updatePercent(double p)               { Q_EMIT percentChanged(percent = p); }
    void updatePosition(double p)              { Q_EMIT positionChanged(position = p); }
    void updateCurrentFrameNumber(int f)       { Q_EMIT frameNumberChanged(currentFrameNumber = f);}
    void updateDuration(double d)              { Q_EMIT durationChanged(duration = d); }
    void updateVolume(int i)                   { Q_EMIT volumeChanged(volume = i); }
    void updateMute(bool b)                    { if (mute != b) Q_EMIT muteChanged(mute = b); }
    void updateVideoSize(int w, int h)         { Q_EMIT videoSizeChanged(videoWidth = w, videoHeight = h); }

    void updateSpeed(double s)                 { Q_EMIT speedChanged(speed = s);}

    void updateHardwareDecode(bool flag)       { Q_EMIT hardwareDecodeChange(hwdec = flag);}

    void updatePlayingIndex(QString index)     { Q_EMIT playingIndexChanged( playingIndex = index);}

    void loadOsdDimensionsParams();
signals:
    void playStateChanged(Mpv::PlayState);
    void fileInfoChanged(const Mpv::FileInfo&);
    void videoParamsChanged(const Mpv::VideoParams&);
    void audioParamsChanged(const Mpv::AudioParams&);

    void percentChanged(double);
    void positionChanged(double);
    void frameNumberChanged(int);
    void durationChanged(double);
    void frameCountChanged(int);

    void volumeChanged(int);
    void muteChanged(bool);

    void hardwareDecodeChange(bool);

    void speedChanged(double s);

    void videoSizeChanged(int, int);

    void playingIndexChanged(QString);

    void osd_dimensionsChanged();

    void messageSignal(QString m);
};


#endif // MPVPLAYERWIDGET_H
