#include "MpvPlayerWidget.h"
#include <stdexcept>
#include <QtGui/QOpenGLContext>
#include <QtCore/QMetaObject>
#include <QCoreApplication>
#include <QtMath>

#include <QFileInfo>
#include <QDebug>


static void wakeup(void *ctx)
{
    QMetaObject::invokeMethod(reinterpret_cast<MpvPlayerWidget*>(ctx), "on_mpv_events", Qt::QueuedConnection);
}

static void *get_proc_address(void *ctx, const char *name) {
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx)
        return nullptr;
    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

MpvPlayerWidget::MpvPlayerWidget(QWidget *parent, Qt::WindowFlags f)
    : QOpenGLWidget(parent, f)
{
    initMpvHandle();
}

MpvPlayerWidget::~MpvPlayerWidget()
{
    makeCurrent();
    if (mpv_gl)
		mpv_render_context_free(mpv_gl);
    mpv_terminate_destroy(mpv);
}

QVariant MpvPlayerWidget::getProperty(const QString &name) const
{
    return mpv::qt::get_property_variant(mpv, name);
}

QString MpvPlayerWidget::getPlayingIndex()
{
    playingIndex = getProperty(QStringLiteral("playlist-playing-pos")).toString();
    return playingIndex;
}

bool MpvPlayerWidget::isSeeking()
{
    return getProperty(QStringLiteral("seeking")).toBool();
}

bool MpvPlayerWidget::isPause()
{
    return getProperty(QStringLiteral("pause")).toBool();
}

bool MpvPlayerWidget::isPlaying()
{
    auto state = getPlayState();
    return (state == Mpv::PlayState::Playing);
}

void MpvPlayerWidget::openFile(const QString & AbsoluteFilePath)
{
    command(QStringList() << QStringLiteral("loadfile") << AbsoluteFilePath);
}

void MpvPlayerWidget::playFile(const QString & AbsoluteFilePath)
{
    if(AbsoluteFilePath.isEmpty())
        return;

    openFile(AbsoluteFilePath);
    play();
}

void MpvPlayerWidget::appendFile(const QString & AbsoluteFilePath)
{
    command(QStringList() << QStringLiteral("loadfile") << AbsoluteFilePath << QStringLiteral("append-play"));
}

void MpvPlayerWidget::playNext()
{
    command(QStringList() << QStringLiteral("playlist-next") << QStringLiteral("force"));
}

void MpvPlayerWidget::playFileOnPlaylistIndex(int index)
{
    command(QStringList() << QStringLiteral("playlist-play-index") << QString::number(index));
}

void MpvPlayerWidget::test()
{
    qDebug()<<"On test"<<getProperty(QStringLiteral("playlist-count")).toInt();
}

void MpvPlayerWidget::play()
{
    if(playState > 0 && mpv)
        setProperty(QStringLiteral("pause"), false);
}
void MpvPlayerWidget::pause()
{
    if(mpv && playState > 0)
        setProperty(QStringLiteral("pause"), true);
}

void MpvPlayerWidget::playOrPause()
{
    if(playState == Mpv::PlayState::Playing)
        pause();
    else if(playState == Mpv::PlayState::Paused)
        play();
}

void MpvPlayerWidget::stop()
{
    restart();
    pause();
}

void MpvPlayerWidget::restart()
{
    seek(0);
    play();
}

void MpvPlayerWidget::clearPlaylist()
{
    stop();
    command(QStringList() << QStringLiteral("playlist-clear"));
}

void MpvPlayerWidget::setVolume(int level)
{
    if(level > 100) level = 100;
    else if(level < 0) level = 0;

    if(playState > 0)
        setProperty(QStringLiteral("volume"), qlonglong(level));
    else
        setOption(QStringLiteral("volume"), qlonglong(level));
}

void MpvPlayerWidget::setMute(bool flag)
{
    if(playState > 0)
        setProperty(QStringLiteral("ao-mute"),flag ? "yes" : "no");

    updateMute(flag);
}

