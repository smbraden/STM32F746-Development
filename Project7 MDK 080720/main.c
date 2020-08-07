/*
Description:	Basic implementation of the Nucleo's Analog to Digital Converter
				An LED display wired to the E port will be utilized 
				to represent the voltage across a photoresistor relative to the supply (3.3V)
				
Author:			Sonja Braden
	
IDE:			Keil uVision 5

uVision 
Dependencies:	CMSIS Core, STM32F746xx Startup files
*/


// microcontroller specific 
#include "stm32f746xx.h"

// C standard libraries
#include <stdint.h>
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"

// user defined libraries
#include "GPIO.h"
#include "GPT.h"
#include "pinDefines.h"

// Macro defines
#define PC10 10
#define PC11 11

// Global variables
uint32_t SystemCoreClock = 16000000;	// 16 MHz
static volatile char recieved;

// Function prototypes
void enableClockUART(uint8_t);	// USART_TypeDef*
void enableClockUSART(uint8_t);	// USART_TypeDef*
void printUART(char *msg, ...);
void UART4_IRQnHandler(void);

int main(void) {
	
	// clock enable UART peripheral
	RCC->APB1ENR &= ~RCC_APB1ENR_UART4EN;
	RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
	// clock enable GPIO
	RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOCEN;	// using PC10 and PC11
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	// configure pins PC10 and PC11 for UART4
	GPIOC->MODER &= ~((0x3UL << (2 * PC10)) | (0x3UL << (2 * PC11))); 
	GPIOC->MODER |= (0x3UL << (2 * PC10)) | (0x3UL << (2 * PC11));
	GPIOC->OTYPER &= ~((0x1UL << (2 * PC10)) | (0x1UL << (2 * PC11)));
	GPIOC->OSPEEDR &= ~((0x3UL << (2 * PC10)) | (0x3UL << (2 * PC11)));
	GPIOC->OSPEEDR |= (0x2UL << (2 * PC10)) | (0x2UL << (2 * PC11));

    GPIOA->AFR[0] &= ~((0xFUL << (2 * PC10)));
    GPIOA->AFR[0] |=  ((0x7UL << (2 * PC10)));
    GPIOA->AFR[1] &= ~((0xFUL << ((PC11 - 8) * 4)));
    GPIOA->AFR[1] |=  ((0x3UL << ((PC11 - 8) * 4)));

	// set the baud rate to 9600
	uint32_t uartdiv = SystemCoreClock / 9600;
	UART4->BRR = (((uartdiv / 16) << USART_BRR_DIV_MANTISSA_Pos) 
				| ((uartdiv % 16) << USART_BRR_DIV_FRACTION_Pos));

	// Enable the USART peripheral with RX and RX timeout interrupts.
	UART4->CR1 |= (USART_CR1_RE | USART_CR1_TE 
					| USART_CR1_UE | USART_CR1_RXNEIE );


}

// USART4 interrupt handler
void UART4_IRQnHandler(void) {

    // 'Receive register not empty' interrupt.
    if (UART4->ISR & USART_ISR_RXNE) {
      // Copy new data into the buffer.
      recieved = UART4->RDR;
    }
}


static void printUART(char *msg, ...) {
	
}






void enableClockUSART(uint8_t USARTx) {	//USART_TypeDef* USARTx

	switch(USARTx) {
		case 1:	// clock enable lives in APB2ENR
			RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
			RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		break;
		case 6:
			RCC->APB2ENR &= ~RCC_APB2ENR_USART6EN;
			RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
		break;
		case 2:	// clock enable lives in APB1ENR
			RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
			RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
		break;
		case 3:
			RCC->APB1ENR &= ~RCC_APB1ENR_USART3EN;
			RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	}
}
	
void enableClockUART(uint8_t USARTx) {	//USART_TypeDef* USARTx

	switch(USARTx) {
		case 1:	// clock enable lives in APB2ENR:
			RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
			RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		break;
		case 6:
			RCC->APB2ENR &= ~RCC_APB2ENR_USART6EN;
			RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
		break;
		case 2:	// clock enable lives in APB1ENR:
			RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
			RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
		break;
		case 3:
			RCC->APB1ENR &= ~RCC_APB1ENR_USART3EN;
			RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
		break;
		case 4:		// UART only:
			RCC->APB1ENR &= ~RCC_APB1ENR_UART4EN;
			RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
		break;
		case 5:
			RCC->APB1ENR &= ~RCC_APB1ENR_UART5EN;
			RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
		break;
		case 7:
			RCC->APB1ENR &= ~RCC_APB1ENR_UART7EN;
			RCC->APB1ENR |= RCC_APB1ENR_UART7EN;
		break;
		case 8:
			RCC->APB1ENR &= ~RCC_APB1ENR_UART8EN;
			RCC->APB1ENR |= RCC_APB1ENR_UART8EN;
	}
}


// The word length can be selected as being either 7 or 8 or 9 bits
// by programming the M[1:0] bits in the USART_CR1 register 
	
	/*
		7-bit character length: M[1:0] = 10
		8-bit character length: M[1:0] = 00
		9-bit character length: M[1:0] = 01		*/


/*
void delay_ms(int time_ms) {
	msTicks = 0;
	while(msTicks < time_ms) {}
}
*/
