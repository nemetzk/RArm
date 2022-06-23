/*
 * opmodeManual.c
 *
 *  Created on: 2022. jún. 17.
 *      Author: nemet
 */
#include "stm32f4xx_hal.h"
#include "motion.h"

#include "opmodeManual.h"

void opManualCycl(struct motionth *motionb)
{
	switch (motionb->opManu.state)
	{
		case OPM_INIT:
			motionb->opManu.state = OPM_WF_RELEASE_SH;
		break;
		case OPM_WF_RELEASE_SH:
			if (SH_A)
				motionb->opManu.state = OPM_WF_IDLE;
			break;
		case OPM_WF_IDLE:
			refreshSbusAnChs(&motionb->sbus);
			if (SD_A && SH_B)
			{
				motionb->opManu.state = OPM_IN_PROGRESS_1;
			}
			if (SD_B && SH_B)
			{
				motionb->opManu.state = OPM_IN_PROGRESS_2;
			}
			if (SD_C && SH_B)
			{
				motionb->opManu.state = OPM_IN_PROGRESS_3;
			}
			break;
		case OPM_IN_PROGRESS_1:
			servoGoPid(&motionb->servoA, motionb->sbus.sbusCh[LS].scaledVal.value);
			if (SH_A || motionb->sbus.sbusHealth.sbusTimeOut) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				//motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 0;
				motionb->opManu.state = OPM_WF_RELEASE_SH;
				servoStop(&motionb->servoA);
			}
		break;
		case OPM_IN_PROGRESS_2:
			servoGoPid(&motionb->servoB, motionb->sbus.sbusCh[LS].scaledVal.value);
			if (SH_A || motionb->sbus.sbusHealth.sbusTimeOut) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				//motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 0;
				motionb->opManu.state = OPM_WF_RELEASE_SH;
				servoStop(&motionb->servoA);
			}

		break;
		case OPM_IN_PROGRESS_3:
			if (SH_A || motionb->sbus.sbusHealth.sbusTimeOut) //JÖHET IDE VALAMI MOTION TIMOUT FÉLE IS
			{
				motionb->opManu.state = OPM_WF_RELEASE_SH;
			}
		break;

	} //switch
}//funct


