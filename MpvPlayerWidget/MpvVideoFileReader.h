#ifndef MPVVIDEOFILEREADER_H
#define MPVVIDEOFILEREADER_H

#include <QObject>
#include <QPointer>

#include "MpvPlayerWidget.h"

class QTimer;

class MpvVideoFileReader : public QObject
{
    Q_OBJECT
public:
    MpvVideoFileReader(QPointer<MpvPlayerWidget> player, QObject * parent = nullptr);
    ~MpvVideoFileReader();

    void readSequenceFiles(const QStringList & files);
    QMap<int,Mpv::FileInfo> GetFileInformatitons() { return _fileMap;}
public slots:
    void fileInfoChanged_slot();
private:
    QPointer<MpvPlayerWidget> _player;

    QStringList _filePaths;
    QMap<int,Mpv::FileInfo> _fileMap;

signals:
    void readFinished();
};

class MpvVideoPlayerSeeker : public QObject
{
    Q_OBJECT
public:
    explicit MpvVideoPlayerSeeker(QPointer<MpvPlayerWidget> player,
                                  QObject *parent = nullptr);
    ~MpvVideoPlayerSeeker();
    void seekPositionOnPlaylistIndex(int index, qreal position);

    void seekPositionOnFilePlaying(const QString &filePath, qreal postion, bool autoPauseFlag = false);

    void seekFrameOnFilePlaying(const QString &filePath, int frameStamp, bool autoPauseFlag = false);
protected slots:
    void fileInfoChanged_slot();
    void frameNumberChanged_slot(int frameNumber);

private slots:
    void handleTimeout();
private:
    QPointer<MpvPlayerWidget> _player;
    QPointer<MpvVideoPlayerSeeker> p_myself;

    bool _autoPauseFlag{false};

    /// 0 位置，1帧数
    int _seekType{0};

    int _index{0};
    qreal _position{0};
    int _frameStamp{0};

    bool _seeked {false};
    bool _timerStartFlag{false};

    QTimer *_exitTimer{nullptr};

    void transformFrameToTimestamp();
signals:
    void seekFinished();
    void seekError();
};



#endif // MPVVIDEOFILEREADER_H
