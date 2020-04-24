#include "simple_gui_fltk.h"
#include <math.h>

void simple_gui_slider_cb(Fl_Widget* w, void* user_data)
{
	*((double*)user_data) = ((Fl_Hor_Value_Slider*)w)->value();
}

void simple_gui_checkbox_cb(Fl_Widget* w, void* user_data)
{
	*((bool*)user_data) = ((Fl_Check_Button*)w)->value();
}

void simple_gui_button_cb(Fl_Widget* w, void* user_data)
{
	// cout << "button pressed..\n";
	// execute std:: function
	(*((std::function<void()>*)user_data))();
}

bool Simple_gui::check_valid()
{ 
	if (window == nullptr)
	{ 
		std::cerr << "Simple_gui: first call setup()" << std::endl;
		return false;
	}
	return true;
}


void Simple_gui::setup(int x, int y, int w, int h, const char* title)
{
	wx = x;
	wy = y;
	ww = w;
	wh = h;

	window = std::make_shared<Fl_Window>(wx, wy, ww, wh, title);
	
	buttton_functions.reserve(1000); // avoid reallocations to keep pointers constant

	/*
	sliders.clear();
	slider_value_refs.clear();
	check_buttons.clear();
	check_buttons_value_refs.clear();
	buttton_functions.clear();

	// widged placement cursor pos
	x = 10;
	y = 10;
	current_box = nullptr;
	*/
}

void Simple_gui::show()
{
	if (window != nullptr)
	{
		window->show();
		Fl::check();
		// the true position and size might deviate from the requested position due to window manager interference.
		wx = window->x();
		wy = window->y();
		ww = window->w();
		wh = window->h();
	}


}

void Simple_gui::hide()
{
	if (window != nullptr)
	{
		window->hide();
	}
}

void Simple_gui::finish()
{
	box_and_group_adjust_size();

	if (window)
	{
		if (current_group)
		{
			current_group->end();
			window->add(current_group);
		}
		window->end();
	}
}



void Simple_gui::update_widgets()
{
	for (int i = 0; i < slider_value_refs.size(); i++)
	{
		sliders[i]->value(*slider_value_refs[i]);
	}

	for (int i = 0; i < check_buttons_value_refs.size(); i++)
	{
		check_buttons[i]->value(*check_buttons_value_refs[i]);
	}
}

void Simple_gui::add_separator_box(const char* label)
{
	if (!check_valid()) { return; }
	
	

	if (current_group)
	{
		box_and_group_adjust_size();
		current_group->end();
		window->add(current_group);
	}

	// spacing between group boxes
	if (current_box)
	{ 
		cy += 10;
	}

	// create a group, such that radio buttons work properly within the box 
	current_group = new Fl_Group(5, cy, ww - 10, 8025); // make to group very large and adjust size later
	//window->add(current_group);

	// generate a box 
	current_box = new Fl_Box(5, cy, ww - 10, w_height + 15, label);
	current_box->box(FL_BORDER_BOX);
	current_box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_TOP);
	window->add(current_box);

	if (std::string(label) != "")
	{
		cy += 15; // spacing for the label string
	}

	 cy+= 5;
}

Fl_Hor_Value_Slider* Simple_gui::add_slider(const char* label, double& val, double min_val, double max_val, double step, const char* tooltip)
{
	if (!check_valid()) { return nullptr; }

	// auto guess max value (to be 10^x)
	if (val >= max_val)
	{
		// TODO: fix for negative values
		max_val = pow(10, ceil(log10(val)));
	}



	// first, generate a slider somewhere
	auto slider = new Fl_Hor_Value_Slider(10, cy, 50, w_height, label);
	slider->align(FL_ALIGN_LEFT);
	slider->bounds(min_val, max_val);
	slider->value(val);
	slider->step(step);
	slider->callback(simple_gui_slider_cb, &val);
	if (tooltip != nullptr) { slider->copy_tooltip(tooltip); }
	sliders.push_back(slider);
	slider_value_refs.push_back(&val);
	cy += w_height + 2;// round(widget_height / 5);

	// now, measure label width and reposition all sliders
	int label_w = 0, label_h = 0;
	slider->measure_label(label_w, label_h);
	if (label_w > max_slider_label_width)
	{
		max_slider_label_width = label_w;
	}

	int sw = ww - 20 - max_slider_label_width; // slider width
	for (auto s : sliders)
	{
		int sy = s->y();
		s->resize(10 + max_slider_label_width, sy, sw, w_height);
	}


	box_and_group_adjust_size();
	
	return slider;
}

