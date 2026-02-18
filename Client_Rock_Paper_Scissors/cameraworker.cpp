#include "cameraworker.h"

CameraWorker::CameraWorker(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &CameraWorker::process);
}

CameraWorker::~CameraWorker()
{
    stopWork();
}

void CameraWorker::startWork()
{
    if (!m_engine.open(0)) {
        return;
    }
    m_timer->start(60);
}

void CameraWorker::stopWork()
{
    m_timer->stop();
    m_engine.close();
}

void CameraWorker::process()
{
    cv::Mat frame = m_engine.getCurrentFrame();
    if (frame.empty()) {
        return;
    }

    /*
    // ۱. تشخیص (بدون رسم) --- OLD HandDetector
    DetectionResult result = m_detector.detect(frame);
    */

    // 1. AI Detection
    DetectionResult result = m_detectorAI.detect(frame);

    // ۲. رندر (بدون تشخیص)
    // Note: renderMain might expect contours which are not populated by HandDetectorAI.
    // It will just draw logic based on empty contours (likely just ROI).
    QImage mainImg  = m_renderer.renderMain(frame, result);
    QImage debugImg = m_renderer.renderDebug(frame);

    if (!mainImg.isNull()) {
        emit frameReady(mainImg, debugImg, result.gesture, result.fingerCount);
    }
}
