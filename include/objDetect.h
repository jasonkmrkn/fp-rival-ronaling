#ifndef OBJDETECT_H
#define OBJDETECT_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
using namespace cv;

int scanObj(Mat &frame, std::vector<std::string> &class_list, dnn::Net &net, int &currentObj);

#endif // !OBJDETECT_H
