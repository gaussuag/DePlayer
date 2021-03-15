#include "CustomRadiusSlider.h"

#include <QtMath>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

#include <QDebug>

CustomRadiusSlider::CustomRadiusSlider(Qt::Orientation Orientation,QWidget *parent)
    : QWidget(parent),_orientation(Orientation)
{
    initWidget();
}

void CustomRadiusSlider::setOrientation(Qt::Orientation Orientation)
{
    _orientation = Orientation;
    update();
}

void CustomRadiusSlider::setRange(int minimum, int maximum)
{
    _minimum = minimum;
    _maximum = maximum;

    update();
}

void CustomRadiusSlider::setValue(int value)
{
    if(value < 0)
        value = 0;
    else if(value > _maximum)
        value = _maximum;

    if(_currentValue == value)
        return;
    _currentValue = value;

    emit(valueChanged(_currentValue));
    update();
}

void CustomRadiusSlider::setSingStep(int step)
{
    _singleStep = step;
}

void CustomRadiusSlider::setWheelControlFlag(bool flag)
{
    _wheelControlFlag = flag;
}

void CustomRadiusSlider::setValueColor(const QColor &color)
{
    _valueColor = color;
    update();
}

void CustomRadiusSlider::setBackgroundColor(const QColor &color)
{
    _backgroundColor = color;
    update();
}

void CustomRadiusSlider::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        auto MousePressPos = event->pos();

        if(rect().contains(MousePressPos))
        {
            _mousePressFlag = true;
            updateValueByMouseLeftButton(MousePressPos);
        }
    }
    return QWidget::mousePressEvent(event);
}

void CustomRadiusSlider::mouseMoveEvent(QMouseEvent *event)
{
    if(_mousePressFlag)
    {
        auto MousePos = event->pos();
        updateValueByMouseLeftButton(MousePos);
    }

    return QWidget::mouseMoveEvent(event);
}

void CustomRadiusSlider::mouseReleaseEvent(QMouseEvent *event)
{
    if(_mousePressFlag)
        _mousePressFlag = false;

    return QWidget::mouseReleaseEvent(event);
}

void CustomRadiusSlider::wheelEvent(QWheelEvent *event)
{
    if(_wheelControlFlag)
        updateValueByMouseWheel(event->angleDelta());
    return QWidget::wheelEvent(event);
}

void CustomRadiusSlider::initWidget()
{
    setMouseTracking(true);
}

void CustomRadiusSlider::updateValueByMouseWheel(const QPoint & angleDelta)
{
    if(angleDelta.y() > 0)
        setValue(_currentValue + _singleStep);
    else
        setValue(_currentValue - _singleStep);
}

void CustomRadiusSlider::updateValueByMouseLeftButton(const QPoint & pos)
{
    ///根据当前显示模式，计算鼠标点击的位置参数。
    /// note:若为垂直，根据绘制坐标体系关系，鼠标当前点击的坐标点的y值，越大意味着当前值越小，所以需要用height()减取y值取出正相关参数。
    int mouseArgument = (_orientation == Qt::Orientation::Horizontal) ? pos.x() : (height() - pos.y());
    int sizeArgument = (_orientation == Qt::Orientation::Horizontal) ? width() : height();

    ///超出控件范围检查
    if(mouseArgument > sizeArgument)
        mouseArgument = sizeArgument;
    else if(mouseArgument < 0)
        mouseArgument = 0;

    ///根据鼠标点击位置，更新当前值。
    setValue(getValueBySizeArgument(sizeArgument,mouseArgument));

    update();
}

int CustomRadiusSlider::getValueBySizeArgument(int SizeArgument,int MouseArgument)
{
    int maxValue = _maximum;
    /// SizeArgument为根据显示模式获取的控件大小参考值
    /// MouseArguMent为根据显示模式获取的鼠标点击的坐标参考值。
    /// 计算鼠标点击位置在控件中的位置百分数
    double percentValue = MouseArgument*1.00 / SizeArgument;
    /// 与最大值相乘获取当前鼠标点击位置对应的值。
    return percentValue * maxValue;
}

QRect CustomRadiusSlider::getValueRectByCurrentValue()
{
    auto Rect = rect();
    ///计算 value rect
    /// 计算当前值在总值的百分比
    double percentValue = _currentValue*1.00 / _maximum;
    ///根据显示模式，获取尺寸参数
    int SizeArgument = ((_orientation == Qt::Orientation::Horizontal) ? Rect.width() : Rect.height());
    ///根据值的百分比，计算出值区域的百分比
    int sizeValue =  percentValue * SizeArgument;

    ///按照不同模式，返回对应的Value Rect
    /// note: 若当前为矩形，则遵循从左上角开始绘制的逻辑，valueRect的起点坐标的y值应是height()减取sizeValue。
    if(_orientation == Qt::Orientation::Horizontal)
        return QRect(0,0,sizeValue,height());
    else
        return QRect(0,height() - sizeValue,width(),sizeValue);
}

void CustomRadiusSlider::paintEvent(QPaintEvent *event)
{
    /*!
        此绘制函数会规避圆角矩形两端超限变成直角的问题。
        以下讲解为水平滑动条的例子。
           * * * * * * * * * * * * *
         *   *         *            *
        *(1) *  (2)     *        (3) *
        *(1) *  (2)     *        (3) *
        *(1) *  (2)     *        (3) *
         *   *         *            *
           * * * * * * * * * * * * *
         (1)(2)区间为ValueRect区域，(1)(2)(3)为整体控件区域。
         1.
         我们使用控件本身的矩形大小，再加上圆角半径来作为裁剪矩形，clipPath中添加RoundedRect作为裁剪参数。
         2.
         首先绘制背景色的圆角矩形，大小为当前控件的rect()。
         3.
         通过当前的Value换算出当前ValueRect的大小，并绘制成相同半径的圆角矩形。

         由于设置了clipPath参数，所以当value比较小时，圆角矩形超出背景圆角矩形的部分会被clipPath裁剪。

    */

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::transparent);

    QRect Rect = rect();

    ///根据显示类型计算偏移参数
    int offsetValue = (_orientation == Qt::Orientation::Horizontal) ? Rect.height() : Rect.width();
    ///圆角半径
    qreal radius = offsetValue/2;

    ///裁剪矩形，用底部的圆角矩形作为裁剪参数
    QPainterPath path;
    path.addRoundedRect(Rect,radius,radius);
    painter.setClipPath(path);

    ///painter background
    painter.setBrush(_backgroundColor);
    painter.drawRoundedRect(Rect,radius,radius);

    ///根据当前的进度值来获取valueRect区域
    auto valueRect = getValueRectByCurrentValue();

    painter.setBrush(_valueColor);
    painter.drawRoundedRect(valueRect,radius,radius);

    return QWidget::paintEvent(event);
}
