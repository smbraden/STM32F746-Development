/*
Description:	Basic implementation of the Nucleo's Analog to Digital Converter
				An LED display wired to the E port will be utilized 
				to represent the voltage across a photoresistor relative to the supply (3.3V)
				
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


// user defined libraries
#include "GPIO.h"
#include "UART.h"
#include "pinDefines.h"


// Macros
#define ROW_Pos (2U)					// LED row position of least sig bit
#define ROW_Msk (0xFFUL << 2)			// LED row bit mask


// Global variables
static volatile uint8_t data;				// data should be a byte, but make it uint8_t to hush compiler warning 
static volatile uint32_t msTicks = 0;		// store millisecond ticks

// Interrupt prototypes
void USART6_IRQnHandler(void);

// SysTick function prototypes
void initSysTick(void);
void SysTick_Handler(void);
void delay_ms(uint32_t);

// GPIO
void configDisplay(void);

int main(void) {	//-----------Main Event Loop----------//
	
	initSysTick();
	configDisplay();
	configUART();
	sendString("Hello World!");
	
	while(1) {
	
		GPIOE->ODR &= ~(ROW_Msk);					// clear the previous display
		GPIOE->ODR |= (data << ROW_Pos);			// output the current data byte in binary
		
	}

}

//----------- USART6 interrupt handler---------//


void USART6_IRQnHandler(void) {

    if (USART6->ISR & USART_ISR_RXNE) {			// 'Receive register not empty' interrupt.
		data = USART6->RDR;						// Copy new data into the buffer.
		sendByte(data);							// echo the characters
    }
	// RXNE bit set by hardware when the content of the 
	// RDR shift register has been transferred to the USART_RDR register
}


//------------SysTick functions---------------//

  
// Configures SysTick to generate 1 ms interrupts
void initSysTick(void) {
	
	// 1 interrupt per millisecond
	SysTick_Config(SystemCoreClock / 1000);	
	// generating 1 interupt per (SystemCoreClock / 1000) 'ticks'
}


// SysTick interrupt Handler
// Will only response to its interrupt if initSysTick() is called beforehand 
void SysTick_Handler(void)  {
	msTicks++;
}


// Can only be called if initSysTick() is called first
void delay_ms(uint32_t delayTime) {
	
	uint32_t curTicks;
	curTicks = msTicks;
	while ((msTicks - curTicks) < delayTime) {}
}


//-----------------LED bank for Reciever binary display---------------//

void configDisplay(void) {
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	
	// Set the wired LED pins to push-pull low-speed output.
	// Using pins 2 through 9, inclusive 
	for (uint8_t PINx = 2; PINx < 10; PINx++) {
		// Set LED pin to push-pull low-speed output.
		GPIOE->MODER &= ~(0x3UL << (PINx*2));		// input mode (reset state)
		GPIOE->MODER |= (0x1UL << (PINx*2));		// output mode
		GPIOE->OTYPER &= ~(1UL << PINx);			// Push-pull output
		GPIOE->PUPDR &= ~(3UL << (PINx*2));		// No pull-up/pull-down resistors
	}	
	
	// Test LED bank with a few flashes
	for (int i = 0; i < 6; i++) {
		GPIOE->ODR ^= ROW_Msk;
		delay_ms(1000);
	}
}
