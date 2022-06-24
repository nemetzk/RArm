/*
 * opmodeAut.c
 *
 *  Created on: 2022. jún. 17.
 *      Author: nemet
 */
#include "stm32f4xx_hal.h"
#include "motion.h"
#include "opmodeAut.h"
#include "servo_definitions.h"

#define OPAUT_STATE motionb->opAut.state

void opAutCycl(struct motionth *motionb)
{
	switch (OPAUT_STATE)
	{
		case OPA_INIT:
			OPAUT_STATE = OPA_WF_RELEASE_SH;
		break;
		case OPA_WF_RELEASE_SH:
			if (SH_A)
				OPAUT_STATE = OPA_WF_IDLE;
			break;
		case OPA_WF_IDLE:
			REFRESH_SBUS_AN;
			if (SD_A && SH_B)
			{
				OPAUT_STATE = OPA_IN_PROGRESS_1;
			}
			if (SD_B && SH_B)
			{
				OPAUT_STATE = OPA_IN_PROGRESS_2;
			}
			if (SD_C && SH_B)
			{
				OPAUT_STATE = OPA_IN_PROGRESS_3;
			}
			break;
		case OPA_IN_PROGRESS_1:
			if (SH_A ||SBUS_ERROR) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				OPAUT_STATE = OPA_WF_RELEASE_SH;
			}
		break;
		case OPA_IN_PROGRESS_2:
			if (SH_A || SBUS_ERROR) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				OPAUT_STATE = OPA_WF_RELEASE_SH;
			}
		break;
		case OPA_IN_PROGRESS_3:
			if (SH_A || SBUS_ERROR) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				OPAUT_STATE = OPA_WF_RELEASE_SH;
			}
		break;

	} //switch
}//funct
