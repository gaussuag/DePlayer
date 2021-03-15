#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <QWidget>
#include <QEvent>
#include <QList>
#include <QMap>

class QMouseEvent;
class QRubberBand;

class CustomWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CustomWidget(QWidget *parent = nullptr);
    ~CustomWidget() override;


    /*!
     * \brief isEnableRubberBand 获取使能框选模式的标志位
     * \return 框选使能标志位
     */
    bool isEnableRubberBand(){return enableRubberBandFlag;}

    /*!
     * \brief removeWidgetObjectOnRubberMap 移除在Map中的Widget对象
     * \param w
     */
    void removeWidgetObjectOnRubberMap(QWidget *w);
public slots:
    /*!
     * \brief setEnableRubberBand 设置框选模式的使能标志位
     * \param flag [true] : 使能框选模式；[false] : 关闭框选模式
     */
    void setEnableRubberBand(bool flag);


protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


private:
    ///Key Flag
    bool controlKeyFlag{false};
    bool shiftKeyFlag{false};
    ///RubberBand Parameter
    bool enableRubberBandFlag{false};
    QMap<QWidget *,bool> historyOnRegionWidgetMap;
    QMap<QWidget *,bool> currentOnRegionWidgetMap;
    QWidget * _lastClickedItemWidget{nullptr};

    QRubberBand *rubberBand{nullptr};
    QRect rbRect;
    QPoint rbStartPos;


    /*!
     * \brief startDrawRubberBand 开始绘制RubberBand到Widget上。
     * \param Pos 鼠标按下起始位置\RubberBand起始位置
     */
    void startDrawRubberBand(const QPoint &Pos);

    /*!
     * \brief continueDrawRubberBand 更新RubberBand的绘制大小信息。
     * \param Pos 鼠标移动位置\RubberBand结束位置
     */
    void continueDrawRubberBand(const QPoint &Pos);

    /*!
     * \brief endDrawRubberBand 绘制RubberBand结束，进行框选范围Widget信息维护。
     */
    void endDrawRubberBand();

    /*!
     * \brief handleRubberBandRect 处理框选范围内的Widget信息。
     * \details 根据框选产生的一个Rect范围，判断与此框选范围相交的Widget，并根据状态发射对应的响应信号。
     * \param rect RubberBand 的 geometry。
     */
    void handleRubberBandRect(const QRect &rect);

    /*!
     * \brief handleEndRubberBandRegionWidgetMap 框选操作结束，维护在框选范围内的Widget的信息。
     */
    void handleEndRubberBandRegionWidgetMap();

    /*!
     * \brief clearHistoryRegionWidgetMap 没有按下辅助热键时，重新开始框选操作时，触发清除历史框选Widget的记录，并发射Widget离开框选范围的信号。
     */
    void clearHistoryRegionWidgetMap();

    /*!
     * \brief handleLeftButtonClicked 左键单击事件处理函数
     * \param Pos
     * \note 此函数负责处理鼠标左键单击事件，包括辅助热键ctrl和shift的协助操作响应。\n
     *       操作逻辑与windows桌面文件系统操作相类似。\n
     *
     */
    void handleLeftButtonClicked(const QPoint &Pos);

    void handleKeyPressedEvent(QKeyEvent *event);
    void handleKeyReleaseEvent(QKeyEvent *event);


    ///Operation Function
    /*!
     * \brief comparePoint 比较两个点的位置关系，以左上角为0，0点，越靠近0，0越小。
     * \param P1
     * \param P2
     * \return [-1] : P1 < P2 \n
     *         [0]  : P1 = P2 \n
     *         [1]  : P1 > P2 \n
     */
    int comparePoint(const QPoint &P1, const QPoint &P2);
signals:
    void clickedRightButton(const QPoint & globalPoint);

    void clickedLeftButtonUnderWidget(QWidget &);

    void widgetshow(QWidget *);
    void selectWidget(QWidget &);
    void unselectWidget(QWidget &);

};

#endif // CUSTOMWIDGET_H
