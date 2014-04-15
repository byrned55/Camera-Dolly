//============================================================================
// Name        : Camdolly.cpp
// Author      : Tom Byrne
// Version     :
// Copyright   :
// Description : Main camera dolly program for Tracking shots or IPA stills
//============================================================================



#include "EasyDriver.h"
#include "SimpleGPIO.h"
#include <iostream>
#include <unistd.h>
#include <cmath>
#include <thread>

using namespace std;

int main(int argc, char ** argv)
{

	EasyDriver PanMotor(60,50,51,4,2,15,14,112,500,200);

	//Pin	Gpio   Address	Function
	//P9_12	60		878		MS0
	//P9_14	50		848		MS1
	//P9_16	51		84c		MS2
	//P9_18	4		958		Sleep
	//P9_22	2		950		!!STEP!!
	//P9_24	15		984		Direction
	//P9_26	14		980		STOP1
	//P9_30	112		998		STOP2

	EasyDriver RotateMotor(22,27,47,45,69,66,65,61,500,200);
	//Pin	Gpio   Address	Function
	//P8_19	22		820		MS0
	//P8_17	27		82c		MS1
	//P8_15	47		83c		MS2
	//P8_11	45		834		Sleep
	//P8_09	69		89c		!!STEP!!
	//P8_07	66		890		Direction
	//P8_18	65		88c		STOP1
	//P8_26	61		87c		STOP2

//INITIALIZING VARIABLES
	float Vidtime =5;
	float Distance= 0.5;
	int IPAsnaps  = 5;

	PanMotor.setSpeed(500);
	int Totalsteps=PanMotor.calibrate()-10;//To stop the cart 10 steps before the end
	//PanMotor.Initialize();
	//int Totalsteps=2653; //Used when calibrated previously
	int Panspeed=Totalsteps/Vidtime;

	float pulley = 22.3;//Diameter dimension in mm
	int gearratio=30; //Rotation gear ratio
	float Rsteps=RotateMotor.getStepsPerRevolution();


	float DollyLength=((Totalsteps/Rsteps)*(2*M_PI*((pulley/2.0)/1000.0)));

	float theta=(atan(Distance/(DollyLength/2)))*(180/M_PI);
	float fi=180-(2*theta);
	int SAngle=90-theta;

	float Angfraction=(fi/360.0);
	float Rotatedistance=((Angfraction*gearratio)*Rsteps);
	float Rotatespeed=Rotatedistance/Vidtime;
	int Rotatesteps=round(Rotatedistance);

//PRINT OUT VARIABLES
	cout<< "Total Pan steps="<< Totalsteps <<endl;
	cout<< "Pan Speed="<< Panspeed<< endl;
	cout<< "Angle of Movement="<< fi <<endl;
	cout<< "Rotate Speed="<< Rotatespeed<< endl;
	cout << "RotateSteps= " << Rotatesteps <<endl;
	cout << "Dolly Length= " << DollyLength <<endl;

	cout << "Values set" << endl;

	usleep(2000*1000);


//INITIALIZING POISITIONS
	RotateMotor.setSpeed(700);
	RotateMotor.rotate(SAngle);
	RotateMotor.reverseDirection();



//SETTING MOTOR MODES
	RotateMotor.setStepMode(STEP_HALF);
	PanMotor.setStepMode(STEP_FULL);

//TRACKING SHOT
	PanMotor.setSpeed(Panspeed);
	RotateMotor.setSpeed(Rotatespeed);
	thread Worm(&EasyDriver::step,RotateMotor,Rotatesteps);
	cout << "Main Thread continues!!" << endl;
	PanMotor.Accelerate(Panspeed,Totalsteps);

/*
//IPA IMAGES
	int Panseg=Totalsteps/(IPAsnaps-1);
	int Rotateseg=Rotatesteps/(IPAsnaps-1);
	PanMotor.setSpeed(300);
	RotateMotor.setSpeed(300);
	for(int P=0; P<=(IPAsnaps-2); P++ )
	{
		usleep(1000*1000);
		PanMotor.step(Panseg);
		RotateMotor.step(Rotateseg);
	}
	usleep(1000*1000);
*/
//FINISHING FUNCTIONS
	Worm.join();
	usleep(2000*1000);
	RotateMotor.setSpeed(700);
	RotateMotor.setStepMode(STEP_FULL);
	int resetangle =fi-SAngle;
		RotateMotor.reverseDirection();
		RotateMotor.rotate(resetangle);
		RotateMotor.reverseDirection();


		cout<< "Total Pan steps="<< Totalsteps <<endl;
			cout<< "Pan Speed="<< Panspeed<< endl;
			cout<< "Angle of Movement="<< fi <<endl;
			cout<< "Rotate Speed="<< Rotatespeed<< endl;
			cout << "RotateSteps= " << Rotatesteps <<endl;
			cout << "Dolly Length= " << DollyLength <<endl;

	PanMotor.unexport();
	RotateMotor.unexport();

	cout << "Stepped!" << endl; // prints Finish!
	return 0;

}
