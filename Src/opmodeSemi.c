/*
 * opmodeSemi.c
 *
 *  Created on: 2022. jún. 17.
 *      Author: nemet
 */

#include "stm32f4xx_hal.h"
#include "motion.h"
#include "opmodeSemi.h"
#include "servo_definitions.h"


#define OPSEMI_STATE motionb->opSemi.state

void opSemiCycl(struct motionth *motionb)
{
	switch (OPSEMI_STATE)
	{
		case OPS_INIT:
			OPSEMI_STATE = OPS_WF_RELEASE_SH;
		break;
		case OPS_WF_RELEASE_SH:
			if (SH_A)
				OPSEMI_STATE = OPS_WF_IDLE;
			break;
		case OPS_WF_IDLE:
			REFRESH_SBUS_AN;
			if (SD_A && SH_B)
			{
				OPSEMI_STATE = OPS_IN_PROGRESS_1;
			}
			if (SD_B && SH_B)
			{
				OPSEMI_STATE = OPS_IN_PROGRESS_2;
			}
			if (SD_C && SH_B)
			{
				OPSEMI_STATE = OPS_IN_PROGRESS_3;
			}
			break;
		case OPS_IN_PROGRESS_1:

			if (moveServoA(motionb, LS_VAL) || SH_A || SBUS_ERROR) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				//motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 0;
				servoStop(SERVOA);
				servoStop(SERVOB);
				servoStop(SERVOC);
				OPSEMI_STATE = OPS_WF_RELEASE_SH;
			}

		break;
		case OPS_IN_PROGRESS_2:
			if (moveServoB(motionb, LS_VAL) || SH_A || SBUS_ERROR) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				//motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 0;
				servoStop(SERVOA);
				servoStop(SERVOB);
				servoStop(SERVOC);
				OPSEMI_STATE = OPS_WF_RELEASE_SH;
			}
		break;

		case OPS_IN_PROGRESS_3:
			if (moveServoC(motionb, LS_VAL) || SH_A || SBUS_ERROR) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				//motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 0;
				servoStop(SERVOA);
				servoStop(SERVOB);
				servoStop(SERVOC);
				OPSEMI_STATE = OPS_WF_RELEASE_SH;
			}
		break;

	} //switch
}//funct