/*
 *
 *
	  if (SH_B && SC_A && SD_A)//NINCS HÁTRAMENETEL CSAK ÁTALAKULÁS
	  {
  		servoGoForPulse(&motionb->servoA,90);
	  }
	  else if (SH_B && SC_C && SD_A) //NINCS HÁTRAMENETEL CSAK ÁTALAKULÁS
	  {
  		servoGoForPulse(&motionb->servoA,-90);
	  }
	  else if (SH_B && SD_B)//HÁTRAMENETELLEL
	  {
		  servoGoForTime_NE(&motionb->servoD, 4000, -1);
	  }
	  else if ((motionb->servoA.servoStatus!=SRDY) && SD_A)
	  {
			motionb->taut_state = MC_WF_MOTION_A;

	  }
	  else if (SH_B && SD_C && SC_A)//SZERVO TESZT
	  {
		  	  servoGoForPulse(&motionb->servoB, 1024);
		  	  motionb->taut_state = MC_WF_TESZT;
	  }

	  else if (SH_B && SD_C && SC_C)//SZERVO TESZT
	  {
		  motionb->sbus.sbusCh[LS].scaledVal.min = 0;
		  motionb->sbus.sbusCh[LS].scaledVal.max = 3000;
		  motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 1;
		  	  set_MOTOR_spd(&motionb->servoB.pwmch, LS_VAL);
		  	  motionb->taut_state = MC_WF_TESZT_DIRECT_SPD;
	  }
	  else if (SH_B && SD_C && SC_B)//PID TESZT
	  {
		  motionb->servoA.myPID.settings.Kp = motionb->sbus.sbusCh[LS].scaledVal.value;
		  motionb->servoA.myPID.settings.Ki = motionb->sbus.sbusCh[RS].scaledVal.value;
		  motionb->servoA.myPID.settings.Kd = motionb->sbus.sbusCh[S2].scaledVal.value;

		  motionb->servoB.myPID.settings.Kp = motionb->sbus.sbusCh[LS].scaledVal.value;
		  motionb->servoB.myPID.settings.Ki = motionb->sbus.sbusCh[RS].scaledVal.value;
		  motionb->servoB.myPID.settings.Kd = motionb->sbus.sbusCh[S2].scaledVal.value;

		  pidInit(&(motionb->servoA.myPID));
		  pidInit(&(motionb->servoB.myPID));

		  motionb->taut_state = MC_PID_CONTROL;
	  }
	  else if ((motionb->servoD.servoStatus!=SNERDY) && SD_B)
			motionb->taut_state = MC_WF_BACKWARD;
  break;

  case MC_WF_BACKWARD:
	  if (motionb->servoD.servoStatus == SNERDY)
	  {
		  if (SC_A)
		  {
			  servoGoForPulse(&motionb->servoA,90);
		  }
		  else if(SC_C)
		  {
			  servoGoForPulse(&motionb->servoA,-90);
		  }

		  if ((motionb->servoA.servoStatus!=SRDY))
		 	  {
		 			motionb->taut_state = MC_WF_MOTION_A;

		 	  }

	  }
  break;

  case MC_WF_MOTION_A:
	  if ((motionb->servoA.servoStatus==SRDY) || ((motionb->servoA.servoStatus==SHALT)))
	  {
		  if (SC_A)
			  servoGoForPulse(&motionb->servoB,90);
		  if (SC_C)
		  			  servoGoForPulse(&motionb->servoB,-90);
	  }
	  if (motionb->servoB.servoStatus!=SRDY)
				motionb->taut_state = MC_WF_MOTION_B;
  break;

  case MC_WF_MOTION_B:
	  if ((motionb->servoB.servoStatus==SRDY) || ((motionb->servoB.servoStatus==SHALT)))
	  {

		  if(SD_A)
			  motionb->taut_state = MC_WF_RELEASE;
		  else if(SD_B)
			  servoGoForTime_NE(&motionb->servoD, 4000, 1);

		  if ((motionb->servoD.servoStatus!=SNERDY) && SD_B)
			  motionb->taut_state = MC_WF_FORWARD;
	  }

  break;

  case MC_WF_FORWARD:
	  if ((motionb->servoD.servoStatus == SNERDY))
	 			  motionb->taut_state = MC_WF_RELEASE;
  break;

  case MC_WF_TESZT:
	  if ((motionb->servoB.servoStatus == SNERDY))
	  	 			  motionb->taut_state = MC_WF_RELEASE;
  break;

  case MC_WF_TESZT_DIRECT_SPD:

	  if (!SH_B)
	  {
		  motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 0;
		  servoStop(&motionb->servoB);
		  motionb->taut_state = MC_WF_RELEASE;
	  }
	  else
	  {
		  set_MOTOR_spd(&motionb->servoB.pwmch, LS_VAL);
	  }
	  break;
  case MC_PID_CONTROL:
	  motionb->servoA.myPID.Inputs.ProcessVariable.RawInput.value = motionb->servoA.encoder.val;
	  motionb->servoB.myPID.Inputs.ProcessVariable.RawInput.value = motionb->servoB.encoder.val;

	  pidCalc(&(motionb->servoA.myPID));
	  pidCalc(&(motionb->servoB.myPID));

	  if((motionb->servoB.myPID.opVariables.ControlVariable.ScaledOutput.value >= 500) &&
	     (motionb->servoB.myPID.opVariables.ControlVariable.ScaledOutput.value <= 1000))
	  	  {
		  	  set_MOTOR_spd(&motionb->servoA.pwmch, motionb->servoA.myPID.opVariables.ControlVariable.ScaledOutput.value);
		  	  set_MOTOR_spd(&motionb->servoB.pwmch, motionb->servoB.myPID.opVariables.ControlVariable.ScaledOutput.value);
	  	  }

	  if (!SH_B || motionb->sbus.sbusHealth.sbusTimeOut)
	  {
		  //motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 0;
		  //motionb->sbus.sbusCh[RS].scaledVal.calculationEnabled = 0;
		  //motionb->sbus.sbusCh[S2].scaledVal.calculationEnabled = 0;
		  servoStop(&motionb->servoA);
		  servoStop(&motionb->servoB);
		  motionb->taut_state = MC_WF_RELEASE;
	  }

	  break;
 */
