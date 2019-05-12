#include "sdl_opencv.h"

#include <iostream>

Sdl_opencv::~Sdl_opencv()
{ 
	if(surface) { SDL_FreeSurface(surface); }
	if (renderer) { SDL_DestroyRenderer(renderer); }
	if (window) { SDL_DestroyWindow(window); }
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Sdl_opencv::re_create_surface(cv::Mat& image)
{
	/*
	if (surface)
	{
		SDL_FreeSurface(surface);
	}
	
	surface = SDL_CreateRGBSurfaceFrom(
		(void*)image.data,
		image.size().width,
		image.size().height,
		8 * image.channels(),
		image.step,
		0xff0000,
		0x00ff00,
		0x0000ff, 0);
	//*/
	
	//*
	if (texture) { SDL_DestroyTexture(texture); }
	auto texture_format = SDL_PIXELFORMAT_RGB24;
	if (image.channels() == 3) { texture_format = SDL_PIXELFORMAT_BGR24; }
	if (image.channels() == 4) { texture_format = SDL_PIXELFORMAT_BGRA8888; }
	texture = SDL_CreateTexture(renderer, texture_format, SDL_TEXTUREACCESS_STREAMING, w, h);
	//*/
}
	
void Sdl_opencv::imshow(cv::Mat& image, int x, int y)
{
	if (!initialized)
	{
		if (!SDL_WasInit(SDL_INIT_VIDEO))
		{
			if (SDL_Init(SDL_INIT_VIDEO) < 0)
			{
				std::cerr << "\nCouldn't initialize SDL:" << SDL_GetError();
				return;
			}
			atexit(SDL_Quit);

			SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

			if (!window)
			{
				w = image.cols;
				h = image.rows;
				// the flags to pass to SDL_SetVideoMode
				int videoFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;// | SDL_WINDOW_FULLSCREEN;
				window = SDL_CreateWindow(title_.c_str(), x, y, w, h, videoFlags);
			}

			if (!renderer)
			{
				renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			}
			re_create_surface(image);
			
			initialized = true;
		}
	}

	// resize the window and (re)create surface
	if (w != image.cols || h != image.rows)
	{
		w = image.cols;
		h = image.rows;
		SDL_SetWindowSize(window, w, h);
		re_create_surface(image);
	}

	//*
	int pitch = 0;
	void* pixels = nullptr;
	SDL_LockTexture(texture, NULL, &pixels, &pitch);
		if (pitch != image.step) { std::cerr << "texture.pitch != image.step" << std::endl; }
		memcpy(pixels, image.data, image.step * h);
	SDL_UnlockTexture(texture);
	//*/

	//texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer); // should be vsynced

	//SDL_DestroyTexture(texture);
}
	

// make sure events are processed
SDL_Keysym Sdl_opencv::waitKey()
{
	SDL_Event e; SDL_PollEvent(&e);
	return e.key.keysym;
}
	

#ifdef __TEST_THIS_MODULE__


#include <thread>
using namespace std;

#include "timer_hd.h"

#include<SDL2/SDL_main.h>
#pragma comment(lib, "SDL2/lib/x64/SDL2.lib")
#pragma comment(lib, "SDL2/lib/x64/SDL2main.lib")
//#pragma comment(lib, "SDL2.lib")




#ifdef _DEBUG
#pragma comment(lib, "opencv40/build/x64/vc15/lib/opencv_world401d.lib")
#else
#pragma comment(lib, "opencv40/build/x64/vc15/lib/opencv_world401.lib")
#endif


void test_module_sdl_opencv()
{
	cv::Mat img;

	Sdl_opencv s("my image");

	//auto fname = "/home/frosch/frosch_klein.jpg";
	//auto fname = "c:/temp/build_opencv401.png";
	//auto fname = "c:/temp/walk_fly_independence.jpg";
	auto fname = "c:/temp/build_opencv401.png";
	img = cv::imread(fname);
	s.imshow(img, 50, 50);

	Timer timer0(1);
	Timer timer1(500);
	int nframes = 0;
	int k = 0;
	while (true)
	{
		timer1.tick();
		timer0.tick();
		
		SDL_Delay(1);
		s.imshow(img, 50, 50);		
		//if(s.waitKey().sym == SDLK_ESC){break;}
		s.waitKey();
		
		auto dt = timer0.tock(false);
		timer1.tock();
		nframes++;
		k++;

		if (dt > 0.018)
		{
			cout << "\nframe drop after " << nframes << " dt=" << dt;
			nframes = 0;
		}
	}
}

#endif	
