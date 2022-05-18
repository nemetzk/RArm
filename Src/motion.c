/*
 * motion.c
 *
 *  Created on: Jan 20, 2022
 *      Author: nemet
 */

#include "motion.h"
#include "ServoFB.h"
#include "main.h"

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

void motion_cycle(struct motionth *motionb)
{
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

	  else if (SH_B && SD_C)//SZERVO TESZT
	  	  {
	  		  servoGoForTime_NE(&motionb->servoD, 4000, -1);
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

  }
}

#define TAUT_WF_SERVO_RDY			0
#define TAUT_INIT_SEQUENCE_GO_UP	1
#define TAUT_INIT_SEQUENCE_WF_GO_UP 2
#define TAUT_INIT_SEQUENCE_WF_GO_DN 3
#define TAUT_WF_RELEASE 			4
#define TAUT_WF_CHANNEL				5
#define TAUT_OP_CHANNEL				6
#define TAUT_SC_OP_CHANNEL 			7
#define	TAUT_SG_OP_CHANNEL			8

void motion_cycle_old(struct motionth *motionb)
{
  switch (motionb->taut_state)
  {
  case TAUT_WF_SERVO_RDY:

  	  if ((motionb->servoA.sInitState == S_INIT_DONE) && (motionb->servoA.servoStatus == SRDY) && (motionb->servoB.sInitState== S_INIT_DONE))
  	  {
  		motionb->taut_state = TAUT_WF_RELEASE;
  	  }
  break;

  case TAUT_WF_RELEASE:
	  if (SC_A && SD_A && SG_B)
	  {
		  motionb->taut_state = TAUT_WF_CHANNEL;
	  }
  break;
  case TAUT_WF_CHANNEL:

	  if (SD_B)
	  {
		  servoGoForPulse(&motionb->servoA,90);
		  if (motionb->servoA.servoStatus!=SRDY)
			  motionb->taut_state = TAUT_OP_CHANNEL;
	  }

	  else if (SD_C)
	  {

		 // servoGoForTime(servo_A_ptr,6000);
		  servoGoForPulse(&motionb->servoA,-90);
		  if (motionb->servoA.servoStatus!=SRDY)
			  motionb->taut_state = TAUT_OP_CHANNEL;

	  }
	  // *** RC switch***
	  else if (SC_B)
	  		  {
	  			  servoGoForPulse(&motionb->servoB,90);
	  			  if (motionb->servoB.servoStatus!=SRDY)
	  				motionb->taut_state = TAUT_SC_OP_CHANNEL;
	  		  }

	  else if (SC_C)
	  		  {
	   			  servoGoForPulse(&motionb->servoB,-90);
	   			  if (motionb->servoB.servoStatus!=SRDY)
	   				motionb->taut_state = TAUT_SC_OP_CHANNEL;
	  	  	  }

	  else if (SG_A)
	  		  {
		  	  	  	  motionb->servoD.CCWcmd=0;
		  	  	  	  motionb->servoD.CWcmd=1;
	  		   		  if (motionb->servoD.servoStatus!=SNERDY)
	  		   			motionb->taut_state = TAUT_SG_OP_CHANNEL;
	  		  }

	  else if (SG_C)
	  		   {
		  	  	  	  motionb->servoD.CWcmd=0;
		  	  	  	  motionb->servoD.CCWcmd=1;
	  		  		  if (motionb->servoD.servoStatus!=SNERDY)
	  		  			motionb->taut_state = TAUT_SG_OP_CHANNEL;
	  		   }

	  else
		  rcCopy(&motionb->servoC);

  break;
  case TAUT_OP_CHANNEL:
	  rcCopy(&motionb->servoC);
	  if (SD_A || (motionb->servoA.servoStatus==SRDY) || ((motionb->servoA.servoStatus==SHALT)))
	  {
		  servoStop(&motionb->servoA);
		  motionb->taut_state = TAUT_WF_RELEASE;
	  }
  break;

  case TAUT_SC_OP_CHANNEL:
	  rcCopy(&motionb->servoC);
	  if (SC_A || (motionb->servoB.servoStatus==SRDY) || ((motionb->servoB.servoStatus==SHALT)))
	  {
		  servoStop(&motionb->servoB);
		  motionb->taut_state = TAUT_WF_RELEASE;
	  }
  break;

  case TAUT_SG_OP_CHANNEL:
	  if (SG_B || (motionb->servoD.servoStatus==SNERDY) || (motionb->servoD.servoStatus==SNEHALT))
	  {
		  motionb->servoD.CWcmd=0;
		  motionb->servoD.CCWcmd=0;
		  motionb->taut_state = TAUT_WF_RELEASE;
	  }
  break;
}

}

motionInit(motiont *motionBlock)
{
	 SBUS_init(&motionBlock->sbus);

	 servoFB_init(&(motionBlock->servoA));
	 servoFB_init(&(motionBlock->servoB));
	 servoNE_init(&(motionBlock->servoC));
	 servoNE_init(&(motionBlock->servoD));

	//HAL_TIM_Encoder_Start_IT(motionBlock->encTim2, TIM_CHANNEL_ALL);
	motionTimer_1.set_value=10;
	motionTimer_1.Callback=NULL;
	initTimer(&motionTimer_1);


}

motion_Cycl(motiont *motionBlock)
{

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


