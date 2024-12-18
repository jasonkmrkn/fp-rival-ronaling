#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "objDetect.h"

using namespace std;
using namespace cv;

const std::vector<cv::Scalar> colors = { cv::Scalar(255, 255, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 255), cv::Scalar(255, 0, 0) };

const float INPUT_WIDTH = 640.0;
const float INPUT_HEIGHT = 640.0;
const float SCORE_THRESHOLD = 0.2;
const float NMS_THRESHOLD = 0.4;
const float CONFIDENCE_THRESHOLD = 0.4;

struct Detection
{
    int class_id;
    float confidence;
    cv::Rect box;
};

cv::Mat format_yolov5(const cv::Mat& source) {
    int col = source.cols;
    int row = source.rows;
    int _max = MAX(col, row);
    cv::Mat result = cv::Mat::zeros(_max, _max, CV_8UC3);
    source.copyTo(result(cv::Rect(0, 0, col, row)));
    return result;
}

void detect(cv::Mat& image, cv::dnn::Net& net, std::vector<Detection>& output, const std::vector<std::string>& className) {
    cv::Mat blob;

    auto input_image = format_yolov5(image);

    cv::dnn::blobFromImage(input_image, blob, 1.0 / 255.0, cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(), true, false);
    net.setInput(blob);
    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    float x_factor = input_image.cols / INPUT_WIDTH;
    float y_factor = input_image.rows / INPUT_HEIGHT;

    if (outputs.empty()) {
        std::cerr << "Error: No output from network\n";
        return;
    }

    float* data = (float*)outputs[0].data;

    const int dimensions = 7;
    const int rows = 25200;

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (int i = 0; i < rows; ++i) {

        float confidence = data[4];

        if (confidence >= CONFIDENCE_THRESHOLD) {

            float* classes_scores = data + 5;
            cv::Mat scores(1, className.size(), CV_32FC1, classes_scores);
            cv::Point class_id;
            double max_class_score;
            minMaxLoc(scores, 0, &max_class_score, 0, &class_id);
            if (max_class_score > SCORE_THRESHOLD) {

                confidences.push_back(confidence);

                class_ids.push_back(class_id.x);
                
                float x = data[0];
                float y = data[1];
                float w = data[2];
                float h = data[3];
                int left = int((x - 0.5 * w) * x_factor);
                int top = int((y - 0.5 * h) * y_factor);
                int width = int(w * x_factor);
                int height = int(h * y_factor);
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
        data += 7;
    }

    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, nms_result);
    for (int i = 0; i < nms_result.size(); i++) {
        int idx = nms_result[i];
        Detection result;
        result.class_id = class_ids[idx];
        result.confidence = confidences[idx];
        result.box = boxes[idx];
        output.push_back(result);
    }
}

int scanObj(Mat &frame, vector<string> &class_list, dnn::Net &net, int &currentObj)
{
    std::vector<Detection> output;
    detect(frame, net, output, class_list);

    if (output.empty())
    {
        cout << "Tidak ada objek" << endl;
        return 1;
    }

    int detections = output.size();
    for (int i = 0; i < detections; ++i)
    {
        Point2f tl_center_box((frame.cols / 2) - 25, 0);
        Point2f br_center_box((frame.cols / 2) + 25, frame.rows);
        Rect2f center_box(tl_center_box, br_center_box);

        auto detection = output[i];
        auto box = detection.box;
        auto classId = detection.class_id;
        const auto color = colors[classId % colors.size()];
        cv::rectangle(frame, box, color, 3);
        cv::rectangle(frame, cv::Point(box.x, box.y - 20), cv::Point(box.x + box.width, box.y), color, cv::FILLED);
        cv::putText(frame, class_list[classId].c_str(), cv::Point(box.x, box.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
        rectangle(frame, center_box, Scalar(255, 255, 255), 3);
        float obj_center = (box.tl().x + ((box.br().x - box.tl().x)/2));

        if (obj_center > br_center_box.x)
        {
            cout << class_list[classId] << " ada di sebelah kanan robot." << endl;
            return 1;
        }
        else if (obj_center < tl_center_box.x)
        {
            cout << class_list[classId] << " ada di sebelah kiri robot." << endl;
            return -1;
        }
        else if ((obj_center >= tl_center_box.x) && obj_center <= br_center_box.x)
        {
            cout << class_list[classId] << " ada di tengah robot" << endl;
            currentObj = classId;
            return 0;
        }
    }
}