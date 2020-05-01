#pragma once

#include <opencv2/opencv.hpp>
#include <memory>

#include "simple_gui_fltk.h"

// very simple Wrapper Class cv::VideoCapture - only needed to store the camera device index
class Camera : public cv::VideoCapture
{
protected:
	int index = -1;
	std::string file_name;
public:
	Camera(std::string fname) : cv::VideoCapture(fname)
	{
		file_name = fname;
	}

	Camera(int idx, cv::VideoCaptureAPIs backend = cv::CAP_ANY) : cv::VideoCapture(idx, backend)
	{
		index = idx;
	}

	void print_info();
	int get_index() { return index; }
};

class Camera_control
{
protected:
	double focus = 0.0, old_focus = 0.0;
	double exposure = 0.0, old_exposure = 0.0;
	double gain = 0.0, old_gain = 0.0;
	int id = 0;
	Simple_gui sg;
	std::shared_ptr<Camera> cam;
	double cam_width = -1, cam_height = -1;
	cv::VideoCaptureAPIs backend = cv::CAP_ANY;

	//void change_resolution(shared_ptr<Camera> cap, int w, int h)
	void change_cam_properties(int w, int h, cv::VideoCaptureAPIs backend=cv::CAP_ANY);
	
public:	
	void setup(std::shared_ptr<Camera> camera, int x, int y, int w, int h, const char* title);
	void update();
	void print_cam_properties();
	void set_camera(std::shared_ptr<Camera> c) { cam = c; }
	void hide() { sg.hide(); }
	void show() { sg.show(); }
};