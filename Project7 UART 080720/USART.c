#include "USART.h"





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
