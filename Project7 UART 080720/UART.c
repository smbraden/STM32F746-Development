#include "UART.h"
#include "pinDefines.h"
#include "GPIO.h"

// C standard libraries
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

// Temporarily, this only configures PC10 and PC11 for UART4
void configUART(void) {
	
	//-----------------Eanble UART Clock------------------------------------//
	
	// clock enable UART peripheral
	RCC->APB1ENR &= ~RCC_APB1ENR_UART4EN;
	RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
	// clock enable GPIO
	RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOCEN;	// PA0 and PA1 for default, remapping to use PC10 and PC11
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

	// GPIO alternate function configuration					// AFR[1] is for the Higher pins register
    GPIOC->AFR[1] &= ~(GPIO_AFRH_AFRH2 & GPIO_AFRH_AFRH3);		// AF8 for UART4_TX and UART4_RX
    GPIOC->AFR[1] |=  GPIO_AFRH_AFRH2_3 | GPIO_AFRH_AFRH3_3;	// UART4 Alt function bits: AF8 = 0b1000

	
	//--------------------Baud rate-----------------------------//
	
	UART4->CR1 |= ~USART_CR1_OVER8;	// Select oversampling by 16 (OVER8=0)
	
	
	/* Equation 1: Baud rate for standard USART (SPI mode included) (OVER8 = 0 or 1)
	In case of oversampling by 16, the equation is:
	
		Tx/Rx baud = fck/USARTDIV --> USARTDIV = fck/baud
		
	In case of oversampling by 8, the equation is:
	
		Tx/Rx baud = (2 * fck)/USARTDIV --> USARTDIV = (2* fck)/baud
		
	USARTDIV is an unsigned fixed point number that is coded on the USART_BRR register.
		When OVER8 = 0, BRR = USARTDIV.
		When OVER8 = 1
			– BRR[2:0] = USARTDIV[3:0] shifted 1 bit to the right.
			– BRR[3] must be kept cleared.
			– BRR[15:4] = USARTDIV[15:4]

	(pg 1040 of Reference Manual)
	*/
	
	
	/*	 For baud of 9600 with OVER8 = 0 at 16MHz:
	
		USARTDIV = fck/baud = 16,000,000/9600 ~= 1666.666667 ~= 1666.65625
		
		binary:	1	1	0	1	0	0	0	0	0	1	0 . 1	0	1	0
		hex:	Mantissa = 628 Fraction = A 	
	*/
		
	UART4->BRR = (0x628 << USART_BRR_DIV_MANTISSA_Pos) 
				| (0xA << USART_BRR_DIV_FRACTION_Pos);
	
	//--------------Hardware Interupts----------------------------------//

	NVIC_SetPriority(UART4_IRQn, 0x03UL);
	NVIC_EnableIRQ(UART4_IRQn);

	// Enable USART interrupts whenever ORE=1 or RXNE=1 in the USART_ISR 
	UART4->CR1 |= USART_CR1_RXNEIE;

	///--------------------Redundant default settings---------------------//
	
	// No parity 					PCE[0] = 0: Parity control disabled
	UART4->CR1 &= ~USART_CR1_PCE;
	// 1 stop bit STOP[0] = 0		STOP[1:0] = 00: 1 stop bit
	UART4->CR2 &= ~USART_CR2_STOP;
	// set UART word length to 8	M[1:0] = 00: 1 Start bit, 8 data bits, n stop bits
	UART4->CR1 &= ~USART_CR1_M;
	
	//---------------------------------------------------------------///
		
	// Enable the USART 
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




