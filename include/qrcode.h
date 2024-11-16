#ifndef QRCODE_H
#define QRCODE_H

#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int scanQR(Mat& frame, string &currentObj);

#endif 
