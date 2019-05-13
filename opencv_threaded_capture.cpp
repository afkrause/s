#include "opencv_threaded_capture.h"
#include "timer_hd.h"

void Threaded_capture::capture_thread_func()
{
	using namespace std;

	cout << "\n" << cam_name << " capture thread has started\n";
	Timer timer(500, "\n" + cam_name + ":\t");
	cv::Mat local_frame;
	while (is_running)
	{
		timer.tick();
		if (camera->read(local_frame))
		{
			mx.lock();
			local_frame.copyTo(frame);
			mx.unlock();
			new_frame = true;
		}
		else
		{
			cerr << "failed to read " << cam_name << " frame!" << endl;
			return;
		}
		timer.tock();
		cv::waitKey(1); // needed or not ?!
	}
	cout << cam_name << " capture thread has stopped\n";
}

void Threaded_capture::setup(std::shared_ptr<Camera> cam, std::string cam_name_)
{
	cam_name = cam_name_;
	camera = cam;
	is_running = true;
	capture_thread = std::thread(&Threaded_capture::capture_thread_func, this);
}

void Threaded_capture::stop()
{
	is_running = false;
	capture_thread.join();
}
