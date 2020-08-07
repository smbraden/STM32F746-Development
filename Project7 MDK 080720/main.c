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
void UART4_IRQnHandler(void);

int main(void) {
	
	// clock enable UART peripheral
	RCC->APB1ENR &= ~RCC_APB1ENR_UART4EN;
	RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
	// clock enable GPIO
	RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOCEN;	// using PC10 and PC11
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	// [For some other STM32's, alternate function clock enable here]
		
	// configure pins PC10 and PC11 for UART4
	GPIOC->MODER &= ~((0x3UL << (2 * PC10)) | (0x3UL << (2 * PC11))); 	// Reset
	GPIOC->MODER |= (0x2UL << (2 * PC10)) | (0x2UL << (2 * PC11));		// Alternate Function mode
	GPIOC->OTYPER &= ~((0x1UL << PC10) | (0x1UL << PC11));				// Output push-pull (reset state)
	GPIOC->OSPEEDR &= ~((0x3UL << (2 * PC10)) | (0x3UL << (2 * PC11)));	// Reset
	GPIOC->OSPEEDR |= (0x2UL << (2 * PC10)) | (0x2UL << (2 * PC11));	// High speed

	// GPIO alternate function configuration
    GPIOC->AFR[0] &= ~((0xFUL << (2 * PC10)));
    GPIOC->AFR[0] |=  ((0x7UL << (2 * PC10)));
    GPIOC->AFR[1] &= ~((0xFUL << ((PC11 - 8) * 4)));
    GPIOC->AFR[1] |=  ((0x3UL << ((PC11 - 8) * 4)));

	// set the baud rate to 9600
	uint32_t uartdiv = SystemCoreClock / 9600;
	UART4->BRR = (((uartdiv / 16) << USART_BRR_DIV_MANTISSA_Pos) 
				| ((uartdiv % 16) << USART_BRR_DIV_FRACTION_Pos));

	// Enable the USART peripheral with RX and RX timeout interrupts.
	UART4->CR1 |= (USART_CR1_RE | USART_CR1_TE 
					| USART_CR1_UE | USART_CR1_RXNEIE );

	// set USART word length to 8
	UART4->CR1 &= ~USART_CR1_M;

	// Enable the USART peripheral: UE, TE, RE bits 
	UART4->CR1 |= ( USART_CR1_RE | USART_CR1_TE | USART_CR1_UE );


}

// USART4 interrupt handler
void UART4_IRQnHandler(void) {

    // 'Receive register not empty' interrupt.
    if (UART4->ISR & USART_ISR_RXNE) {	// RXNE bit set by hardware when the content of the RDR shift register has been transferred to the USART_RDR register
      // Copy new data into the buffer.
      recieved = UART4->RDR;
    }
}


