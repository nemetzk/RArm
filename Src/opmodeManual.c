/*
 * opmodeManual.c
 *
 *  Created on: 2022. jún. 17.
 *      Author: nemet
 */
#include "stm32f4xx_hal.h"
#include "motion.h"
#include "opmodeManual.h"
#include "servo_definitions.h"

#define OPSEMI_MAN motionb->opManu.state

void opManualCycl(struct motionth *motionb)
{
	switch (OPSEMI_MAN)
	{
		case OPM_INIT:
			OPSEMI_MAN = OPM_WF_RELEASE_SH;
		break;
		case OPM_WF_RELEASE_SH:
			if (SH_A)
				OPSEMI_MAN = OPM_WF_IDLE;
			break;
		case OPM_WF_IDLE:
			REFRESH_SBUS_AN;
			if (SD_A && SH_B)
			{
				OPSEMI_MAN = OPM_IN_PROGRESS_1;
			}
			if (SD_B && SH_B)
			{
				OPSEMI_MAN = OPM_IN_PROGRESS_2;
			}
			if (SD_C && SH_B)
			{
				OPSEMI_MAN = OPM_IN_PROGRESS_3;
			}
			break;
		case OPM_IN_PROGRESS_1:
			servoGoPid(SERVOA, LS_VAL,1);
			if (SH_A || SBUS_ERROR) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				OPSEMI_MAN = OPM_WF_RELEASE_SH;
				servoStop(SERVOA);
			}
		break;
		case OPM_IN_PROGRESS_2:
			servoGoPid(SERVOB, LS_VAL,-1);
			if (SH_A || SBUS_ERROR) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				OPSEMI_MAN = OPM_WF_RELEASE_SH;
				servoStop(SERVOA);
			}

		break;
		case OPM_IN_PROGRESS_3:
			servoGoPid(SERVOC, LS_VAL,1);
			//servoGoForTime(SERVOC, 500, 850);
			if (SH_A || SBUS_ERROR) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				OPSEMI_MAN = OPM_WF_RELEASE_SH;
				servoStop(SERVOC);
			}
		break;

	} //switch
}//fun