Fl_Check_Button* Simple_gui::add_checkbox(const char* label, bool& val, const char* tooltip)
{
	if (!check_valid()) { return nullptr; }
	if (!current_group) { add_separator_box(""); }

	const int col_width = (ww - 20) / num_cols;
	int pos_x = 10 + cur_col * col_width;

	auto button = new Fl_Check_Button(pos_x, cy, 100, w_height, label);
	button->value(val);
	button->callback(simple_gui_checkbox_cb, &val);
	if (tooltip != nullptr) { button->tooltip(tooltip); }
	check_buttons.push_back(button);
	check_buttons_value_refs.push_back(&val);

	cur_col++;
	// go to next line
	if (cur_col == num_cols)
	{
		cy += w_height + 2;
		cur_col = 0;
	}


	box_and_group_adjust_size();
	current_group->add(button);
	
	return button;
}


/*
Fl_Radio_Round_Button* ba = Fl_Radio_Round_Button(20, 20, 180, 20, "Button A");
Fl_Radio_Round_Button* bb = Fl_Radio_Round_Button(20, 50, 180, 20, "Button B");
Fl_Radio_Round_Button* bc = Fl_Radio_Round_Button(20, 80, 180, 20, "Button C");
*/

// explicit instantiation
template Fl_Button* Simple_gui::add_button_helper<Fl_Button>(const char* label, std::function<void()> func, const char* tooltip);
template Fl_Radio_Round_Button* Simple_gui::add_button_helper<Fl_Radio_Round_Button>(const char* label, std::function<void()> func, const char* tooltip);
template Fl_Light_Button* Simple_gui::add_button_helper<Fl_Light_Button>(const char* label, std::function<void()> func, const char* tooltip);

template<class T> T* Simple_gui::add_button_helper(const char* label, std::function<void()> func, const char* tooltip)
{
	if (!check_valid()) { return nullptr; }

	if (!current_group) { add_separator_box(""); }


	if (buttton_functions.size() < buttton_functions.capacity())
	{
		buttton_functions.push_back(func);
	}
	else
	{
		std::cerr << "maximum number of allowed buttons reached.\n";
		return nullptr;
	}
	const int col_width = (ww - 20) / num_cols;
	int pos_x = 10 + cur_col * col_width;
	auto button = new T(pos_x, cy, 100, w_height, label);

	// fit button size to label width
	int label_w = 0, label_h = 0;
	button->measure_label(label_w, label_h);
	button->resize(pos_x, cy, label_w + 20, w_height);

	//button->callback(simple_gui_button_cb, (void*)&(func));
	button->callback(simple_gui_button_cb, (void*)&(buttton_functions.back()));

	if (tooltip != nullptr) { button->tooltip(tooltip); }

	current_group->add(button);

	cur_col++;
	// go to next line
	if (cur_col == num_cols)
	{
		cy += w_height + 2;
		cur_col = 0;
	}

	return button;
}

Fl_Light_Button* Simple_gui::add_toggle_button(const char* label, bool& state, const char* tooltip)
{
	auto button = add_button_helper<Fl_Light_Button>(label, []() {}, tooltip);
	buttton_functions.back() = [b = button, &state]()
	{
		state = false;
		if (int(b->value()) == 1) { state = true; }
		//std::cerr << "\n toggle button" << int(b->value()); 
	};

	return button;
}


