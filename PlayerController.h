#ifndef MPVPLAYERCONTROLLER_H
#define MPVPLAYERCONTROLLER_H

#include <QObject>

#include "MpvPlayerWidget.h"

class VideoPlayerControlBar;
class bulletEngine;


class PlayerController : public QObject
{
    Q_OBJECT
public:
    explicit PlayerController(QObject *parent = nullptr);
    ~PlayerController();

    void setPlayer(MpvPlayerWidget *player, VideoPlayerControlBar * controlBar, QWidget *MainWidget);


private:
    QWidget *_mainWidget{nullptr};
    MpvPlayerWidget *_player{nullptr};
    VideoPlayerControlBar *_controlBar{nullptr};
    bulletEngine *_bulletEngine{nullptr};

    int _currentFrame{0};

    void connectPlay();
    void connectControlBar();

    void initBulletEngine();

    qreal transformFrameToTimestamp(int frameToPosition);
signals:
    void requestFullScreen();
    void fontChanged(const QFont & font);
private slots:
    void player_playStateChanged_slot(Mpv::PlayState state);
    void player_fileInfoChanged_slot(const Mpv::FileInfo &VideoFileInfo);
    void player_frameNumberChanged_slot(int frame);
    void player_frameCountChanged_slot(int frameCount);
    void player_volumeChanged_slot(int volume);
    void player_muteChanged_slot(bool flag);

    void controlChanged_slot();
    void requestOpenFile_slot();
    void requestEmitBullet_slot(const QString &text);
    void requestSetFont_slot();
    void requestSetColor_slot();

    void positionChanged_slot(qreal position);
    void requestStep_slot();
    void requestBackStep_slot();
};

#endif // MPVPLAYERCONTROLLER_H
