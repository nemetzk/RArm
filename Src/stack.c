/*
 * stack.c
 *
 *  Created on: 2022. jún. 10.
 *      Author: nemet
 */

#include "stack.h"


// function to create a stack of given capacity. It initializes size of
// stack as 0
Stackt* createStack(unsigned capacity)
{
    struct Stackth* stack = (struct Stackth*)malloc(sizeof(struct Stackth));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (int32_t*)malloc(stack->capacity * sizeof(int32_t));
    return stack;
}

// Stack is full when top is equal to the last index
int isFull( Stackt* stack)
{
    return stack->top == stack->capacity - 1;
}

// Stack is empty when top is equal to -1
int isEmpty( Stackt* stack)
{
    return stack->top == -1;
}

// Function to add an item to stack.  It increases top by 1
void push( Stackt* stack, int item)
{
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
    //printf("%d pushed to stack\n", item);
}

// Function to remove an item from stack.  It decreases top by 1
int pop( Stackt* stack)
{
    if (isEmpty(stack))
        return INT_MIN;
    return stack->array[stack->top--];
}

void clearStack( Stackt* stack)
{
	while (!isEmpty(stack))
		pop(stack);
}

// Function to return the top from stack without removing it
int peek( Stackt* stack)
{
    if (isEmpty(stack))
        return INT_MIN;
    return stack->array[stack->top];
}

