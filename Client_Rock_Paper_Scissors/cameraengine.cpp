#include "cameraengine.h"

CameraEngine::~CameraEngine()
{
    close();
}

bool CameraEngine::open(int id)
{
    if (m_cap.isOpened()) {
        close();
    }

    m_cap.open(id);
    return m_cap.isOpened();
}

void CameraEngine::close()
{
    if (m_cap.isOpened()) {
        m_cap.release();
    }
    m_currentFrame.release();
}

void CameraEngine::capture()
{
    if (!m_cap.isOpened()) {
        return;
    }

    cv::Mat frame;
    m_cap >> frame;

    if (!frame.empty()) {
        m_currentFrame = frame;
    }
}

cv::Mat CameraEngine::getCurrentFrame()
{
    capture();
    return m_currentFrame;
}
