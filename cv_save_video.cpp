#include "cv_save_video.h"

#include <chrono>

void Save_video::write(cv::Mat& img, double timestamp)
{
	using namespace std;

	const lock_guard<mutex> lock(mx);
	
	if (free_list.empty())
	{
		std::cerr << "\nframe dropped while writing to: " << video_file_name;
	}
	else
	{
		auto frame = free_list.back();
		free_list.pop_back();
		
		frame->timestamp = timestamp;
		img.copyTo(frame->data);
		
		fifo.push_back(frame);
	}
}

void Save_video::open(std::string fname_video, std::string fname_timestamp, int fps, cv::Size2i size, int buffer_size, int codec)
{
	close();
	free_list.clear();
	fifo.clear();
	buffer.resize(buffer_size);
	for (auto& x : buffer) { free_list.push_back(&x); }
	std::cout << "\nvideo writer buffer size=" << free_list.size();

	video_file_name = fname_video;
	
	video_writer.open(fname_video, codec, fps, size);
	fstream_timestamp.open(fname_timestamp, std::ios::out);
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
	
	// wait for thread to finish and write remaining frames to disk
	run_thread = false;
	if (local_thread.joinable())
	{
		local_thread.join(); 
	}

	// release video_writer and close timestamp fstream
	video_writer.release();
	fstream_timestamp.close();
}

void Save_video::video_writer_thread()
{
	using namespace std;
	using namespace std::chrono;
	cout << "\nvideo writer thread started.";

	bool run = true;
	while (run)
	{
		Frame* f = nullptr;
		
		// lock because we access the shared data in the fifo
		mx.lock();
		
		// run as long as some frames remain in the fifo
		if (!run_thread && fifo.empty()) { run = false; }

		// get a new frame from the fifo
		if (!fifo.empty())
		{
			f = fifo.front();
			fifo.erase(fifo.begin());
		}
		mx.unlock();

		// new data ? write to video file
		if (f != nullptr)
		{
			video_writer.write(f->data);
			fstream_timestamp << f->timestamp << "\n";

			mx.lock();
			free_list.push_back(f);
			// debug cout << "\nfree_list size = " << free_list.size();
			mx.unlock();			
		}

		std::this_thread::sleep_for(1ms);
		//cv::waitKey(1); // needed for event propagation (or not needed??)
	}

	std::cout << "\nvideo writer thread stopped.";
}