#ifndef UART_H
#define UART_H

#include "stm32f746xx.h"



// Temporarily, this only configures PC10 and PC11 for UART4
void configUART(void);
void sendByte(char);
void sendString(char* msg);


#endif
