/*
 * GUI.cpp
 *
 *  Created on: Aug 10, 2016
 *      Author: smith
 */
#include "Screen_Icons.h"


GUI::GUI(void):
	slider_ (0)
{

}

void GUI::setSlider(float val)
{
	slider_ = val;
}
float GUI::getSlider(void)
{
	return(slider_);
}