void Simple_gui::box_and_group_adjust_size()
{
	// fix the situation where the user wanted to have e.g. 3 columns, but placed just 2 widgets 
	if (cur_col < num_cols && cur_col > 0)
	{
		cy += w_height + 2;
		cur_col = 0;
	}

	if (current_group)
	{
		current_group->resizable(NULL); // do not change the size and position of child widgets
		current_group->resize(current_group->x(), current_group->y(), current_group->w(), cy - current_group->y() + 5);
	}

	if (current_box)
	{
		current_box->size(current_box->w(), cy - current_box->y() + 5);
	}
}


#ifdef __TEST_THIS_MODULE__

//#pragma comment ( lib, "comctl32.lib")
//#pragma comment ( lib, "ws2_32.lib")

#ifdef _DEBUG
#pragma comment(lib, "fltk14/bin/lib/Debug/fltkd.lib")
#else
#pragma comment(lib, "fltk14/bin/lib/Release/fltk.lib")
#endif

#ifdef _WIN32
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "winspool.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "uuid.lib")
#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "advapi32.lib")
#endif


void test_module_simple_gui()
{
	/*
	auto w = new Fl_Window(50, 50, 500, 500, "hallo");
	auto slider = new Fl_Hor_Value_Slider(10, 10, 400, 25, "hallo");

	slider->align(FL_ALIGN_LEFT);
	slider->bounds(0, 100);
	slider->value(50);
	slider->step(1);
	//slider->callback(simple_gui_slider_cb, &val);
	const char* tooltip = "nice slider!";
	if (tooltip != nullptr) { slider->tooltip(tooltip); }
	w->end();
	w->show();
	while(true) { Fl::check(); }
	return;
	//*/

	using namespace std;

	// first, create a gui window:
	Simple_gui sg(1000, 100, 480, 480);

	/*
	Simple_gui sg_local(50, 50, 150, 100, "Record and Stream");
	sg_local.add_button("stop streaming", [&]() { run = false;  });
	sg_local.add_button("quit program", [&]() { exit(EXIT_SUCCESS); });
	sg_local.finish();
	*/

	
	// easiest is to add a button: 
	sg.num_columns(1);
	sg.add_button("print hello world!", []() {cout << "Hello world!\n"; }, "prints hello world!");


	// now, adding some sliders is also easy:
	double params[3]{ 10,110, 0.2 };
	sg.add_slider("contrast:", params[0], 0, 100,1, "slider for contrast setting");
	sg.add_slider("blur:", params[1], 0, 100,1, "slider for blur");
	sg.add_slider("window size:", params[2], 640, 1920, 1, "slider for window size");


	// you cann add a box to visually separate and group widgets:
	sg.add_separator_box("some checkboxes:");
	bool options[4]{ true, false, true, true };
	sg.add_checkbox("first option 1", options[0], "tooltip checkbox 1");
	sg.add_checkbox("second option 2", options[1], "tooltip checkbox 2");
	sg.add_checkbox("third option 3", options[2], "tooltip checkbox 3");

	sg.add_separator_box("more checkboxes:");
	bool options2[3]{ true, false, true };
	sg.num_columns(3);
	sg.add_checkbox("checkbox column 1", options[0],"tooltip checkbox 1");
	sg.add_checkbox("checkbox column 2", options[1], "tooltip checkbox 2");
	sg.add_checkbox("checkbox column 3", options[2], "tooltip checkbox 3");



	// a bit more complicated - using radio buttons
	sg.add_separator_box("a radio button group:");
	int my_val = 0;
	auto cb_func = [&](int val) { my_val = val; std::cout << "my value set to:" << val << std::endl; };
	sg.num_columns(1);
	sg.add_radio_button("radio button 1", [&]() {cb_func(0); },"radio button 1");
	// pre-select this radio buttton
	auto b = sg.add_radio_button("radio button 2", [&]() {cb_func(1); }, "radio button 2");  if (b) { b->value(true); }
	sg.add_radio_button("radio button 3", [&]() {cb_func(2); }, "radio button 3");


	// finish gui creation
	sg.finish();
	sg.show();

	while (Fl::check())
	{
		std::cout << "p1 = " << params[0] << "\tp2 = " << params[1] << "\tp3 = " << params[2] << std::endl;
		Fl::wait();
	}
}

#endif