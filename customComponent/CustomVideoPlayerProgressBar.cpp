#include "CustomVideoPlayerProgressBar.h"

#include <QtMath>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

#include <QDebug>

CustomVideoPlayerProgressBar::CustomVideoPlayerProgressBar(QWidget *parent)
    : QWidget(parent)
{
    initWidget();
}


/// 设置视频进度
void CustomVideoPlayerProgressBar::setVideoDuration(const QMap<int,qreal> &videoDurationMap)
{
    _videoDurationMap.clear();
    _videoDurationMap = videoDurationMap;

    updateProgressBarByVideoDurationList();

    update();
}
void CustomVideoPlayerProgressBar::setVideoDuration(qreal duration,int index)
{
    _videoDurationMap.insert(index,duration);

    updateProgressBarByVideoDurationList();

    update();
}

/// 根据视频索引以及视频对应的position，设置一个mark
void CustomVideoPlayerProgressBar::setVideoMarkPosition(qreal position, int index)
{
    if(!_videoDurationMap.contains(index) || !_videoDurationPercentMap.contains(index))
        return;

    updateVideoMarkPositionPercentSetBySplitVideoIndexAndPosition(index,position);

    update();
}

void CustomVideoPlayerProgressBar::setPosition(qreal value,int index)
{
    QMap<int,qreal>::iterator it;
    for(it = _videoDurationMap.begin();it != _videoDurationMap.end();++it)
    {
        if(it.key() < index)
            value += it.value();
    }
    _currentPositionValue = value;
    update();
}

void CustomVideoPlayerProgressBar::setSingStep(int step)
{
    _singleStep = step;
}

void CustomVideoPlayerProgressBar::setWheelControlFlag(bool flag)
{
    _wheelControlFlag = flag;
}

void CustomVideoPlayerProgressBar::setProgressBarHeight(int progressBarHeight)
{
    _progressBarHeight = progressBarHeight;
    update();
}

void CustomVideoPlayerProgressBar::setProgressBarHandleSize(int width, int height)
{
    _progressBarHandleWidth = width;
    _progressBarHandleHeight = height;

    update();
}

void CustomVideoPlayerProgressBar::setMarkSize(int width, int height)
{
    _progressBarMarkWidth = width;
    _progressBarMarkHeight = height;

    update();
}

void CustomVideoPlayerProgressBar::setSplitBlockSize(int width, int height)
{
    _progressBarSplitBlockWidth = width;
    _progressBarSplitBlockHeight = height;

    update();
}

void CustomVideoPlayerProgressBar::setValueColor(const QColor &color)
{
    _valueColor = color;
    update();
}

void CustomVideoPlayerProgressBar::setBackgroundColor(const QColor &color)
{
    _backgroundColor = color;
    update();
}

void CustomVideoPlayerProgressBar::setProgressBarColor(const QColor &color)
{
    _progressBarColor = color;
    update();
}

void CustomVideoPlayerProgressBar::setMarkColor(const QColor &color)
{
    _markColor = color;
    update();
}

QPair<int, qreal> CustomVideoPlayerProgressBar::splitVideoPosition()
{
    int index = 0;
    qreal percent = 0;
    qreal position = 0;
    getSplitVideoArgumentByCurrentValue(index,percent,position);

    return qMakePair(index,position);
}


///根据当前播放进度值，向前跳转到最近的一个标记点位置
void CustomVideoPlayerProgressBar::jumpPreviousMarkPosition()
{
    int index = getMarkPositionIndexOnByCurrentPosition();

    ///当前位置处于第一个的位置，前面没有标记点可用
    if(index == 0)
        return;

    updatePositionValue(_videoMarkPositionMap.value(index-1));
}

///根据当前播放进度值，向后跳转到最近的一个标记点位置
void CustomVideoPlayerProgressBar::jumpNextMarkPosition()
{
    int index = getMarkPositionIndexOnByCurrentPosition(false);

    ///索引超出列表区间，意味着后面没有标记点
    if(index == _videoMarkIndex_MarkRect_Map.size())
        return;

    updatePositionValue(_videoMarkPositionMap.value(index));
}

