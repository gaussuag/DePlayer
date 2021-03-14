#include "MpvVideoFileReader.h"

#include <QDebug>
#include <QWidget>
#include <QEventLoop>
#include <QTimer>

MpvVideoFileReader::MpvVideoFileReader(QPointer<MpvPlayerWidget> player, QObject *parent):QObject(parent)
{
    _player = player;
    connect(_player,&MpvPlayerWidget::fileInfoChanged,this,&MpvVideoFileReader::fileInfoChanged_slot);

}

MpvVideoFileReader::~MpvVideoFileReader()
{

}

void MpvVideoFileReader::readSequenceFiles(const QStringList &files)
{
    _filePaths = files;
    _player->pause();
    _player->openFile(_filePaths.takeLast());
}

void MpvVideoFileReader::fileInfoChanged_slot()
{
    qDebug()<<"read fileinfo changed"<<_player->isLoaded()<<_filePaths.size();
    _fileMap.insert(_filePaths.size(),_player->getMpvFileInfo());
    if(!_filePaths.isEmpty())
        _player->openFile(_filePaths.takeLast());
    else
        emit(readFinished());
}




MpvVideoPlayerSeeker::MpvVideoPlayerSeeker(QPointer<MpvPlayerWidget> player, QObject *parent)
    :QObject(parent)
{
    p_myself = QPointer<MpvVideoPlayerSeeker>(this);
    _player = player;
    connect(_player,&MpvPlayerWidget::fileInfoChanged,this,&MpvVideoPlayerSeeker::fileInfoChanged_slot);
    connect(_player,&MpvPlayerWidget::frameNumberChanged,this,&MpvVideoPlayerSeeker::frameNumberChanged_slot);
}

MpvVideoPlayerSeeker::~MpvVideoPlayerSeeker()
{
    qDebug()<<"seeker destory"<<this;
}

void MpvVideoPlayerSeeker::seekPositionOnPlaylistIndex(int index,qreal position)
{
    _index = index;
    _position = position;

    if(_player.isNull())
        emit(seekError());

    _player->playFileOnPlaylistIndex(index);
}

void MpvVideoPlayerSeeker::seekPositionOnFilePlaying(const QString & filePath,qreal postion,bool autoPauseFlag)
{
    _autoPauseFlag = autoPauseFlag;

    _position = postion;
    if(_player.isNull())
        emit(seekError());

    _player->clearPlaylist();
    _player->playFile(filePath);
}

void MpvVideoPlayerSeeker::seekFrameOnFilePlaying(const QString & filePath,int frameStamp,bool autoPauseFlag)
{
    _seekType = 1;

    _autoPauseFlag = autoPauseFlag;

    _frameStamp = frameStamp;
    if(_player.isNull())
        emit(seekError());

    _player->clearPlaylist();
    _player->playFile(filePath);
}

void MpvVideoPlayerSeeker::fileInfoChanged_slot()
{
    if(_seekType == 1)
        transformFrameToTimestamp();

    _seeked = true;
    _player->seek(_position);
}

void MpvVideoPlayerSeeker::frameNumberChanged_slot(int frameNumber)
{
    if(_seeked)
    {
        if(_autoPauseFlag)
            _player->pause();

        if(_seekType == 1)
        {
            if(!_timerStartFlag)
            {
                if(frameNumber == _frameStamp)
                {
                    _exitTimer = new QTimer(this);
                    connect(_exitTimer,&QTimer::timeout,this,&MpvVideoPlayerSeeker::handleTimeout);
                    _exitTimer->start(1500);
                    _timerStartFlag = true;
                }
            }
            else
            {
                if(frameNumber != _frameStamp)
                {
                    auto diff = frameNumber - _frameStamp;
                    if(diff > 0)
                        _player->frameBackStep();
                    else if(diff < 0)
                        _player->frameStep();
                }
            }
        }
    }
}

void MpvVideoPlayerSeeker::handleTimeout()
{
    if(!p_myself.isNull())
        emit(seekFinished());
}

void MpvVideoPlayerSeeker::transformFrameToTimestamp()
{
    ///帧取整
    /// 根据fps获取单帧时长，毫秒单位
    auto fps = _player->getMpvFileInfo().video_params.frameRates;
    auto singFrameTime = 1000.000/fps;

    _position = singFrameTime * _frameStamp;
}
