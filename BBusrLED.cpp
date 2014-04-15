//============================================================================
// Name        : BBusrLED.cpp
// Author      : This code is based on the work of Dr. Derek Molloy
// Molloy, D. [DerekMolloyDCU]. (2012,Apr,10). Beaglebone: C/C++ Programming
// Introduction for ARM Embedded Linux Development using Eclipse CDT[Video file]
// Modified by : Tom Byrne
// Description : Beagle Bone user led flash
// Main Changes: Updated user led path. Optional line of code for hardware pins
//============================================================================

#include <iostream>
#include <stdio.h>
#include <unistd.h>
using namespace std;

int main() {

	cout << "Program Start" << endl;

	FILE *LEDcontrol = NULL;

	//char *LEDBrightness = "/sys/class/gpio/gpio60/value";

	/*The line of code above can be used to operate the header pin "gpio 60"
	 * which is pin 12 on the P9 header. The line above must replace the one below.
	 * Note that setting the mode of the pin or exporting it has not been included
	 * in this code. The pin must be set to output mode with device tree overlays
	 * and then exported, this can be done through code.
	 */
	char *LEDBrightness = "/sys/class/leds/beaglebone:green:usr3/brightness";
	cout << "Variables set" << endl;
	for(int i = 0; i<20; i++)
	{

		if((LEDcontrol = fopen(LEDBrightness, "r+"))!= NULL)
		{
			cout << "LED should be flashing" << endl;//A note to relate the code and Led operations
			fwrite("1", sizeof(char), 1, LEDcontrol);
			fclose(LEDcontrol);
		}

	usleep(500*1000);//Half of a second sleep time


	if((LEDcontrol = fopen(LEDBrightness, "r+"))!= NULL)
			{
				fwrite("0", sizeof(char), 1, LEDcontrol);
				fclose(LEDcontrol);
			}


	usleep(300*1000);


	}
}
