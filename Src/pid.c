/*
 * pid.c
 *
 *  Created on: 2022. máj. 24.
 *      Author: nemet
 */
#include "pid.h"

void pidErrorCalc(pidt *hpid)
{
	hpid->opVariables.Error = hpid->Inputs.SetPoint.ScaledOutput.value -  hpid->Inputs.ProcessVariable.ScaledOutput.value;
	if (hpid->opVariables.Error > ERROR_MAX)
		hpid->opVariables.Error = ERROR_MAX;
	else if (hpid->opVariables.Error < ERROR_MIN)
		hpid->opVariables.Error = ERROR_MIN;
}

void pidIntegralCalc(pidt *hpid)
{
	hpid->opVariables.Integral = hpid->opVariables.Integral + hpid->opVariables.Error;
	if (hpid->opVariables.Integral > INTEGRAL_MAX)
		hpid->opVariables.Integral  = INTEGRAL_MAX;
	else if (hpid->opVariables.Integral < INTEGRAL_MIN)
		hpid->opVariables.Integral  = INTEGRAL_MIN;
}

void pidDerivativeCalc(pidt *hpid)
{
	hpid->opVariables.Derivative = hpid->opVariables.Error - hpid->opVariables.lastError;
	if (hpid->opVariables.Derivative > DERIVATIVE_MAX)
		hpid->opVariables.Derivative  = DERIVATIVE_MAX;
	else if (hpid->opVariables.Derivative < DERIVATIVE_MIN)
		hpid->opVariables.Derivative  = DERIVATIVE_MIN;
	hpid->opVariables.lastError = hpid->opVariables.Error;
}

void pidCalc(pidt *hpid)
{
	calcScaledVal(&(hpid->Inputs.ProcessVariable));
	calcScaledVal(&(hpid->Inputs.SetPoint));


	pidErrorCalc(hpid);
	pidIntegralCalc(hpid);
	pidDerivativeCalc(hpid);

	hpid->opVariables.ControlVariable.RawInput.value =
			hpid->settings.Kp * hpid->opVariables.Error    +
			hpid->settings.Ki * hpid->opVariables.Integral +
			hpid->settings.Kd * hpid->opVariables.Derivative;

	if (hpid->opVariables.ControlVariable.RawInput.value > CONTROL_VAR_MAX)
		hpid->opVariables.ControlVariable.RawInput.value = CONTROL_VAR_MAX;
	else if (hpid->opVariables.ControlVariable.RawInput.value < CONTROL_VAR_MIN)
		hpid->opVariables.ControlVariable.RawInput.value = CONTROL_VAR_MIN;

	calcScaledVal(&(hpid->opVariables.ControlVariable));
}

void pidInitStartup(pidt *hpid)
{
	initScaledVal(&(hpid->Inputs.ProcessVariable));
	initScaledVal(&(hpid->Inputs.SetPoint));
	initScaledVal(&(hpid->opVariables.ControlVariable));

}

void pidInit(pidt *hpid)
{

	calcScaledVal(&(hpid->Inputs.ProcessVariable));
	calcScaledVal(&(hpid->Inputs.SetPoint));

	hpid->opVariables.lastError = 0;
	hpid->opVariables.Error = 0;
	hpid->opVariables.Derivative = 0;
	hpid->opVariables.Integral = 0;
	hpid->opVariables.ControlVariable.RawInput.value = 0;

}
