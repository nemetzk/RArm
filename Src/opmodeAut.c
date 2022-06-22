/*
 * opmodeAut.c
 *
 *  Created on: 2022. jún. 17.
 *      Author: nemet
 */
#include "stm32f4xx_hal.h"
#include "motion.h"

#include "opmodeAut.h"

void opAutCycl(struct motionth *motionb)
{
	switch (motionb->opAut.state)
	{
		case OPA_INIT:
			motionb->opAut.state = OPA_WF_RELEASE_SH;
		break;
		case OPA_WF_RELEASE_SH:
			if (SH_A)
				motionb->opAut.state = OPA_WF_IDLE;
			break;
		case OPA_WF_IDLE:
			refreshSbusAnChs(&motionb->sbus);
			if (SD_A && SH_B)
			{
				motionb->opAut.state = OPA_IN_PROGRESS_1;
			}
			if (SD_B && SH_B)
			{
				motionb->opAut.state = OPA_IN_PROGRESS_2;
			}
			if (SD_C && SH_B)
			{
				motionb->opAut.state = OPA_IN_PROGRESS_3;
			}
			break;
		case OPA_IN_PROGRESS_1:
			if (SH_A || motionb->sbus.sbusHealth.sbusTimeOut) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				motionb->opAut.state = OPA_WF_RELEASE_SH;
			}
		break;
		case OPA_IN_PROGRESS_2:
			if (SH_A || motionb->sbus.sbusHealth.sbusTimeOut) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				motionb->opAut.state = OPA_WF_RELEASE_SH;
			}
		break;
		case OPA_IN_PROGRESS_3:
			if (SH_A || motionb->sbus.sbusHealth.sbusTimeOut) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				motionb->opAut.state = OPA_WF_RELEASE_SH;
			}
		break;

	} //switch
}//funct