bool CustomVideoPlayerProgressBar::hasPreviousMark()
{
    int indexByCurrentValue = getMarkPositionIndexOnByCurrentPosition();
    ///当前位置处于第一个的位置，前面没有标记点可用
    if(indexByCurrentValue == 0)
        return false;

    return true;
}

bool CustomVideoPlayerProgressBar::hasNextMark()
{
    int indexByCurrentValue = getMarkPositionIndexOnByCurrentPosition(false);
    ///索引超出列表区间，意味着后面没有标记点
    if(indexByCurrentValue == _videoMarkIndex_MarkRect_Map.size())
        return false;

    return true;
}

void CustomVideoPlayerProgressBar::clear()
{
    _videoDurationMap.clear();
    _videoDurationPercentMap.clear();
    _videoMarkPositionMap.clear();
    _videoMarkIndex_MarkRect_Map.clear();
}

int CustomVideoPlayerProgressBar::getMarkPositionIndexOnByCurrentPosition(bool isLeftBoundFlag)
{
    auto referencePosition = _currentPositionValue;
    ///标记点累积值数据
    auto MarkPositions = _videoMarkPositionMap.values();
    if(MarkPositions.size() == 0)
        return 0;

    int leftIndex = 0;
    int rightIndex = MarkPositions.size();

    if(isLeftBoundFlag)
    {
        ///二分查找左边界索引
        while (leftIndex < rightIndex) {
            int mid = (leftIndex+rightIndex)/2;

            if(referencePosition <= MarkPositions.at(mid))
                rightIndex = mid;
            else if(referencePosition > MarkPositions.at(mid))
                leftIndex = mid + 1;
        }
    }else
    {
        ///二分查找右边界索引
        while (leftIndex < rightIndex) {
            int mid = (leftIndex+rightIndex)/2;

            if(referencePosition >= MarkPositions.at(mid))
                leftIndex = mid + 1;
            else if(referencePosition < MarkPositions.at(mid))
                rightIndex = mid;
        }
    }
   return leftIndex;
}

void CustomVideoPlayerProgressBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        auto MousePressPos = event->pos();

        if(_progressRect.contains(MousePressPos))
        {
            _mousePressFlag = true;
            updateValueByMouseLeftButton(MousePressPos);

            emit(clickedProgressBar());
        }
    }
    return QWidget::mousePressEvent(event);
}

void CustomVideoPlayerProgressBar::mouseMoveEvent(QMouseEvent *event)
{
    if(_mousePressFlag)
    {
        auto MousePos = event->pos();
        ///根据鼠标控制来设置当前值信息
        updateValueByMouseLeftButton(MousePos);
    }else
        handleHoverMouseEvent(event->pos());


    return QWidget::mouseMoveEvent(event);
}

void CustomVideoPlayerProgressBar::mouseReleaseEvent(QMouseEvent *event)
{
    if(_mousePressFlag)
        _mousePressFlag = false;

    if(event->button() == Qt::LeftButton)
        handleLeftButtonClickedEvent(event->pos());


    return QWidget::mouseReleaseEvent(event);
}

void CustomVideoPlayerProgressBar::wheelEvent(QWheelEvent *event)
{
    if(_wheelControlFlag)
        updateValueByMouseWheel(event->angleDelta());
    return QWidget::wheelEvent(event);
}

///处理鼠标左击事件，判断是否点击了标记区域
void CustomVideoPlayerProgressBar::handleLeftButtonClickedEvent(const QPoint & pos)
{
    QMap<int,QRectF>::iterator it;
    for(it = _videoMarkIndex_MarkRect_Map.begin();it != _videoMarkIndex_MarkRect_Map.end();++it)
    {
        if(it.value().contains(pos))
        {
            updatePositionValue(_videoMarkPositionMap.value(it.key()));
            break;
        }
    }
}

///处理鼠标悬浮事件
void CustomVideoPlayerProgressBar::handleHoverMouseEvent(const QPoint & pos)
{
    auto markRects = _videoMarkIndex_MarkRect_Map.values();
    for(const auto & markRect : markRects)
    {
        if(markRect.contains(pos))
        {
            _hoverMarkRect = markRect;
            update();
            return;
        }
    }

    _hoverMarkRect = QRect(0,0,0,0);
    update();
}

