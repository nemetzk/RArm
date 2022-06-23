/*
 * opmodeSemi.c
 *
 *  Created on: 2022. jún. 17.
 *      Author: nemet
 */

#include "stm32f4xx_hal.h"
#include "motion.h"
#include "opmodeSemi.h"



uint8_t ops_state = 0;

void opSemiCycl(struct motionth *motionb)
{
	switch (motionb->opSemi.state)
	{
		case OPS_INIT:
			motionb->opSemi.state = OPS_WF_RELEASE_SH;
		break;
		case OPS_WF_RELEASE_SH:
			if (SH_A)
				motionb->opSemi.state = OPS_WF_IDLE;
			break;
		case OPS_WF_IDLE:
			refreshSbusAnChs(&motionb->sbus);
			if (SD_A && SH_B)
			{
				motionb->opSemi.state = OPS_IN_PROGRESS_1;
			}
			if (SD_B && SH_B)
			{
				motionb->opSemi.state = OPS_IN_PROGRESS_2;
			}
			if (SD_C && SH_B)
			{
				motionb->opSemi.state = OPS_IN_PROGRESS_3;
			}
			break;
		case OPS_IN_PROGRESS_1:
			servoGoPid(&motionb->servoA, motionb->sbus.sbusCh[LS].scaledVal.value);
			cpyAxis(&motionb->servoB, &motionb->servoA, -1);
			if (SH_A || motionb->sbus.sbusHealth.sbusTimeOut) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				//motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 0;
				servoStop(&motionb->servoA);
				servoStop(&motionb->servoB);
				motionb->opSemi.state = OPS_WF_RELEASE_SH;
			}

		break;
		case OPS_IN_PROGRESS_2:

		break;

		case OPS_IN_PROGRESS_3:
			if (SH_A || motionb->sbus.sbusHealth.sbusTimeOut) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				motionb->opSemi.state = OPS_WF_RELEASE_SH;
			}
		break;

	} //switch
}//funct
