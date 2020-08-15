/*
Project:		USART Echo

Description:	Basic implementation of the Nucleo's UART peripheral.
				The characters sent to the reciever through the keyboard
				are echoed back to the serial terminal by the transmitter
				
Author:			Sonja Braden

References:		https://github.com/WRansohoff/STM32_UART_Examples/tree/master/receive_irq
				http://web.sonoma.edu/users/f/farahman/sonoma/courses/es310/310_arm/lectures/resources/ARM-UART-setting.pdf
				https://www.youtube.com/watch?v=nieOpDR7kBs&list=PLmY3zqJJdVeNIZ8z_yw7Db9ej3FVG0iLy&index=10&t=489s

IDE:			Keil uVision 5

uVision 
Dependencies:	CMSIS Core, STM32F746xx Startup files
*/


// microcontroller specific 
#include "stm32f746xx.h"


// Default UART6 pins
#define TX6 6 // PC6 
#define RX6 7 // PC7


// Global variables
volatile uint8_t data = '0';


// Prototypes
void configUART(void);
void printString(const char[]);
void transmitByte(char);
void USART6_IRQnHandler(void);


int main(void) {	//-----------Main Event Loop----------//
	
	configUART();
	printString("Hello World!");
	
	while(1) {
	
	}

}

//----------- USART6 interrupt handler---------//

void USART6_IRQnHandler(void) {

    if (USART6->ISR & USART_ISR_RXNE) {			// 'Receive register not empty' interrupt.
		data = USART6->RDR;						// Copy new data into the buffer.
		USART6->TDR = data;						// echo the character
		while(!(USART6->ISR & USART_ISR_TC)){}	// wait until char sent
    }
	// "RXNE bit set by hardware when the content of the 
	// RDR shift register has been transferred to the USART_RDR register"
}


//-------- UART Functions --------------------//


void transmitByte(char ch)
{
	while(!(USART6->ISR & USART_ISR_TC)){}	// Wait for empty flag
	USART6->TDR = ch;						// Set data to Transmit Data Register
}
// "TC: Transmission complete
// Bit is set by hardware if the transmission of a data frame is complete and if TXE is set."



void printString(const char msg[]) {
	
	uint8_t i = 0;
	while(msg[i]) {
		transmitByte(msg[i]);
		i++;
	}
}


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
	GPIOC->OTYPER	&=	~(0x1UL << TX6);				// Output push-pull (reset state)
	GPIOC->OSPEEDR	&=	~(0x3UL << (2 * TX6));			// Reset
	GPIOC->PUPDR	&=	~(0x3UL << (2 * TX6));			// No pull-up or pull-down
	
	// configure pin PC7 reciever (RX6) USART6	
	// Besides "Alt Function", these should be default
	GPIOC->MODER	&=	~(0x3UL << (2 * RX6)); 			// Reset
	GPIOC->MODER	|=	(0x2UL << (2 * RX6));			// Alternate Function mode
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
	
	//--------------------------------------------------------------//
	
	// Enable USART interrupts whenever ORE=1 or RXNE=1 in the USART_ISR 
	USART6->CR1 |= USART_CR1_RXNEIE;

	NVIC_SetPriority(USART6_IRQn, 0x0UL);
	NVIC_EnableIRQ(USART6_IRQn);

	// Enable the USART 
	// UE, TE, RE ("UART Enable", "Reciever Enable", "Transmitter Enable" bits) 
	USART6->CR1 |= (USART_CR1_UE | USART_CR1_RE | USART_CR1_TE);
	
}




