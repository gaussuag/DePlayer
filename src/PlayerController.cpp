#include "PlayerController.h"

#include "MpvPlayerWidget.h"
#include "VideoPlayerControlBar.h"
#include "bulletEngine.h"
#include "TipsBubbleFloatingHelper.h"

#include <QTime>
#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>

PlayerController::PlayerController(QObject *parent) : QObject(parent)
{

}

PlayerController::~PlayerController()
{

}

void PlayerController::setPlayer(MpvPlayerWidget *player, VideoPlayerControlBar *controlBar, QWidget *MainWidget)
{
    _mainWidget = MainWidget;
    _player = player;
    _controlBar = controlBar;

    connectPlay();
    connectControlBar();

    initBulletEngine();

    _controlBar->setDurationString(durationStringFromFrame());
}

void PlayerController::connectPlay()
{
    connect(_player,&MpvPlayerWidget::playStateChanged,this,&PlayerController::player_playStateChanged_slot);
    connect(_player,&MpvPlayerWidget::fileInfoChanged,this,&PlayerController::player_fileInfoChanged_slot);
    connect(_player,&MpvPlayerWidget::frameNumberChanged,this,&PlayerController::player_frameNumberChanged_slot);
    connect(_player,&MpvPlayerWidget::frameCountChanged,this,&PlayerController::player_frameCountChanged_slot);
    connect(_player,&MpvPlayerWidget::volumeChanged,this,&PlayerController::player_volumeChanged_slot);
    connect(_player,&MpvPlayerWidget::muteChanged,this,&PlayerController::player_muteChanged_slot);
}

void PlayerController::connectControlBar()
{
    connect(_controlBar,&VideoPlayerControlBar::controlChanged,this,&PlayerController::controlChanged_slot);
    connect(_controlBar,&VideoPlayerControlBar::requestOpenFile,this,&PlayerController::requestOpenFile_slot);
    connect(_controlBar,&VideoPlayerControlBar::requestFullScreen,this,&PlayerController::requestFullScreen);

    connect(_controlBar,&VideoPlayerControlBar::requestEmitBullet,this,&PlayerController::requestEmitBullet_slot);
    connect(_controlBar,&VideoPlayerControlBar::requestSetFont,this,&PlayerController::requestSetFont_slot);
    connect(_controlBar,&VideoPlayerControlBar::requestSetColor,this,&PlayerController::requestSetColor_slot);
    connect(_controlBar,&VideoPlayerControlBar::positionChanged,this,&PlayerController::positionChanged_slot);

    connect(_controlBar,&VideoPlayerControlBar::requestFrameStep,_player,&MpvPlayerWidget::frameStep);
    connect(_controlBar,&VideoPlayerControlBar::requestFrameBackStep,_player,&MpvPlayerWidget::frameBackStep);

    connect(_controlBar,&VideoPlayerControlBar::requestStep,this,&PlayerController::requestStep_slot);
    connect(_controlBar,&VideoPlayerControlBar::requestBackStep,this,&PlayerController::requestBackStep_slot);


    connect(_controlBar,&VideoPlayerControlBar::requestSetVolume,_player,&MpvPlayerWidget::setVolume);
    connect(_controlBar,&VideoPlayerControlBar::requestSetMute,_player,&MpvPlayerWidget::setMute);

    connect(this,&PlayerController::fontChanged,_controlBar,&VideoPlayerControlBar::fontChanged_slot);
}

void PlayerController::player_playStateChanged_slot(Mpv::PlayState state)
{
    if(_controlBar)
    {
        if(state == Mpv::PlayState::StartFile)
        {}
        else if(state == Mpv::PlayState::FileLoaded)
        {
            _controlBar->setEnabledProgressBar(true);
        }
        else if(state == Mpv::PlayState::Playing)
        {
            _controlBar->setPlayingState();
            _bulletEngine->setPlaying();
        }
        else if(state == Mpv::PlayState::Paused)
        {
            _controlBar->setPauseState();
            _bulletEngine->setPause();
        }
        else if(state == Mpv::PlayState::Idle)
        {
            //handlePlayStateOnIdle();
        }
        else if(state == Mpv::PlayState::PlayFinished)
        {}
    }
}

