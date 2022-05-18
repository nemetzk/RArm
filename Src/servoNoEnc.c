/*
 * servoNoEnc.c
 *
 *  Created on: 2022. márc. 25.
 *      Author: nemet
 */
#include "encoder.h"
#include "ServoNoEnc.h"
#include "sbus_fnc.h"

#define NO_SERVOS_NE 2  				 	//servo array -hez
servoNet *servones[NO_SERVOS_NE+1]; 	    //servo array -hez
uint8_t noServosNe = 0;

servoNE_init(struct servoNeth *hservoNE)
{
	hservoNE->pwmch.htim = hservoNE->pwmTim;
	hservoNE->pwmch.ch = hservoNE->pwmCh;
	hservoNE->number=++noServosNe;  //servo array -hez
	servones[noServosNe]=hservoNE;   	//servo array -hez
	if (hservoNE->opMode ==SNE_OPMODE_COPY)
			HAL_TIM_PWM_Start(hservoNE->pwmch.htim, hservoNE->pwmch.ch);

	/*if (hservoNE->opMode ==SNE_OPMODE_JOGMODE)
		{
			hservoNE->processTime.set_value = 7000;
			hservoNE->processTime.ownerPtr=hservoNE;
			hservoNE->processTime.Callback=&servoStopNE;
			initTimer(&hservoNE->processTime);
		}*/
			// szervókat egyszerre kezelő ciklusfüggvényt meghívó timer készítése
				// csak az első szervó initnél hozunk létre összesen 1 timert
	servoGoForTimeInit_NE(hservoNE);
	if (noServosNe == 1)
	{
		hservoNE->servoCycleTim.set_value = 150;
		hservoNE->servoCycleTim.Callback = &servoCycle_Callback_NE;
		hservoNE->servoCycleTim.ownerPtr = hservoNE;
		initTimer(&hservoNE->servoCycleTim);
		setTimer(&hservoNE->servoCycleTim);
	}

}

void servoGoForTimeInit_NE(struct servoNeth *hservoNE)
{
	hservoNE->goForTimer.Callback = servoStopNE;
	hservoNE->goForTimer.ownerPtr = hservoNE;
	initTimer(&hservoNE->goForTimer);
}

void servoGoForTime_NE(struct servoNeth *hservoNE,uint16_t setTime,int8_t direction)
{
	if (direction>0)
		hservoNE->CWcmd=1;
	else if (direction<0)
		hservoNE->CCWcmd=1;

	hservoNE->goForTimer.set_value = setTime;
	setTimer(&hservoNE->goForTimer);
}

void rcCopy(struct servoNeth *hservoNE)
{
		pwmSet(&(hservoNE->pwmch),(uint32_t)(hservoNE->inputRcCH->scaledVal.value));
}

void servoCycle_Callback_NE(void)
{
	uint8_t eidx = 0;
	for (eidx=1;eidx<=noServosNe;eidx++)							 //encoder array -hez
	{
		if (servones[eidx]->opMode == SNE_OPMODE_JOGMODE)
			ServoStatusCyclNE(servones[eidx]);
		if (servones[eidx]->opMode == SNE_OPMODE_COPY)
			rcCopy(servones[eidx]) ;
		}
	setTimer(&(servones[1]->servoCycleTim));
}

void servoStopNE(struct servoNeth *servoNE)
{
	set_MOTOR_STOP(&(servoNE->pwmch));
	servoNE->CWcmd=0;
	servoNE->CCWcmd=0;

}

void readServoNEGPIO(struct servoNeth *servoNE)
{
	if (servoNE->limitSwitch.Conf.LSSignalsrEnabled)
	{
		servoNE->limitSwitch.CW_signal = HAL_GPIO_ReadPin(servoNE->limitSwitch.Conf.GPIO_port_CW, servoNE->limitSwitch.Conf.GPIO_Pin_CW);
		servoNE->limitSwitch.CCW_signal = HAL_GPIO_ReadPin(servoNE->limitSwitch.Conf.GPIO_port_CCW, servoNE->limitSwitch.Conf.GPIO_Pin_CCW);
	}
}

#define SNEHALT 	0
#define SNERDY		1
#define SNECW		2
#define SNECCW		3
#define SNEERR		4

#define IS_LIMITSW_EN_AND_CW_ACTIVE  ((servoNE->limitSwitch.Conf.LSSignalsrEnabled) && (servoNE->limitSwitch.CW_signal))
#define IS_LIMITSW_EN_AND_CCW_ACTIVE  ((servoNE->limitSwitch.Conf.LSSignalsrEnabled) && (servoNE->limitSwitch.CCW_signal))



void ServoStatusCyclNE(struct servoNeth *servoNE)
{
	readServoNEGPIO(servoNE);
	switch (servoNE->servoStatus)
	{
	case SNEHALT:
		if ((!servoNE->CWcmd) && (!servoNE->CCWcmd))
		{

			servoNE->servoStatus=SNERDY;
		}

	break;

	case SNERDY:
	if (servoNE->CWcmd && (TIM_CHANNEL_STATE_GET(servoNE->pwmch.htim, servoNE->pwmch.ch) == HAL_TIM_CHANNEL_STATE_READY))
		{
			if (!IS_LIMITSW_EN_AND_CW_ACTIVE)
			{
				set_MOTOR_CW(&(servoNE->pwmch));
				servoNE->servoStatus=SNECW;
			}
			//setTimer(&(servoNE->processTime));
		}
	else if (servoNE->CCWcmd && (TIM_CHANNEL_STATE_GET(servoNE->pwmch.htim, servoNE->pwmch.ch) == HAL_TIM_CHANNEL_STATE_READY))
		{
		if (!IS_LIMITSW_EN_AND_CCW_ACTIVE)
		{
			set_MOTOR_CCW(&(servoNE->pwmch));
			servoNE->servoStatus=SNECCW;
		}
			//setTimer(&(servoNE->processTime));
		}
	break;

	case SNECW:
		if ((!servoNE->CWcmd) || IS_LIMITSW_EN_AND_CW_ACTIVE)
			{
				set_MOTOR_STOP(&(servoNE->pwmch));
				servoNE->servoStatus=SNEHALT;
			}


	break;

	case SNECCW:
		if ((!servoNE->CCWcmd) || IS_LIMITSW_EN_AND_CCW_ACTIVE)
			{
				set_MOTOR_STOP(&(servoNE->pwmch));
				servoNE->servoStatus=SNEHALT;
			}

	break;

	case SNEERR:
	break;
	}

}

