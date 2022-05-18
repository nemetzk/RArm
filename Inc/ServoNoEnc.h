/*
 * ServoNoEnc.h
 *
 *  Created on: 2022. márc. 25.
 *      Author: nemet
 */

#ifndef INC_SERVONOENC_H_
#define INC_SERVONOENC_H_

#define SNE_OPMODE_COPY			0
#define SNE_OPMODE_JOGMODE		1

#include "pwm.h"

typedef struct LSConfth
{
	uint8_t isCWReversed;
	uint8_t isCCWReversed;
	uint8_t LSSignalsrEnabled;
	GPIO_TypeDef* GPIO_port_CW;
	uint16_t GPIO_Pin_CW;
	GPIO_TypeDef* GPIO_port_CCW;
	uint16_t GPIO_Pin_CCW;
}LSConft;


typedef struct snelimitSwitchth
{
	uint8_t CW_signal;
	uint8_t CCW_signal;
	struct LSConfth Conf;
}snelimitSwitcht;


typedef struct servoNeth
{
	struct pwmchth pwmch;
	uint32_t *pwmCh;
	TIM_HandleTypeDef *pwmTim;
	uint8_t number;
	struct sbusChth *inputRcCH;
	struct snelimitSwitchth limitSwitch;
	uint8_t CWcmd;
	uint8_t CCWcmd;
	uint8_t opMode;

	uint8_t sInitState;
	uint8_t servoStatus;
	myTimerType goForTimer;
	myTimerType servoCycleTim; //obsolete
	myTimerType processTime;

}servoNet;


//servoStatus states
#define SNEHALT 						0
#define SNERDY						1
#define SNECW							2
#define SNECCW						3
#define SNEERR						99

void servoStopNE(struct servoNeth *servoNE);
void servoCycle_Callback_NE(void);
void ServoStatusCyclNE(struct servoNeth *servoNE);
void rcCopy(struct servoNeth *hservoNE);
#endif /* INC_SERVONOENC_H_ */