void PlayerController::player_fileInfoChanged_slot(const Mpv::FileInfo& VideoFileInfo)
{
    /// 根据fps获取单帧时长，毫秒单位
    auto fps = VideoFileInfo.video_params.frameRates;
    _singFrameTime = 1000.000/fps;
}
void PlayerController::player_frameNumberChanged_slot(int frame)
{
    if(_currentFrame == frame)
        return;

    _currentFrame = frame;

    if(_controlBar)
    {
        _controlBar->setPosition(frame);
        _controlBar->setDurationString(durationStringFromFrame());
    }

    if(_bulletEngine)
        _bulletEngine->synchronizingVideoPosition(frame);
}
void PlayerController::player_frameCountChanged_slot(int frameCount)
{
    _frameCount = frameCount;

    if(_controlBar)
        _controlBar->setDuration(frameCount);
}

void PlayerController::player_volumeChanged_slot(int volume)
{
    if(_controlBar)
        _controlBar->setVolume(volume);
}
void PlayerController::player_muteChanged_slot(bool flag)
{
    if(_controlBar)
        _controlBar->setMute(flag);
}

void PlayerController::controlChanged_slot()
{
    _player->playOrPause();
}

void PlayerController::requestOpenFile_slot()
{
    auto filePath = QFileDialog::getOpenFileName(_mainWidget,
                                                 QString(),QString(),
                                                 tr("Video Files (*.avi *.divx *.mpg *.mpeg "
                                                    "*.m1v *.m2v *.mpv *.dv *.3gp *.mov *.mp4 "
                                                    "*.m4v *.mqv *.dat *.vcd *.ogm *.ogv *.asf "
                                                    "*.wmv *.vob *.mkv *.ram *.flv *.rm *.ts "
                                                    "*.rmvb *.dvr-ms *.m2t *.m2ts *.rec *.f4v "
                                                    "*.hdmov *.webm *.vp8 *.letv *.hlv *.mts)"));



    if(!filePath.isEmpty())
            _player->openFile(filePath);
}

void PlayerController::requestEmitBullet_slot(const QString & text)
{
    if(!text.isEmpty())
    {
        if(!_player->isPlaying())
        {
            TipsHelper->showTips(QString::fromLocal8Bit("请播放视频哦"));
            return;
        }
        _bulletEngine->addBullet(text);
    }
}

void PlayerController::requestSetFont_slot()
{
    if(_bulletEngine)
    {
        bool flag;
        auto font = QFontDialog::getFont(&flag,_bulletEngine->getFont(),_mainWidget);
        _bulletEngine->setFont(font);
        emit(fontChanged(font));
    }
}

void PlayerController::requestSetColor_slot()
{
    if(_bulletEngine)
    {
        auto color = QColorDialog::getColor(_bulletEngine->getColor(),_mainWidget);
        _bulletEngine->setColor(color);
    }
}

void PlayerController::positionChanged_slot(qreal position)
{
    /// frame to timestamp
    _player->seek(transformFrameToTimestamp(position));
}

void PlayerController::requestStep_slot()
{
    if(_player)
        _player->seek(_player->getPosition() + 3000);
}

void PlayerController::requestBackStep_slot()
{
    if(_player)
        _player->seek(_player->getPosition() - 3000);
}

void PlayerController::initBulletEngine()
{
    _bulletEngine = new bulletEngine(this);
    _bulletEngine->setPlayer(_player);
}

qreal PlayerController::transformFrameToTimestamp(int frameToPosition)
{
    ///帧取整
    int frame = frameToPosition;
    /// 根据fps获取单帧时长，毫秒单位
    auto fps = _player->getMpvFileInfo().video_params.frameRates;
    auto singFrameTime = 1000.000/fps;

    qreal timeStamp = singFrameTime * frame;
    return timeStamp;
}

QString PlayerController::durationStringFromFrame()
{
    if(_singFrameTime == 0)
    {
        auto fps = _player->getMpvFileInfo().video_params.frameRates;
        _singFrameTime = 1000.000/fps;
    }

    qreal timeStamp = _currentFrame * _singFrameTime;
    qreal duration = _frameCount * _singFrameTime;

    QTime time = QTime::fromMSecsSinceStartOfDay(qRound(timeStamp));
    QTime totalTime = QTime::fromMSecsSinceStartOfDay(qRound(duration));

    /// hours
    if(duration >= 3600)
        return QString("%1 / %2").arg(time.toString(QStringLiteral("h:mm:ss"))).arg(totalTime.toString(QStringLiteral("h:mm:ss")));
    /// minutes
    if(duration >= 60)
        return QString("%1 / %2").arg(time.toString(QStringLiteral("mm:ss"))).arg(totalTime.toString(QStringLiteral("mm:ss")));

    /// second
    return QString("%1 / %2").arg(time.toString(QStringLiteral("00:ss"))).arg(totalTime.toString(QStringLiteral("00:ss")));

}


