#include "cv_camera_control.h"


void Camera::print_info()
{
	using namespace std;
	cout << "\n camera properties:";
	cout << "\nis open     = " << isOpened();
	cout << "\nid          = " << get_index();
	cout << "\nbackend     = " << getBackendName();
	cout << "\nformat      = " << get(cv::CAP_PROP_FORMAT); //deafult CV_8UC3?!
	cout << "\nframe width = " << get(cv::CAP_PROP_FRAME_WIDTH);
	cout << "\nframe height= " << get(cv::CAP_PROP_FRAME_HEIGHT);
	cout << "\nframe rate  = " << get(cv::CAP_PROP_FPS);
	cout << "\nexposure    = " << get(cv::CAP_PROP_EXPOSURE);
	cout << "\ngain        = " << get(cv::CAP_PROP_GAIN);	
	cout << "\ncontrast    = " << get(cv::CAP_PROP_CONTRAST);
	cout << "\nbrightness  = " << get(cv::CAP_PROP_BRIGHTNESS);
	cout << "\nsaturation  = " << get(cv::CAP_PROP_SATURATION);
	cout << "\nsharpness   = " << get(cv::CAP_PROP_SHARPNESS);
	cout << "\nhue         = " << get(cv::CAP_PROP_HUE);
	cout << "\nmonochrome  = " << get(cv::CAP_PROP_MONOCHROME);
	cout << "\nframe pos   = " << get(cv::CAP_PROP_POS_FRAMES);
	cout << "\nrgbconverted= " << get(cv::CAP_PROP_CONVERT_RGB);
	
	// TODO - not properly working
	int ex = static_cast<int>(get(cv::CAP_PROP_FOURCC));
	//char codec[] = { ex & 0XFF , (ex & 0XFF00) >> 8,(ex & 0XFF0000) >> 16,(ex & 0XFF000000) >> 24, 0 };
	char codec[] = { (char)(ex & 255) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0 };
	cout << "\n camera codec       = " << codec; 
}


void Camera_control::change_cam_properties(int w, int h, cv::VideoCaptureAPIs backend)
{
	using namespace std;

	if (cam == nullptr)
	{
		cerr << "\n camera object is a nullpointer."; 
		cam = make_shared<Camera>(id, backend);
		if (cam == nullptr)
		{
			cerr << "\ncreating a new camera object failed.";
			return;
		}
	}

	if (!cam->isOpened())
	{ 
		cam->open(id, backend);
		if (!cam->isOpened())
		{
			std::cerr << "\n could not open camera with id=" << id << " and selected backend=" << backend;
			return;
		}		
	}

	if (cam->isOpened())
	{
		cout << "\n camera properties before changing:";
		cam->print_info();
		id = cam->get_index();
		if (backend == cv::CAP_ANY) { backend = cv::VideoCaptureAPIs(int(cam->get(cv::CAP_PROP_BACKEND))); } // if default value is given, try to choose the backend stored in the cam object
		if (w == -1) { w = cam->get(cv::CAP_PROP_FRAME_WIDTH) ; }
		if (h == -1) { h = cam->get(cv::CAP_PROP_FRAME_HEIGHT); }

		cam->release();
		cam->open(id, backend);
		if (!cam->isOpened())
		{
			cerr << "\n could not open camera with the new setting.";
			return;
		}
		cam->set(cv::CAP_PROP_FRAME_WIDTH, w);
		cam->set(cv::CAP_PROP_FRAME_HEIGHT, h);

		// probe cam and read real width and height
		cv::Mat img;
		if (cam->read(img))
		{
			cam_width = cam->get(cv::CAP_PROP_FRAME_WIDTH);
			cam_height = cam->get(cv::CAP_PROP_FRAME_HEIGHT);
			sg.update_widgets();
			cout << "\n camera properties after changing:";
			cam->print_info();
		}
		else
		{
			cerr << "\n could not grab a frame.";
		}
	}
}

