/*
 * ServoFB.c
 *
 *  Created on: Jan 20, 2022
 *      Author: nemet
 */
#include "encoder.h"
#include "ServoFB.h"

myTimerType motionTimeoutTimer;

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
void servoCycle_Callback(void)
{
	uint8_t eidx = 0;
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


	for (eidx=1;eidx<=noServos;eidx++)							 //encoder array -hez
	{
		ServoStatusCycl(servos[eidx]);
		/*
		if ( !(servos[eidx]->sInitState ==S_INIT_DONE))
			  ServoInitProcess(servos[eidx]);
			  */
		}



	setTimer(&servos[1]->servoCycleTim);
}

servoFB_init(struct servoFBth *hservoFB)
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
	hservoFB->processTime.set_value = 10000;
	hservoFB->processTime.ownerPtr=hservoFB;
	hservoFB->processTime.Callback=&servoStop;
	initTimer(&hservoFB->processTime);

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
	servoFB->CWcmd=0;
	servoFB->CCWcmd=0;
	//servoFB->encoder.state = 0;
	//servoFB->encoder.encoderTim.Enabled = 0;
	//servoFB->encoder.encoderMeasTim.Enabled = 0;
	servoFB->processTime.Elapsed=0;
	servoFB->processTime.Enabled=0;
}

void servoGoForTimeInit(struct servoFBth *servoFB)
{

	servoFB->goForTimer.Callback = servoStop;
	servoFB->goForTimer.ownerPtr = servoFB;
	initTimer(&servoFB->goForTimer);
}

void servoGoForTime(struct servoFBth *servoFB,uint16_t setTime)
{
	setTimer(&(servoFB->encoder.encoderTim));
	servoFB->CWcmd=1;
	servoFB->goForTimer.set_value = setTime;
	setTimer(&servoFB->goForTimer);
}

void servoGoForPulseCycl(struct servoFBth *servoFB)
{
	if((servoFB->pulse_start) < (servoFB->pulse_dest))
	{ //CW
		if (((servoFB->pulse_dest)-(servoFB->encoder.val))<=4)
		{
			servoFB->CWcmd=0;
			servoFB->encoder.ownerPtr = NULL;
			servoFB->encoder.Callback = NULL;
		}
	}
	else
	{//CCW
		if (((servoFB->encoder.val)-(servoFB->pulse_dest))<=4)
		{
			servoFB->CCWcmd=0;
			servoFB->encoder.ownerPtr = NULL;
			servoFB->encoder.Callback = NULL;
		}
	}
}



void servoGoForPulse(struct servoFBth *servoFB,int32_t setPulse)
{
	servoFB->encoder.ownerPtr = servoFB;
	servoFB->encoder.Callback = servoGoForPulseCycl;

	//servoFB->encoder.val =  __HAL_TIM_GET_COUNTER(servoFB->encoder.enctim);
	servoFB->pulse_start = servoFB->encoder.val;
	servoFB->pulse_dest = servoFB->encoder.val+setPulse;
	setTimer(&(servoFB->encoder.encoderTim));
	if (setPulse > 0)
	{
		servoFB->CWcmd=0XFF;
		servoFB->CCWcmd=0X00;
	}
	else if (setPulse < 0)
	{
		servoFB->CCWcmd=0XFF;
		servoFB->CWcmd=0X00;
	}


}

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
	  	  if (servoFB->servoStatus == SRDY)
	  	  {
	  		servoFB->encoder.pulseInitPassed = 0;
	  		servoFB->sInitState = S_INIT_SEQUENCE_GO_UP;
	  	  }
	  break;
	  case S_INIT_SEQUENCE_GO_UP:
		  //servoFB->reverse_stop_op = 1;
		  servoGoForPulse(servoFB,20);
		  if (servoFB->servoStatus == SCW)
		  //if (servoFB->encoder.speed > 50)
		  {
			 servoFB->sInitState=S_INIT_SEQUENCE_WF_GO_UP;
		  }
	  break;

	  case S_INIT_SEQUENCE_WF_GO_UP:

		  if (servoFB->encoder.speed >= 5)
		  {
			  servoFB->encoder.pulseInitPassed = 1;
			  servoFB->encoder.pulseInitContinue = 0xFF;
		  }
		  else if (servoFB->encoder.speed <= -5)
		  {
			  servoFB->encoder.reverseEncoderValue = 0xFF;
			  servoFB->encoder.pulseInitContinue = 0xFF;
			  servoFB->encoder.pulseInitPassed = 1;
			  servoFB->encoder.pulseInitContinue = 0xFF;
		  }

		  if (servoFB->servoStatus == SRDY)
		  {
		    servoGoForPulse(servoFB,-20);
		  }
		  if (servoFB->servoStatus == SCCW)
		  {
			  servoFB->sInitState=S_INIT_SEQUENCE_WF_GO_DN;
		  }
	  break;

	  case S_INIT_SEQUENCE_WF_GO_DN:

		  if (servoFB->encoder.speed <= -5)
		  {
			  servoFB->encoder.pulseInitPassed = 1;
			  servoFB->encoder.pulseInitContinue = 0xFF;
		  }
		  else if (servoFB->encoder.speed >= 5)
		  {
			  servoFB->encoder.reverseEncoderValue = 0xFF;
			  servoFB->encoder.pulseInitContinue = 0xFF;
			  servoFB->encoder.pulseInitPassed = 1;
			  servoFB->encoder.pulseInitContinue = 0xFF;
		  }

		  if (servoFB->servoStatus == SRDY)
		  {
			  if (servoFB->encoder.pulseInitPassed)
			  {
				  servoFB->reverse_stop_op = 0;
				  servoFB->sInitState = S_INIT_DONE;
			  }
			  else
			  {
				  servoFB->encoder.pulseInitPassed = -1;
				  servoFB->sInitState = SERR;
			  }
		  }

	  break;


	  case S_INIT_DONE:

	  break;

	  case SERR:

	  break;
	}

}

