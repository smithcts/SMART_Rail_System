/*
 * GUI.h
 *
 *  Created on: Aug 10, 2016
 *      Author: smith
 */

#ifndef SCREEN_ICONS_H_
#define SCREEN_ICONS_H_

#include <stdint.h>

class GUI
{
public:
	GUI(void);

	void setSlider(float val);

	float getSlider(void);

private:
	float slider_;
};


#endif /* SCREEN_ICONS_H_ */
