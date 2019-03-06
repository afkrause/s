#include "simple_gui.h"

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
