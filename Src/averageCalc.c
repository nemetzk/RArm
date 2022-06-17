/*
 * averageCalc.c
 *
 *  Created on: 2022. jún. 14.
 *      Author: nemet
 */

#include "averagecalc.h"

void averageCalcCycl(averageCalct* averageCalc,int32_t newVal)
{
switch (averageCalc->state)
{
	case acInit:
		averageCalc->Stack1 = createStack (averageCalc->stackCapacity);
		averageCalc->Stack2 = createStack (averageCalc->stackCapacity);
		push(averageCalc->Stack1,newVal);
 		averageCalc->state = acFill;
	break;
	case acFill:
		push(averageCalc->Stack1,newVal);
		if (isFull(averageCalc->Stack1))
		{
			averageCalc->averageVal = avgCalc(averageCalc->Stack1,averageCalc->Stack2);
			averageCalc->state = acS1ToS2;
		}
	break;
	case acS1ToS2:
		push(averageCalc->Stack2,newVal);
		averageCalc->averageVal = avgCalc(averageCalc->Stack1,averageCalc->Stack2);
		//averageCalc->state = acS2ToS1;
	break;

} //switch
}

double avgCalc(Stackt* stackFrom, Stackt* stackTo)
{
	int32_t sum = 0;
	int32_t item;
	double avg = 0;


	while (!isFull(stackTo))
	{
		item = pop(stackFrom);
		sum+=item;
		push(stackTo, item);
	}
	pop(stackFrom);
	sum=0;
	while (!isFull(stackFrom))
	{
		item = pop(stackTo);
		sum+=item;
		push(stackFrom, item);
	}


	avg = (double)sum / ((double)stackFrom->capacity);


	return avg;
}
/*
double averageCalculation (Stackt* stack)
{
	int32_t sum = 0;
	double avg = 0;
	int i = 0;
	for (i=stack->array;stack->array+i->capacity;i++)
		sum +=stack->array;
	avg = (double)sum / (double)stack->capacity;
	return avg;
}
*/
