#include "VideoPlayerVolumeControlWidget.h"

#include <QVBoxLayout>



VideoPlayerVolumeControlWidget::VideoPlayerVolumeControlWidget(QWidget *parent):QWidget(parent)
{
    initWidget();
}

bool VideoPlayerVolumeControlWidget::isMute()
{
    return _muteControlBt->isMute();
}

int VideoPlayerVolumeControlWidget::getVolume()
{
    return _volumeControlSlider->value();
}

void VideoPlayerVolumeControlWidget::setMute(bool flag)
{
    if(_muteControlBt)
        _muteControlBt->setMute(flag);
}

void VideoPlayerVolumeControlWidget::setColor(const QColor &color)
{
    _muteControlBt->setColor(color);
    _volumeControlSlider->setValueColor(color);
}

void VideoPlayerVolumeControlWidget::setMuteColor(const QColor & color)
{
    _muteControlBt->setMuteColor(color);
}

void VideoPlayerVolumeControlWidget::setVolume(int volume)
{
    if(_volumeControlSlider)
        _volumeControlSlider->setValue(volume);
    if(_muteControlBt)
        _muteControlBt->setVolumeValue(volume);
}

void VideoPlayerVolumeControlWidget::initWidget()
{
    setFixedSize(167,32);

    _muteControlBt = new CustomVolumeWidget(this);
    _muteControlBt->setFixedSize(32,32);

    connect(_muteControlBt,&CustomVolumeWidget::muteChanged,this,&VideoPlayerVolumeControlWidget::requestSetMute);

    _volumeControlSlider = new CustomRadiusSlider(Qt::Orientation::Horizontal,this);
    _volumeControlSlider->setFixedSize(90,5);
    _volumeControlSlider->setRange(0,100);

    connect(_volumeControlSlider,&CustomRadiusSlider::valueChanged,this,&VideoPlayerVolumeControlWidget::requestSetVolume);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(5);

    mainLayout->addStretch();
    mainLayout->addWidget(_muteControlBt);
    mainLayout->addWidget(_volumeControlSlider);
    mainLayout->addStretch();
}
