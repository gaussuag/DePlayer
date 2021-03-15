#ifndef CUSTOVIDEOPLAYERPROGRESSBAR_H
#define CUSTOVIDEOPLAYERPROGRESSBAR_H

#include <QProgressBar>
#include <QMap>
#include <QSet>

class CustomVideoPlayerProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit CustomVideoPlayerProgressBar(QWidget *parent = nullptr);

    /*!
     * \brief setVideoDuration 设置视频总时长列表，支持传入多个视频的总时长，会分段显示每个视频的对应长度
     * \param videoDurationList
     */
    void setVideoDuration(const QMap<int, qreal> &videoDurationMap);

    /*!
     * \brief setVideoDuration 设置视频时长，支持多次传入不同索引值的视频来设置多个视频进度，若index重复则会替换
     * \param duration
     * \param index 视频索引，按照视频索引排序
     */
    void setVideoDuration(qreal duration, int index = 0);

    /*!
     * \brief setVideoMarkPosition 设置视频标记点,需要先调用setVideDuration之后才有效果。
     * \param position 该标记点处于视频的position参数
     * \param index 视频索引值
     */
    void setVideoMarkPosition(qreal position,int index = 0);


    void setPosition(qreal value, int index = 0);

    void setSingStep(int step);

    void setWheelControlFlag(bool flag);

    void setProgressBarHeight(int progressBarHeight);

    void setProgressBarHandleSize(int width,int height);

    void setMarkSize(int width,int height);

    void setSplitBlockSize(int width,int height);

    void setValueColor(const QColor & color);

    void setBackgroundColor(const QColor & color);

    void setProgressBarColor(const QColor & color);

    void setMarkColor(const QColor & color);

    qreal value() { return _currentPositionValue;}

    QPair<int, qreal> splitVideoPosition();

    ///根据当前进度判断是否具有标记点
    bool hasPreviousMark();
    bool hasNextMark();

    /// 清空播放器上的数据
    void clear();
public slots:
    void jumpPreviousMarkPosition();
    void jumpNextMarkPosition();


protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);

    ///处理鼠标左击事件,检查是否点击了标记点区域
    void handleLeftButtonClickedEvent(const QPoint &pos);
    ///处理鼠标悬浮事件，检查是否处于标记点区域
    void handleHoverMouseEvent(const QPoint &pos);


    void paintEvent(QPaintEvent *event);
    void drawSplitRects(QPainter &painter);
    void drawMarkRects(QPainter &painter);

private:
    bool _wheelControlFlag{false};
    bool _mousePressFlag{false};

    int _singleStep{1};

    /// 与当前播放器的播放进度相同,序列播放时会根据索引进行值累积
    qreal _currentPositionValue{0};

    ///进度条区域
    int _progressBarHeight{0};
    int _progressBarHandleWidth{0};
    int _progressBarHandleHeight{0};
    int _progressBarMarkWidth{0};
    int _progressBarMarkHeight{0};
    int _progressBarSplitBlockWidth{0};
    int _progressBarSplitBlockHeight{0};
    QRectF _progressRect;
    ///进度条控制模块区域
    QRectF _progressHandleRect;

    QColor _valueColor{Qt::white};
    QColor _progressBarColor{Qt::gray};
    QColor _backgroundColor{Qt::transparent};
    QColor _markColor{QColor("#E34850")};

    qreal _totalDuration{1000};
    /// 视频总时长列表
    QMap<int,qreal> _videoDurationMap;
    /// 视频时长占比列表
    QMap<int,qreal> _videoDurationPercentMap;
    /// 视频切换分段矩形列表
    QMap<int,QRectF> _videoSplitRectMap;



    /// 视频标记值信息，[key]:标记点索引;[value]:标记点的进度累计值
    QMap<int,qreal> _videoMarkPositionMap;
    /// 视频标记信息列表。[Key]:标记点索引;[Value]:标记矩形区域
    QMap<int,QRectF> _videoMarkIndex_MarkRect_Map;
    QRectF _hoverMarkRect;


    void initWidget();

    void updateValueByMouseWheel(const QPoint &angleDelta);
    void updateValueByMouseLeftButton(const QPoint &pos);

    /*!
     * \brief updateProgressBarByVideoDurationList 根据视频播放进度列表更新计算出各视频时长占比
     */
    void updateProgressBarByVideoDurationList();

    /*!
     * \brief updateVideoMarkPositionPercentSetBySplitVideoIndexAndPosition
     *          根据视频索引值，视频位置来计算获取该标记点位置在总进度的百分占比
     * \param index
     * \param position
     */
    void updateVideoMarkPositionPercentSetBySplitVideoIndexAndPosition(int index, qreal position);

    /// 获取播放进度的累积值在标记点位置列表中的索引值
    /*!
     * \brief getMarkPositionIndexOnByCurrentPosition 查找标记点进度列表，根据当前进度累积值返回其值的下一个标志点索引
     * \param isLeftBoundFlag 左边界查找标志位
     * \return
     */
    int getMarkPositionIndexOnByCurrentPosition(bool isLeftBoundFlag = true);

    /// 根据当前进度累积值计算对应的控件横坐标
    qreal getValuePosXByCurrentValue();
    /// 根据当前进度累积值计算对应的进度值绘制矩形
    QRectF getValueRectByCurrentValue();
    /// 获取视频分割点矩形列表
    QList<QRectF> getVideoSplitRectListByVideoDurationPercentMap();
    /// 获取视频标记矩形列表
    QList<QRectF> getMarkRectListByVideoMarkPercentSet();

    /*!
     * \brief getSplitVideoArgumentByMouseX 根据当前鼠标点击的横坐标参数，计算鼠标位置处于的视频分段索引、\n
     *                                      当前鼠标所在位置处于视频分段中的百分比以及对应的视频播放进度值
     * \param x
     * \param index
     * \param splitVideoPercent
     * \param videoDurationByX
     */
    void getSplitVideoArgumentByMouseX(int x, int &index, qreal &splitVideoPercent, qreal videoDurationByX);

    /*!
     * \brief getSplitVideoArgumentByCurrentValue 根据当前进度累积值，获取对应的视频分段区间内的参数
     * \param index 视频分段索引
     * \param splitVideoPercent 视频分段区间内当前占比
     * \param videoDuration 视频分段区间内当前进度
     */
    void getSplitVideoArgumentByCurrentValue(int & index, qreal &splitVideoPercent, qreal &splitVideoPosition);


    ///更新值，触发valueChanged
    void updatePositionValue(qreal value);
signals:
    void clickedProgressBar();
    void positionChanged(qreal value);
    void sequenceVideoPositionChanged(qreal,int);
    void totalDurationChanged(qreal);
};

#endif // CUSTOVIDEOPLAYERPROGRESSBAR_H
