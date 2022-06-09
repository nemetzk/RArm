/*
 * motion.c
 *
 *  Created on: Jan 20, 2022
 *      Author: nemet
 */

#include "motion.h"
#include "ServoFB.h"
#include "main.h"
#include "pid.h"

#define MT_RESET		0
#define MT_WF_START		1
#define MT_WF_TIMER		2


myTimerType motionTimer_1;

#define MC_WF_SERVO_RDY		0
#define MC_WF_RELEASE		1
#define MC_WF_START_SIGNAL	2
#define MC_WF_BACKWARD		3
#define MC_WF_MOTION_A		4
#define MC_WF_MOTION_B		5
#define MC_WF_FORWARD		6
#define MC_WF_TESZT 		7
#define MC_WF_TESZT_DIRECT_SPD	8
#define	MC_PID_CONTROL			9

#define TAUT_WF_SERVO_RDY			0
#define TAUT_INIT_SEQUENCE_GO_UP	1
#define TAUT_INIT_SEQUENCE_WF_GO_UP 2
#define TAUT_INIT_SEQUENCE_WF_GO_DN 3
#define TAUT_WF_RELEASE 			4
#define TAUT_WF_CHANNEL				5
#define TAUT_OP_CHANNEL				6
#define TAUT_SC_OP_CHANNEL 			7
#define	TAUT_SG_OP_CHANNEL			8


void motion_cycle(struct motionth *motionb)
{
	refreshSbusDigitChs(&motionb->sbus);
  switch (motionb->taut_state)
  {
  case MC_WF_SERVO_RDY:
  	  if ((motionb->servoA.sInitState == S_INIT_DONE) && (motionb->servoA.servoStatus == SRDY))
  		if ((motionb->servoB.sInitState == S_INIT_DONE) && (motionb->servoB.servoStatus == SRDY))
  	  {
  		motionb->taut_state = MC_WF_RELEASE;
  	  }
  break;

  case MC_WF_RELEASE:
	  if (SH_A)
	  {
		  motionb->taut_state = MC_WF_START_SIGNAL;
	  }
  break;

  case MC_WF_START_SIGNAL:
	  refreshSbusAnChs(&motionb->sbus);
	  rcCopy(&(motionb->servoC));

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
	  /*
	  else if (SH_B && SD_C && SC_B)//SZERVO TESZT
	  {
		  	  servoGoForPulse(&motionb->servoB, -1024);
		  	  motionb->taut_state = MC_WF_TESZT;
	  }*/
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
/*
	  motionb->servoB.myPID.settings.Kp = motionb->sbus.sbusCh[LS].scaledVal.value;
	  motionb->servoB.myPID.settings.Ki = motionb->sbus.sbusCh[RS].scaledVal.value;
	  motionb->servoB.myPID.settings.Kd = motionb->sbus.sbusCh[S2].scaledVal.value;
*/
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

  } //switch
  setTimer(&motionTimer_1);
}

