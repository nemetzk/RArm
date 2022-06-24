/*
 * servoMFP.c
 *
 *  Created on: 2022. jún. 17.
 *      Author: nemet
 */

#include "encoderMFP.h"
#include "ServoMFP.h"

myTimerType motionTimeoutTimer;
uint8_t sidx = 1;
#define NO_SERVOS 3  				 	//servo array -hez
servoFBt *servos[NO_SERVOS]; 	    //servo array -hez
uint8_t noServos = 0;
/*
void encoderTimEA(struct servoFBth *hservoFB) //amikor megáll a mozgás ez az interrupt rutin fut le
{
	//hservoFB->pwmch;
	//set_MOTOR_STOP(&(hservoFB->pwmch));
	servoStop(hservoFB);
}
*/

uint8_t servoGoPid(struct servoFBth *servo,int32_t dist)
{
	  uint8_t result;
	  servo->myPID.Inputs.ProcessVariable.RawInput.value = servo->encoder.val;
	  servo->myPID.Inputs.SetPoint.RawInput.value = dist;
	  pidCalc(&(servo->myPID));
	  if((servo->myPID.opVariables.ControlVariable.ScaledOutput.value >= 500) &&
	     (servo->myPID.opVariables.ControlVariable.ScaledOutput.value <= 1000))
	  	  {
		  	  set_MOTOR_spd(&servo->pwmch, servo->myPID.opVariables.ControlVariable.ScaledOutput.value);
		  	  result = 0;
	  	  }
/*
	  if (abs(servo->myPID.opVariables.Error) < 2)
	  {
		  servoStop(servo);
		  servo->myPID.opVariables.justStartedFlg = 1;
		  result = 1;
	  }*/
	  return result;
}

void cpyAxis(struct servoFBth *servo_to,struct servoFBth *servo_from,int32_t dir)
{
	int16_t spd_from;
	int16_t spd_to;
	spd_from = servo_from->myPID.opVariables.ControlVariable.ScaledOutput.value;
	if (dir == 1)
	{
		spd_to = spd_from;
	}
	else if (dir == -1)
	{
		spd_to = spd_from - 2*(spd_from-750);
	}
	set_MOTOR_spd(&servo_to->pwmch, spd_to);
}


void servoCycle_Callback(void)
{
	if (!(servos[sidx]->sInitState ==S_INIT_DONE))
		{
			ServoInitProcess(servos[sidx]);
		}
	else if ((servos[sidx]->sInitState ==S_INIT_DONE))
		{
			sidx++;
		}

	if (!(sidx = noServos && servos[sidx]->sInitState ==S_INIT_DONE))
		{
			setTimer(&servos[1]->servoCycleTim);
		}
}

/* deactivated at: 220624
	if (!(servos[1]->sInitState ==S_INIT_DONE))
		{
			ServoInitProcess(servos[1]);
		}
	if (!(servos[2]->sInitState ==S_INIT_DONE))
			{
				if ((servos[1]->sInitState ==S_INIT_DONE)){
					ServoInitProcess(servos[2]);
				}
			}
	if(!((servos[1]->sInitState ==S_INIT_DONE)  && (servos[2]->sInitState ==S_INIT_DONE)))
		setTimer(&servos[1]->servoCycleTim);
*/



servoInit(struct servoFBth *hservoFB)
{
	hservoFB->number=++noServos;  //servo array -hez
	servos[noServos]=hservoFB;   	//servo array -hez

	hservoFB->encoder.enctim = hservoFB->encTim;
	hservoFB->encoder.encoderTim.set_value=500;
	//hservoFB->encoder.encoderTim.Callback=&encoderTimEA;
	hservoFB->encoder.encoderTim.Callback=&servoStop;
	hservoFB->encoder.encoderTim.ownerPtr = hservoFB;
	hservoFB->encoder.ownerPtr = NULL;
	hservoFB->encoder.Callback = NULL;
	encoder_init(&(hservoFB->encoder),(hservoFB->encTim));
	/*
	hservoFB->processTime.set_value = 10000;
	hservoFB->processTime.ownerPtr=hservoFB;
	hservoFB->processTime.Callback=&servoStop;
	initTimer(&hservoFB->processTime);
	*/
	servoGoForTimeInit(hservoFB);


	hservoFB->myPID.settings.Kp = 6000;
	hservoFB->myPID.settings.Ki = 50;
	hservoFB->myPID.settings.Kd = 1000;
	pidInit(&(hservoFB->myPID));

	//********************************************
	// szervókat egyszerre kezelő ciklusfüggvényt meghívó timer készítése
	// csak az első szervó initnél hozunk létre összesen 1 timert


	if (noServos == 1)
	{
		hservoFB->servoCycleTim.set_value = 50;
		hservoFB->servoCycleTim.Callback = &servoCycle_Callback;
		hservoFB->servoCycleTim.ownerPtr = hservoFB;
		initTimer(&hservoFB->servoCycleTim);
		setTimer(&hservoFB->servoCycleTim);
	}
/*
 * PWM Init
 */

	  hservoFB->pwmch.htim = hservoFB->pwmTim;
	  hservoFB->pwmch.ch = hservoFB->pwmCh;
	  pwm_init(&(hservoFB->pwmch));
	  servoGoForTimeInit(hservoFB);

}

