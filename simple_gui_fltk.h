#pragma once
#include<memory>
#include<vector>
#include<iostream>
#include<functional>

#ifdef _DEBUG
//#pragma comment ( lib, "fltkd.lib")
//#pragma comment ( lib, "fltk_gld.lib")
#else
//#pragma comment ( lib, "fltk.lib")
//#pragma comment ( lib, "fltk_gl.lib")
#endif

//#pragma comment ( lib, "comctl32.lib")
//#pragma comment ( lib, "ws2_32.lib")

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
//#include <FL/Fl_s


// TODO move into class, make pointer to class member..
void simple_gui_slider_cb(Fl_Widget* w, void* user_data);
void simple_gui_checkbox_cb(Fl_Widget* w, void* user_data);
void simple_gui_button_cb(Fl_Widget* w, void* user_data);

class Simple_gui
{
protected:
	std::shared_ptr<Fl_Window> window;
	
	std::vector< Fl_Hor_Value_Slider* > sliders;
	std::vector<double*> slider_value_refs;

	std::vector < Fl_Check_Button* > check_buttons;
	std::vector < bool* > check_buttons_value_refs;

	std::vector< std::function<void()> > buttton_functions;
	int w = 480;
	int h = 480;

	// widged placement cursor pos
	int x = 10;
	int y = 10;

	Fl_Box* current_box = nullptr; 
	void box_adjust_size()
	{
		if (current_box) { current_box->size(current_box->w(), y - current_box->y() + 5 ); }
	}

	int max_slider_label_width = 0;

	bool check_valid();
public:
	Simple_gui() {}
	
	Simple_gui(int x_, int y_, int w_, int h_, const char* title = nullptr)
	{
		setup(x_, y_, w_, h_, title);
	}

	void setup(int x_, int y_, int w_, int h_, const char* title = nullptr);

	void show();

	// propagate events
	void update()
	{
		Fl::check();
	}

	// this updates all sliders and check boxes based on the referenced values
	// do not call regularly, only during initialization or resetting of values
	// or if you want to have the sliders animated 
	void update_widgets();

	// adds a horizontal box with a label spanning the whole width of the window
	// can be used to separate groups of buttons etc. 
	void add_separator_box(const char* label);

	// call this if you are done with building the gui. this ensures that the last separator box has the proper size.
	void finish()
	{
		box_adjust_size();
	}

	// limited to positive values for now
	void add_slider(const char* label, double& val, double min_val = 0, double max_val = 1, double step = 0, const char* tooltip = nullptr);


	// create a checkbox. 
	// example if checkboxes shall be placed in 3 columns:
	// add_checkbox("checkbox in column 1", b, 3, 0); add_checkbox("checkbox in column 2", b, 3, 1); add_checkbox("checkbox in column 3", b, 3, 2);
	void add_checkbox(const char* label, bool& val, int num_cols = 1, int col = 0, const char* tooltip = nullptr);

	// create a button. argument semantics similiar to add_checkbox
	void add_button(const char* label, std::function<void()> func, int num_cols = 1, int col = 0, const char* tooltip = nullptr);

};


#ifdef __TEST_THIS_MODULE__
void test_module();
#endif