#ifndef BULLETENGINE_H
#define BULLETENGINE_H

#include <QObject>
#include <QGraphicsTextItem>
#include <QEvent>
#include <QMap>
#include <QFont>

class QGraphicsView;
class QGraphicsScene;
class MpvPlayerWidget;
class QPropertyAnimation;
class bulletTextItem;

class bulletEngine : public QObject
{
    Q_OBJECT
public:
    explicit bulletEngine(QObject * parent = nullptr);
    ~bulletEngine();

    void setPlayer(MpvPlayerWidget * player);

    void addBullet(const QString &text);

    void synchronizingVideoPosition(int frame);

    void setPlaying();
    void setPause();

    void setFont(const QFont & font);
    QFont getFont() { return _font;}

    void setColor(const QColor & color);
    QColor getColor() { return _color;}
protected:
    bool eventFilter(QObject *watched, QEvent *event);
private slots:
    void bulletAnimationStart_slot(QPropertyAnimation *animationObj);
private:
    MpvPlayerWidget *_player{nullptr};
    QGraphicsView *_view{nullptr};
    QGraphicsScene *_scene{nullptr};

    QMap<int,QList<bulletTextItem*>> _bulletMap;
    QList<QPropertyAnimation *> _activedAnimationList;

    QFont _font;
    QColor _color;

    void initCanvas();
    void addBulletItemToMap(int frame, bulletTextItem *item);
    void checkBulletMapToStartAnimation(int frame);
};

class bulletTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    bulletTextItem(const QString &text,int frameStamp, QGraphicsItem *parent = nullptr);
    ~bulletTextItem();

    void startAnimation();
    bool isAnimation();
    bool isPauseOfAnimation();
private:
    int _frameStamp{0};
    qreal _bulletSpeed{0.2};
    QPropertyAnimation *_animation{nullptr};

    int getDuration(int distance);

signals:
    void bulletAnimationStart(QPropertyAnimation *);
};

#endif // BULLETENGINE_H
