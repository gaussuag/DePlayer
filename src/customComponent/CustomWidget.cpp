#include "CustomWidget.h"
#include <QDebug>
#include <QApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QRubberBand>
#include <QRect>

#include <QPushButton>

CustomWidget::CustomWidget(QWidget *parent) : QWidget(parent)
{
    installEventFilter(this);
    qApp->installEventFilter(this);
}

CustomWidget::~CustomWidget()
{

}


void CustomWidget::removeWidgetObjectOnRubberMap(QWidget * w)
{
    if(historyOnRegionWidgetMap.contains(w))
        historyOnRegionWidgetMap.remove(w);

    if(currentOnRegionWidgetMap.contains(w))
        currentOnRegionWidgetMap.remove(w);
}

void CustomWidget::setEnableRubberBand(bool flag)
{
    enableRubberBandFlag = flag;
}

bool CustomWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == this)
    {
        if(event->type() == QEvent::Show)
            emit(widgetshow(this));
    }
    if(event->type() == QEvent::KeyPress)
    {
        auto keyEvent = dynamic_cast<QKeyEvent *>(event);
        if(keyEvent)
            handleKeyPressedEvent(keyEvent);
    }
    else if(event->type() == QEvent::KeyRelease)
    {
        auto keyEvent = dynamic_cast<QKeyEvent *>(event);
        if(keyEvent)
            handleKeyReleaseEvent(keyEvent);
    }

    return QWidget::eventFilter(watched,event);
}

void CustomWidget::handleKeyPressedEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key::Key_Control)
        controlKeyFlag = true;
    else if(event->key() == Qt::Key::Key_Shift)
        shiftKeyFlag = true;
}
void CustomWidget::handleKeyReleaseEvent(QKeyEvent* event)
{
    if(event->key() == Qt::Key::Key_Control)
        controlKeyFlag = false;
    else if(event->key() == Qt::Key::Key_Shift)
        shiftKeyFlag = false;
}


void CustomWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        handleLeftButtonClicked(event->pos());

    if(enableRubberBandFlag && (event->button() == Qt::LeftButton))
        startDrawRubberBand(event->pos());
    return QWidget::mousePressEvent(event);
}

void CustomWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(enableRubberBandFlag && rubberBand)
        continueDrawRubberBand(event->pos());

    return QWidget::mouseMoveEvent(event);
}

void CustomWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(enableRubberBandFlag && rubberBand)
        endDrawRubberBand();

    if(event->button() == Qt::RightButton)
        emit(clickedRightButton(event->globalPos()));

    return QWidget::mouseReleaseEvent(event);
}

void CustomWidget::startDrawRubberBand(const QPoint &Pos)
{
    if(rubberBand == nullptr)
        rubberBand = new QRubberBand(QRubberBand::Shape::Rectangle,this);

    rbStartPos = Pos;
    rbRect = QRect(rbStartPos,QPoint(0,0));
    rubberBand->setGeometry(rbRect);
    rubberBand->show();
}

void CustomWidget::continueDrawRubberBand(const QPoint &Pos)
{
    QRect newR(rbStartPos,Pos);
    newR = newR.normalized();
    if(newR == rbRect)
        return ;
    if(newR.width() > 1|| newR.height() > 1)
    {
        rbRect = newR;
        rubberBand->setGeometry(rbRect);
        handleRubberBandRect(rbRect);
    }
}
void CustomWidget::endDrawRubberBand()
{
    if(rubberBand)
    {
        delete  rubberBand;
        rubberBand = nullptr;

        ///处理在框选范围内的WidgetMap
        handleEndRubberBandRegionWidgetMap();
    }
}

