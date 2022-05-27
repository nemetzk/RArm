/*
 * sbus_fnc.h
 *
 *  Created on: Jan 10, 2022
 *      Author: nemet
 */
#pragma once
#include "stm32f4xx_hal.h"
#include "timerFunctions.h"


typedef struct sbusHealthth
{
	uint8_t frameLost;
	uint8_t failsafeActivated;
	int16_t sbusTimeOut;
}sbusHealtht;


typedef struct rawValTH
{
	int16_t value;
	int16_t min;
	int16_t max;
	uint8_t outRangeFlg;
}rawValT;

typedef struct digValTH
{
	uint8_t calculationEnabled;
	uint8_t A;
	uint8_t B;
	uint8_t C;
	uint16_t Aval;
	uint16_t Bval;

}digValT;

typedef struct scaledValTH
{
	uint8_t calculationEnabled;
	uint32_t value;
	int16_t min;
	int16_t max;

}scaledValT;

typedef struct sbusChth
{
	uint8_t num;
	struct rawValTH rawVal;
	struct digValTH digVal;
	struct scaledValTH scaledVal;

}sbusCht;

typedef struct sbusfuncth
{
	uint8_t SBUS_rxBuffer[25];
	uint8_t oneCharBuffer;
	uint16_t elapsedTime;
	uint8_t no_char;
	myTimerType sbusTimer;
}sbusfunct;

typedef struct sbusth
{
	uint8_t number;
	struct UART_HandleTypeDef *sbusUart;
	sbusCht sbusCh[17];
	struct sbusHealthth sbusHealth;
	struct sbusfuncth func;
}sbust;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void parseSbusMessage(struct sbusth *hsbus, uint8_t *sbus_msg_bytes);
void refreshSbusCh(struct sbusChth *sbusCh);

#define SD 7
#define SC 10
#define SG 11
#define SH 6

#define LS 13
#define RS 15
#define S2 9


#define MAX_NO_SBUSES 2
uint8_t no_sbuses;
sbust *sbuses[MAX_NO_SBUSES+1];

