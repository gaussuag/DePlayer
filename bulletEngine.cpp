#include "bulletEngine.h"

#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>

#include <QTimer>
#include <QPropertyAnimation>
#include <QDebug>

#include "MpvPlayerWidget.h"

bulletEngine::bulletEngine(QObject *parent) : QObject(parent)
{

}

bulletEngine::~bulletEngine()
{

}

void bulletEngine::setPlayer(MpvPlayerWidget *player)
{
    _player = player;

    _color = Qt::white;
    _font.setPointSize(24);
    _font.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));

    initCanvas();
}

bool bulletEngine::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == _view)
    {
        if(event->type() == QEvent::Resize)
            _scene->setSceneRect(_view->rect());
    }

    return QObject::eventFilter(watched,event);
}

void bulletEngine::initCanvas()
{
    QVBoxLayout *mpvLayout = new QVBoxLayout(_player);
    mpvLayout->setMargin(0);
    mpvLayout->setSpacing(0);

    _view = new QGraphicsView(_player);
    _view->installEventFilter(this);
    _view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _view->setAttribute(Qt::WA_TranslucentBackground);
    _view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    _view->setStyleSheet("background-color:transparent;border:0px;");

    _scene = new QGraphicsScene(this);
    _view->setScene(_scene);

    mpvLayout->addWidget(_view);
}

void bulletEngine::addBullet(const QString & text)
{
    if(_player->isPlaying())
    {
        auto frame = _player->getFrameNumber();
        bulletTextItem *bulletItem = new bulletTextItem(text,frame);
        bulletItem->setFont(_font);
        bulletItem->setDefaultTextColor(_color);
        connect(bulletItem,&bulletTextItem::bulletAnimationStart,this,&bulletEngine::bulletAnimationStart_slot);
        if(_scene)
            _scene->addItem(bulletItem);
        bulletItem->startAnimation();

        addBulletItemToMap(frame,bulletItem);
    }
}

void bulletEngine::addBulletItemToMap(int frame,bulletTextItem * item)
{
    if(!_bulletMap.contains(frame))
    {
        QList<bulletTextItem*> list;
        _bulletMap.insert(frame,list);
    }
    _bulletMap[frame].append(item);
}

void bulletEngine::synchronizingVideoPosition(int frame)
{
    /// check bullet Map,enable animation
    checkBulletMapToStartAnimation(frame);
}

void bulletEngine::setPlaying()
{
    if(_activedAnimationList.isEmpty())
        return;

    for(auto & animationObj : _activedAnimationList)
    {
        if(!animationObj)
            continue;

        if(animationObj->state() == QPropertyAnimation::State::Paused)
            animationObj->resume();
    }
}

void bulletEngine::setPause()
{
    if(_activedAnimationList.isEmpty())
        return;

    for(auto & animationObj : _activedAnimationList)
    {
        if(!animationObj)
            continue;
        if(animationObj->state() == QPropertyAnimation::State::Running)
            animationObj->pause();
    }
}

void bulletEngine::setFont(const QFont &font)
{
    _font = font;

}

void bulletEngine::setColor(const QColor &color)
{
    _color = color;
}

void bulletEngine::checkBulletMapToStartAnimation(int frame)
{
    if(_bulletMap.contains(frame))
    {
        auto list = _bulletMap.value(frame);
        for(auto & item : list)
        {
            if(!item->isAnimation())
                item->startAnimation();
        }
    }
}

void bulletEngine::bulletAnimationStart_slot(QPropertyAnimation * animationObj)
{
    connect(animationObj,&QPropertyAnimation::finished,this,[&](){
        auto obj = qobject_cast<QPropertyAnimation*>(sender());
        if(_activedAnimationList.contains(obj))
            _activedAnimationList.removeOne(obj);
    });
    _activedAnimationList.append(animationObj);
}


bulletTextItem::bulletTextItem(const QString &text,int frameStamp, QGraphicsItem *parent)
    : QGraphicsTextItem(text,parent),_frameStamp(frameStamp)
{
}

bulletTextItem::~bulletTextItem()
{

}

void bulletTextItem::startAnimation()
{
    if(_animation == nullptr)
    {
        auto screenRect = scene()->sceneRect();
        _animation = new QPropertyAnimation(this,"pos");
        _animation->setStartValue(QPoint(screenRect.width(),10));
        _animation->setEndValue(QPoint(0 - boundingRect().width(),10));
        _animation->setDuration(getDuration(screenRect.width() + boundingRect().width()));

        connect(_animation,&QPropertyAnimation::finished,this,[&](){
            _animation->deleteLater();
            _animation = nullptr;
        });
    }
    _animation->start();

    emit(bulletAnimationStart(_animation));
}

bool bulletTextItem::isAnimation()
{
    return (_animation != nullptr);
}

bool bulletTextItem::isPauseOfAnimation()
{
    if(_animation)
        return (_animation->state() == QPropertyAnimation::State::Paused);
    else
        return false;
}

int bulletTextItem::getDuration(int distance)
{
    return distance / _bulletSpeed;
}