void CustomVideoPlayerProgressBar::initWidget()
{
    setMinimumSize(100,10);
    setMouseTracking(true);
}

///更新当前值参数，触发valueChanged,splitVideoPositionChanged信号
void CustomVideoPlayerProgressBar::updatePositionValue(qreal value)
{
    if(value < 0)
        value = 0;
    else if(value > _totalDuration)
        value = _totalDuration;


    if(_currentPositionValue == value)
        return;

    setPosition(value);
    emit(positionChanged(_currentPositionValue));

    int index = 0;
    qreal splitVideoPercent = 0;
    qreal splitVideoPosition = 0;

    ///根据当前值来获取当前视频分段区间内参数
    getSplitVideoArgumentByCurrentValue(index,splitVideoPercent,splitVideoPosition);
    emit(sequenceVideoPositionChanged(splitVideoPosition,index));
}

///updateProgressBarByVideoDurationList 根据视频播放进度列表更新计算出各视频时长占比
void CustomVideoPlayerProgressBar::updateProgressBarByVideoDurationList()
{
    _videoDurationPercentMap.clear();

    auto videoDurationList = _videoDurationMap.values();

    ///求和
    qreal totalDuration = 0;
    for(const auto & duration : videoDurationList)
        totalDuration += duration;

    _totalDuration = totalDuration;

    QMap<int,qreal>::iterator it;
    for(it = _videoDurationMap.begin();it != _videoDurationMap.end(); ++it)
        _videoDurationPercentMap.insert(it.key(),it.value()*1.00/totalDuration);

    emit(totalDurationChanged(_totalDuration));
}

///根据视频索引值，视频位置来计算获取该标记点位置在总进度的百分占比
void CustomVideoPlayerProgressBar::updateVideoMarkPositionPercentSetBySplitVideoIndexAndPosition(int index, qreal position)
{
    qDebug()<<"set Mark posistion"<<index<<position;
    ///过滤超限标志点
    auto videoDuration = _videoDurationMap.value(index);
    if(position > videoDuration)
        return;

    qreal accumulationValue = position;
    ///计算累积进度值
    QMap<int,qreal>::iterator it;
    for(it = _videoDurationPercentMap.begin();it!=_videoDurationPercentMap.end();++it)
    {
        if(it.key() < index)
            accumulationValue += _videoDurationMap.value(it.key());
        else if(it.key() == index)
            break;
    }

    /// 标记点索引值与其对应的进度累积值，用于跳转定位
    /// 去重处理
    if(!_videoMarkPositionMap.values().contains(accumulationValue))
        _videoMarkPositionMap.insert(_videoMarkPositionMap.size(),accumulationValue);
}




///通过鼠标滚轮来控制当前值
void CustomVideoPlayerProgressBar::updateValueByMouseWheel(const QPoint &angleDelta)
{
    if(angleDelta.y() > 0)
        updatePositionValue(_currentPositionValue + _singleStep);
    else
        updatePositionValue(_currentPositionValue - _singleStep);
}

///通过鼠标的位置坐标来控制当前值
void CustomVideoPlayerProgressBar::updateValueByMouseLeftButton(const QPoint &pos)
{
    ///根据当前显示模式，计算鼠标点击的位置参数。
    /// SizeArgument为根据显示模式获取的控件大小参考值
    int sizeArgument =  _progressRect.width();

    /// MouseArguMent为根据显示模式获取的鼠标点击的坐标参考值。
    int mouseArgument = pos.x();

    ///超出控件范围检查
    if(mouseArgument > sizeArgument)
        mouseArgument = sizeArgument;
    else if(mouseArgument < 0)
        mouseArgument = 0;

    ///根据鼠标点击位置，更新当前值。

    /// 计算鼠标点击位置在控件中的位置百分数
    qreal percentValue = mouseArgument / (sizeArgument*1.00);
    /// 与最大值相乘获取当前鼠标点击位置对应的值。
    qreal newValue = qRound(percentValue * _totalDuration);

    ///更新值信息
    updatePositionValue(newValue);
}