void MpvPlayerWidget::setSpeed(double value)
{
    if(playState > 0)
        setProperty(QStringLiteral("speed"), value);
}

void MpvPlayerWidget::hardwareDecode（(bool flag)
{
    if(playState > 0)
        setProperty(QStringLiteral("hwdec"), flag ? "auto" : "no");
    else
        setOption(QStringLiteral("hwdec"), flag ? "auto" : "no");
}

void MpvPlayerWidget::setPreProperties()
{
    setVolume(volume);
    setSpeed(speed);
    setMute(mute);
}

void MpvPlayerWidget::seek(qreal pos)
{
    ///数值降级，将为秒级
    pos /= 1000;

    if(playState > 0)
        command(QVariantList() << QStringLiteral("seek") << QString::number(pos,'f',3) << QStringLiteral("absolute"));
}

void MpvPlayerWidget::frameStep()
{
    command(QStringList() << QStringLiteral("frame_step"));
}

void MpvPlayerWidget::frameBackStep()
{
    command(QStringList()<< QStringLiteral("frame_back_step"));
}


void MpvPlayerWidget::command(const QVariant& params)
{
    mpv::qt::command_variant(mpv, params);
}

void MpvPlayerWidget::setOption(const QString &name, const QVariant &value)
{
    mpv::qt::set_option_variant(mpv, name, value);
}

void MpvPlayerWidget::setProperty(const QString& name, const QVariant& value)
{
    mpv::qt::set_property_variant(mpv, name, value);
}



void MpvPlayerWidget::on_mpv_events()
{
    // Process all events, until the event queue is empty.
    while (mpv) {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event->event_id == MPV_EVENT_NONE) {
            break;
        }
        handle_mpv_event(event);
    }
}

