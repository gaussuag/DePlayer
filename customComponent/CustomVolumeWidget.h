#ifndef CUSTOMVOLUMEWIDGET_H
#define CUSTOMVOLUMEWIDGET_H

#include <QWidget>
#include <QColor>

class CustomVolumeWidget : public QWidget
{
    Q_OBJECT
public:
    enum VolumeLevel{
        mute = 0,
        level1 = 1,
        level2 = 2,
        level3 = 3
    };
    explicit CustomVolumeWidget(QWidget *parent = nullptr);

    void setColor(const QColor & color);
    void setMuteColor(const QColor & color);
    void setDisableColor(const QColor & color);

    bool isMute() { return _isMuteFlag;}
public slots:
    void setVolumeValue(int Volume);
    void setMute(bool flag);

    void setRange(int Minimum,int Maximum);
protected:
    void paintEvent(QPaintEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);
private:
    CustomVolumeWidget::VolumeLevel _volumeLevel{VolumeLevel::mute};
    bool _isMuteFlag{false};

    int _volume{0};
    int _minimum{0};
    int _maximum{100};

    QColor _mainColor{Qt::white};
    QColor _muteColor{Qt::white};
    QColor _disableColor{Qt::gray};

    void updateMuteFlag(bool flag);
    void updateVolumeLevelByVolume();
signals:
    void muteChanged(bool flag);

};

#endif // CUSTOMVOLUMEWIDGET_H