///根据当前值信息，换算出该值对应的控件横坐标位置
qreal CustomVideoPlayerProgressBar::getValuePosXByCurrentValue()
{
    auto Rect = _progressRect;
    ///计算 value rect
    /// 计算当前值在总值的百分比
    qreal percentValue = _currentPositionValue*1.00 / _totalDuration;
    ///根据显示模式，获取尺寸参数
    qreal SizeArgument = Rect.width();
    ///根据值的百分比，计算出值区域的百分比
    qreal valuePosX =  percentValue * SizeArgument;

    return valuePosX;
}

///根据当前值计算出值区域的绘制矩形
QRectF CustomVideoPlayerProgressBar::getValueRectByCurrentValue()
{
    qreal valuePosX = getValuePosXByCurrentValue();
    return QRectF(_progressRect.x(),_progressRect.y(),valuePosX,_progressRect.height());
}

///根据各个视频的时长占比，计算出绘制视频分割点的小矩形列表
QList<QRectF> CustomVideoPlayerProgressBar::getVideoSplitRectListByVideoDurationPercentMap()
{
    auto percentList = _videoDurationPercentMap.values();
    if(percentList.isEmpty())
        return QList<QRectF>();

    percentList.pop_back();

    QRectF Rect = _progressRect;

    if(_progressBarSplitBlockHeight == 0)
        _progressBarSplitBlockHeight = Rect.height()*2;

    if(_progressBarSplitBlockWidth == 0)
        _progressBarSplitBlockWidth = 5;

    QList<QRectF> rectList;
    qreal accumulationPercent = 0;

    for(const auto & percent : percentList)
    {
        qreal currentX = (accumulationPercent + percent) * Rect.width();
        rectList.append(QRectF(currentX,(Rect.y()- (_progressBarSplitBlockHeight - Rect.height())/2),
                               _progressBarSplitBlockWidth,_progressBarSplitBlockHeight));
        accumulationPercent += percent;
    }

    return rectList;
}
///根据视频标记点占比，计算出绘制视频标志点的小矩形列表
QList<QRectF> CustomVideoPlayerProgressBar::getMarkRectListByVideoMarkPercentSet()
{
    _videoMarkIndex_MarkRect_Map.clear();

    /// 根据累积进度值，求出累积百分比
    QList<QRectF> markRects;


    if(_progressBarMarkHeight == 0)
        _progressBarMarkHeight = _progressRect.height();

    if(_progressBarMarkWidth == 0)
        _progressBarMarkWidth = 5;

    QMap<int,qreal>::iterator it;
    for(it = _videoMarkPositionMap.begin();it != _videoMarkPositionMap.end();++it)
    {
        auto markPercent = it.value() * 1.00 / _totalDuration;
        qreal position = markPercent * _progressRect.width();
        QRectF markRect = QRectF(position - _progressBarMarkWidth,rect().height() - _progressBarMarkHeight,
                               _progressBarMarkWidth,_progressBarMarkHeight);
        markRects.append(markRect);
        _videoMarkIndex_MarkRect_Map.insert(it.key(),markRect);
    }

    return markRects;
}

void CustomVideoPlayerProgressBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::transparent);

    QRect Rect = rect();

    if(_progressBarHeight == 0)
        _progressBarHeight = Rect.height()/6.0;

    _progressRect = QRectF(0,Rect.height()/2.0-_progressBarHeight/2.0,
                          Rect.width(),_progressBarHeight);

    if(_progressBarHandleWidth == 0)
        _progressBarHandleWidth = 8;
    if(_progressBarHandleHeight == 0)
        _progressBarHandleHeight = _progressBarHeight * 2.0;

    ///根据显示类型计算偏移参数
    int offsetValue = _progressRect.height();
    ///圆角半径
    qreal radius = offsetValue/2;


    ///painter background
    painter.setBrush(_backgroundColor);
    painter.drawRect(Rect);

    painter.setBrush(_progressBarColor);
    painter.drawRoundedRect(_progressRect,radius,radius);

    /// 绘制视频分割点矩形
    drawSplitRects(painter);

    /// 绘制视频标记矩形
    drawMarkRects(painter);

    ///根据当前的进度值来获取valueRect区域
    auto valueRect = getValueRectByCurrentValue();

    _progressHandleRect = QRectF(valueRect.right()-_progressBarHandleWidth,
                                 (_progressRect.y() - (_progressBarHandleHeight - _progressBarHeight)/2.0),
                                _progressBarHandleWidth,_progressBarHandleHeight);

    painter.setBrush(_valueColor);
    painter.drawRoundedRect(valueRect,radius,radius);
    painter.drawRect(_progressHandleRect);


   // qDebug()<<"update"<<_currentPositionValue<<_totalDuration<<valueRect;

    return QWidget::paintEvent(event);
}

