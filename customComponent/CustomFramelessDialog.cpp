#include "CustomFramelessDialog.h"
#include <QStyle>
#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QDialog>
#include <QSpacerItem>
#include <QDebug>

CustomFramelessDialog::CustomFramelessDialog(QWidget *parent) : CustomWidget(parent)
{
	initWidget();
}
CustomFramelessDialog::~CustomFramelessDialog()
{
}

void CustomFramelessDialog::setLayout(QLayout * layout)
{
    mainLayout->addLayout(layout,10);
}

void CustomFramelessDialog::hideCloseButton()
{
    if(closeBt)
        closeBt->hide();
}

void CustomFramelessDialog::setWidgetTitleBottomLine(bool flag)
{
    if(flag)
        titleWidget->setStyleSheet("QWidget#titleWidget{"
                                   "border-top:0px solid gray;"
                                   "border-bottom:1px solid gray;"
                                   "}");
    else
        titleWidget->setStyleSheet("QWidget#titleWidget{"
                                   "border-top:0px solid gray;"
                                   "border-bottom:0px solid gray;"
                                   "}");
}

void CustomFramelessDialog::setWidgetTitleStyleSheet(const QString & textColor,const QString & fontSize)
{
    if(titleTextLabel)
        titleTextLabel->setStyleSheet(QString("QLabel{color:%1;"
                                              "font-size:%2px;}").arg(textColor).arg(fontSize));
}

void CustomFramelessDialog::setDisplayWidgetTitle(bool flag)
{
    if(flag && !titleWidget->isVisible())
    {
        titleWidget->show();
        mainLayout->insertSpacerItem(mainLayout->indexOf(titleWidget),spacer);
    }
    else
    {
        titleWidget->hide();
        mainLayout->removeItem(spacer);
    }
}

void CustomFramelessDialog::setDarkTheme()
{
    titleTextLabel->setStyleSheet("QLabel{color:white;}");
    setBackgroundColor(Qt::black);
}
void CustomFramelessDialog::setBrightTheme()
{
	titleTextLabel->setStyleSheet("QLabel{color:black;}");
    setBackgroundColor(Qt::white);
}

void CustomFramelessDialog::setWidgetTitleText(const QString & text)
{
	titleTextLabel->setText(text);
}

void CustomFramelessDialog::setWidgetTitleIcon(const QIcon & icon)
{
    setWidgetTitleIcon(icon.pixmap(titleIconLabel->size()));
}

void CustomFramelessDialog::setWidgetTitleIcon(const QPixmap & pixmap)
{
    titleIconLabel->setPixmap(pixmap.scaled(titleIconLabel->size(),Qt::AspectRatioMode::KeepAspectRatio,Qt::TransformationMode::SmoothTransformation));
}

void CustomFramelessDialog::setWidgetTitleDirection(Qt::LayoutDirection direction)
{
    titleWidget->setLayoutDirection(direction);
}

void CustomFramelessDialog::setWidgetTitleVisible(bool flag)
{
    titleWidget->setVisible(flag);
}

void CustomFramelessDialog::setBorderColor(const QColor &color)
{
    _borderColor = color;
}

void CustomFramelessDialog::setBackgroundColor(const QColor &color)
{
    _backgroundColor = color;
}

void CustomFramelessDialog::setBorderRadius(qreal x, qreal y, const QColor &BorderColor, const QColor &BackgroundColor)
{
    _radiusX = x;
    _radiusY = y;

    _borderColor = BorderColor;
    _backgroundColor = BackgroundColor;
}

void CustomFramelessDialog::setResult(int r)
{
    _result = r;
}
int CustomFramelessDialog::result() const
{
    return _result;
}

int CustomFramelessDialog::exec()
{
    if (Q_UNLIKELY(_eventLoop)) {
        qWarning("QDialog::exec: Recursive call detected");
        return -1;
    }

    bool deleteOnClose = testAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_DeleteOnClose, false);

    bool wasShowModal = testAttribute(Qt::WA_ShowModal);
    setAttribute(Qt::WA_ShowModal, true);

    show();

    QPointer<CustomFramelessDialog> guard = this;
    QEventLoop eventLoop;
    _eventLoop = &eventLoop;
    (void) eventLoop.exec(QEventLoop::DialogExec);
    if (guard.isNull())
        return QDialog::Rejected;

    _eventLoop = 0;

    setAttribute(Qt::WA_ShowModal, wasShowModal);

    int res = result();
    if (deleteOnClose)
        delete this;
    return res;
}

void CustomFramelessDialog::done(int r)
{
    hide();
    setResult(r);

    emit(finished(r));
    if (r == QDialog::Accepted)
        emit(accepted());
    else if (r == QDialog::Rejected)
        emit(rejected());

    close();
}

void CustomFramelessDialog::accept()
{
    done(QDialog::Accepted);
}

void CustomFramelessDialog::reject()
{
    done(QDialog::Rejected);
}

