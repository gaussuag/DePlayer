#ifndef TIPSBUBBLEFLOATINGHELPER_H
#define TIPSBUBBLEFLOATINGHELPER_H

#include "CustomFramelessDialog.h"

#define TipsHelper TipsBubbleFloatingHelper::GetInstance()

class QLabel;
class TipsBubbleFloatingWidget;
class AnimationTipsFloatingWidget;
class QMovie;


class TipsBubbleFloatingHelper : public QObject
{
public:
    static TipsBubbleFloatingHelper* GetInstance();
    ///单例类初始化相关函数
    enum SingletonFlagEnum{SingletonFlag};
    TipsBubbleFloatingHelper(SingletonFlagEnum flag);
    ~TipsBubbleFloatingHelper();

    /*!
     * \brief showTips 设置一段文字显示在消息提示窗口中
     * \param text 消息文本
     * \param referenceWidgetGeometry 参考控件的Geometry，传递此参数来使得Tips在控件居中显示，不传递则默认在屏幕居中
     * \param displayTime 显示的时间
     * \param refreshRate 刷新频率
     * \param decreaseOpacity 单次褪色递减值
     * \attention
     *        传入参数列表中，displayTime、refreshRate和decreaseOpacity这三者之间有一定的数学关系。\n
     *        要求传入的参数满足 displayTime / refreshRate * decreaseOpacity <= 1 \n
     *        使用默认参数计算，即意味每隔41ms便会刷新一次控件透明度，单次刷新递减0.05，在700ms内会刷新17次，透明度递减总量为0.85。\n
     *        意味着会从透明度会1递减到0.15，接近透明后消失。
     *
     */
    void showTips(const QString &text,const QRect & referenceWidgetGeometry = QRect(),
                  int displayTime = 2000,int refreshRate = 80,qreal decreaseOpacity = 0.04);






    static QPoint centerPoint(const QSize &ReferenceSize);
    static QRect currentScreenRect();
    static QPoint centerPointByGeometry(const QSize &ReferenceSize, const QRect &globalGeometry);
private:
    TipsBubbleFloatingWidget *_tipsBubbleWidget{nullptr};
    explicit TipsBubbleFloatingHelper(QObject *parent = nullptr);

    TipsBubbleFloatingWidget *plainTextTipsBubbleObject();

    volatile bool DisplayFlag{false};

    void initPlainTipsBubble();

    /*!
     * \brief fadeTips 褪色动画操作函数
     * \param tipsPointer
     * \param timeCount
     * \param timeStamp
     * \param refreshRate
     */
    void fadeTips(QPointer<TipsBubbleFloatingWidget> tipsPointer,
                  int timeCount, const int timeStamp, const int refreshRate, const qreal decreaseOpacity);

    /*!
     * \brief clearTips 清理函数
     */
    void clearTips();
};


class TipsBubbleFloatingWidget : public QWidget
{
public:
    TipsBubbleFloatingWidget(QWidget *parent = nullptr);
    ~TipsBubbleFloatingWidget();


    void setTips(const QString &text);
private:
    QLabel *_tipsLabel{nullptr};

    void initWidget();
    void initTipsLabel();
};


#endif // TIPSBUBBLEFLOATINGHELPER_H
