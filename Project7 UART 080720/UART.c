#include "UART.h"
#include "pinDefines.h"
#include "GPIO.h"

// C standard libraries
#include <stdio.h>
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"

// Temporarily, this only configures PC10 and PC11 for UART4
void configUART(void) {
	
	//-----------------Eanble UART Clock------------------------------------//
	
	// clock enable UART peripheral
	RCC->APB1ENR &= ~RCC_APB1ENR_UART4EN;
	RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
	// clock enable GPIO
	RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOCEN;	// using PC10 and PC11
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	// [For some other STM32's, alternate function clock enable here]
	
	//-----------------Configure UART GPIO--------------------------------//
	
	// configure pin PC10 transmitter (TX4) UART4
	GPIOC->MODER	&=	~(0x3UL << (2 * TX4));			// Reset
	GPIOC->MODER	|=	(0x2UL << (2 * TX4));			// Alternate Function mode
	GPIOC->OTYPER	&=	~(0x1UL << TX4);				// Push-pull
	GPIOC->OSPEEDR	&=	~(0x3UL << (2 * TX4));			// Reset
	GPIOC->OSPEEDR	|=	(0x2UL << (2 * TX4));			// High speed
	GPIOC->PUPDR	&=	~(0x3UL << (2 * TX4));			// No pull-up or pull-down
	
	// configure pin PC11 reciever (RX4) UART4	
	// Besides "Alt Function", these should be default
	GPIOC->MODER	&=	~(0x3UL << (2 * RX4)); 			// Reset
	GPIOC->MODER	|=	(0x2UL << (2 * RX4));			// Alternate Function mode
	GPIOC->OTYPER	&=	~(0x1UL << RX4);				// Input, pull-up (reset state)
	GPIOC->OSPEEDR	&=	~(0x3UL << (2 * RX4));			// Reset
	GPIOC->OSPEEDR	|=	(0x2UL << (2 * RX4));			// High speed
	GPIOC->PUPDR	&=	~(0x3UL << (2 * RX4));			// Reset
	GPIOC->PUPDR	|=	(0x1UL << (2 * RX4));			// Pull-up 
	
	//--------------------------------------------------------------//
	
	// GPIO alternate function configuration					// AFR[1] is for the Higher pins register
    GPIOC->AFR[1] &= ~(GPIO_AFRH_AFRH2 & GPIO_AFRH_AFRH3);		// AF8 for UART4_TX and UART4_RX
    GPIOC->AFR[1] |=  GPIO_AFRH_AFRH2_3 | GPIO_AFRH_AFRH3_3;	// UART4 Alt function bits: AF8 = 0b1000


	//--------------------Baud rate-----------------------------//
		
	// set the baud rate to 9600... see pg 1040 of Ref Manual
	uint32_t uartdiv = SystemCoreClock / 9600;
	
	UART4->BRR = (((uartdiv / 16) << USART_BRR_DIV_MANTISSA_Pos) 
				| ((uartdiv % 16) << USART_BRR_DIV_FRACTION_Pos));
	
	// Enable USART interrupts whenever ORE=1 or RXNE=1 in the USART_ISR 
	UART4->CR1 |= USART_CR1_RXNEIE;

	///----------These should be the default settings already---------//
	
	// No parity 					PCE[0] = 0: Parity control disabled
	UART4->CR1 &= ~USART_CR1_PCE;
	// 1 stop bit STOP[0] = 0		STOP[1:0] = 00: 1 stop bit
	UART4->CR2 &= ~USART_CR2_STOP;
	// set UART word length to 8	M[1:0] = 00: 1 Start bit, 8 data bits, n stop bits
	UART4->CR1 &= ~USART_CR1_M;
	
	//---------------------------------------------------------------///
		
	// Enable the USART peripheral: 
	// UE, TE, RE ("UART Enable", "Transmitter Enable", Reciever Enable" bits) 
	UART4->CR1 |= (USART_CR1_RE | USART_CR1_TE | USART_CR1_UE );
	
}


void print(const char* msg, ...) {
	
	char buffer[80];
	va_list args;
	va_start(args, msg);
	vsprintf(buffer, msg, args);
	
	for(int i = 0; i < strlen(buffer); i++) {
		UART4->RDR = buffer[i];					// RDR is 8-bit (ASCII)
		while(!(UART4->ISR & USART_ISR_TC)){}
	}
}





