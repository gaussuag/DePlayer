QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MpvPlayerWidget/MediaInfoReader.cpp \
    MpvPlayerWidget/MpvPlayerWidget.cpp \
    MpvPlayerWidget/MpvVideoFileReader.cpp \
    PlayerController.cpp \
    VideoPlayerControlBar.cpp \
    bulletEngine.cpp \
    customComponent/CustomFramelessDialog.cpp \
    customComponent/CustomRadiusSlider.cpp \
    customComponent/CustomVideoPlayerProgressBar.cpp \
    customComponent/CustomVolumeWidget.cpp \
    customComponent/CustomWidget.cpp \
    customComponent/VideoPlayerVolumeControlWidget.cpp \
    main.cpp \
    MainWidget.cpp

HEADERS += \
    MainWidget.h \
    MpvPlayerWidget/MediaInfoReader.h \
    MpvPlayerWidget/MpvPlayerWidget.h \
    MpvPlayerWidget/MpvTypes.h \
    MpvPlayerWidget/MpvVideoFileReader.h \
    PlayerController.h \
    VideoPlayerControlBar.h \
    bulletEngine.h \
    customComponent/CustomFramelessDialog.h \
    customComponent/CustomRadiusSlider.h \
    customComponent/CustomVideoPlayerProgressBar.h \
    customComponent/CustomVolumeWidget.h \
    customComponent/CustomWidget.h \
    customComponent/VideoPlayerVolumeControlWidget.h \
    include/mediainfoDLL/MediaInfoDLL.h \
    include/mpv/client.h \
    include/mpv/opengl_cb.h \
    include/mpv/qthelper.hpp \
    include/mpv/render.h \
    include/mpv/render_gl.h \
    include/mpv/stream_cb.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/lib/mpv/ -lmpv
win32: LIBS += -L$$PWD/lib/mediainfo/ -lMediaInfo

INCLUDEPATH += $$PWD/lib/mpv \
                $$PWD/lib/mediainfo \
                include/mediainfoDLL/ \
                include/mpv/ \
                MpvPlayerWidget/ \
                customComponent/

DEPENDPATH += $$PWD/lib/mpv
DEPENDPATH += $$PWD/lib/mediainfo

RESOURCES += \
    res.qrc

RC_ICONS = icon/appicon.ico