void servoStop(struct servoFBth *servoFB)
{
	set_MOTOR_STOP(&(servoFB->pwmch));
	//servoFB->processTime.Elapsed=0;
	//servoFB->processTime.Enabled=0;
	servoFB->myPID.opVariables.justStartedFlg = 1;
}

void servoGoForTimeInit(struct servoFBth *servoFB)
{

	servoFB->goForTimer.Callback = servoStop;
	servoFB->goForTimer.ownerPtr = servoFB;
	initTimer(&servoFB->goForTimer);
}

void servoGoForTime(struct servoFBth *servoFB,uint16_t setTime, int16_t motorSpeed)
{
	if (motorSpeed>=500 && motorSpeed <=1000 && setTime>0 && setTime<10000)
	{
		servoFB->goForTimer.set_value=setTime;
		set_MOTOR_spd(&servoFB->pwmch, motorSpeed);
		servoFB->goForTimer.set_value = setTime;
		setTimer(&servoFB->goForTimer);
	}
}

// setTimer(&(servoFB->goForPulseCycleTimer));


#define spINIT 		0
#define spRELEASE	2
#define spHOME		3
#define spPROCESS   4
#define spFINISHED  1
#define spError		-1

//FELADAT: STÁTUSZ ÜZENETEKET MEGOL



// BE KELL TENNI A VALAMI CIKLUSBA VIGYÁZZ MINDEN SZERVÓRA KÜLÖN!
//PL. MEGCSINÁLHAT MAGÁNAK EGY X MS-ES TIMER CIKLUST A SERVO AZ INITJEKOR


void ServoInitProcess(struct servoFBth *servoFB)
{
	switch (servoFB->sInitState)
	{
	  case S_WF_SERVO_RDY:
	  		servoFB->sInitState = S_INIT_SEQUENCE_GO_UP;
	  break;
	  case S_INIT_SEQUENCE_GO_UP:
		  servoGoForTime(servoFB, 200, 850);
  		  servoFB->sInitState=S_INIT_SEQUENCE_WF_GO_UP;
	  break;

	  case S_INIT_SEQUENCE_WF_GO_UP:


		  if (servoFB->encoder.diff_10ms >= 2)
		  {

			  servoFB->encoder.pulseInitPassed = 1;
			  servoFB->encoder.pulseInitContinue = 0xFF;
			  servoGoForTime(servoFB, 200, 650);
			  servoFB->sInitState=S_INIT_SEQUENCE_WF_GO_DN;

		  }
		  else if (servoFB->encoder.diff_10ms <= -2)
		  {
			  servoFB->encoder.reverseEncoderValue = 0xFF; //encoder irányt fordít
			  servoFB->encoder.pulseInitContinue = 0xFF;
			  servoFB->encoder.pulseInitPassed = 1;
			  servoFB->encoder.pulseInitContinue = 0xFF;
			  servoGoForTime(servoFB, 200, 650);
			  servoFB->sInitState=S_INIT_SEQUENCE_WF_GO_DN;
		  }

	  break;

	  case S_INIT_SEQUENCE_WF_GO_DN:


		  if (servoFB->encoder.diff_10ms <= -2)
		  {
			  servoFB->encoder.reverseEncoderValue = 0xFF;
			  servoFB->encoder.pulseInitPassed = 1;
			  servoFB->encoder.pulseInitContinue = 0xFF;
		  }
		  else if (servoFB->encoder.diff_10ms >= 2)
		  {

			  servoFB->encoder.pulseInitContinue = 0xFF;
			  servoFB->encoder.pulseInitPassed = 1;
			  servoFB->encoder.pulseInitContinue = 0xFF;
		  }

		  if (servoFB->encoder.pulseInitPassed)
			  {
			  	  servoStop(servoFB);
				  servoFB->sInitState = S_INIT_DONE;
			  }

/*
 * 			  else
			  {
				  servoFB->encoder.pulseInitPassed = -1;
				  servoFB->sInitState = SERR;
			  }
 */
	  break;


	  case S_INIT_DONE:

	  break;

	  case SERR:

	  break;
	}

}

void servoPidCycl(struct servoFBth *servoFB)
{
	servoFB->myPID.Inputs.ProcessVariable.RawInput.value = servoFB->encoder.val;
	  pidCalc(&(servoFB->myPID));
	  if((servoFB->myPID.opVariables.ControlVariable.ScaledOutput.value >= 500) &&
	     (servoFB->myPID.opVariables.ControlVariable.ScaledOutput.value <= 1000))
	  	  {
		  	  set_MOTOR_spd(&servoFB->pwmch, servoFB->myPID.opVariables.ControlVariable.ScaledOutput.value);
	  	  }
}