void Camera_control::setup(std::shared_ptr<Camera> camera, int x, int y, int w, int h, const char* title)
{
	cam = camera;

	auto current_backend = cv::CAP_ANY;
	if (cam) { current_backend = cv::VideoCaptureAPIs(int(cam->get(cv::CAP_PROP_BACKEND))); }

	sg.hide();

	sg = Simple_gui(x, y, w, h, title);

	sg.add_separator_box("select the video capture backend:");
	sg.num_columns(2);

	auto b = sg.add_radio_button("Auto Detect", [&]() {backend = cv::CAP_ANY; change_cam_properties(-1,-1, backend); }, "OpenCV tries to autmatically detect the appropriate backend for video capture.");
	if (current_backend == cv::CAP_ANY) { b->value(true); }

	b = sg.add_radio_button("Direct Show", [&]() {backend = cv::CAP_DSHOW; change_cam_properties(-1, -1, backend); }, "select the direct show backend for video capture.");
	if (current_backend == cv::CAP_DSHOW) { b->value(true); }

	b = sg.add_radio_button("MS Media Foundation", [&]() {backend = cv::CAP_MSMF; change_cam_properties(-1, -1, backend); }, "select the Microsoft Media Foundation backend for video capture.");
	if (current_backend == cv::CAP_MSMF) { b->value(true); }

	b = sg.add_radio_button("MS Windows Runtime", [&]() {backend = cv::CAP_WINRT; change_cam_properties(-1, -1, backend); }, "select the Microsoft Windows Runtime backend for video capture.");
	if (current_backend == cv::CAP_WINRT) { b->value(true); }

	sg.add_separator_box("control (auto)focus, exposure and gain:");
	sg.add_slider("focus:", focus, 0, 255, 1);
	sg.add_slider("exposure:", exposure, -20, 20, 0.01);
	sg.add_slider("gain:", gain, 0, 255, 1);

	sg.num_columns(3);
	sg.add_button("autofocus on", [&]() {cam->set(cv::CAP_PROP_AUTOFOCUS, 1); });
	sg.add_button("autofocus off", [&]() {cam->set(cv::CAP_PROP_AUTOFOCUS, 0); });
	sg.add_button("extended settings", [&]()
		{
			if (cam) { cam->set(cv::CAP_PROP_SETTINGS, 0); }
		}, "extended camera settings. works only with the direct show backend. ");


	sg.add_separator_box("try to set camera resolution to common values:");
	sg.num_columns(4);
	sg.add_button("320x240", [&]() { change_cam_properties(320, 240); });
	sg.add_button("640x480", [&]() { change_cam_properties(640, 480); });
	sg.add_button("800x600", [&]() { change_cam_properties(800, 600); });
	sg.add_button("1280x1024", [&]() { change_cam_properties(1280, 1024); });
	sg.add_separator_box("manual resolution setting:");
	sg.add_slider("width :", cam_width, 0, 1920, 10);
	sg.add_slider("height:", cam_height, 0, 1080, 10);
	
	sg.num_columns(1);
	sg.add_button("try to set selected resolution", [&]()
	{
			change_cam_properties(cam_width, cam_height);
	});

	//sg.add_slider("window size:", params[2]);

	sg.finish();
	sg.show();
}

void Camera_control::update()
{
	if (nullptr == cam) { return; }
	
	sg.update();
	
	if (focus != old_focus)
	{
		old_focus = focus;
		cam->set(cv::CAP_PROP_FOCUS, focus);
	}

	if (exposure != old_exposure)
	{
		old_exposure = exposure;
		cam->set(cv::CAP_PROP_EXPOSURE, exposure);
	}

	if (gain != old_gain)
	{
		old_gain = gain;
		cam->set(cv::CAP_PROP_GAIN, gain);
	}
}



#ifdef __TEST_THIS_MODULE__

#include <iostream>

#ifdef _DEBUG
#pragma comment(lib, "opencv42/build/x64/vc15/lib/opencv_world420d.lib")
#pragma comment(lib, "fltk14/bin/lib/Debug/fltkd.lib")
#else

#pragma comment(lib, "opencv42/build/x64/vc15/lib/opencv_world420.lib")
#pragma comment(lib, "fltk14/bin/lib/Release/fltk.lib")
#endif

#ifdef _WIN32
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "comctl32.lib")
#endif 

#include "timer_hd.h"

using namespace std;
using namespace cv;

void test_module_cv_camera_control()
{
	setUseOptimized(true);

	Mat img;
	cout << "\nenter camera id:";
	int id = 0; cin >> id;

	// special settings for the pupil labs camera
	auto cam = make_shared<Camera>(id, cv::CAP_DSHOW);
	cam->set(cv::CAP_PROP_FRAME_WIDTH, 400);
	cam->set(cv::CAP_PROP_FRAME_HEIGHT, 400);
	int codec = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
	cam->set(cv::CAP_PROP_FOURCC, codec);
	cam->print_info();
	//


	//cap->set(cv::CAP_PROP_AUTOFOCUS, 0);
	//cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 0);
	//cam->set(cv::CAP_PROP_EXPOSUREPROGRAM, 0);

	Camera_control cam_control;
	
	cam_control.setup(cam, 20, 20, 400, 400, "My Cam");

	Timer timer(50);
	
	while (true)
	{
		timer.tick();
		if (cam->read(img))
		{
			imshow("frame", img);
			timer.tock();
		}

		cam_control.update();


		
	}

	cv::destroyAllWindows();

	return;// EXIT_SUCCESS;
}

#include "simple_gui_fltk.cpp"

#endif