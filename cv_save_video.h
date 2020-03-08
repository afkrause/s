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

	std::vector<cv::Mat> buffer;
	std::vector<cv::Mat*> free_list;
	std::vector<cv::Mat*> fifo;
	std::atomic<bool> run_thread = true;
	void video_writer_thread();
	//std::atomic<bool> new_frame = false;
public:
	void open(std::string fname, int fps, cv::Size2i size, int buffer_size = 10, int codec = cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
	void close();	
	void write(cv::Mat& frame);

	~Save_video();
};
