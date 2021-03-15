#ifndef VIDEOPLAYERVOLUMECONTROLWIDGET_H
#define VIDEOPLAYERVOLUMECONTROLWIDGET_H

#include <QWidget>

#include "CustomRadiusSlider.h"
#include "CustomVolumeWidget.h"

class VideoPlayerVolumeControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoPlayerVolumeControlWidget(QWidget *parent = nullptr);

    bool isMute();
    int getVolume();

    void setVolume(int volume);

    void setMute(bool flag);

    void setColor(const QColor & color);
    void setMuteColor(const QColor &color);
private:
    CustomVolumeWidget *_muteControlBt{nullptr};
    CustomRadiusSlider *_volumeControlSlider{nullptr};

    void initWidget();

signals:
    void requestSetMute(bool);
    void requestSetVolume(int);

};

#endif // CUSTOMVIDEOPLAYERCONTROLBAR_H
