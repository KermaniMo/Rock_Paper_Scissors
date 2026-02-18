#ifndef APPCONSTANTS_H
#define APPCONSTANTS_H

#include <opencv2/opencv.hpp>

// ثابت‌های مشترک اپلیکیشن — یکجا تعریف، همه‌جا استفاده
namespace AppConstants {

    // ---------- ROI ----------
    constexpr int ROI_SIZE = 200;

    // محاسبه موقعیت ROI (چسبیده به راست، وسط عمودی)
    inline cv::Rect getROI(int frameWidth, int frameHeight) {
        int x = frameWidth - ROI_SIZE;
        int y = (frameHeight - ROI_SIZE) / 2;

        // clamp
        if (x < 0) x = 0;
        if (y < 0) y = 0;

        return cv::Rect(x, y, ROI_SIZE, ROI_SIZE);
    }

    // ---------- Detection Thresholds ----------
    constexpr double MIN_CONTOUR_AREA = 1000.0;
    constexpr float  MIN_DEFECT_DEPTH = 10.0f;
    constexpr double MAX_DEFECT_ANGLE = 90.0;
    constexpr int    MAX_FINGERS      = 5;

} // namespace AppConstants

#endif // APPCONSTANTS_H