void CustomWidget::handleRubberBandRect(const QRect & rect)
{
    auto cl = findChildren<QWidget *>(QString(),Qt::FindChildOption::FindDirectChildrenOnly);
    const QRect selectRect(mapToGlobal(rect.topLeft()),mapToGlobal(rect.bottomRight()));
    for(const auto & w : cl)
    {
        ///Get Children Widget Global Geometry
        const QPoint P = w->mapToGlobal(QPoint(0,0));
        const QRect cr(P,w->size());

        ///Shitf键:不清空此前的框选Widget，若再次框选了相同的Widget，不反选，但框选范围离开了该Widget，Widget会触发离开范围事件。
        /// Ctrl键：不清空此前的框选Widget，若再次框选相同的Widget，进行反选，若框选范围离开了该Widget，则该Widget将进行反-反选操作，意味着再次选中。
        ///没有按下辅助热键，则清空历史记录的Widget
        if(!controlKeyFlag && !shiftKeyFlag)
            clearHistoryRegionWidgetMap();


        ///RubberBand Rect Enter Widget Region
        if(cr.adjusted(10,10,-10,-10).intersects(selectRect) && !cr.contains(selectRect))
        {
            ///此Widget被框选过，则进行辅助按键判断并进行对应的操作。
            if(historyOnRegionWidgetMap.contains(w))
            {
                ///按下Shitf键，对此前包含进RubberBand范围的Widget移动到当前操作列表。
                if(shiftKeyFlag)
                {
                    currentOnRegionWidgetMap.insert(w,true);
                    historyOnRegionWidgetMap.remove(w);
                }
                ///按下Ctrl键，对此前包含进RubberBand范围的的Widget进行反选操作。
                else if(controlKeyFlag)
                {
                    if(historyOnRegionWidgetMap[w])
                    {
                        historyOnRegionWidgetMap[w] = false;
                        emit(unselectWidget(*w));
                    }
                }
            }
            ///当前的选取Widget列表不存在这个控件，则将其添加进记录列表中。
            else if(!currentOnRegionWidgetMap.contains(w))
            {
                currentOnRegionWidgetMap.insert(w, true);
                emit(selectWidget(*w));
            }
        }
        ///RubberBand Rect Leave Widget Region
        else{
            ///历史列表中的Widget被执行了反选，现在Widget离开了当前的RubberBand范围，需要再次反-反选，即选中。
            if(historyOnRegionWidgetMap.contains(w))
            {
                if(historyOnRegionWidgetMap[w] == false)
                {
                    historyOnRegionWidgetMap[w] = true;
                    emit(selectWidget(*w));
                }
            }
            ///当前的选取Widget列表中包含了这个控件，则将其剔除记录列表。
            else if(currentOnRegionWidgetMap.contains(w))
            {
                currentOnRegionWidgetMap.remove(w);
                emit(unselectWidget(*w));
            }
        }
    }
}
void CustomWidget::handleEndRubberBandRegionWidgetMap()
{
    QMap<QWidget*,bool>::iterator it;
    ///遍历当前的框选范围内的Widget，整合到历史列表中
    for(it = currentOnRegionWidgetMap.begin();it != currentOnRegionWidgetMap.end();++it)
        historyOnRegionWidgetMap.insert(it.key(),it.value());
    ///过滤掉历史列表中被反选的Widget，意味该Widget已经被剔除。
    for(auto & w : historyOnRegionWidgetMap.keys(false))
    {
        if(historyOnRegionWidgetMap.contains(w))
            historyOnRegionWidgetMap.remove(w);
    }

    currentOnRegionWidgetMap.clear();
}
void CustomWidget::clearHistoryRegionWidgetMap()
{
    ///遍历当前选中的Widget，触发一次Widget离开框选的信号。
    for(auto & w : historyOnRegionWidgetMap.keys())
        emit(unselectWidget(*w));

    historyOnRegionWidgetMap.clear();
}