bool CustomFramelessDialog::eventFilter(QObject * watcher, QEvent * event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		auto mouseEvent = dynamic_cast<QMouseEvent*>(event);
		if (mouseEvent != nullptr)
		{
			if (mouseEvent->button() == Qt::LeftButton)
			{
				if (watcher == titleWidget)
				{
					///鼠标按下的坐标
					_PressedPos = QCursor::pos();
					///当前控件的左上角坐标
					_CurrentPos = this->pos();
					_moveFlag = true;
				}
			}
		}
	}
	
	return QWidget::eventFilter(watcher, event);
}

void CustomFramelessDialog::closeEvent(QCloseEvent *event)
{
    if(!_eventLoop.isNull())
    {
        _eventLoop.data()->exit();
        _eventLoop = 0;
    }

    return QWidget::closeEvent(event);
}

void CustomFramelessDialog::mousePressEvent(QMouseEvent *event)
{
	isResizeOperation(*event);
	return QWidget::mousePressEvent(event);
}

void CustomFramelessDialog::mouseMoveEvent(QMouseEvent *event)
{
	///移动使能标志位使能，进行鼠标移动控件操作
	if (_moveFlag)
	{
		QPoint pos = _CurrentPos + (event->screenPos().toPoint() - _PressedPos);
		move(pos);
	}
    ///边缘拖动调整窗口大小标志位使能，进行鼠标移动调整窗口大小的操作
	else if (_dragMouseAdjustSizeFlag)
    {
        if (!dragMouseAdjustSize(event->screenPos().toPoint()))
            return;
        event->accept();
	}
	else
		setResizeCursor(event->pos());

	return QWidget::mouseMoveEvent(event);
}
void CustomFramelessDialog::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->type() == QEvent::MouseButtonRelease)
	{
		if (_moveFlag)
			_moveFlag = false;
		if (_dragMouseAdjustSizeFlag)
            _dragMouseAdjustSizeFlag = false;


        setResizeCursor(event->pos());
    }
	return QWidget::mouseReleaseEvent(event);
}

void CustomFramelessDialog::isResizeOperation(const QMouseEvent & event)
{
	auto CurrentCursor = cursor();
	if (CurrentCursor == Qt::ArrowCursor)
		return;

    _dragMouseAdjustSizeFlag = true;
    //根据按下鼠标位置获取拖动方向类型
	_dragBorderType = getDragBorderType(event.pos());

    _startGeometry = geometry();
}

CustomFramelessDialog::DragBorder CustomFramelessDialog::getDragBorderType(const QPoint & Pos)
{
	if (_leftTopZone.contains(Pos))
        return CustomFramelessDialog::DragBorder::TopLeft;
	else if (_rightTopZone.contains(Pos))
        return CustomFramelessDialog::DragBorder::TopRight;
	else if (_leftBottomZone.contains(Pos))
        return CustomFramelessDialog::DragBorder::BottomLeft;
	else if (_rightBottomZone.contains(Pos))
        return CustomFramelessDialog::DragBorder::BottomRight;
	else if (_topZone.contains(Pos))
        return CustomFramelessDialog::DragBorder::Top;
	else if (_bottomZone.contains(Pos))
        return CustomFramelessDialog::DragBorder::Bottom;
	else if (_leftZone.contains(Pos))
        return CustomFramelessDialog::DragBorder::Left;
	else if (_rightZone.contains(Pos))
        return CustomFramelessDialog::DragBorder::Right;
	else 
        return CustomFramelessDialog::DragBorder::Error;
}

bool CustomFramelessDialog::dragMouseAdjustSize(const QPoint & Pos)
{
	if (!_dragMouseAdjustSizeFlag)
		return false;
	switch (_dragBorderType)
	{
    case CustomFramelessDialog::Top:
		return dragTopBorder(Pos);
    case CustomFramelessDialog::TopLeft:
		return dragTopLeftBorder(Pos);
    case CustomFramelessDialog::TopRight:
		return dragTopRightBorder(Pos);
    case CustomFramelessDialog::Bottom:
		return dragBottomBorder(Pos);
    case CustomFramelessDialog::BottomLeft:
		return dragBottomLeftBorder(Pos);
    case CustomFramelessDialog::BottomRight:
		return dragBottomRightBorder(Pos);
    case CustomFramelessDialog::Left:
		return dragLeftBorder(Pos);
    case CustomFramelessDialog::Right:
		return dragRightBorder(Pos);
    case CustomFramelessDialog::Error:
        return false;
    }
	return false;
}

bool CustomFramelessDialog::dragTopBorder(const QPoint &Pos)
{	
    //Drag Top Border
    QPoint endPoint = QPoint(_startGeometry.topLeft().x(),Pos.y());
    QRect newRect = QRect(_startGeometry.bottomRight(),endPoint).normalized();
    if (newRect.height() <= mainLayout->sizeHint().height())
        return false;
    setGeometry(newRect);
	
	return true;
}

bool CustomFramelessDialog::dragBottomBorder(const QPoint & Pos)
{
    QPoint endPoint = QPoint(_startGeometry.bottomRight().x(),Pos.y());
    QRect newRect = QRect(_startGeometry.topLeft(),endPoint).normalized();
    if (newRect.height() <= mainLayout->sizeHint().height())
		return false;
    setGeometry(newRect);

	return true;
}

