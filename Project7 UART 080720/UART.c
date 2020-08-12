#include "UART.h"
#include "pinDefines.h"
#include "GPIO.h"
#include <string.h>	// for strlen


// This only configures PC6 and PC7 for USART6
void configUART(void) {
	
	// Off while configuring
	USART6->CR1 &= ~(USART_CR1_RE | USART_CR1_TE | USART_CR1_UE );
		
	//-----------------Eanble UART Clock------------------------------------//
	
	// clock enable UART peripheral
	RCC->APB2ENR &= ~RCC_APB2ENR_USART6EN;
	RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
	// clock enable GPIO
	RCC->AHB1ENR &= ~RCC_AHB1ENR_GPIOCEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
		
	//-----------------Configure UART GPIO--------------------------------//
	
	// configure pin PC6 transmitter (TX6) USART6
	GPIOC->MODER	&=	~(0x3UL << (2 * TX6));			// Reset
	GPIOC->MODER	|=	(0x2UL << (2 * TX6));			// Alternate Function mode
	GPIOC->OTYPER	&=	~(0x1UL << TX6);				// Push-pull
	GPIOC->OSPEEDR	&=	~(0x3UL << (2 * TX6));			// Reset
	GPIOC->OSPEEDR	|=	(0x3UL << (2 * TX6));			// High speed
	GPIOC->PUPDR	&=	~(0x3UL << (2 * TX6));			// No pull-up or pull-down
	
	// configure pin PC7 reciever (RX6) USART6	
	// Besides "Alt Function", these should be default
	GPIOC->MODER	&=	~(0x3UL << (2 * RX6)); 			// Reset
	GPIOC->MODER	|=	(0x2UL << (2 * RX6));			// Alternate Function mode
	GPIOC->OTYPER	&=	~(0x1UL << RX6);				// Input, pull-up (reset state) Default
	GPIOC->PUPDR	&=	~(0x3UL << (2 * RX6));			// No pull-up or pull-down
	

	// GPIO alternate function configuration					// AFR[0] is for the lower pins' register
    GPIOC->AFR[0] &= ~(GPIO_AFRL_AFRL6 & GPIO_AFRL_AFRL7);		// 
    GPIOC->AFR[0] |=  GPIO_AFRL_AFRL6_3 | GPIO_AFRL_AFRL7_3;	// UART6 Alt function bits: AF8 = 0b1000
	
	//--------------------Redundant default settings---------------------//
	
	// set UART word length to 8	M[1:0] = 00: 1 Start bit, 8 data bits, n stop bits
	USART6->CR1 &= ~USART_CR1_M;
	// No parity 					PCE[0] = 0: Parity control disabled
	USART6->CR1 &= ~USART_CR1_PCE;
	// 1 stop bit STOP[0] = 0		STOP[1:0] = 00: 1 stop bit
	USART6->CR2 &= ~USART_CR2_STOP;
	
	//--------------------Baud rate(*)-----------------------------//
	
	USART6->CR1 |= ~USART_CR1_OVER8;	// Select oversampling by 16 (OVER8=0)
	
	/*	 For baud of 9600 with OVER8 = 0 at 16MHz:
	
		USARTDIV = fck/baud = 16,000,000/9600 ~= 1666.666667 ~= 1666.65625
		
		binary:	0110 1000 0010 . 1010
		hex:	Mantissa = 0x682	Fraction = 0xA 	
	*/
	
	USART6->BRR = (0x682 << USART_BRR_DIV_MANTISSA_Pos) 
				| (0xA << USART_BRR_DIV_FRACTION_Pos);
	
	// Equivalently: USART6->BRR = (0x682A << USART_BRR_DIV_FRACTION_Pos)
	
	//--------------Hardware Interupt----------------------------------//
	
	// Enable USART interrupts whenever ORE=1 or RXNE=1 in the USART_ISR 
	USART6->CR1 |= USART_CR1_RXNEIE;

	NVIC_SetPriority(USART6_IRQn, 0x0UL);
	NVIC_EnableIRQ(USART6_IRQn);


	// UART4->CR1 |= USART_CR1_TXEIE;	// Transmit data register empty interrupt enable
	// UART4->CR1 |= USART_CR1_TCIE;	// Transmission Complete interrupt enable
	

	//---------------------------------------------------------------///

	// Enable the USART 
	// UE, TE, RE ("UART Enable", "Transmitter Enable", Reciever Enable" bits) 
	USART6->CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_TE);
	
}



void sendByte(char ch)
{
	while(!(USART6->ISR & USART_ISR_TC)){}	// Wait for empty flag
	USART6->TDR = ch;						// Set data to Transmit Data Register
	while(!(USART6->ISR & USART_ISR_TC)){}	// Wait for empty flag
}
// TC: Transmission complete
// Bit is set by hardware if the transmission of a data frame is complete and if TXE is set. 



void sendString(char* msg) {
	
	uint32_t length = strlen(msg);
	
	for(uint8_t i = 0; i < length; i++)
		sendByte(msg[i]);
}





/* 
	(*)Equation 1: Baud rate for standard USART (SPI mode included) (OVER8 = 0 or 1)
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

