#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include "qrcode.h"

using namespace std;
using namespace cv;

// Draw bounding box and scanned qr string
void drawQR(Mat &frame, Mat &qr_points, string &text)
{
	Point2f tl = qr_points.at<Point2f>(0);
	Point2f br = qr_points.at<Point2f>(2);
	Point2f qr_txt_point = {qr_points.at<Point2f>(0).x, qr_points.at<Point2f>(0).y - 10};

	if (text == "PENJARA BIRU")
	{
		rectangle(frame, tl, br, Scalar(255, 0, 0), 2);
		putText(frame, text, qr_txt_point, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 2);
	}
	else if (text == "PENJARA PUTIH")
	{
		rectangle(frame, tl, br, Scalar(255, 255, 255), 2);
		putText(frame, text, qr_txt_point, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 2);
	}
}

// Scan any qr code that appears
int scanQR(Mat &frame, string &currentObj)
{
	QRCodeDetector qrcode;
	Mat qr_points;

	frame.convertTo(frame, -1, 1, 20);
	string qr_string = qrcode.detectAndDecode(frame, qr_points);

	if (!qr_points.empty() && (qr_points.total() == 4)) 
	{
		drawQR(frame, qr_points, qr_string);

		string qr_target;
		if (currentObj == "obj-Biru")
		{
			qr_target = "PENJARA BIRU";
		}
		else
		{
			qr_target = "PENJARA PUTIH";
		}

		Point2f tl_center_box((frame.cols / 2) - 30, 0);
		Point2f br_center_box((frame.cols / 2) + 30, frame.rows);
		Rect2f center_box(tl_center_box, br_center_box);
		float center_frame_x = frame.cols / 2;
		float center_qr_x = qr_points.at<Point2f>(0).x + ((qr_points.at<Point2f>(1).x - qr_points.at<Point2f>(0).x) / 2);
		rectangle(frame, center_box, Scalar(255, 255, 255), 3);

		if (qr_target == qr_string)
		{
			if (center_qr_x > (center_frame_x + 25)) 
			{
				cout << "qr ada di kanan" << endl;
				cout << "QR DETECTED: " << qr_string << endl;
				return 1;
			}
			else if (center_qr_x < (center_frame_x - 25)) 
			{
				cout << "qr ada di kiri" << endl;
				cout << "QR DETECTED: " << qr_string << endl;
				return -1;
			}
			else if ((center_qr_x >= center_frame_x - 25) && (center_qr_x <= center_frame_x + 25))
			{
				cout << "qr ada ditengah" << endl;
				cout << "QR DETECTED: " << qr_string << endl;
				return 0;
			}
		}
		else
		{
			cout << "bukan qr yang dituju" << endl;
			return 1;
		}
	}
	else
	{
		cout << "Tidak ada qr" << endl;
		return 1;
	}
}