#include "cv_save_video.h"

#include <chrono>

void Save_video::write(cv::Mat& img)
{
	using namespace std;

	const lock_guard<mutex> lock(mx);
	
	if (free_list.empty())
	{
		std::cerr << "\nframe dropped while writing to: " << video_file_name;
	}
	else
	{
		auto mat = free_list.back();
		free_list.pop_back();
		img.copyTo(*mat);
		fifo.push_back(mat);
	}
	


	/*
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
	*/
}

void Save_video::open(std::string fname, int fps, cv::Size2i size, int buffer_size, int codec)
{
	close();
	free_list.clear();
	fifo.clear();
	buffer.resize(buffer_size);
	for (auto& x : buffer) { free_list.push_back(&x); }
	std::cout << "\nvideo writer buffer size=" << free_list.size();

	video_file_name = fname;
	
	video_writer.open(fname, codec, fps, size);
	run_thread = true;
	local_thread = std::thread(&Save_video::video_writer_thread, this);
}

Save_video::~Save_video()
{
	close();
}

void Save_video::close()
{
	using namespace std;
	using namespace chrono;

	// write remaining frames to disk
	bool frames_remaining = true;
	while (frames_remaining)
	{
		mx.lock();
		if (fifo.empty())
		{
			frames_remaining = false;
		}
		mx.unlock();
		std::this_thread::sleep_for(100ms);
	}
	
	// wait for thread to finish
	run_thread = false;
	if (local_thread.joinable())
	{
		local_thread.join(); 
	}

	// release video_writer and close timestamp fstream
	video_writer.release();

	// todo 
	// fstream_timestamp.close();
}

void Save_video::video_writer_thread()
{
	using namespace std;
	using namespace std::chrono;
	cout << "\nvideo writer thread started.";

	while (run_thread)
	{
		cv::Mat* mat = nullptr;
		mx.lock();
		if (!fifo.empty())
		{
			mat = fifo.front();
			fifo.erase(fifo.begin());
		}
		mx.unlock();

		if (mat != nullptr)
		{
			video_writer.write(*mat);

			mx.lock();
			free_list.push_back(mat);
			// debug cout << "\nfree_list size = " << free_list.size();
			mx.unlock();			
		}

		std::this_thread::sleep_for(1ms);
		cv::waitKey(1); // needed for event propagation (or not needed??)
	}
	std::cout << "\nvideo writer thread stopped.";
}