void MpvPlayerWidget::handle_mpv_event(mpv_event *event)
{
//    if(event->event_id != 22)
//        qDebug()<<MpvEventString::EventString(event->event_id);

    switch (event->event_id)
    {
    case MPV_EVENT_PROPERTY_CHANGE:
    {
        mpv_event_property *prop = (mpv_event_property *)event->data;
//        if(prop->name != QLatin1String("playback-time")
//                && prop->name != QLatin1String("percent-pos")
//                && prop->name != QLatin1String("time-pos")
//                && prop->name != QLatin1String("playtime-remaining")
//                && prop->name != QLatin1String("time-remaining"))
//            qDebug()<<prop->name<<prop->format;

        if(QString(prop->name) == QLatin1String("percent-pos"))
        {
            if(prop->format == MPV_FORMAT_DOUBLE)
                updatePercent((double)*(double*)prop->data);
        }
        else if(QString(prop->name) == QLatin1String("time-pos"))
        {
            ///毫秒级别的时间变更
            if(prop->format == MPV_FORMAT_DOUBLE)
            {
                updatePosition(1000 * (double)*(double*)prop->data);
                ///获取当前帧进度并发射信号
                updateCurrentFrameNumber(getProperty(QStringLiteral("estimated-frame-number")).toInt());
            }
        }
        else if(QString(prop->name) == QLatin1String("duration"))
        {
            if(prop->format == MPV_FORMAT_DOUBLE)
                updateDuration(1000 * (double)*(double*)prop->data);
        }
        else if(QString(prop->name) == QLatin1String("ao-volume")
                || QString(prop->name) == QLatin1String("volume"))
        {
            if(prop->format == MPV_FORMAT_DOUBLE)
                 updateVolume((int)*(double*)prop->data);
        }
        else if(QString(prop->name) == QLatin1String("ao-mute"))
        {
            if(prop->format == MPV_FORMAT_FLAG)
                updateMute((bool)*(unsigned*)prop->data);
        }
        else if(QString(prop->name) == QLatin1String("speed"))
        {
            if(prop->format == MPV_FORMAT_DOUBLE)
                updateSpeed((double)*(double*)prop->data);
        }
        else if(QString(prop->name) == QLatin1String("hwdec"))
        {
            if(prop->format == MPV_FORMAT_STRING)
            {
                auto hardwareDecoder = *(char **)(prop->data);
                bool flag = false;
                if(hardwareDecoder == QLatin1String("auto"))
                    flag = true;
                else if(hardwareDecoder == QLatin1String("no"))
                    flag = false;

                updateHardwareDecode(flag);
            }
        }
        else if(QString(prop->name) == QLatin1String("playlist-playing-pos"))
        {
            if(prop->format == MPV_FORMAT_STRING)
                updatePlayingIndex(*(char **)(prop->data));
        }
        else if(QString(prop->name) == QLatin1String("osd-dimensions"))
        {
            if(prop->format == MPV_FORMAT_NODE)
                loadOsdDimensionsParams();
        }
        break;
    }
    case MPV_EVENT_VIDEO_RECONFIG:
    {
        // Retrieve the new video size.
        int vw = 0, vh = 0;
        vw = getProperty(QStringLiteral("width")).toInt();
        vh = getProperty(QStringLiteral("height")).toInt();
        if (vw > 10 && vh > 10)
        {
            if (vw != videoWidth || vh != videoHeight)
                updateVideoSize(vw,vh);
        }
        break;
    }
    case MPV_EVENT_IDLE:
    {
        updatePlayState(Mpv::Idle);
        break;
        // these two look like they're reversed but they aren't. the names are misleading.
    }
    case MPV_EVENT_START_FILE:
        updatePlayState(Mpv::StartFile);
        break;
    case MPV_EVENT_END_FILE:
        ///更新播放进度到文件末尾
        updatePosition(fileInfo.length);
        updateCurrentFrameNumber(fileInfo.video_params.estimated_frames);
        ///卸载文件信息
        unloadFileInfo();
        updatePlayState(Mpv::PlayFinished);
    case MPV_EVENT_FILE_LOADED:
        updatePlayState(Mpv::FileLoaded);
        setPreProperties();
        break;
    case MPV_EVENT_UNPAUSE:
        updatePlayState(Mpv::Playing);
        break;
    case MPV_EVENT_PLAYBACK_RESTART:
    {
        if(!loadedFlag)
            loadFileInfo();

        if(isSeeking())
            updatePlayState(Mpv::Seeking);
        else
        {
            if(isPause())
                updatePlayState(Mpv::Paused);
            else
                updatePlayState(Mpv::Playing);
        }
        break;
    }
    case MPV_EVENT_PAUSE:
        updatePlayState(Mpv::Paused);
        break;
    case MPV_EVENT_SHUTDOWN:
        updatePlayState(Mpv::Stopped);
        QCoreApplication::quit();
        break;
    case MPV_EVENT_LOG_MESSAGE:
    {
        auto *message = static_cast<mpv_event_log_message*>(event->data);
        if(message != nullptr)
            emit(messageSignal(message->text));
        break;
    }
    default:;
        // Ignore uninteresting or unknown events.
    }
}

// Make Qt invoke mpv_render_context_render() to draw a new/updated video frame.
void MpvPlayerWidget::maybeUpdate()
{
    // If the Qt window is not visible, Qt's update() will just skip rendering.
    // This confuses mpv's render API, and may lead to small occasional
    // freezes due to video rendering timing out.
    // Handle this by manually redrawing.
    // Note: Qt doesn't seem to provide a way to query whether update() will
    //       be skipped, and the following code still fails when e.g. switching
    //       to a different workspace with a reparenting window manager.
    if (window()->isMinimized()) {
        makeCurrent();
        paintGL();
        context()->swapBuffers(context()->surface());
        doneCurrent();
    } else {
        update();
    }
}

void MpvPlayerWidget::on_update(void *ctx)
{
    QMetaObject::invokeMethod(reinterpret_cast<MpvPlayerWidget*>(ctx), "maybeUpdate");
}