void CustomVideoPlayerProgressBar::drawSplitRects(QPainter & painter)
{
    QList<QRectF> videoSplitRectList = getVideoSplitRectListByVideoDurationPercentMap();
    painter.setBrush(_progressBarColor);
    for(const auto & splitRect : videoSplitRectList)
        painter.drawRect(splitRect);
}

void CustomVideoPlayerProgressBar::drawMarkRects(QPainter & painter)
{
    auto markRects = getMarkRectListByVideoMarkPercentSet();
    painter.setBrush(_markColor);
    for(const auto & markRect : markRects)
    {
        ///hover
        if(markRect == _hoverMarkRect)
        {
            painter.setBrush(_markColor.dark(150));
            painter.drawRect(markRect);
            painter.setBrush(_markColor);
        }else
            painter.drawRect(markRect);
    }
}

///根据当前鼠标在进度条上x坐标，换算出当前鼠标点位置对应哪个分段区间
void CustomVideoPlayerProgressBar::getSplitVideoArgumentByMouseX(int x,int & index,qreal & splitVideoPercent,qreal videoDurationByX)
{
    ///参数超限检查
    if(x > _progressRect.width())
        x = _progressRect.width();
    else if( x < 0)
        x = 0;

    ///X坐标在进度条上的百分比
    qreal xPercent = x * 1.00/ _progressRect.width();

    ///遍历视频时长占比，通过递减X坐标占比，找到对应的分段视频索引
    QMap<int,qreal>::iterator it;
    for(it = _videoDurationPercentMap.begin();it != _videoDurationPercentMap.end();++it)
    {
        ///若X坐标占比小于当前索引指向的视频百分比，则属于此视频区间
        if(xPercent <= it.value())
        {
            ///视频分段索引
            index = it.key();

            /// 计算出当前鼠标位置在此分段视频区间中的百分比：
            /// 由于x会递减掉不存在的区间，x的值会随着遍历而减小。先根据当前视频的百分比计算出在控件上的占比宽度，再由X求出当前X坐标的占比
            splitVideoPercent = x/(it.value()*_progressRect.width());

            ///根据当前X坐标在此分段视频区间的占比，计算出此分段视频区间的进度值。
            videoDurationByX = splitVideoPercent*_videoDurationMap.value(it.key());

            return;
        }
        ///减去不符合的前缀参数
        xPercent -= it.value();
        x -= it.value()*_progressRect.width();
    }
}

void CustomVideoPlayerProgressBar::getSplitVideoArgumentByCurrentValue(int &index,
                                                                         qreal &splitVideoPercent, qreal & splitVideoPosition)
{
    auto currentValue = _currentPositionValue;
    /// 根据当前值，换算占最大值的百分比
    qreal valuePercent = currentValue * 1.00/ _totalDuration;

    ///终点位置，检查数据正确并返回最末尾的参数
    if(valuePercent == 1 && _videoDurationMap.count() > 0)
    {
        index = _videoDurationMap.lastKey();
        splitVideoPercent = 1;
        splitVideoPosition = _videoDurationMap.last();
        return;
    }

    ///遍历视频时长占比，通过递减百分占比，找到对应的分段视频索引
    QMap<int,qreal>::iterator it;
    for(it = _videoDurationPercentMap.begin();it != _videoDurationPercentMap.end();++it)
    {
        ///若X坐标占比小于当前索引指向的视频百分比，则属于此视频区间
        if(valuePercent <= it.value())
        {
            ///视频分段索引
            index = it.key();

            /// 当前视频分段区间内的值占比
            splitVideoPercent = currentValue/(it.value()*_totalDuration);
            ///视频分段区间内的视频进度
            splitVideoPosition = splitVideoPercent*_videoDurationMap.value(it.key());
            return;
        }
        ///减去不符合的前缀参数
        valuePercent -= it.value();
        currentValue -= it.value()*_totalDuration;
    }
}
