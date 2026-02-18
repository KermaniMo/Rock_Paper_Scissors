#ifndef FRAMERENDERER_H
#define FRAMERENDERER_H

#include <QImage>
#include <opencv2/opencv.hpp>
#include "handdetector.h"

// مسئول نمایش — رسم ROI، کانتور، هال، تبدیل به QImage
class FrameRenderer
{
public:
    FrameRenderer() = default;

    // ساخت تصویر اصلی (با مربع سبز ROI و اطلاعات دیباگ)
    QImage renderMain(const cv::Mat &frame, const DetectionResult &result);

    // ساخت تصویر دیباگ (ماسک باینری ناحیه ROI)
    QImage renderDebug(const cv::Mat &frame);

private:
    // رسم اطلاعات دیباگ (کانتور، هال، نقاط فرو رفتگی)
    void drawOverlay(cv::Mat &frame,
                     const DetectionResult &result,
                     const cv::Rect &roi);

    // تبدیل cv::Mat به QImage
    QImage matToQImage(const cv::Mat &mat) const;
};

#endif // FRAMERENDERER_H
