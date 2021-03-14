#ifndef VIDEOPLAYERCONTROLBAR_H
#define VIDEOPLAYERCONTROLBAR_H

#include <QWidget>

class QPushButton;
class QLineEdit;
class CustomVideoPlayerProgressBar;

class VideoPlayerControlBar :  public QWidget
{
    Q_OBJECT
public:
    explicit VideoPlayerControlBar(QWidget * parent = nullptr);
    ~VideoPlayerControlBar();

    void setEnabledProgressBar(bool flag);

    void setPlayingState();
    void setPauseState();

    void setPosition(int position);
    void setDuration(int duration);

    void setVolume(int volume);
    void setMute(bool flag);

private:
    CustomVideoPlayerProgressBar *_progressBar{nullptr};
    QPushButton *_controlBt{nullptr};
    QPushButton *_openBt{nullptr};
    QPushButton *_fullScreen{nullptr};
    QLineEdit *_lineEdit{nullptr};

    void initWidget();

    void initButtonStyleSheet(QPushButton *bt);
    void initActions();
signals:
    void controlChanged(bool);
    void requestOpenFile();
    void requestFullScreen();

    void requestEmitBullet(const QString &);
    void requestSetFont();
    void requestSetColor();

    void positionChanged(qreal);

    void requestFrameStep();
    void requestFrameBackStep();


    void requestStep();
    void requestBackStep();
};
#endif // VIDEOPLAYERCONTROLBAR_H
