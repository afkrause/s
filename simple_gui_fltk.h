#pragma once
#include<memory>
#include<vector>
#include<iostream>
#include<functional>


#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
//#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Radio_Round_Button.H>


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
	
	// window position and size
	int ww = 480;
	int wh = 480;
	int wx = 5;
	int wy = 25;

	// cursor position for widged placement
	int cx = 10;
	int cy = 10;

	// number of columns == number of widgets (e.g. buttons) in one row
	int num_cols = 3; 
	int cur_col = 0;  // current column

	Fl_Box* current_box = nullptr; 
	Fl_Group* current_group = nullptr;

	void box_and_group_adjust_size();

	int max_slider_label_width = 0;

	int w_height = 25; // default widget height

	bool check_valid();
	template<class T> T* add_button_helper(const char* label, std::function<void()> func, const char* tooltip = nullptr);
public:
	Simple_gui() {}
	
	Simple_gui(int x_, int y_, int w_, int h_, const char* title = nullptr)
	{
		setup(x_, y_, w_, h_, title);
	}

	void setup(int x_, int y_, int w_, int h_, const char* title = nullptr);

	// set the widget height. default = 25. set to e.g. 15 if you want to place more widgets into a smaller space
	int widget_height() { return w_height; }
	void widget_height(int w) { w_height = w; }
	
	void num_columns(int n) { num_cols = n; }
	int num_columns() { return num_cols; }

	int w() { return ww; }
	int h() { return wh; }
	int x() { return wx; }
	int y() { return wy; }

	void show();
	void hide();

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
	void finish();

	// limited to positive values for now
	Fl_Hor_Value_Slider* add_slider(const char* label, double& val, double min_val = 0, double max_val = 1, double step = 0, const char* tooltip = nullptr);


	// create a checkbox. 
	// example if checkboxes shall be placed in 3 columns:
	// num_columns(3);
	// add_checkbox("checkbox in column 1", b1);
	// add_checkbox("checkbox in column 2", b2);
	// add_checkbox("checkbox in column 3", b3);
	Fl_Check_Button* add_checkbox(const char* label, bool& val, const char* tooltip = nullptr);

	// create a button. argument semantics similiar to add_checkbox
	Fl_Button* add_button(const char* label, std::function<void()> func, const char* tooltip = nullptr)
	{
		return add_button_helper<Fl_Button>(label, func, tooltip);
	}

	// create a radio button. readio buttons must be grouped. hence, first create a group using add_separator_box.
	// finish the group be creating the next separator_box.
	Fl_Radio_Round_Button* add_radio_button(const char* label, std::function<void()> func, const char* tooltip = nullptr)
	{
		return add_button_helper<Fl_Radio_Round_Button>(label, func, tooltip);
	}

	// create a radio button. readio buttons must be grouped. hence, first create a group using add_separator_box.
	// finish the group be creating the next separator_box.
	Fl_Light_Button* add_toggle_button(const char* label, bool& state, const char* tooltip = nullptr);


};


#ifdef __TEST_THIS_MODULE__
void test_module_simple_gui();
#endif