#include "CustomVolumeWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
CustomVolumeWidget::CustomVolumeWidget(QWidget *parent) : QWidget(parent)
{
}

void CustomVolumeWidget::setColor(const QColor &color)
{
    _mainColor = color;
    update();
}

void CustomVolumeWidget::setMuteColor(const QColor &color)
{
    _muteColor = color;
    update();
}

void CustomVolumeWidget::setDisableColor(const QColor &color)
{
    _disableColor = color;
    update();
}

void CustomVolumeWidget::setVolumeValue(int Volume)
{
    if(Volume < 0)
        Volume = 0;
    else if(Volume > _maximum)
        Volume = _maximum;

    _volume = Volume;

    updateVolumeLevelByVolume();
    update();
}

void CustomVolumeWidget::setMute(bool flag)
{
    _isMuteFlag = flag;
    if(_isMuteFlag)
        _volumeLevel = CustomVolumeWidget::VolumeLevel::mute;
    else
        updateVolumeLevelByVolume();

    update();
}

void CustomVolumeWidget::setRange(int Minimum, int Maximum)
{
    _minimum = Minimum;
    _maximum = Maximum;

    updateVolumeLevelByVolume();

    update();
}

void CustomVolumeWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        updateMuteFlag(!_isMuteFlag);

    return QWidget::mouseReleaseEvent(event);
}

void CustomVolumeWidget::updateVolumeLevelByVolume()
{
    qreal oneLevelValue = _maximum/3;
    if(_volume == 0)
        _volumeLevel = CustomVolumeWidget::VolumeLevel::mute;
    else if(_volume > 0 && _volume < oneLevelValue)
        _volumeLevel = CustomVolumeWidget::VolumeLevel::level1;
    else if(_volume >= oneLevelValue && _volume < oneLevelValue * 2)
        _volumeLevel = CustomVolumeWidget::VolumeLevel::level2;
    else if(_volume >= oneLevelValue*2 && _volume <= _maximum)
        _volumeLevel = CustomVolumeWidget::VolumeLevel::level3;
}

void CustomVolumeWidget::updateMuteFlag(bool flag)
{
    setMute(flag);
    emit(muteChanged(_isMuteFlag));
}

void CustomVolumeWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect Rect = rect();
    qreal middle = Rect.width() / 8.0;
    QRect LeftRect = QRect(1,0,Rect.width()/2 - middle/2,Rect.height());
    QRect RightRect = QRect(Rect.width()/2 + middle/2,0,LeftRect.width(),Rect.height());

    QPen Pen;
    Pen.setColor(_mainColor);
    Pen.setWidth(2);
    Pen.setCapStyle(Qt::RoundCap);
    Pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(Pen);

    QPolygon Polygon_Left;
    Polygon_Left.append(QPoint(1,LeftRect.height()/3));
    Polygon_Left.append(QPoint(LeftRect.width()/2+LeftRect.width()/6,LeftRect.height()/3));

    Polygon_Left.append(QPoint(LeftRect.width(),LeftRect.height()/6));
    Polygon_Left.append(QPoint(LeftRect.width(),LeftRect.height()*5/6));

    Polygon_Left.append(QPoint(LeftRect.width()/2+LeftRect.width()/6,LeftRect.height()*2/3));
    Polygon_Left.append(QPoint(1,LeftRect.height()*2/3));

    Polygon_Left.append(QPoint(1,LeftRect.height()/3));

    painter.drawPolygon(Polygon_Left);

    if(_volumeLevel == CustomVolumeWidget::VolumeLevel::mute)
    {
        int Cx = RightRect.center().x();
        int Cy = RightRect.center().y();
        int adValue = RightRect.width()/3;
        auto Pen = painter.pen();
        Pen.setWidth(3);
        Pen.setColor(_muteColor);
        painter.setPen(Pen);

        QRect XRect = QRect(Cx-adValue,Cy-adValue,adValue*2,adValue*2);
        painter.drawLines(QVector<QLine>({QLine(XRect.topLeft(),XRect.bottomRight()),QLine(XRect.bottomLeft(),XRect.topRight())}));
    }

    if(_volumeLevel >= CustomVolumeWidget::VolumeLevel::level1)
        painter.drawArc(RightRect.x(),RightRect.height()/2 - RightRect.height()/12,
                    RightRect.width()/4,RightRect.height()/6,90*16,-180*16);

    if(_volumeLevel >= CustomVolumeWidget::VolumeLevel::level2)
        painter.drawArc(RightRect.x(),RightRect.height()/2 - RightRect.height()/4,
                    RightRect.width()/2,RightRect.height()/2,90*16,-180*16);

    if(_volumeLevel >= CustomVolumeWidget::VolumeLevel::level3)
        painter.drawArc(RightRect.x(),RightRect.height()/12,
                    RightRect.width() - RightRect.width()/4,RightRect.height()*5/6,90*16,-180*16);

    return QWidget::paintEvent(event);
}
