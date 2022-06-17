/*
 * stack.h
 *
 *  Created on: 2022. jún. 10.
 *      Author: nemet
 */

#ifndef INC_STACK_H_
#define INC_STACK_H_

#include <limits.h>
#include "stm32f4xx_hal.h"

// A structure to represent a stack
typedef struct Stackth {
    int top;
    unsigned capacity;
    int32_t* array;
}Stackt;

Stackt* createStack(unsigned capacity);
int isFull(Stackt* stack);
int isEmpty(Stackt* stack);
void push(Stackt* stack, int item);
int pop( Stackt* stack);
int peek( Stackt* stack);
void stackClear(Stackt* stack);

#endif /* INC_STACK_H_ */