void MpvPlayerWidget::initMpvHandle()
{
    mpv = mpv_create();
    if (!mpv)
        throw std::runtime_error("could not create mpv context");

    //mpv_set_option_string(mpv, "terminal", "yes");
    //mpv_set_option_string(mpv, "msg-level", "all=v");

    setOption(QStringLiteral("input-default-bindings"), QStringLiteral("no")); // disable mpv default key bindings
    setOption(QStringLiteral("input-vo-keyboard"), QStringLiteral("no")); // disable keyboard input on the X11 window
    setOption(QStringLiteral("input-cursor"), QStringLiteral("no")); // no mouse handling
    setOption(QStringLiteral("cursor-autohide"), QStringLiteral("no")); // no cursor-autohide, we handle that
    setOption(QStringLiteral("sub-auto"), QStringLiteral("fuzzy")); // Automatic subfile detection

    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");

    ///设置要监听mpv的属性变化事件
    mpv_observe_property(mpv, 0, "percent-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "ao-volume", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "volume", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "ao-mute", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "time-remaining", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "playtime-remaining", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "speed", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "core-idle", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "paused-for-cache", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "hwdec", MPV_FORMAT_STRING);
    mpv_observe_property(mpv, 0, "playlist-playing-pos", MPV_FORMAT_STRING);
    mpv_observe_property(mpv, 0, "osd-dimensions", MPV_FORMAT_NODE);


    //int64_t wid = this->winId();
    //mpv_set_option(mpv, "wid", MPV_FORMAT_INT64, &wid);


    // Request hw decoding, just for testing.
    mpv::qt::set_option_variant(mpv, "hwdec", "auto");


    mpv_set_wakeup_callback(mpv, wakeup, this);
}

void MpvPlayerWidget::initializeGL()
{
    mpv_opengl_init_params gl_init_params{get_proc_address, nullptr, nullptr};
    mpv_render_param params[]{
        {MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };

    if (mpv_render_context_create(&mpv_gl, mpv, params) < 0)
        throw std::runtime_error("failed to initialize mpv GL context");
    mpv_render_context_set_update_callback(mpv_gl, MpvPlayerWidget::on_update, reinterpret_cast<void *>(this));
}

void MpvPlayerWidget::paintGL()
{
    mpv_opengl_fbo mpfbo{static_cast<int>(defaultFramebufferObject()), width(), height(), 0};
    int flip_y{1};

    mpv_render_param params[] = {
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
        {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };
    // See render_gl.h on what OpenGL environment mpv expects, and
    // other API details.
    mpv_render_context_render(mpv_gl, params);
}

void MpvPlayerWidget::loadFileInfo()
{
    // get media-title
    fileInfo.media_title = getProperty(QStringLiteral("media-title")).toString();
    // get length
    fileInfo.length = getProperty(QStringLiteral("duration")).toDouble() * 1000;
    // get Path
    fileInfo.media_absolutePath = getProperty(QStringLiteral("path")).toString();

    loadVideoParams();
    loadAudioParams();
    loadMetadata();

    loadedFlag = true;

    emit(fileInfoChanged(fileInfo));
}
void MpvPlayerWidget::loadVideoParams()
{
    fileInfo.video_params.codec = getProperty(QStringLiteral("video-codec")).toString();
    fileInfo.video_params.width = getProperty(QStringLiteral("width")).toInt();
    fileInfo.video_params.height = getProperty(QStringLiteral("height")).toInt();
    fileInfo.video_params.dwidth = getProperty(QStringLiteral("dwidth")).toInt();
    fileInfo.video_params.dheight = getProperty(QStringLiteral("dheight")).toInt();
    fileInfo.video_params.aspect = getProperty(QStringLiteral("video-aspect")).toDouble();
    fileInfo.video_params.estimated_frames = getProperty(QStringLiteral("estimated-frame-count")).toInt();
    fileInfo.video_params.frameRates = getProperty(QStringLiteral("container-fps")).toDouble();

    estimatedFrames = fileInfo.video_params.estimated_frames;

    emit(videoParamsChanged(fileInfo.video_params));
    emit(frameCountChanged(estimatedFrames));
}

void MpvPlayerWidget::loadAudioParams()
{
    fileInfo.audio_params.codec = getProperty(QStringLiteral("audio-codec")).toString();
    mpv_node node;
    mpv_get_property(mpv, "audio-params", MPV_FORMAT_NODE, &node);
    if(node.format == MPV_FORMAT_NODE_MAP)
    {
        for(int i = 0; i < node.u.list->num; i++)
        {
            if(QString(node.u.list->keys[i]) == QLatin1String("samplerate"))
            {
                if(node.u.list->values[i].format == MPV_FORMAT_INT64)
                    fileInfo.audio_params.samplerate = node.u.list->values[i].u.int64;
            }
            else if(QString(node.u.list->keys[i]) == QLatin1String("channel-count"))
            {
                if(node.u.list->values[i].format == MPV_FORMAT_INT64)
                    fileInfo.audio_params.channels = node.u.list->values[i].u.int64;
            }
        }
    }

    emit(audioParamsChanged(fileInfo.audio_params));
}

void MpvPlayerWidget::loadMetadata()
{
    fileInfo.metadata.clear();
    mpv_node node;
    mpv_get_property(mpv, "metadata", MPV_FORMAT_NODE, &node);
    if(node.format == MPV_FORMAT_NODE_MAP)
        for(int n = 0; n < node.u.list->num; n++)
            if(node.u.list->values[n].format == MPV_FORMAT_STRING)
                fileInfo.metadata[node.u.list->keys[n]] = node.u.list->values[n].u.string;
}

void MpvPlayerWidget::loadOsdDimensionsParams()
{
    osdDimensionsInfo.osd_aspect = getProperty(QStringLiteral("osd-dimensions/aspect")).toDouble();
    osdDimensionsInfo.osd_width = getProperty(QStringLiteral("osd-dimensions/w")).toDouble();
    osdDimensionsInfo.osd_height = getProperty(QStringLiteral("osd-dimensions/h")).toDouble();
    osdDimensionsInfo.osd_margins_left = getProperty(QStringLiteral("osd-dimensions/ml")).toDouble();
    osdDimensionsInfo.osd_margins_top = getProperty(QStringLiteral("osd-dimensions/mt")).toDouble();
    osdDimensionsInfo.osd_margins_right = getProperty(QStringLiteral("osd-dimensions/mr")).toDouble();
    osdDimensionsInfo.osd_margins_bottom = getProperty(QStringLiteral("osd-dimensions/mb")).toDouble();

    emit(osd_dimensionsChanged());
}

void MpvPlayerWidget::unloadFileInfo()
{
    loadedFlag = false;
    fileInfo = Mpv::FileInfo();

    currentFrameNumber = -1;
    position = -1;
    duration = -1;

}

QString MpvEventString::EventString(mpv_event_id eventId)
{
    switch (eventId) {
    case MPV_EVENT_START_FILE:
        return "MPV_EVENT_START_FILE";
    case MPV_EVENT_END_FILE:
        return "MPV_EVENT_END_FILE";
    case MPV_EVENT_FILE_LOADED:
        return "MPV_EVENT_FILE_LOADED";
    case MPV_EVENT_IDLE:
        return "MPV_EVENT_IDLE";
    case MPV_EVENT_PAUSE:
        return "MPV_EVENT_PAUSE";
    case MPV_EVENT_UNPAUSE:
        return "MPV_EVENT_UNPAUSE";
    case MPV_EVENT_VIDEO_RECONFIG:
        return "MPV_EVENT_VIDEO_RECONFIG";
    case MPV_EVENT_AUDIO_RECONFIG:
        return "MPV_EVENT_AUDIO_RECONFIG";
    case MPV_EVENT_METADATA_UPDATE:
        return "MPV_EVENT_METADATA_UPDATE";
    case MPV_EVENT_SEEK:
        return "MPV_EVENT_SEEK";
    case MPV_EVENT_PLAYBACK_RESTART:
        return "MPV_EVENT_PLAYBACK_RESTART";
    default:
        return QString("UNSET %1").arg(int(eventId));
    }
}
