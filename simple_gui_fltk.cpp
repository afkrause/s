#include "simple_gui_fltk.h"
#include <math.h>

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


void Simple_gui::setup(int x_, int y_, int w_, int h_, const char* title)
{
	w = w_;
	h = h_;
	window = std::make_shared<Fl_Window>(x, y, w, h, title);
	window->end();
	//window->show(argc, argv);
	window->show();// (0, nullptr);
	buttton_functions.reserve(100); // avoid reallocations to keep pointers constant
}

void Simple_gui::show()
{
	if (window != nullptr)
	{
		window->show();
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
	if (current_box) { y += 10; }
	// first, generate a slider somewhere
	auto box = new Fl_Box(5, y, w - 10, 25, label);
	box->box(FL_BORDER_BOX);
	box->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_TOP);
	current_box = box;
	window->add(box);
	y += 15 + 5;
}

void Simple_gui::add_slider(const char* label, double& val, double min_val, double max_val, double step, const char* tooltip)
{
	if (!check_valid()) { return; }

	// auto guess max value (to be 10^x)
	if (val >= max_val)
	{
		// TODO: fix for negative values
		max_val = pow(10, ceil(log10(val)));
	}



	// first, generate a slider somewhere
	auto slider = new Fl_Hor_Value_Slider(10, y, 50, 25, label);
	slider->align(FL_ALIGN_LEFT);
	slider->bounds(min_val, max_val);
	slider->value(val);
	slider->step(step);
	slider->callback(simple_gui_slider_cb, &val);
	slider->tooltip(tooltip);
	sliders.push_back(slider);
	slider_value_refs.push_back(&val);
	y += 25 + 5;

	// now, measure label width and reposition all sliders
	int label_w = 0, label_h = 0;
	slider->measure_label(label_w, label_h);
	if (label_w > max_slider_label_width)
	{
		max_slider_label_width = label_w;
	}

	int sw = w - 20 - max_slider_label_width; // slider width
	for (auto s : sliders)
	{
		int sy = s->y();
		s->resize(10 + max_slider_label_width, sy, sw, 25);
	}


	box_adjust_size();
	window->add(slider);
}

void Simple_gui::add_checkbox(const char* label, bool& val, int num_cols, int col)
{
	if (!check_valid()) { return; }

	const int col_width = (w - 20) / num_cols;
	int pos_x = 10 + col * col_width;
	auto button = new Fl_Check_Button(pos_x, y, 100, 20, label);
	button->value(val);
	button->callback(simple_gui_checkbox_cb, &val);
	check_buttons.push_back(button);
	check_buttons_value_refs.push_back(&val);
	// go to next line
	if (col == num_cols - 1)
	{
		y += 20 + 2;
	}

	box_adjust_size();
	window->add(button);
}

void Simple_gui::add_button(const char* label, std::function<void()> func, int num_cols, int col)
{
	if (!check_valid()) { return; }

	if (buttton_functions.size() < buttton_functions.capacity())
	{
		buttton_functions.push_back(func);
	}
	else
	{
		std::cerr << "maximum number of allowed buttons reached.\n";
		return;
	}
	const int col_width = (w - 20) / num_cols;
	int pos_x = 10 + col * col_width;
	auto button = new Fl_Button(pos_x, y, 100, 25, label);

	// fit button size to label width
	int label_w = 0, label_h = 0;
	button->measure_label(label_w, label_h);
	button->resize(pos_x, y, label_w + 20, 25);

	//button->callback(simple_gui_button_cb, (void*)&(func));
	button->callback(simple_gui_button_cb, (void*)&(buttton_functions.back()));

	// go to next line
	if (col == num_cols - 1)
	{
		y += 25 + 2;
	}

	box_adjust_size();
	window->add(button);
}


#ifdef __TEST_THIS_MODULE__
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


void test_module()
{
	double params[3]{ 10,110, 0.2 };
	Simple_gui sg(1000, 100, 480, 480);
	sg.add_slider("contrast:", params[0]);
	sg.add_slider("blur:", params[1]);
	sg.add_slider("window size:", params[2]);

	bool options[4]{ true, false, true, true };
	sg.add_checkbox("first option 1", options[0]);
	sg.add_checkbox("second option 2", options[1]);
	sg.add_checkbox("third option 3", options[2]);

	bool options2[3]{ true, false, true };
	sg.add_checkbox("checkbox in column 1", options[0], 3, 0);
	sg.add_checkbox("checkbox in column 2", options[1], 3, 1);
	sg.add_checkbox("checkbox in column 3", options[2], 3, 2);

	while (Fl::check())
	{
		std::cout << "p1 = " << params[0] << "\tp2 = " << params[1] << "\tp3 = " << params[2] << std::endl;
		Fl::wait();
	}
}

#endif