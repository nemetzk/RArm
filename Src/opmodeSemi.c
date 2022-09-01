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
				OPSEMI_STATE = OPS_IN_PROGRESS_A1;
			}
			if (SD_B && SH_B)
			{
				OPSEMI_STATE = OPS_IN_PROGRESS_B1;
			}
			if (SD_C && SH_B)
			{
				OPSEMI_STATE = OPS_IN_PROGRESS_C1;
			}


			if (SG_C && SH_B)
			{
				motionb->servoD.CWcmd = 1;
				OPSEMI_STATE = OPS_MOVE_BACKWARD;
			}

			if (SG_A && SH_B)
			{
				motionb->servoD.CCWcmd = 1;
				OPSEMI_STATE = OPS_MOVE_FORWARD;
			}

			break;

		case OPS_MOVE_BACKWARD:
			if (SH_A || SBUS_ERROR||motionb->servoD.servoStatus == SNEHALT)
			{
				motionb->servoD.CWcmd = 0;
				OPSEMI_STATE = OPS_WF_RELEASE_SH;
			}
			break;

		case OPS_MOVE_FORWARD:
			if (SH_A || SBUS_ERROR||motionb->servoD.servoStatus == SNEHALT)
			{
				motionb->servoD.CCWcmd = 0;
				OPSEMI_STATE = OPS_WF_RELEASE_SH;
			}
			break;

		case OPS_IN_PROGRESS_A1:

			moveServoA(motionb, LS_VAL);
			if (motionb->servoA.encoder.isMoving)
			{
				OPSEMI_STATE = OPS_IN_PROGRESS_A2;
			}

			if (SH_A || SBUS_ERROR) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				//motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 0;
				servoStop(SERVOA);
				servoStop(SERVOB);
				servoStop(SERVOC);
				OPSEMI_STATE = OPS_WF_RELEASE_SH;
			}
		break;
		case OPS_IN_PROGRESS_A2:
			moveServoA(motionb, LS_VAL);
			if (SH_A || SBUS_ERROR || !(motionb->servoA.encoder.isMoving)) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				//motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 0;
				servoStop(SERVOA);
				servoStop(SERVOB);
				servoStop(SERVOC);
				OPSEMI_STATE = OPS_WF_RELEASE_SH;
			}

		break;

		case OPS_IN_PROGRESS_B1:
			moveServoB(motionb, RS_VAL_INV);

			if (motionb->servoB.encoder.isMoving)
			{
				OPSEMI_STATE = OPS_IN_PROGRESS_B2;
			}

			if (SH_A || SBUS_ERROR) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				//motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 0;
				servoStop(SERVOA);
				servoStop(SERVOB);
				servoStop(SERVOC);
				OPSEMI_STATE = OPS_WF_RELEASE_SH;
			}
		break;

		case OPS_IN_PROGRESS_B2:
			//moveServoA(motionb, LS_VAL);
			moveServoB(motionb, RS_VAL_INV);

			if (SH_A || SBUS_ERROR || !(motionb->servoB.encoder.isMoving)) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				//motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 0;
				servoStop(SERVOA);
				servoStop(SERVOB);
				servoStop(SERVOC);
				OPSEMI_STATE = OPS_WF_RELEASE_SH;
			}
		break;

		case OPS_IN_PROGRESS_C1:
			moveServoC(motionb, S2_VAL);

			if (motionb->servoC.encoder.isMoving)
			{
				OPSEMI_STATE = OPS_IN_PROGRESS_C2;
			}

			if (SH_A || SBUS_ERROR) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				//motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 0;
				servoStop(SERVOA);
				servoStop(SERVOB);
				servoStop(SERVOC);
				OPSEMI_STATE = OPS_WF_RELEASE_SH;
			}
		break;

		case OPS_IN_PROGRESS_C2:
			//moveServoA(motionb, LS_VAL);
			//moveServoB(motionb, RS_VAL_INV);
			moveServoC(motionb, S2_VAL);

			if (SH_A || SBUS_ERROR || !(motionb->servoC.encoder.isMoving)) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
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
