#ifndef CAMERAWORKER_H
#define CAMERAWORKER_H

#include <QObject>
#include <QTimer>
#include <QImage>
#include <QString>
#include "cameraengine.h"
#include "handdetector.h"
#include "handdetectorai.h"
#include "framerenderer.h"

class CameraWorker : public QObject
{
    Q_OBJECT

public:
    explicit CameraWorker(QObject *parent = nullptr);
    ~CameraWorker();

signals:
    void frameReady(QImage mainImg, QImage debugImg, QString msg, int fingers);

public slots:
    void startWork();
    void stopWork();

private slots:
    void process();

private:
    CameraEngine  m_engine;
    HandDetector  m_detector;   // فقط تشخیص
    HandDetectorAI m_detectorAI; // AI Detection
    FrameRenderer m_renderer;   // فقط نمایش
    QTimer *m_timer;
};

#endif // CAMERAWORKER_H
