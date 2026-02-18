#ifndef HANDDETECTOR_H
#define HANDDETECTOR_H

#include <QString>
#include <opencv2/opencv.hpp>

// نتیجه تشخیص دست — خروجی خالص بدون هیچ رندری
struct DetectionResult {
    QString gesture;       // "Rock (Sang)" / "Scissors (Gheychi)" / "Paper (Kaghaz)"
    int fingerCount = 0;
    bool handFound = false;

    // داده‌های خام برای FrameRenderer (اختیاری — اگه دیباگ بخواد نمایش بده)
    std::vector<cv::Point> contour;
    std::vector<std::vector<cv::Point>> hullPoints;
    std::vector<cv::Vec4i> defects;
    
    // AI Landmarks (21 points)
    std::vector<cv::Point2f> landmarks;
};

class HandDetector
{
public:
    HandDetector() = default;

    // فقط تشخیص — هیچ رسمی انجام نمیده، هیچ QImage‌ای نمیسازه
    DetectionResult detect(const cv::Mat &frame);

private:
    int findLargestContour(const std::vector<std::vector<cv::Point>> &contours) const;
    int countFingers(const std::vector<cv::Point> &contour,
                     const std::vector<cv::Vec4i> &defects) const;
    QString getGestureName(int count) const;
};

#endif // HANDDETECTOR_H
