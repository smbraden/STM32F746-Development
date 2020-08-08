/*
Description:	Basic implementation of the Nucleo's Analog to Digital Converter
				An LED display wired to the E port will be utilized 
				to represent the voltage across a photoresistor relative to the supply (3.3V)
				
Author:			Sonja Braden

References:		https://github.com/WRansohoff/STM32_UART_Examples/tree/master/receive_irq
				https://www.youtube.com/watch?v=nieOpDR7kBs&list=PLmY3zqJJdVeNIZ8z_yw7Db9ej3FVG0iLy&index=10&t=489s

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
// uint32_t SystemCoreClock = 16000000;	// 16 MHz
static volatile char recieved;
volatile uint32_t msTicks = 0;		// store millisecond ticks


// Function prototypes
void UART4_IRQnHandler(void);
static void print(char* msg, int length);
void SysTick_Handler(void);
void initSysTick(void);

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
    GPIOC->AFR[1] &= ~GPIO_AFRH_AFRH2;		// AF8 for UART4_RX & UART4_TX 
    GPIOC->AFR[1] |=  GPIO_AFRH_AFRH1_3;	// bits to configure alternate function I/Os: AF8 = 1000
    GPIOC->AFR[1] &= ~GPIO_AFRH_AFRH2;
    GPIOC->AFR[1] |=  GPIO_AFRH_AFRH1_3;

	// set the baud rate to 9600... see pg 1040 of Ref Manual
	uint32_t uartdiv = SystemCoreClock / 9600;
	
	UART4->BRR = (((uartdiv / 16) << USART_BRR_DIV_MANTISSA_Pos) 
				| ((uartdiv % 16) << USART_BRR_DIV_FRACTION_Pos));
	// UART4->BRR = uartdiv;

	// Enable the USART peripheral with RX and TX timeout interrupts.
	UART4->CR1 |= (USART_CR1_RE | USART_CR1_TE 
				 | USART_CR1_UE | USART_CR1_RXNEIE );

	// set USART word length to 8
	UART4->CR1 &= ~USART_CR1_M;

	// Enable the USART peripheral: UE, TE, RE bits 
	UART4->CR1 |= (USART_CR1_RE | USART_CR1_TE | USART_CR1_UE );

	
	char string[] = "Hello World!\n";
	int strLen = strlen(string);
	
	initSysTick();
	while(1) {
		
		print(string, strLen);
		while(msTicks != 0){}
	}

}

static void print(char* msg, int length) {
	
	for(int i = 0; i < length; i++) {
		UART4->RDR = msg[i];					// RDR is 8-bit (ASCII)
		while(!(UART4->ISR & USART_ISR_TXE)){}
	}
}


/*

static void print(const char* msg, ...) {
	
	char buffer[80];
	va_list args;
	va_start(args, msg);
	vsprintf(buffer, msg, args);
	
	for(int i = 0; i < strlen(buffer); i++) {
		UART4->RDR = buffer[i];					// RDR is 8-bit (ASCII)
		while(!(UART4->ISR & USART_ISR_TXE)){}
	}
}

*/

// USART4 interrupt handler
void UART4_IRQnHandler(void) {

    // 'Receive register not empty' interrupt.
    if (UART4->ISR & USART_ISR_RXNE) {	// RXNE bit set by hardware when the content of the RDR shift register has been transferred to the USART_RDR register
      // Copy new data into the buffer.
      recieved = UART4->RDR;
    }
}

  
void SysTick_Handler(void)  {                               /* SysTick interrupt Handler. */
	msTicks = (msTicks < 1000)? (msTicks + 1) : 0;
}

void initSysTick(void) {
	
	uint32_t returnStatus;
	returnStatus = SysTick_Config(SystemCoreClock / 1000);	// Configure SysTick to generate an interrupt every millisecond */

	//if (returnStatus != 0)  {Error Handling}   // Check return code for errors 
}


