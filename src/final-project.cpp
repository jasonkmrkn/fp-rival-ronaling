#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <winsock2.h>
#include <opencv2/opencv.hpp>
#include "communicate.h"
#include "qrcode.h"
#include "objDetect.h"

#pragma comment(lib, "ws2_32.lib")
using namespace std;
using namespace cv;

/// CONFIGURATION ///////////////////////////////////////////////////////////////////////////////////////////
int camera = 0;
string videopath = "C:\\Robotics\\RIVAL\\Projects\\final-project-magang\\final-project\\sample\\qr.mp4";
string class_txt_path = "C:\\Robotics\\RIVAL\\Projects\\final-project-magang\\final-project\\models\\objclass.txt";
string model_path = "C:\\Robotics\\RIVAL\\Projects\\final-project-magang\\final-project\\models\\best_simplify.onnx";
bool is_cuda = true;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

vector<string> load_class_list()
{
	vector<string> class_list;
	ifstream ifs(class_txt_path);
	string line;
	while (getline(ifs, line))
	{
		class_list.push_back(line);
	}
	return class_list;
}

void load_net(cv::dnn::Net& net, bool is_cuda)
{
	auto result = cv::dnn::readNet(model_path);
	if (result.empty())
	{
		std::cout << "Can't read model!\n";
		return;
	}
	if (is_cuda)
	{
		std::cout << "Attempty to use CUDA\n";
		result.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
		result.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA_FP16);
	}
	else
	{
		std::cout << "Running on CPU\n";
		result.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
		result.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
	}
	cout << "Model loaded!\n";
	net = result;
}


int main()
{
	ESP32Communicator esp;
	try {
		esp.initialize("192.168.4.1", 8080);
		esp.connectToServer();
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return -1;
	}
	VideoCapture cap(camera);
	if (!cap.isOpened())
	{
		cerr << "Couldn't open camera!" << endl;
	}

	dnn::Net net;
	vector<string> class_list = load_class_list();
	load_net(net, is_cuda);

	Mat frame;
	int current_stage = 1;
	string temp_stage;
	int currentObj;

	while (cap.isOpened())
	{
		cap >> frame;
		
		switch (current_stage)
		{
		case 1:
			switch (scanObj(frame, class_list, net, currentObj))
			{
			case 1 :
				esp.sendCommand("R");
				this_thread::sleep_for(chrono::seconds(3));
				break;
			case -1 :
				esp.sendCommand("L");
				this_thread::sleep_for(chrono::seconds(3));
				break;
			case 0 :
				esp.sendCommand("A");
				current_stage = 2;
				this_thread::sleep_for(chrono::seconds(15));
				break;
			}
			break;

		case 2:
			switch (scanQR(frame, class_list[currentObj]))
			{
			case 1 :
				esp.sendCommand("R");
				this_thread::sleep_for(chrono::seconds(3));
				break;
			case -1:
				esp.sendCommand("L");
				this_thread::sleep_for(chrono::seconds(3));
				break;
			case 0:
				esp.sendCommand("T");
				current_stage = 1;
				this_thread::sleep_for(chrono::seconds(15));
				break;
			}
			break;
		}
		imshow("ronaling cam", frame);
		if (waitKey(30) == 27) break;
	}
	return 0;
}