pidInpuVarInit(motiont *motionb)
{
	  motionb->sbus.sbusCh[LS].rawVal.min = 165;
	  motionb->sbus.sbusCh[LS].rawVal.max = 1799;
	  motionb->sbus.sbusCh[LS].scaledVal.min = 0;
	  motionb->sbus.sbusCh[LS].scaledVal.max = PROP_LS_MAX;
	  motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 1;
	  refreshSbusCh(&(motionb->sbus.sbusCh[LS]));

	  motionb->sbus.sbusCh[RS].rawVal.min = 141;
	  motionb->sbus.sbusCh[RS].rawVal.max = 1738;
	  motionb->sbus.sbusCh[RS].scaledVal.min = 0;
	  motionb->sbus.sbusCh[RS].scaledVal.max = INT_RS_MAX;
	  motionb->sbus.sbusCh[RS].scaledVal.calculationEnabled = 1;
	  refreshSbusCh(&(motionb->sbus.sbusCh[RS]));

	  motionb->sbus.sbusCh[S2].rawVal.min = 37;
	  motionb->sbus.sbusCh[S2].rawVal.max = 1807;
	  motionb->sbus.sbusCh[S2].scaledVal.min = 0;
	  motionb->sbus.sbusCh[S2].scaledVal.max = DERIV_S2_MAX;
	  motionb->sbus.sbusCh[S2].scaledVal.calculationEnabled = 1;
	  refreshSbusCh(&(motionb->sbus.sbusCh[S2]));
	  /* **** SERVO A **** */
	  motionb->servoA.myPID.Inputs.SetPoint.RawInput.value = 57;
	  motionb->servoA.myPID.Inputs.SetPoint.RawInput.max = 1808;
	  motionb->servoA.myPID.Inputs.SetPoint.RawInput.min = -1808;
	  motionb->servoA.myPID.Inputs.SetPoint.ScaledOutput.max = 3616;
	  motionb->servoA.myPID.Inputs.SetPoint.ScaledOutput.min = 0;

	  motionb->servoA.myPID.Inputs.ProcessVariable.RawInput.max = 65534;
	  motionb->servoA.myPID.Inputs.ProcessVariable.RawInput.min = 0;
	  motionb->servoA.myPID.Inputs.ProcessVariable.ScaledOutput.max = 3616;
	  motionb->servoA.myPID.Inputs.ProcessVariable.ScaledOutput.min = 0;

	  motionb->servoA.myPID.opVariables.ControlVariable.RawInput.max = CONTROL_VAR_MAX;
	  motionb->servoA.myPID.opVariables.ControlVariable.RawInput.min = CONTROL_VAR_MIN;
	  motionb->servoA.myPID.opVariables.ControlVariable.ScaledOutput.max = 1000;
	  motionb->servoA.myPID.opVariables.ControlVariable.ScaledOutput.min = 500;

	  /* **** SERVO B **** */

	  motionb->servoB.myPID.Inputs.SetPoint.RawInput.value = 57;
	  motionb->servoB.myPID.Inputs.SetPoint.RawInput.max = 1808;
	  motionb->servoB.myPID.Inputs.SetPoint.RawInput.min = -1808;
	  motionb->servoB.myPID.Inputs.SetPoint.ScaledOutput.max = 3616;
	  motionb->servoB.myPID.Inputs.SetPoint.ScaledOutput.min = 0;

	  motionb->servoB.myPID.Inputs.ProcessVariable.RawInput.max = 65534;
	  motionb->servoB.myPID.Inputs.ProcessVariable.RawInput.min = 0;
	  motionb->servoB.myPID.Inputs.ProcessVariable.ScaledOutput.max = 3616;
	  motionb->servoB.myPID.Inputs.ProcessVariable.ScaledOutput.min = 0;

	  motionb->servoB.myPID.opVariables.ControlVariable.RawInput.max = CONTROL_VAR_MAX;
	  motionb->servoB.myPID.opVariables.ControlVariable.RawInput.min = CONTROL_VAR_MIN;
	  motionb->servoB.myPID.opVariables.ControlVariable.ScaledOutput.max = 1000;
	  motionb->servoB.myPID.opVariables.ControlVariable.ScaledOutput.min = 500;

}

motionInit(motiont *motionBlock)
{
	 SBUS_init(&motionBlock->sbus);



	 servoFB_init(&(motionBlock->servoA));
	 servoFB_init(&(motionBlock->servoB));
	 servoNE_init(&(motionBlock->servoC));
	 servoNE_init(&(motionBlock->servoD));

	 pidInpuVarInit(motionBlock);
	 pidInitStartup(&(motionBlock->servoA.myPID));
	 pidInitStartup(&(motionBlock->servoB.myPID));

	//HAL_TIM_Encoder_Start_IT(motionBlock->encTim2, TIM_CHANNEL_ALL);
	motionTimer_1.set_value = PID_CYCLE_INTERVAL;
	motionTimer_1.ownerPtr = motionBlock;
	motionTimer_1.Callback=	  motion_cycle;
	initTimer(&motionTimer_1);
	setTimer(&motionTimer_1);
}


uint8_t mt_aut_state;
/*
myTimerType motionTestTimer;

motionTestStop(struct servoFBth *servoFB)
{
	servoStop(servoFB);
}

motionTest(motiont *motionBlock)
{
	motionTestTimer.set_value = 4000;
	motionTestTimer.Callback = motionTestStop;
	motionTestTimer.ownerPtr = &motionBlock->servoA;
	setTimer(&motionTestTimer);
	initTimer(&motionTestTimer);

	//uint8_t mt_start_signal =HAL_GPIO_ReadPin(VA_HATUL_GPIO_Port, VA_HATUL_Pin);
	servoStartCW(&(motionBlock->servoA));
	//setTimer(&motionTestStop);

}
*/


