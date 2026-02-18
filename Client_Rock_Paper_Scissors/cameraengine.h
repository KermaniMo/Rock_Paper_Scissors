#ifndef CAMERAENGINE_H
#define CAMERAENGINE_H

#include <opencv2/opencv.hpp>

class CameraEngine
{
public:
    CameraEngine() = default;
    ~CameraEngine();

    bool open(int id);
    void close();

    // گرفتن فریم BGR تازه از دوربین
    cv::Mat getCurrentFrame();

private:
    void capture();

    cv::VideoCapture m_cap;
    cv::Mat m_currentFrame;
};

#endif // CAMERAENGINE_H