void ServoStatusCycl(struct servoFBth *servoFB)
{
	switch (servoFB->servoStatus)
	{
	case SHALT:

			if ((!servoFB->CWcmd) && (!servoFB->CCWcmd))
			{

				servoFB->servoStatus=SRDY;
			}

			if (((!servoFB->CWcmd) && (!servoFB->CCWcmd)&& (servoFB->encoder.state>=0)))
			{
				servoFB->servoStatus=SRDY;
			}
	break;

	case SRDY:
		if (servoFB->CWcmd)
			{
				set_MOTOR_CW(&(servoFB->pwmch));
				setTimer(&(servoFB->encoder.encoderTim));
				servoFB->servoStatus=SCW;
				setTimer(&(servoFB->processTime));
			}
		else if (servoFB->CCWcmd)
			{
				set_MOTOR_CCW(&(servoFB->pwmch));
				setTimer(&(servoFB->encoder.encoderTim));
				servoFB->servoStatus=SCCW;
				setTimer(&(servoFB->processTime));
			}
	break;
	case SCW:
		if (servoFB->processTime.cur_value> 300)
		{
			if (servoFB->CWcmd)
			{
				if (servoFB->encoder.speed > 0)
				{
					servoFB->encoder.state = 1;
				}
				else if (servoFB->encoder.speed < 0)
				{
					servoFB->encoder.state = -1;
				}

		}

		if (servoFB->encoder.speed == 0)
			{
				//servoFB->encoder.state = 0;
				//set_MOTOR_STOP(&(servoFB->pwmch));
				//servoFB->servoStatus=SHALT;
			}
		}

		if (!servoFB->CWcmd)
			{
				set_MOTOR_STOP(&(servoFB->pwmch));
				servoFB->servoStatus=SHALT;
			}

	break;
	case SCCW:
		if (servoFB->processTime.cur_value> 300)
		{

			if (servoFB->encoder.speed < 0)
			{
				servoFB->encoder.state = 1;
			}
			else if (servoFB->encoder.speed > 0)
			{
				servoFB->encoder.state = -1;
			}

			if (servoFB->encoder.speed == 0)
				{
					//servoFB->encoder.state = 0;
					//set_MOTOR_STOP(&(servoFB->pwmch));
					//servoFB->servoStatus=SHALT;
				}
		}

			if (!servoFB->CCWcmd)
			{
					set_MOTOR_STOP(&(servoFB->pwmch));
					servoFB->servoStatus=SHALT;
			}

	break;
	case SERR:
	break;
	}

}



