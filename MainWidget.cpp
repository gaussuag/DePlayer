#include "MainWidget.h"

#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>

#include <QDebug>

#include <QLineEdit>

#include "bulletEngine.h"
#include "MpvPlayerWidget.h"
#include "VideoPlayerControlBar.h"
#include "PlayerController.h"

MainWidget::MainWidget(QWidget *parent)
    : CustomFramelessDialog(parent)
{
    init();
}

MainWidget::~MainWidget()
{
}

void MainWidget::init()
{
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    _player = new MpvPlayerWidget(this);
    VideoPlayerControlBar *controlBar= new VideoPlayerControlBar(this);

    mainLayout->addWidget(_player);
    mainLayout->addWidget(controlBar);

    PlayerController *controller = new PlayerController(this);
    connect(controller,&PlayerController::requestFullScreen,this,&MainWidget::requestFullScreen_slot);
    controller->setPlayer(_player,controlBar);

    setLayout(mainLayout);

    setWidgetTitleText("DePlayer");
    setGeometry(400,100,1280,860);

    setBackgroundColor("#f0ecf0f6");
}

void MainWidget::requestFullScreen_slot()
{
    if(_fullScreenFlag)
        showNormal();
    else
        showFullScreen();

    _fullScreenFlag = !_fullScreenFlag;
}