void CustomWidget::handleLeftButtonClicked(const QPoint &Pos)
{
    ///坐标下Widget判断，不存在Widget则无需响应，此处获取的underWidget有可能是间接child对象。
    auto underWidget = childAt(Pos);
    if(underWidget == nullptr)
        return;

    ///没有按下辅助热键，则清空历史记录的Widget
    if(!controlKeyFlag && !shiftKeyFlag)
        clearHistoryRegionWidgetMap();

    ///根据光标位置，获取其下的ItemWidget，此处获取的underItemWidget对象是直接Child对象。
    QPoint G_Pos = mapToGlobal(Pos);
    QWidget *underItemWidget = nullptr;
    auto ChildrenList = findChildren<QWidget *>(QString(),Qt::FindChildOption::FindDirectChildrenOnly);
    for(const auto & w : ChildrenList)
    {
        ///Get Children Widget Global Geometry
        const QPoint P = w->mapToGlobal(QPoint(0,0));
        const QRect cr(P,w->size());

        if(cr.contains(G_Pos))
        {
            underItemWidget = w;
            break;
        }
    }

    ///获取直接对象指针出错，跳出
    if(underItemWidget == nullptr)
        return;

    ///进入鼠标左键事件响应处理
    ///按下shift辅助热键，需要自动补选间隔ItemWidget
    if(shiftKeyFlag)
    {
        ///判断补选方向
        /// 根据最后单选的Item作为参考Item，以此Item为原点，当前光标下Item对象为终点，进行补选操作。
        QPoint lastClickedItemWidget_G_Pos;
        if(_lastClickedItemWidget)
            lastClickedItemWidget_G_Pos = mapToGlobal(_lastClickedItemWidget->pos());

        ///通过鼠标所在位置下的ItemWidget，作为起点位置
        QPoint G_UnderItemWidgetPos = underItemWidget->mapToGlobal(QPoint(0,0));
        QPoint G_StartPos = G_UnderItemWidgetPos;
        QPoint G_EndPos = lastClickedItemWidget_G_Pos;

        ///判断点击的坐标与当前选中的Item中最小坐标的位置关系,若鼠标点击位置比当前选中的Item的最小位置更大，则交换两点位置。
        /// 确保开始位置比结束位置小。
        if(comparePoint(G_StartPos,G_EndPos) == 1)
        {
            G_StartPos = G_EndPos;
            G_EndPos = G_UnderItemWidgetPos;
        }

        ///清空选中的Item
        clearHistoryRegionWidgetMap();
        ///遍历列表，并选中在两点区间的所有Item
        for(const auto & w : ChildrenList)
        {
            ///Get Children Widget Global Geometry
            const QPoint Item_Pos = w->mapToGlobal(QPoint(0,0));
            ///Item的位置坐标在参考坐标两点之间，则选中
            if((comparePoint(G_StartPos,Item_Pos) < 1) && (comparePoint(G_EndPos,Item_Pos) > -1))
            {
                historyOnRegionWidgetMap.insert(w,true);
                emit(clickedLeftButtonUnderWidget(*w));
            }
        }
    }
    else
    {
        ///非Shitf 补全模式，进入单选或ctrl复选模式

        ///更新最后点击的ItemWidget
        _lastClickedItemWidget = underItemWidget;

        ///点击响应模式
        if(controlKeyFlag)
        {
            ///单击模式下的反选操作
            if(historyOnRegionWidgetMap.contains(underItemWidget))
            {
                if(historyOnRegionWidgetMap[underItemWidget])
                {
                    historyOnRegionWidgetMap.remove(underItemWidget);
                    emit(unselectWidget(*underItemWidget));
                    return;
                }
            }
        }
        historyOnRegionWidgetMap.insert(underItemWidget,true);
        emit(clickedLeftButtonUnderWidget(*underItemWidget));
    }
}

int CustomWidget::comparePoint(const QPoint & P1,const QPoint & P2)
{
    if((P1.x() == P2.x()) && (P1.y() == P2.y()))
        return 0;

    ///P1 < P2
    if( ( (P1.x() < P2.x()) && (P1.y() <= P2.y()) )
            || ( (P1.x() >= P2.x()) && (P1.y() < P2.y()) ) )
        return -1;
    else
        return 1;
}
