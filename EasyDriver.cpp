/*
 * EasyDriver.cpp
 *
 * Copyright Derek Molloy, School of Electronic Engineering, Dublin City University
 * www.eeng.dcu.ie/~molloyd/
 *
 * YouTube Channel: http://www.youtube.com/derekmolloydcu/
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL I
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "EasyDriver.h"
#include "SimpleGPIO.h"
#include <iostream>
#include <unistd.h>
#include <math.h>

using namespace std;

EasyDriver::EasyDriver(int gpio_MS0,int gpio_MS1, int gpio_MS2, int gpio_SLP, int gpio_STEP,
					   int gpio_DIR, int gpio_STOP1, int gpio_STOP2, int speedPPS, int stepsPerRevolution)
{

	this->gpio_MS0  = gpio_MS0;
	this->gpio_MS1  = gpio_MS1;
	this->gpio_MS2  = gpio_MS2;
	this->gpio_SLP  = gpio_SLP;
	this->gpio_STEP = gpio_STEP;
	this->gpio_DIR  = gpio_DIR;
	this->gpio_STOP1 = gpio_STOP1;
	this->gpio_STOP2 = gpio_STOP2;

	gpio_export(this->gpio_MS0);
	gpio_set_dir(this->gpio_MS0, OUTPUT_PIN);
	gpio_export(this->gpio_MS1);
	gpio_set_dir(this->gpio_MS1, OUTPUT_PIN);
	gpio_export(this->gpio_MS2);
	gpio_set_dir(this->gpio_MS2, OUTPUT_PIN);
	gpio_export(this->gpio_SLP);
	gpio_set_dir(this->gpio_SLP, OUTPUT_PIN);
	gpio_export(this->gpio_STEP);
	gpio_set_dir(this->gpio_STEP, OUTPUT_PIN);
	gpio_export(this->gpio_DIR);
	gpio_set_dir(this->gpio_DIR, OUTPUT_PIN);
	gpio_export(this->gpio_STOP1);
	gpio_set_dir(this->gpio_STOP1, INPUT_PIN);
	gpio_export(this->gpio_STOP2);
	gpio_set_dir(this->gpio_STOP2, INPUT_PIN);

	// default to clockwise direction
	setDirection(true); //clockwise = true;
	// default to full stepping
	setStepMode(STEP_FULL);
	cout << "Stepmode set" << endl;
	// the default number of steps per revolution
	setStepsPerRevolution(stepsPerRevolution);
	// the default speed in rpm
	setSpeed(speedPPS);
	//wake up the controller - holding torque..
	wake();
}

void EasyDriver::setStepMode(STEP_MODE mode) {
	this->stepMode = mode;
	switch(stepMode){
	case STEP_FULL:
		gpio_set_value(this->gpio_MS0, LOW);
		gpio_set_value(this->gpio_MS1, LOW);
		gpio_set_value(this->gpio_MS2, LOW);
		this->delayFactor = 1;

		break;
	case STEP_HALF:
		gpio_set_value(this->gpio_MS0, HIGH);
		gpio_set_value(this->gpio_MS1, LOW);
		gpio_set_value(this->gpio_MS2, LOW);
		this->delayFactor = 2;

		break;
	case STEP_QUARTER:
		gpio_set_value(this->gpio_MS0, LOW);
		gpio_set_value(this->gpio_MS1, HIGH);
		gpio_set_value(this->gpio_MS2, LOW);
		this->delayFactor = 4;

		break;
	case STEP_EIGHT:
		gpio_set_value(this->gpio_MS0, HIGH);
		gpio_set_value(this->gpio_MS1, HIGH);
		gpio_set_value(this->gpio_MS2, LOW);
		this->delayFactor = 8;

		break;
	case STEP_SIXTEEN:
			gpio_set_value(this->gpio_MS0, LOW);
			gpio_set_value(this->gpio_MS1, LOW);
			gpio_set_value(this->gpio_MS2, HIGH);
			this->delayFactor = 16;

			break;
	case STEP_THIRTYTWO:
			gpio_set_value(this->gpio_MS0, HIGH);
			gpio_set_value(this->gpio_MS1, HIGH);
			gpio_set_value(this->gpio_MS2, HIGH);
			this->delayFactor = 32;
			break;

	}
}

void EasyDriver::setSpeed(float pps) {
	this->speed = pps;
	float delayPerSec = (1/speed);// delay per step in seconds
	cout<<"SETSPEED "<<speed<< " FRACTION " << delayPerSec <<endl;
	this->uSecDelay = (int)(delayPerSec * 1000 * 1000); // in microseconds
}
void EasyDriver::setDirection(bool direction) {
	if(direction)
	{  clockwise=true;
		gpio_set_value(this->gpio_DIR, HIGH);
	}
	else if(!direction)
	{
		clockwise=false;
		gpio_set_value(this->gpio_DIR, LOW);
	}
}

void EasyDriver::rotate(int degrees){
	float degreesPerStep = 360.0f/getStepsPerRevolution();
	int gearratio = 30;
	int numberOfSteps = gearratio*(degrees/degreesPerStep);
	cout << "(ROTATION)The number of steps is " << numberOfSteps << endl;
	step(numberOfSteps*delayFactor); //need look at rotation

}

void EasyDriver::step(int numberOfSteps){ //,float sPPs

	//float sleepsec = (1/sPPs);
	//int sleeptime = sleepsec *1000*1000;
	this->sleepDelay = uSecDelay/delayFactor;
	int Steps= numberOfSteps*delayFactor;
	//cout << "The sleep delay factor is " << delayFactor << endl;

	cout << "Doing "<< Steps << " at " << uSecDelay << "PPS TD~~ "<< sleepDelay <<"\n";

	if (clockwise)
	{
		gpio_set_value(this->gpio_DIR, HIGH);
		for(int i=0; i<Steps; i++)
		{
			gpio_set_value(this->gpio_STEP, LOW);
			usleep(sleepDelay*0.5);
			//cout<<"Address="<<&uSecDelay<<endl;
			gpio_set_value(this->gpio_STEP, HIGH);
			usleep(sleepDelay*0.5);
		}
	}
	else if(!clockwise)
	{
		gpio_set_value(this->gpio_DIR, LOW);
		for(int i=0; i<Steps; i++)
		{
		gpio_set_value(this->gpio_STEP, LOW);
		usleep(sleepDelay*0.5);
		//cout<<"Address="<<&uSecDelay<<endl;
		gpio_set_value(this->gpio_STEP, HIGH);
		usleep(sleepDelay*0.5);
		}
	}
}




unsigned int EasyDriver::calibrate()
{


	unsigned int  STOP1, STOP2;

			gpio_get_value(this->gpio_STOP1, &STOP1);
			gpio_get_value(this->gpio_STOP2, &STOP2);

			while(STOP1 !=0 && STOP2 !=0)  //Testing if limits are already pressed
				{
				step(1);
				gpio_get_value(this->gpio_STOP1, &STOP1);
				gpio_get_value(this->gpio_STOP2, &STOP2);
				}
	//Cart is now at one end of the rig

	usleep(500*1000);
	int count=0;
	reverseDirection();

	if(STOP1 != 1)
	{
	while(STOP2 == 1)
		{
		step(1);
		count=count+1;
		gpio_get_value(this->gpio_STOP2, &STOP2);
		}
	}
	else
	{
	while(STOP1 == 1)
		{
		step(1);
		count=count+1;
		gpio_get_value(this->gpio_STOP1, &STOP1);
		}
	}

	reverseDirection();
	return count;


}

void EasyDriver::Initialize(){
	reverseDirection();
	unsigned int  STOP1, STOP2;
		gpio_get_value(this->gpio_STOP1, &STOP1);
		gpio_get_value(this->gpio_STOP2, &STOP2);

		while(STOP1 !=0 && STOP2 !=0)  //Testing if limits are already pressed
			{
			step(1);
			gpio_get_value(this->gpio_STOP1, &STOP1);
			gpio_get_value(this->gpio_STOP2, &STOP2);
			}
		reverseDirection();
}

void EasyDriver::Accelerate(int fspeed,int tsteps)
{
	int accstep=100;
this-> Acc=round(fspeed/accstep);
cout<<"Number of acc steps="<< Acc << endl;
for(i=1; i<=Acc; i++)//i and Acc are class variables
{
setSpeed(i*accstep);
step(3);
}
setSpeed(fspeed);
step(tsteps-(2*Acc*3));

for(i=Acc; i>0; i--)
{
setSpeed(i*accstep);
step(3);
}
}


void EasyDriver::unexport(){
		gpio_unexport(this->gpio_MS0);
		gpio_unexport(this->gpio_MS1);
		gpio_unexport(this->gpio_MS2);
		gpio_unexport(this->gpio_SLP);
		gpio_unexport(this->gpio_STEP);
		gpio_unexport(this->gpio_DIR);
		gpio_unexport(this->gpio_STOP1);
		gpio_unexport(this->gpio_STOP2);

	}

EasyDriver::~EasyDriver() {
	cout <<"~~~EasyDriver~~~" << endl;
}


//Changes made
//Sleep and Step positions swapped
//MS0 added
//Speed changed to Pulses per second
//Testing calibrate function
//Test functions made to test threads

