#ifndef CUSTOMRADIUSSLIDER_H
#define CUSTOMRADIUSSLIDER_H

#include <QWidget>

class CustomRadiusSlider : public QWidget
{
    Q_OBJECT
public:
    explicit CustomRadiusSlider(Qt::Orientation Orientation = Qt::Orientation::Horizontal, QWidget *parent = nullptr);

    void setOrientation(Qt::Orientation Orientation);

    void setRange(int minimum, int);

    void setValue(int value);

    void setSingStep(int step);

    void setWheelControlFlag(bool flag);

    void setValueColor(const QColor & color);

    void setBackgroundColor(const QColor & color);

    int maximum() { return _maximum;}

    int value() { return _currentValue;}
private:
    Qt::Orientation _orientation{Qt::Orientation::Horizontal};

    bool _wheelControlFlag{true};
    bool _mousePressFlag{false};

    int _singleStep{1};

    int _maximum{100};
    int _minimum{0};
    int _currentValue{10};

    QColor _valueColor{Qt::white};
    QColor _backgroundColor{Qt::gray};

    void initWidget();

    void updateValueByMouseWheel(const QPoint &angleDelta);
    void updateValueByMouseLeftButton(const QPoint &pos);

    int getValueBySizeArgument(int SizeArgument, int MouseArgument);
    QRect getValueRectByCurrentValue();
signals:
    void valueChanged(int value);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *event);
};

#endif // CUSTOMRADIUSSLIDER_H
