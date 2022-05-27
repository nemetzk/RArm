/*
 * pid.h
 *
 *  Created on: 2022. máj. 24.
 *      Author: nemet
 */

#ifndef INC_PID_H_
#define INC_PID_H_

#include "scaling.h"

#define INT32MAX 0x7FFFFFFF 	// 2 147 483 647
#define INT32MIN 0x80000000 	//-2 147 483 646

#define ERROR_MAX		400
#define ERROR_MIN	   -ERROR_MAX
#define DERIVATIVE_MAX	500
#define DERIVATIVE_MIN -DERIVATIVE_MAX
#define INTEGRAL_MAX	500
#define INTEGRAL_MIN   -INTEGRAL_MAX

#define CONTROL_VAR_MAX 400000
#define CONTROL_VAR_MIN -CONTROL_VAR_MAX

typedef struct pidOpVarsth
{
	int32_t Error;
	int32_t lastError;
	int32_t Integral;
	int32_t Derivative;
	scaledNumt ControlVariable; //ez az output skálázatlanul

}pidOpVarst;

typedef struct pidInputsth
{
	scaledNumt SetPoint;
	scaledNumt ProcessVariable;

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
	pidOpVarst opVariables;
	pidInputst Inputs;

}pidt;


#endif /* INC_PID_H_ */
