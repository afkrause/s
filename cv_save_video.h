#pragma once

#include <thread>
#include <string>
#include <fstream>
#include <atomic>
#include <mutex>
#include <opencv2/opencv.hpp>

class Save_video
{
protected:
	class Frame
	{
	public:
		cv::Mat data;
		double timestamp;
	};

	std::fstream fstream_timestamp;
	cv::VideoWriter video_writer;
	std::thread local_thread;
	std::mutex mx;
	std::string video_file_name;
	std::vector<Frame> buffer;
	std::vector<Frame*> free_list;
	std::vector<Frame*> fifo;
	std::atomic<bool> run_thread{true};
	void video_writer_thread();
public:
	void open(std::string fname, std::string fname_timestamp, int fps, cv::Size2i size, int buffer_size = 10, int codec = cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
	void close();	
	void write(cv::Mat& frame, double timestamp);

	~Save_video();
};