bool CustomFramelessDialog::dragLeftBorder(const QPoint & Pos)
{
    QPoint endPoint = QPoint(Pos.x(),_startGeometry.topLeft().y());
    QRect newRect = QRect(_startGeometry.bottomRight(),endPoint).normalized();
    if (newRect.width() <= mainLayout->sizeHint().width())
		return false;
    setGeometry(newRect);

	return true;
}

bool CustomFramelessDialog::dragRightBorder(const QPoint & Pos)
{
    QPoint endPoint = QPoint(Pos.x(),_startGeometry.bottomRight().y());
    QRect newRect = QRect(_startGeometry.topLeft(),endPoint).normalized();
    if (newRect.width() <= mainLayout->sizeHint().width())
		return false;
    setGeometry(newRect);

	return true;
}

bool CustomFramelessDialog::dragTopLeftBorder(const QPoint & Pos)
{
	QSize limitSize = mainLayout->sizeHint();

    QRect newRect = QRect(_startGeometry.bottomRight(),Pos).normalized();
    if (( newRect.width() <= limitSize.width() )
        || (newRect.height() <= limitSize.height()))
		return false;
    setGeometry(newRect);

	return true;
}

bool CustomFramelessDialog::dragTopRightBorder(const QPoint & Pos)
{
	QSize limitSize = mainLayout->sizeHint();

    QRect newRect = QRect(_startGeometry.bottomLeft(),Pos).normalized();
    if ((newRect.width() <= limitSize.width())
        || (newRect.height() <= limitSize.height()))
		return false;
    setGeometry(newRect);

	return true;
}

bool CustomFramelessDialog::dragBottomLeftBorder(const QPoint & Pos)
{
	QSize limitSize = mainLayout->sizeHint();

    QRect newRect = QRect(_startGeometry.topRight(),Pos).normalized();
    if ((newRect.width() <= limitSize.width())
        || (newRect.height() <= limitSize.height()))
		return false;
    setGeometry(newRect);

	return true;
}

bool CustomFramelessDialog::dragBottomRightBorder(const QPoint & Pos)
{
	QSize limitSize = mainLayout->sizeHint();

    QRect newRect = QRect(_startGeometry.topLeft(),Pos).normalized();
    if ((newRect.width() <= limitSize.width())
        || (newRect.height() <= limitSize.height()))
		return false;
    setGeometry(newRect);

	return true;
}

void CustomFramelessDialog::setResizeCursor(const QPoint & Pos)
{
	initZoneParameter();

	if (_leftTopZone.contains(Pos) || _rightBottomZone.contains(Pos))
		setCursor(Qt::SizeFDiagCursor);
	else if (_leftBottomZone.contains(Pos) ||_rightTopZone.contains(Pos))
		setCursor(Qt::SizeBDiagCursor);
	else if (_topZone.contains(Pos) || _bottomZone.contains(Pos))
		setCursor(Qt::SizeVerCursor);
	else if (_leftZone.contains(Pos)|| _rightZone.contains(Pos))
		setCursor(Qt::SizeHorCursor);
	else
		setCursor(Qt::ArrowCursor);
}


void CustomFramelessDialog::initWidget()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
	initTitleWidget();

    spacer = new QSpacerItem(1,0);

	mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10,5,10,5);
	mainLayout->addWidget(titleWidget);
    mainLayout->addSpacerItem(spacer);
}

void CustomFramelessDialog::initTitleWidget()
{
    titleWidget = new QWidget(this);
    titleWidget->setObjectName("titleWidget");
    titleWidget->setFixedHeight(30);
	titleWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	titleWidget->installEventFilter(this);

	QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->setMargin(0);
	
    titleIconLabel = new QLabel(titleWidget);
    titleIconLabel->setFixedSize(24,24);
    titleTextLabel = new QLabel(titleWidget);

	titleTextLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    closeBt = new QPushButton(titleWidget);
    closeBt->setStyleSheet("QPushButton{border:0px solid gray;"
                           "background-color:transparent;}"
                           "QPushButton::hover{border:1px solid gray;"
                           "background-color:rgba(124,185,203,77)}");
	closeBt->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton));
	connect(closeBt, &QPushButton::clicked, this, &QWidget::close);

	titleLayout->addWidget(titleIconLabel);
	titleLayout->addWidget(titleTextLabel);
	titleLayout->addWidget(closeBt);
}

void CustomFramelessDialog::initZoneParameter()
{	
	_leftTopZone = QRect{0,0,3,3};
	_rightTopZone = QRect{ width() - 3,0,width(),3 };
	_leftBottomZone = QRect{0,height()-3,3,height()};
	_rightBottomZone = QRect{ width() - 3,height() - 3,width(),height() };

	_topZone = QRect{ 0,0,width(),3 };
	_bottomZone = QRect{ 0,height() - 3,width(),height() };
	_leftZone = QRect{ 0,0,3,height() };
	_rightZone = QRect{ width() - 3,0,width(),height() };
}

void CustomFramelessDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(_borderColor);
    painter.setBrush(_backgroundColor);
    painter.drawRoundedRect(rect(),_radiusX,_radiusY,Qt::SizeMode::AbsoluteSize);
}
