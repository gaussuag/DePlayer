#ifndef CUSTOMFRAMELESSDIALOG_H
#define CUSTOMFRAMELESSDIALOG_H

#include "CustomWidget.h"
#include <QEvent>
#include <QMouseEvent>
#include <QStyleOption>
#include <QEventLoop>
#include <QPointer>

class QSpacerItem;
class QVBoxLayout;
class QPushButton;
class QLabel;

class CustomFramelessDialog: public CustomWidget
{
    Q_OBJECT
public:
    explicit CustomFramelessDialog(QWidget *parent = nullptr);
    ~CustomFramelessDialog();

    void setLayout(QLayout *layout);
    QVBoxLayout *getMainLayout(){ return mainLayout;}


    void setDarkTheme();

    void setBrightTheme();

    void setWidgetTitleDirection(Qt::LayoutDirection direction);

    void setWidgetTitleVisible(bool flag);
    void setWidgetTitleText(const QString & text);
    void setWidgetTitleIcon(const QIcon &icon);
    void setWidgetTitleIcon(const QPixmap & pixmap);
    void setWidgetTitleBottomLine(bool flag);
    void setWidgetTitleStyleSheet(const QString & textColor, const QString &fontSize);

    void hideCloseButton();

    void setDisplayWidgetTitle(bool flag);

    void setBorderColor(const QColor & color);
    void setBackgroundColor(const QColor & color);

    void setBorderRadius(qreal x, qreal y,
                         const QColor & BorderColor = Qt::transparent, const QColor & BackgroundColor = Qt::white);


    void setResult(int r);
    int result() const;
public slots:

    int exec();
    void done(int);
    void accept();
    void reject();
protected:
    bool eventFilter(QObject * watcher, QEvent * event);
    void closeEvent(QCloseEvent *event);

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);

    void mouseReleaseEvent(QMouseEvent * event);



    void paintEvent(QPaintEvent *event);
private:
    ///Dialog Parameter
    QPointer<QEventLoop> _eventLoop;
    int _result{0};

    ///Title Widget Parameter
    QWidget *titleWidget{nullptr};
    QLabel *titleIconLabel{nullptr};
    QLabel *titleTextLabel{ nullptr };
    QPushButton *closeBt{nullptr};

    QVBoxLayout *mainLayout{nullptr};
    QSpacerItem *spacer{nullptr};

    //Resize Parameter
    enum DragBorder {
        Top,
        TopLeft,
        TopRight,
        Bottom,
        BottomLeft,
        BottomRight,
        Left,
        Right,
        Error
    };
    bool _zoneParameterInitFlag{false};
    QRect _leftTopZone;
    QRect _rightTopZone;
    QRect _leftBottomZone;
    QRect _rightBottomZone;
    QRect _topZone;
    QRect _bottomZone;
    QRect _leftZone;
    QRect _rightZone;

    bool _dragMouseAdjustSizeFlag = false;
    QRect _startGeometry;
    CustomFramelessDialog::DragBorder _dragBorderType;

    //Move  Parameter
    bool _moveFlag{false};
    QPoint _PressedPos;
    QPoint _CurrentPos;

    //Painter Parameter
    QColor _backgroundColor{Qt::white};
    QColor _borderColor{Qt::transparent};
    qreal _radiusX{3.0};
    qreal _radiusY{3.0};

    void initWidget();
    void initTitleWidget();

    void initZoneParameter();


    void isResizeOperation(const QMouseEvent & event);
    CustomFramelessDialog::DragBorder getDragBorderType(const QPoint & Pos);
    bool dragMouseAdjustSize(const QPoint & Pos);

    bool dragTopBorder(const QPoint & Pos);

    bool dragBottomBorder(const QPoint & Pos);

    bool dragLeftBorder(const QPoint & Pos);

    bool dragRightBorder(const QPoint & Pos);

    bool dragTopLeftBorder(const QPoint & Pos);

    bool dragTopRightBorder(const QPoint & Pos);

    bool dragBottomLeftBorder(const QPoint & Pos);

    bool dragBottomRightBorder(const QPoint & Pos);

    void setResizeCursor(const QPoint & Pos);

signals:
    void finished(int result);
    void accepted();
    void rejected();
};

#endif // CUSTOMFRAMELESSDIALOG_H
