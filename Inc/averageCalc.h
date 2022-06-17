/*
 * averageCalc.h
 *
 *  Created on: 2022. jún. 14.
 *      Author: nemet
 */
/****************************************************
 *  FOLYAMATOSAN VÁLTOZÓ ÉRTÉKEK ÁTLAGÁNAK SZÁMÍTÁSA
 *
 * 1	2	3	4	5	6	7  ÁTLAG: 4
 * 2	3	4	5	6	7	8  ÁTLAG: 5
 * 3	4	5	6	7	8	9  ÁTLAG: 6
 *
 * HASZNÁLATA:
 * 0) #include "averageCalc.h"
 * 1) TÍPUS LÉTREHOZÁSA averageCalct avgTest;
 * 2) ÁTLAGOLÓ VEREM KAPACITÁSÁNAK MEGHATÁROZÁSA (A FENTI PÉLDÁBAN 7)
 * 	  avgTest.stackCapacity = 7;
 * 3) be kell tenni valami ciklusosan ismétlődő programrészbe ezt:
 *    averageCalcCycl(&avgTest,myidx);
******************************************************/


#ifndef INC_AVERAGECALC_H_
#define INC_AVERAGECALC_H_

#include "stack.h"

#define acInit		0
#define acFill		1
#define acS1ToS2	2
#define acS2ToS1	3


typedef struct acerageCalcth
{
	double  averageVal;
	uint8_t stackCapacity;
	uint8_t state;
	Stackt* Stack1;
	Stackt* Stack2;
}averageCalct;

double avgCalc(Stackt* stackFrom, Stackt* stackTo);
void averageCalcCycl(averageCalct* averageCalc,int32_t newVal);
//double averageCalculation (Stackt* stack);
#endif /* INC_AVERAGECALC_H_ */
