#pragma once

#include <thread>
#include <string>
#include <atomic>
#include <mutex>
#include <opencv2/opencv.hpp>

class Save_video
{
protected:
	cv::Mat frame;
	cv::VideoWriter video_writer;
	std::thread local_thread;
	std::mutex mx;
	std::string video_file_name;
	void run_thread();
public:
	void open(std::string fname, int fps, cv::Size2i size, int codec = cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
	void close();
	std::atomic<bool> new_frame = false;
	void write(cv::Mat& frame);
};
