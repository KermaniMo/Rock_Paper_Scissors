#include "framerenderer.h"
#include "appconstants.h"
#include <cmath>

// =====================================================================
//  ساخت تصویر اصلی (با ROI و اطلاعات دیباگ)
// =====================================================================
QImage FrameRenderer::renderMain(const cv::Mat &frame, const DetectionResult &result)
{
    if (frame.empty()) {
        return QImage();
    }

    cv::Mat display = frame.clone();
    cv::Rect roi = AppConstants::getROI(frame.cols, frame.rows);

    // مربع سبز ROI
    cv::rectangle(display, roi, cv::Scalar(0, 255, 0), 2);

    // اگه دست پیدا شده، اطلاعات دیباگ رسم کن
    if (result.handFound) {
        drawOverlay(display, result, roi);
    }

    // BGR → RGB
    cv::cvtColor(display, display, cv::COLOR_BGR2RGB);
    return matToQImage(display);
}

// =====================================================================
//  ساخت تصویر دیباگ (ماسک باینری ROI)
// =====================================================================
QImage FrameRenderer::renderDebug(const cv::Mat &frame)
{
    if (frame.empty()) {
        return QImage();
    }

    cv::Rect roi = AppConstants::getROI(frame.cols, frame.rows);
    cv::Mat roiFrame = frame(roi);

    cv::Mat gray;
    cv::cvtColor(roiFrame, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(7, 7), 0);
    cv::threshold(gray, gray, 0, 255, cv::THRESH_BINARY_INV + cv::THRESH_OTSU);

    return matToQImage(gray);
}

// =====================================================================
//  رسم اطلاعات دیباگ روی فریم
// =====================================================================
void FrameRenderer::drawOverlay(cv::Mat &frame,
                                 const DetectionResult &result,
                                 const cv::Rect &roi)
{
    cv::Point offset(roi.x, roi.y);

    // --- کانتور دست (آبی) ---
    std::vector<std::vector<cv::Point>> shiftedContour(1);
    for (const auto &pt : result.contour) {
        shiftedContour[0].push_back(pt + offset);
    }
    cv::drawContours(frame, shiftedContour, 0, cv::Scalar(255, 0, 0), 2);

    // --- Convex Hull (سبز) ---
    if (!result.hullPoints.empty()) {
        std::vector<std::vector<cv::Point>> shiftedHull(1);
        for (const auto &pt : result.hullPoints[0]) {
            shiftedHull[0].push_back(pt + offset);
        }
        cv::drawContours(frame, shiftedHull, 0, cv::Scalar(0, 255, 0), 2);
    }

    // --- نقاط Defect (قرمز + زرد) ---
    for (const auto &defect : result.defects) {
        float depth = defect[3] / 256.0f;
        if (depth < AppConstants::MIN_DEFECT_DEPTH) continue;

        cv::Point start = result.contour[defect[0]];
        cv::Point end   = result.contour[defect[1]];
        cv::Point far   = result.contour[defect[2]];

        double a = cv::norm(start - far);
        double b = cv::norm(end - far);
        double c = cv::norm(start - end);
        double angle = std::acos((a * a + b * b - c * c) / (2.0 * a * b));
        if (angle * 180.0 / CV_PI > AppConstants::MAX_DEFECT_ANGLE) continue;

        // دایره قرمز توپر (فرو رفتگی)
        cv::circle(frame, far + offset, 6, cv::Scalar(0, 0, 255), -1);

        // دایره زرد (نوک انگشت)
        cv::circle(frame, start + offset, 4, cv::Scalar(0, 255, 255), -1);
        cv::circle(frame, end + offset, 4, cv::Scalar(0, 255, 255), -1);
    }

    // --- AI Landmarks (Skeleton) ---
    if (!result.landmarks.empty()) {
        cv::Point2f offsetF(offset.x, offset.y);
        // Draw points
        for (const auto &pt : result.landmarks) {
            cv::circle(frame, cv::Point(pt + offsetF), 3, cv::Scalar(0, 0, 255), -1);
        }

        // Draw connections (Skeleton)
        // MediaPipe Hand Connections
        const std::vector<std::pair<int, int>> connections = {
            {0, 1}, {1, 2}, {2, 3}, {3, 4},       // Thumb
            {0, 5}, {5, 6}, {6, 7}, {7, 8},       // Index
            {0, 9}, {9, 10}, {10, 11}, {11, 12},  // Middle
            {0, 13}, {13, 14}, {14, 15}, {15, 16},// Ring
            {0, 17}, {17, 18}, {18, 19}, {19, 20} // Pinky
        };

        for (const auto &conn : connections) {
            if (conn.first < result.landmarks.size() && conn.second < result.landmarks.size()) {
                cv::line(frame, 
                         cv::Point(result.landmarks[conn.first] + offsetF), 
                         cv::Point(result.landmarks[conn.second] + offsetF), 
                         cv::Scalar(255, 255, 0), 1);
            }
        }
    }
}

// =====================================================================
//  تبدیل cv::Mat به QImage
// =====================================================================
QImage FrameRenderer::matToQImage(const cv::Mat &mat) const
{
    if (mat.empty()) {
        return QImage();
    }

    if (mat.channels() == 1) {
        QImage image(mat.data, mat.cols, mat.rows,
                     static_cast<int>(mat.step), QImage::Format_Grayscale8);
        return image.copy();
    } else if (mat.channels() == 3) {
        QImage image(mat.data, mat.cols, mat.rows,
                     static_cast<int>(mat.step), QImage::Format_RGB888);
        return image.copy();
    }

    return QImage();
}
