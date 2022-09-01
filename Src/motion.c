/*
 * motion.c
 *
 *  Created on: Jan 20, 2022
 *      Author: nemet
 */

#include "motion.h"
#include "main.h"
#include "pid.h"
#include "ServoMFP.h"
#include "servo_definitions.h"
#include "opmodeManual.h"
#include "opmodeSemi.h"
#include "opmodeAut.h"

#define MT_RESET		0
#define MT_WF_START		1
#define MT_WF_TIMER		2


myTimerType motionTimer_1;
myTimerType stat_led_timer_1;

#define MC_WF_SERVOA_RDY	0
#define MC_WF_SERVOB_RDY	1
#define MC_WF_SERVOC_RDY	2

#define MC_WF_SUBMODULES	11
#define MC_IDLE				12
#define MC_OP_MODE_MANUAL	13
#define MC_OP_MODE_SEMI		14
#define MC_OP_MODE_AUT		15


#define TAUT_WF_SERVO_RDY			0
#define TAUT_INIT_SEQUENCE_GO_UP	1
#define TAUT_INIT_SEQUENCE_WF_GO_UP 2
#define TAUT_INIT_SEQUENCE_WF_GO_DN 3
#define TAUT_WF_RELEASE 			4
#define TAUT_WF_CHANNEL				5
#define TAUT_OP_CHANNEL				6
#define TAUT_SC_OP_CHANNEL 			7
#define	TAUT_SG_OP_CHANNEL			8


