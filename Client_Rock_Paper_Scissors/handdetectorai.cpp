#include "handdetectorai.h"
#include "appconstants.h" // اضافه شد برای دسترسی به getROI
#include <QDebug>
#include <cmath>

HandDetectorAI::HandDetectorAI()
{
    // مطمئن شوید فایل کنار فایل اجرایی است
    loadModel("hand_landmark_sparse_Nx3x224x224.onnx");
}

void HandDetectorAI::loadModel(const std::string &modelPath)
{
    try {
        m_net = cv::dnn::readNetFromONNX(modelPath);
        m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        m_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        m_modelLoaded = !m_net.empty();

        if (m_modelLoaded) {
            qDebug() << "AI Loaded Successfully from:" << QString::fromStdString(modelPath);
        } else {
            qWarning() << "AI Error: Model file empty!";
        }
    } catch (const cv::Exception &e) {
        qWarning() << "AI Exception:" << e.what();
        m_modelLoaded = false;
    }
}

float HandDetectorAI::getDistance(const cv::Point2f &p1, const cv::Point2f &p2) const
{
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

DetectionResult HandDetectorAI::detect(const cv::Mat &frame)
{
    DetectionResult result;
    result.handFound = false;
    result.fingerCount = 0;
    result.gesture = "Unknown";

    if (frame.empty() || !m_modelLoaded) return result;

    // 1. برش تصویر (ROI)
    cv::Rect roiRect = AppConstants::getROI(frame.cols, frame.rows);
    if (roiRect.x < 0 || roiRect.width <= 0) return result;
    cv::Mat roiFrame = frame(roiRect).clone();

    // 2. آماده‌سازی ورودی
    cv::Mat inputBlob;
    // Keep [-1, 1] for now as it produced non-zero/non-tiny values, but we need to check layers.
    cv::dnn::blobFromImage(roiFrame, inputBlob, 1.0 / 127.5, cv::Size(224, 224), cv::Scalar(127.5, 127.5, 127.5), true, false);

    m_net.setInput(inputBlob);
    
    // DEBUG: Get ALL outputs (Silent Search)
    std::vector<std::string> outNames = m_net.getUnconnectedOutLayersNames();
    std::vector<cv::Mat> outs;
    m_net.forward(outs, outNames);

    if (outs.empty()) return result;

    cv::Mat landmarksBlob;
    bool foundLandmarks = false;

    for (size_t i = 0; i < outs.size(); ++i) {
        if (outs[i].total() == 63) {
            landmarksBlob = outs[i];
            foundLandmarks = true;
            break;
        }
    }

    if (!foundLandmarks) {
        landmarksBlob = outs[0]; // Fallback
    }
    
    float* data = (float*)landmarksBlob.data;
    std::vector<cv::Point2f> landmarks;

    // 3. استخراج مختصات (مدل PINTO: 21 نقطه سه‌بعدی)
    for (int i = 0; i < 21; ++i) {
        float x_raw = data[i * 3];
        float y_raw = data[i * 3 + 1];
        
        // FIX: Model outputs 224x224 pixel coordinates. Normalize by 224.
        float x_norm = x_raw / 224.0f;
        float y_norm = y_raw / 224.0f;

        float x = x_norm * roiRect.width;
        float y = y_norm * roiRect.height;
        
        landmarks.push_back(cv::Point2f(x, y));
    }
    result.landmarks = landmarks;

    // 4. بررسی کف دست (فیلتر نویز)
    float palmSize = getDistance(landmarks[0], landmarks[9]);

    // Threshold: 30 pixels (Valid hand was ~63)
    if (palmSize < 30.0f) {
        result.gesture = "Too Far";
        return result;
    }

    result.handFound = true;
    cv::Point2f wrist = landmarks[0];

    // 5. منطق جدید سخت‌گیرانه
    auto isFingerOpen = [&](int tip, int pip, int mcp) {
        float dTip = getDistance(landmarks[tip], wrist);
        float dPip = getDistance(landmarks[pip], wrist);
        float dMcp = getDistance(landmarks[mcp], wrist);
        float margin = std::max(5.0f, palmSize * 0.15f); 
        return (dTip > dPip + margin) && (dTip > dMcp + margin);
    };

    bool index  = isFingerOpen(8, 6, 5);
    bool middle = isFingerOpen(12, 10, 9);
    bool ring   = isFingerOpen(16, 14, 13);
    bool pinky  = isFingerOpen(20, 18, 17);

    // Thumb ignored for R/P/S stability
    int openCount = 0;
    if (index)  openCount++;
    if (middle) openCount++;
    if (ring)   openCount++;
    if (pinky)  openCount++;

    result.fingerCount = openCount;

    // 6. تعیین حرکت نهایی
    if (openCount == 0) {
        result.gesture = "Rock";
    } else if (openCount == 4) {
        result.gesture = "Paper";
    } else if (index && middle && !ring && !pinky) {
        result.gesture = "Scissors";
    } else {
        result.gesture = "Unknown";
    }

    return result;
}


