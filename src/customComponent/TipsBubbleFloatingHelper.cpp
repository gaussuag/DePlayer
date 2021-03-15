#include "TipsBubbleFloatingHelper.h"

#include <QMovie>
#include <QTimer>
#include <QPointer>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>

Q_GLOBAL_STATIC_WITH_ARGS(TipsBubbleFloatingHelper, instance, (TipsBubbleFloatingHelper::SingletonFlagEnum::SingletonFlag))

TipsBubbleFloatingHelper *TipsBubbleFloatingHelper::GetInstance()
{
    return instance;
}

TipsBubbleFloatingHelper::TipsBubbleFloatingHelper(TipsBubbleFloatingHelper::SingletonFlagEnum flag):QObject(nullptr)
{
    Q_UNUSED(flag);
    initPlainTipsBubble();
}

TipsBubbleFloatingHelper::~TipsBubbleFloatingHelper()
{

}

void TipsBubbleFloatingHelper::showTips(const QString &text, const QRect &referenceWidgetGeometry,
                                        int displayTime, int refreshRate, qreal decreaseOpacity)
{
    ///单例显示标志位，若有消息正在显示，则清除旧消息窗口
    if(DisplayFlag)
        clearTips();

    ///获取tips对象设置text,并显示
    auto tipsObject = plainTextTipsBubbleObject();
    tipsObject->setTips(text);

    tipsObject->show();
    tipsObject->move(TipsBubbleFloatingHelper::centerPointByGeometry(tipsObject->size(),referenceWidgetGeometry));

    ///更新标志位
    DisplayFlag = true;
    ///开启褪色动画
    fadeTips(QPointer<TipsBubbleFloatingWidget>(tipsObject),0,displayTime,refreshRate,decreaseOpacity);
}


void TipsBubbleFloatingHelper::fadeTips(QPointer<TipsBubbleFloatingWidget> tipsPointer,int timeCount,const int timeStamp,const int refreshRate,const qreal decreaseOpacity)
{
    ///动画刷新处理函数
    auto func = [&,timeCount,timeStamp,refreshRate,tipsPointer,decreaseOpacity](){
        ///检查对象指针有效
        if(tipsPointer.isNull())
            return;

        auto tipsObject = tipsPointer.data();

        ///递减窗口透明度
        qreal opacity = tipsObject->windowOpacity();
        if(opacity == decreaseOpacity)
            opacity += decreaseOpacity;
        tipsObject->setWindowOpacity(opacity - decreaseOpacity);

        ///检查动画时间，动画时间结束则清空消息窗口，否则继续执行定时器
        if(timeCount < timeStamp)
            fadeTips(tipsPointer,timeCount+refreshRate,timeStamp,refreshRate,decreaseOpacity);
        else
            clearTips();
    };

    ///根据刷新频率，执行定时器函数
    QTimer::singleShot(refreshRate,func);
}

void TipsBubbleFloatingHelper::clearTips()
{
    if(_tipsBubbleWidget)
    {
        ///清理刷新控件对象
        _tipsBubbleWidget->hide();
        delete _tipsBubbleWidget;
        _tipsBubbleWidget = nullptr;

    }

    ///标志位复位
    DisplayFlag = false;
}

TipsBubbleFloatingHelper::TipsBubbleFloatingHelper(QObject *parent) : QObject(parent)
{

}

TipsBubbleFloatingWidget *TipsBubbleFloatingHelper::plainTextTipsBubbleObject()
{
    if(_tipsBubbleWidget == nullptr)
        initPlainTipsBubble();

    return _tipsBubbleWidget;
}

void TipsBubbleFloatingHelper::initPlainTipsBubble()
{
    if(_tipsBubbleWidget == nullptr)
        _tipsBubbleWidget = new TipsBubbleFloatingWidget;
}


QRect TipsBubbleFloatingHelper::currentScreenRect()
{
    //获取当前鼠标所在屏幕的具体大小
    int screenNumber = QApplication::desktop()->screenNumber(QCursor::pos());
    QScreen* screen = QApplication::screens().at(screenNumber);
    QRect CurrentScreenRect = screen->geometry();

    return CurrentScreenRect;
}

QPoint TipsBubbleFloatingHelper::centerPoint(const QSize &ReferenceSize)
{
    QPoint centerPoint = currentScreenRect().center() - QPoint(ReferenceSize.width() / 2, ReferenceSize.height() / 2);
    return centerPoint;
}

QPoint TipsBubbleFloatingHelper::centerPointByGeometry(const QSize & ReferenceSize, const QRect & globalGeometry)
{
    ///geometry为空，则使用屏幕rect
    if(globalGeometry.isNull())
        return centerPoint(ReferenceSize);

    QPoint Point = globalGeometry.center() - QPoint(ReferenceSize.width() / 2, ReferenceSize.height() / 2);

    return Point;
}










TipsBubbleFloatingWidget::TipsBubbleFloatingWidget(QWidget *parent) : QWidget(parent)
{
    initWidget();
}

TipsBubbleFloatingWidget::~TipsBubbleFloatingWidget()
{

}

void TipsBubbleFloatingWidget::setTips(const QString & text)
{
    if(_tipsLabel)
    {
        _tipsLabel->setText(text);
        _tipsLabel->adjustSize();
    }
}

void TipsBubbleFloatingWidget::initWidget()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    initTipsLabel();
}

void TipsBubbleFloatingWidget::initTipsLabel()
{
    _tipsLabel = new QLabel("INFORMATION",this);
    _tipsLabel->setAlignment(Qt::AlignCenter);
    _tipsLabel->setMinimumSize(400,60);
    _tipsLabel->setStyleSheet("QLabel{"
                  "background-color:#b1b1b1;"
                  "border:2px solid white;"
                  "border-radius:10px;"
                  "font-size:20px;"
                  "color:white;}");


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->addWidget(_tipsLabel);
}

