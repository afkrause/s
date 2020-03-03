#include "cv_save_video.h"

#include <chrono>

void Save_video::write(cv::Mat& img)
{
	// videowriter has not finished yet (new_video_writer_frame not set to false) -- framedrop detected..
	if (video_writer.isOpened() && new_frame == true)
	{
		std::cerr << "\nframe dropped while writing to: " << video_file_name;
	}

	if (new_frame == false)
	{
		mx.lock();
		img.copyTo(frame);
		mx.unlock();
		new_frame = true;
	}
}

void Save_video::open(std::string fname, int fps, cv::Size2i size, int codec)
{
	video_file_name = fname;
	close();
	video_writer.open(fname, codec, fps, size);
	new_frame = false;
	local_thread = std::thread(&Save_video::run_thread, this);
}

void Save_video::close()
{
	video_writer.release();
	new_frame = false;
	if (local_thread.joinable())
	{
		local_thread.join(); // wait for thread to finish
	}
}


void Save_video::run_thread()
{
	using namespace std::chrono;
	std::cout << "\nvideo writer thread started.";
	while (video_writer.isOpened())
	{
		if (new_frame)
		{
			mx.lock();
			video_writer.write(frame);
			mx.unlock();
			new_frame = false;
		}
		std::this_thread::sleep_for(1ms);
		cv::waitKey(1); // needed for event propagation (or not needed??)
	}
	std::cout << "\nvideo writer thread stopped.";
}