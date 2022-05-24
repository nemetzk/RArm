/*
 * pid.h
 *
 *  Created on: 2022. máj. 24.
 *      Author: nemet
 */

#ifndef INC_PID_H_
#define INC_PID_H_

#include "scaling.h"

typedef struct pidProcessVarsth
{
	int32_t Error;
	int32_t lastError;
	int32_t Integral;
	int32_t Derivative;
	scaledNumItemth ControlVariable; //ez az output skálázatlanul

}pidProcessVarst;

typedef struct pidInputsth
{
	scaledNumItemth SetPoint;
	scaledNumItemth ProcessVariable;

}pidInputst;



typedef struct pidSettingsth
{
	int32_t Kp;
	int32_t Kd;
	int32_t Ki;
	int32_t timeIntervalMs;

}pidSettingst;


typedef struct pidth
{
	pidSettingst settings;
	pidProcessVarst processVariables;
	pid
	myTimerType PIDCycleTimer;
}pidt;


#endif /* INC_PID_H_ */