void led_cycle(struct motionth *motionb)
{
// ************************************
// ********* servo error **************
// ************************************

 switch (motionb->error_state)
 {
 case ERR_NOERR:
	 HAL_GPIO_WritePin(LED_STAT_0_GPIO_Port, LED_STAT_0_Pin, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(LED_STAT_1_GPIO_Port, LED_STAT_1_Pin, GPIO_PIN_RESET);
	 break;
 case ERR_SERVO_A_INIT:
	 HAL_GPIO_TogglePin(LED_STAT_1_GPIO_Port, LED_STAT_1_Pin);
	 break;
 case ERR_SERVO_B_INIT:
	 HAL_GPIO_TogglePin(LED_STAT_1_GPIO_Port, LED_STAT_1_Pin);
 	 break;
 case ERR_SERVO_C_INIT:
	 HAL_GPIO_TogglePin(LED_STAT_1_GPIO_Port, LED_STAT_1_Pin);
 	 break;
 } //switch

 // ************************************
 // ********** SBUS error **************
 // ************************************

 if (motionb->sbus.sbusHealth.sbusTimeOut)
	 HAL_GPIO_WritePin(LED_STAT_1_GPIO_Port, LED_STAT_1_Pin, GPIO_PIN_SET);


 setTimer(&motionb->ledCyclTim);
}

void motion_cycle(struct motionth *motionb)
{
  refreshSbusDigitChs(&motionb->sbus);

  switch (motionb->taut_state)
  {
  case MC_WF_SERVOA_RDY:
	  // ******************************
	  // *      SERVO A INIT SUCCEEDED
	  // *****************************/
  	  if (motionb->servoA.sInitState == S_INIT_DONE)
  	  {
  		setTimer(&motionb->initTim);
  		motionb->taut_state = MC_WF_SERVOB_RDY;
  	  }
	  // *********************************
	  //      SERVO A INIT FAILED
  	  // *********************************

  	  if (motionb->initTim.Elapsed)
  	  {
  			motionb->error_state = ERR_SERVO_A_INIT;
  	  		motionb->taut_state = MC_WF_SERVOB_RDY;
  	  		setTimer(&motionb->initTim);
	  }


  break;

  case MC_WF_SERVOB_RDY:
	  // ******************************
	  // *      SERVO B INIT SUCCEEDED
	  // *****************************/
  	  if (motionb->servoB.sInitState == S_INIT_DONE)
  	  {
  		setTimer(&motionb->initTim);
  		motionb->taut_state = MC_WF_SERVOC_RDY;
  	  }
	  // *********************************
	  //      SERVO B INIT FAILED
  	  // *********************************

  	  if (motionb->initTim.Elapsed)
  	  {
  			motionb->error_state = ERR_SERVO_B_INIT;
  	  		motionb->taut_state = MC_WF_SERVOC_RDY;
  	  		setTimer(&motionb->initTim);
	  }
  break;

  case MC_WF_SERVOC_RDY:
	  // ******************************
	  // *      SERVO C INIT SUCCEEDED
	  // *****************************/
  	  if (motionb->servoC.sInitState == S_INIT_DONE)
  	  {
  		setTimer(&motionb->initTim);
  		motionb->taut_state = MC_WF_SUBMODULES;
  	  }
	  // *********************************
	  //      SERVO C INIT FAILED
  	  // *********************************

  	  if (motionb->initTim.Elapsed)
  	  {
  			motionb->error_state = ERR_SERVO_C_INIT;
  	  		motionb->taut_state = MC_WF_SUBMODULES;
  	  		setTimer(&motionb->initTim);
	  }
  break;

  case MC_WF_SUBMODULES:
	  opAutCycl(motionb);
	  opManualCycl(motionb);
	  opSemiCycl(motionb);
	  if ((motionb->opManu.state == OPM_WF_IDLE &&
		  motionb->opSemi.state == OPM_WF_IDLE &&
		  motionb->opAut.state  == OPM_WF_IDLE) ||
	      motionb->sbus.sbusHealth.sbusTimeOut  )
	  {
		  motionb->taut_state = MC_IDLE;
	  }
  break;

  case MC_IDLE:
	  if (SC_A)
		  motionb->taut_state = MC_OP_MODE_MANUAL;
	  else if (SC_B)
		  motionb->taut_state = MC_OP_MODE_SEMI;
	  else if (SC_C)
	  	  motionb->taut_state = MC_OP_MODE_AUT;

	  //refreshSbusAnChs(&motionb->sbus);
	  break;

  case MC_OP_MODE_MANUAL:
	  opManualCycl(motionb);
	  if (!SC_A || motionb->sbus.sbusHealth.sbusTimeOut)
		  motionb->taut_state = MC_WF_SUBMODULES;
	  break;

  case MC_OP_MODE_SEMI:
	  opSemiCycl(motionb);
	  if (!SC_B || motionb->sbus.sbusHealth.sbusTimeOut)
		  motionb->taut_state = MC_WF_SUBMODULES;
	  break;

  case MC_OP_MODE_AUT:
	  opAutCycl(motionb);
	  if (!SC_C || motionb->sbus.sbusHealth.sbusTimeOut)
		  motionb->taut_state = MC_WF_SUBMODULES;
	  break;



  } //switch

  setTimer(&motionTimer_1);
}

void pidInpuVarInit(motiont *motionb)
{
	  motionb->sbus.sbusCh[LS].rawVal.min = 165;
	  motionb->sbus.sbusCh[LS].rawVal.max = 1799;
	  motionb->sbus.sbusCh[LS].scaledVal.min = 165;
	  motionb->sbus.sbusCh[LS].scaledVal.max = 1799;//PROP_LS_MAX;
	  motionb->sbus.sbusCh[LS].scaledVal.calculationEnabled = 1;
	  refreshSbusCh(&(motionb->sbus.sbusCh[LS]));

	  motionb->sbus.sbusCh[RS].rawVal.min = 141;
	  motionb->sbus.sbusCh[RS].rawVal.max = 1739;
	  motionb->sbus.sbusCh[RS].scaledVal.min = 165;
	  motionb->sbus.sbusCh[RS].scaledVal.max = 1799;
	  motionb->sbus.sbusCh[RS].scaledVal.calculationEnabled = 1;
	  refreshSbusCh(&(motionb->sbus.sbusCh[RS]));

	  motionb->sbus.sbusCh[S2].rawVal.min = 37;
	  motionb->sbus.sbusCh[S2].rawVal.max = 1807;
	  motionb->sbus.sbusCh[S2].scaledVal.min = 165;
	  motionb->sbus.sbusCh[S2].scaledVal.max = 1799;
	  motionb->sbus.sbusCh[S2].scaledVal.calculationEnabled = 1;
	  refreshSbusCh(&(motionb->sbus.sbusCh[S2]));

	  /* **** SERVO A **** */
	  motionb->servoA.myPID.Inputs.SetPoint.RawInput.value = 57;
	  motionb->servoA.myPID.Inputs.SetPoint.RawInput.max = 1799;
	  motionb->servoA.myPID.Inputs.SetPoint.RawInput.min = 165;
	  motionb->servoA.myPID.Inputs.SetPoint.ScaledOutput.max = 1900; //mm-re skálázva
	  motionb->servoA.myPID.Inputs.SetPoint.ScaledOutput.min = 1700;

	  motionb->servoA.myPID.Inputs.ProcessVariable.RawInput.max = 65534;
	  motionb->servoA.myPID.Inputs.ProcessVariable.RawInput.min = 0;
	  motionb->servoA.myPID.Inputs.ProcessVariable.ScaledOutput.max = 3616; //mm-re skálázva
	  motionb->servoA.myPID.Inputs.ProcessVariable.ScaledOutput.min = 0;

	  motionb->servoA.myPID.opVariables.ControlVariable.RawInput.max = CONTROL_VAR_MAX;
	  motionb->servoA.myPID.opVariables.ControlVariable.RawInput.min = CONTROL_VAR_MIN;
	  motionb->servoA.myPID.opVariables.ControlVariable.ScaledOutput.max = 900;
	  motionb->servoA.myPID.opVariables.ControlVariable.ScaledOutput.min = 600;

	  /* **** SERVO B **** */

	  motionb->servoB.myPID.Inputs.SetPoint.RawInput.value = 1;
	  motionb->servoB.myPID.Inputs.SetPoint.RawInput.max = 1799;
	  motionb->servoB.myPID.Inputs.SetPoint.RawInput.min = 165;
	  motionb->servoB.myPID.Inputs.SetPoint.ScaledOutput.max = 1900; //mm-re skálázva
	  motionb->servoB.myPID.Inputs.SetPoint.ScaledOutput.min = 1700;

	  motionb->servoB.myPID.Inputs.ProcessVariable.RawInput.max = 65534;
	  motionb->servoB.myPID.Inputs.ProcessVariable.RawInput.min = 0;
	  motionb->servoB.myPID.Inputs.ProcessVariable.ScaledOutput.max = 3616; //mm-re skálázva
	  motionb->servoB.myPID.Inputs.ProcessVariable.ScaledOutput.min = 0;

	  motionb->servoB.myPID.opVariables.ControlVariable.RawInput.max = CONTROL_VAR_MAX;
	  motionb->servoB.myPID.opVariables.ControlVariable.RawInput.min = CONTROL_VAR_MIN;
	  motionb->servoB.myPID.opVariables.ControlVariable.ScaledOutput.max = 900;
	  motionb->servoB.myPID.opVariables.ControlVariable.ScaledOutput.min = 600;

	  /* **** SERVO C **** */

	  motionb->servoC.myPID.Inputs.SetPoint.RawInput.value = 1;
	  motionb->servoC.myPID.Inputs.SetPoint.RawInput.max = 1799;
	  motionb->servoC.myPID.Inputs.SetPoint.RawInput.min = 165;
	  motionb->servoC.myPID.Inputs.SetPoint.ScaledOutput.max = 1900; //mm-re skálázva
	  motionb->servoC.myPID.Inputs.SetPoint.ScaledOutput.min = 1700;

	  motionb->servoC.myPID.Inputs.ProcessVariable.RawInput.max = 65534;
	  motionb->servoC.myPID.Inputs.ProcessVariable.RawInput.min = 0;
	  motionb->servoC.myPID.Inputs.ProcessVariable.ScaledOutput.max = 3616; //mm-re skálázva
	  motionb->servoC.myPID.Inputs.ProcessVariable.ScaledOutput.min = 0;

	  motionb->servoC.myPID.opVariables.ControlVariable.RawInput.max = CONTROL_VAR_MAX;
	  motionb->servoC.myPID.opVariables.ControlVariable.RawInput.min = CONTROL_VAR_MIN;
	  motionb->servoC.myPID.opVariables.ControlVariable.ScaledOutput.max = 900;
	  motionb->servoC.myPID.opVariables.ControlVariable.ScaledOutput.min = 600;


}

uint8_t MSA_state;
uint8_t MSB_state;
uint8_t MSC_state;

#define MSA_INIT	0
#define MSA_PROCESS	1

int8_t moveServoA(motiont *motionb,int32_t position)
{
	servoGoPid(SERVOA,position,1);
	cpyAxis(SERVOB, SERVOA, -1);
	cpyAxis(SERVOC, SERVOA, 1);

	return !SERVO_A_HAS_SPD;

}

int8_t moveServoB(motiont *motionb,int32_t position)
{
	servoGoPid(SERVOB,position,1);
	cpyAxis(SERVOC, SERVOB, -1);
	return !SERVO_B_HAS_SPD;
}

int8_t moveServoC(motiont *motionb,int32_t position)
{
	servoGoPid(SERVOC,position,1);
	return !SERVO_C_HAS_SPD;
}

motionInit(motiont *motionBlock)
{
	 SBUS_init(&motionBlock->sbus);
	 servoInit(&(motionBlock->servoA));
	 servoInit(&(motionBlock->servoB));
	 servoInit(&(motionBlock->servoC));
	 servoNE_init(&(motionBlock->servoD));

	 pidInpuVarInit(motionBlock);
	 pidInitStartup(&(motionBlock->servoA.myPID));
	 pidInitStartup(&(motionBlock->servoB.myPID));
	 pidInitStartup(&(motionBlock->servoC.myPID));

	//HAL_TIM_Encoder_Start_IT(motionBlock->encTim2, TIM_CHANNEL_ALL);
	motionTimer_1.set_value = PID_CYCLE_INTERVAL;
	motionTimer_1.ownerPtr = motionBlock;
	motionTimer_1.Callback=	  motion_cycle;
	initTimer(&motionTimer_1);
	setTimer(&motionTimer_1);

	stat_led_timer_1.set_value = STAT_LED_1_BLINK_NOENC_INTERVAL;
	stat_led_timer_1.ownerPtr = motionBlock;
	stat_led_timer_1.Callback = NULL;
	initTimer(&stat_led_timer_1);
	setTimer(&stat_led_timer_1);

	motionBlock->initTim.set_value = ALLOWABLE_INIT_TIME;
	motionBlock->initTim.ownerPtr = motionBlock;
	motionBlock->initTim.Callback = NULL;
	initTimer(&motionBlock->initTim);
	setTimer(&motionBlock->initTim);

	motionBlock->ledCyclTim.set_value = LED_CYCL_TIME_FREQUENCY;
	motionBlock->ledCyclTim.ownerPtr = motionBlock;
	motionBlock->ledCyclTim.Callback = led_cycle;
	initTimer(&motionBlock->ledCyclTim);
	setTimer(&motionBlock->ledCyclTim);
}


uint8_t mt_aut_state;
