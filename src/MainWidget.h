#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "CustomFramelessDialog.h"

class MpvPlayerWidget;

class MainWidget : public CustomFramelessDialog
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

    void showFullScreen();
private slots:
    void requestFullScreen_slot();
private:
    MpvPlayerWidget *_player{nullptr};

    bool _fullScreenFlag{false};

    QRect _normalGeometry;

    void init();

};
#endif // MAINWIDGET_H
