/*
 * ServoFB.h
 *
 *  Created on: Jan 20, 2022
 *      Author: nemet
 */

#ifndef INC_SERVOFB_H_
#define INC_SERVOFB_H_

#include "encoder.h"
#include "pwm.h"

#define S_STOP 			1
#define S_MOVE_CW 		2
#define S_MOVE_CCW 		3

#define MR_INIT				0
#define MR_RESET			1
#define MR_WF_START			2
#define MR_WF_ARRIVE_CW 	3
#define MR_WF_ARRIVE_CCW 	4

#define SIGNAL_MOVREL 		1

typedef struct servoFBth
{
	struct Encoderh encoder;
	struct pwmchth pwmch;
	TIM_HandleTypeDef *encTim;
	TIM_HandleTypeDef *pwmTim;
	uint8_t number;
	uint32_t *pwmCh;
	int32_t pulse_dest;
	int32_t pulse_start;
	uint8_t reverse_stop_op;
	uint8_t CWcmd;
	uint8_t CCWcmd;
	uint8_t sInitState;
	uint8_t servoStatus;
	int8_t subProcessState;
	int32_t pos;
	myTimerType goForTimer;
	myTimerType servoCycleTim; //obsolete
	myTimerType processTime;

}servoFBt;


//init sequence
#define S_WF_SERVO_RDY 				0
#define S_INIT_SEQUENCE_GO_UP		1
#define S_INIT_SEQUENCE_WF_GO_UP	2
#define S_INIT_SEQUENCE_WF_GO_DN	3
#define S_INIT_DONE					10
#define S_INIT_FAILED				99

//servoStatus states
#define SHALT 						0
#define SRDY						1
#define SCW							2
#define SCCW						3
#define SERR						99


//,,uint32_t *pwmCh
void servoStop(struct servoFBth *servoFB);

#endif /* INC_SERVOFB_H_ */
