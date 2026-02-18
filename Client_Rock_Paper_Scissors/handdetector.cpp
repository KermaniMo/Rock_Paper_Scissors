#include "handdetector.h"
#include "appconstants.h"
#include <cmath>

// =====================================================================
//  تشخیص دست — فقط آنالیز، بدون رسم
// =====================================================================
DetectionResult HandDetector::detect(const cv::Mat &frame)
{
    DetectionResult result;

    if (frame.empty()) {
        return result;
    }

    // --- ROI از ثابت‌ها ---
    cv::Rect roi = AppConstants::getROI(frame.cols, frame.rows);
    cv::Mat roiFrame = frame(roi);

    // --- ساخت ماسک باینری ---
    cv::Mat gray;
    cv::cvtColor(roiFrame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(7, 7), 0);
    cv::threshold(gray, gray, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

    // --- پیدا کردن کانتورها ---
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(gray.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    int largestIdx = findLargestContour(contours);
    if (largestIdx < 0) {
        result.gesture = "No hand detected";
        return result;
    }

    result.handFound = true;
    result.contour = contours[largestIdx];

    // --- Convex Hull و Defects ---
    std::vector<int> hullIndices;
    cv::convexHull(result.contour, hullIndices, false, false);

    result.hullPoints.resize(1);
    cv::convexHull(result.contour, result.hullPoints[0], false, true);

    if (hullIndices.size() > 3) {
        cv::convexityDefects(result.contour, hullIndices, result.defects);
    }

    // --- شمارش و تشخیص ---
    result.fingerCount = countFingers(result.contour, result.defects);
    result.gesture = getGestureName(result.fingerCount);

    return result;
}

// =====================================================================
//  پیدا کردن بزرگترین کانتور
// =====================================================================
int HandDetector::findLargestContour(const std::vector<std::vector<cv::Point>> &contours) const
{
    int largestIdx = -1;
    double maxArea = 0;

    for (int i = 0; i < static_cast<int>(contours.size()); i++) {
        double area = cv::contourArea(contours[i]);
        if (area > maxArea) {
            maxArea = area;
            largestIdx = i;
        }
    }

    if (maxArea < AppConstants::MIN_CONTOUR_AREA) {
        return -1;
    }

    return largestIdx;
}

// =====================================================================
//  شمارش انگشت
// =====================================================================
int HandDetector::countFingers(const std::vector<cv::Point> &contour,
                               const std::vector<cv::Vec4i> &defects) const
{
    if (defects.empty()) {
        return 0;
    }

    int fingerGaps = 0;

    for (const auto &defect : defects) {
        cv::Point start = contour[defect[0]];
        cv::Point end   = contour[defect[1]];
        cv::Point far   = contour[defect[2]];
        float depth = defect[3] / 256.0f;

        if (depth < AppConstants::MIN_DEFECT_DEPTH) {
            continue;
        }

        double a = cv::norm(start - far);
        double b = cv::norm(end - far);
        double c = cv::norm(start - end);
        double angle = std::acos((a * a + b * b - c * c) / (2.0 * a * b));
        double angleDeg = angle * 180.0 / CV_PI;

        if (angleDeg > AppConstants::MAX_DEFECT_ANGLE) {
            continue;
        }

        fingerGaps++;
    }

    int fingers = fingerGaps + 1;
    if (fingers > AppConstants::MAX_FINGERS) {
        fingers = AppConstants::MAX_FINGERS;
    }

    return fingers;
}

// =====================================================================
//  تشخیص نام حرکت
// =====================================================================
QString HandDetector::getGestureName(int count) const
{
    if (count < 2) {
        return "Rock (Sang)";
    } else if (count == 2) {
        return "Scissors (Gheychi)";
    } else {
        return "Paper (Kaghaz)";
    }
}
