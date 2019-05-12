#define __TEST_THIS_MODULE__

#include <iostream>

//#include "savi.h"
//#include "archive_creator.h"
//#include "vector_stack_allocated.hpp"
//#include "array.h"
//#include "string_replace.h"
//#include "tiny_png.h"
//#include "tiny_jpeg.h"
//#include "tiny_gif.h"
//#include "ringbuffer_array.h"
//#include "clustering.h"
//#include "highres_dft.h"
//#include "timer_hd.h"
//#include "speech.h"
//#include "eigen_pseudoinverse.h"
//#include "simple_gui_fltk.h"
#include "sdl_opencv.h"
#include "sdl_opencv.cpp"

//#define MULTIPLOT_WIN32
//#define MULTIPLOT_FLTK
//#include "../multiplot/multiplot.h" // this makes Multiplot to use GL_Fl_Window  and fltk gl fonts



int main(int argc, char* argv[])
{

	
	test_module_sdl_opencv();
	//test_module();
	//test_Avi_generator();
	//test_Archive();
	//test_vector_s();
	//test_array_classes();
	//test_string_replace();
	//test_tiny_jpeg();
	//test_tiny_gif();
	//test_ringbuffer_array();
	//test_multiplot();
	//test_highres_dft();
	//test_clustering();
	//test_speech();

	using namespace std;
	cout << endl << "press enter" << endl;
	char c=0; cin.get(&c,1);

	return EXIT_SUCCESS;
}

// #include "bzlib.c"
//#include<SDL2/sdl_windows_main.c>