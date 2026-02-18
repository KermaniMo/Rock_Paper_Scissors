#ifndef HANDDETECTORAI_H
#define HANDDETECTORAI_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include "handdetector.h" // For DetectionResult struct

class HandDetectorAI
{
public:
    HandDetectorAI();

    // Loads the ONNX model
    void loadModel(const std::string &modelPath);

    // Main detection method
    DetectionResult detect(const cv::Mat &frame);

private:
    cv::dnn::Net m_net;
    bool m_modelLoaded;

    // Helper: Calculate Euclidean distance
    float getDistance(const cv::Point2f &p1, const cv::Point2f &p2) const;
};

#endif // HANDDETECTORAI_H
