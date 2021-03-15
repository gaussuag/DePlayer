#include "VideoPlayerControlBar.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QAction>

#include "CustomVideoPlayerProgressBar.h"
#include "VideoPlayerVolumeControlWidget.h"

VideoPlayerControlBar::VideoPlayerControlBar(QWidget *parent) : QWidget(parent)
{
    initWidget();
    initActions();
}

VideoPlayerControlBar::~VideoPlayerControlBar()
{

}

void VideoPlayerControlBar::setEnabledProgressBar(bool flag)
{
    setEnabled(flag);
}

void VideoPlayerControlBar::setPlayingState()
{
    _controlBt->setIcon(QIcon(":/icon/pause.png"));
}

void VideoPlayerControlBar::setPauseState()
{
    _controlBt->setIcon(QIcon(":/icon/play.png"));
}

void VideoPlayerControlBar::setPosition(int position)
{
    _progressBar->setPosition(position);
}

void VideoPlayerControlBar::setDuration(int duration)
{
    _progressBar->setVideoDuration(duration);
}

void VideoPlayerControlBar::setVolume(int volume)
{
    _volumeWidget->setVolume(volume);
}

void VideoPlayerControlBar::setMute(bool flag)
{
    _volumeWidget->setMute(flag);
}

void VideoPlayerControlBar::setDurationString(const QString &duration)
{
    _durationLabel->setText(duration);
}

void VideoPlayerControlBar::fontChanged_slot(const QFont &font)
{
    auto myFont = font;
    myFont.setPointSize(18);
    _lineEdit->setFont(myFont);
}

void VideoPlayerControlBar::initWidget()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(5);
    mainLayout->setSpacing(0);

    _progressBar = new CustomVideoPlayerProgressBar(this);
    _progressBar->setValueColor("#adb1e4");
    _progressBar->setProgressBarColor("#3E3E3E");
    _progressBar->setMinimumWidth(912);
    _progressBar->setFixedHeight(44);
    _progressBar->setProgressBarHeight(8);
    _progressBar->setProgressBarHandleSize(4,16);
    _progressBar->setSplitBlockSize(4,16);
    _progressBar->setMarkSize(4,8);
    connect(_progressBar,&CustomVideoPlayerProgressBar::positionChanged,this,&VideoPlayerControlBar::positionChanged);

    mainLayout->addWidget(_progressBar);

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    _durationLabel = new QLabel(this);
    _durationLabel->setFixedWidth(160);
    _durationLabel->setStyleSheet("QLabel{"
                                  "font-size:16px;"
                                  "color:black;}");

    _controlBt = new QPushButton(this);
    _controlBt->setIconSize(QSize(32,32));
    _controlBt->setIcon(QIcon(":/icon/play.png"));
    connect(_controlBt,&QPushButton::clicked,this,&VideoPlayerControlBar::controlChanged);

    _openBt = new QPushButton(this);
    _openBt->setIcon(QIcon(":/icon/open.png"));
    connect(_openBt,&QPushButton::clicked,this,&VideoPlayerControlBar::requestOpenFile);

    _fullScreen = new QPushButton(this);
    _fullScreen->setIcon(QIcon(":/icon/fullScreen.png"));
    connect(_fullScreen,&QPushButton::clicked,this,&VideoPlayerControlBar::requestFullScreen);

    _lineEdit = new QLineEdit(this);
    _lineEdit->setPlaceholderText("按下回车发送biubiubiu ~");

    QAction *fontAction = new QAction(_lineEdit);
    fontAction->setIcon(QIcon(":/icon/font.png"));
    _lineEdit->addAction(fontAction,QLineEdit::LeadingPosition);
    connect(fontAction,&QAction::triggered,this,&VideoPlayerControlBar::requestSetFont);

    QAction *colorAction = new QAction(_lineEdit);
    colorAction->setIcon(QIcon(":/icon/color.png"));
    _lineEdit->addAction(colorAction,QLineEdit::TrailingPosition);
    connect(colorAction,&QAction::triggered,this,&VideoPlayerControlBar::requestSetColor);

    auto lineEditFont = _lineEdit->font();
    lineEditFont.setPointSize(14);
    _lineEdit->setFont(lineEditFont);

    connect(_lineEdit,&QLineEdit::returnPressed,this,[&](){
        auto text = _lineEdit->text();
        if(!text.isEmpty())
        {
            _lineEdit->clear();
            emit(requestEmitBullet(std::move(text)));
        }
    });

    _volumeWidget = new VideoPlayerVolumeControlWidget(this);
    _volumeWidget->setColor("#adb1e4");
    _volumeWidget->setMuteColor("#cf0003");

    connect(_volumeWidget,&VideoPlayerVolumeControlWidget::requestSetMute,this,&VideoPlayerControlBar::requestSetMute);
    connect(_volumeWidget,&VideoPlayerVolumeControlWidget::requestSetVolume,this,&VideoPlayerControlBar::requestSetVolume);

    buttonLayout->addSpacing(30);
    buttonLayout->addWidget(_durationLabel);
    buttonLayout->addStretch(5);
    buttonLayout->addWidget(_controlBt);
    buttonLayout->addSpacing(30);
    buttonLayout->addWidget(_lineEdit,5);
    buttonLayout->addSpacing(30);
    buttonLayout->addWidget(_volumeWidget);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(_openBt);
    buttonLayout->addWidget(_fullScreen);

    mainLayout->addLayout(buttonLayout);

    initButtonStyleSheet(_controlBt);
    initButtonStyleSheet(_openBt);
    initButtonStyleSheet(_fullScreen);
}

void VideoPlayerControlBar::initActions()
{
    QAction *frameStepAction = new QAction(this);
    frameStepAction->setShortcut(QKeySequence("ctrl+right"));
    connect(frameStepAction,&QAction::triggered,this,&VideoPlayerControlBar::requestFrameStep);

    QAction *frameBackStepAction = new QAction(this);
    frameBackStepAction->setShortcut(QKeySequence("ctrl+left"));
    connect(frameBackStepAction,&QAction::triggered,this,&VideoPlayerControlBar::requestFrameBackStep);

    QAction *stepAction = new QAction(this);
    stepAction->setShortcut(QKeySequence("right"));
    connect(stepAction,&QAction::triggered,this,&VideoPlayerControlBar::requestStep);

    QAction *backStepAction = new QAction(this);
    backStepAction->setShortcut(QKeySequence("left"));
    connect(backStepAction,&QAction::triggered,this,&VideoPlayerControlBar::requestBackStep);

    addAction(frameStepAction);
    addAction(frameBackStepAction);
    addAction(stepAction);
    addAction(backStepAction);
}

void VideoPlayerControlBar::initButtonStyleSheet(QPushButton *bt)
{
    bt->setStyleSheet("QPushButton{"
                      "border:0px;"
                      "border-radius:4px;"
                      "background-color:transparent;}"
                      "QPushButton::hover{"
                      "background-color:#decfde;}"
                      "QPushButton::pressed{"
                      "padding-left:2px;"
                      "padding-top:2px;}");
}


