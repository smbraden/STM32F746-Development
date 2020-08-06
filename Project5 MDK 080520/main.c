/*
Description:	Basic implementation of the Nucleo's General Purpose Timer Peripheral. 
				8 LED are wired to pins PE2 through PE9. They blink in a successive 
				pattern down the row.
				
Author:			Sonja Braden

Reference:		
	
IDE:			Keil uVision 5

uVision 
Dependencies:	CMSIS Core, STM32F746xx Startup files
*/

#include "stm32f746xx.h"
#include "GPIO.h"
#include "GPT.h"
#include "pinDefines.h"

// Macro defines
//#define GPIOx GPIOE


// Global variables
uint32_t core_clock_hz = 48000000;		// clockConfig() will set the Sys clock as such
uint16_t timer1_ms = 100;				// timer milliseconds
volatile uint16_t timer2_ms = 1000;
volatile uint8_t LEDx = 1;				// toggled by timer interupt
uint32_t ROW_MASK = (0xFF << 2);		// LED row bit mask
volatile uint8_t Cycles = 0;


// Function prototypes
void clockConfig(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
//void delay_ms(TIM_TypeDef*, uint16_t);


int main(void) {
	
	
	// Configure the clock for PLL at 48 MHz
	clockConfig();
	
	// Enable the timer 2 peripheral
	enableGPT(TIM2);
	enableGPT(TIM3);
	
	// Enable the TIM2 clock.
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	// enable clock for GPIOE and GPIOB peripheral
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	// Set the wired LED pins to push-pull low-speed output.
	// For convenience, using pins 2 through 9, inclusive 
	for (uint8_t PINx = 2; PINx < 10; PINx++) {
		configLED(PINx, GPIOE);
	}
	
	// User LED to push-pull low-speed output
	configLED(LED1_PIN, GPIOB);
	
	// Enable the NVIC interrupt for TIM2.
	NVIC_SetPriority(TIM2_IRQn, 0x04);
	NVIC_EnableIRQ(TIM2_IRQn);
	
	// Enable the NVIC interrupt for TIM2.
	NVIC_SetPriority(TIM3_IRQn, 0x03);
	NVIC_EnableIRQ(TIM3_IRQn);

	// Start the TIM2 timer.
	initGPT(TIM2, timer1_ms, core_clock_hz);
	initGPT(TIM3, timer2_ms, core_clock_hz);
	
	// main event loop
	while (1) {
		
		// Testing the timer off macro and prescalar reseting
		if (Cycles == 8 && timer2_ms == 500) {
			
			stopGPT(TIM3);
			Cycles = 0;
			timer2_ms = 1000;
			
			initGPT(TIM3, timer2_ms, core_clock_hz);
		}
		else if (Cycles == 8)	{
			stopGPT(TIM3);
			Cycles = 0;
			timer2_ms = 500;
			
			initGPT(TIM3, timer2_ms, core_clock_hz);
		}
			
	}
}


void TIM2_IRQHandler(void) {
	// Handle a timer 'update' interrupt event
	if (TIM2->SR & TIM_SR_UIF) {
		
		LEDx = (LEDx == 9)? 1 : (LEDx + 1);
		
		GPIOE->ODR &= ~ROW_MASK;
		GPIOE->ODR |= (1 << LEDx);
		
		// Reset the Update Interrupt Flag (UIF)
		TIM2->SR &= ~(TIM_SR_UIF);
	}
}

void TIM3_IRQHandler(void) {
	// Handle a timer 'update' interrupt event
	if (TIM3->SR & TIM_SR_UIF) {
		
		// Reset the Update Interrupt Flag (UIF)
		TIM3->SR &= ~(TIM_SR_UIF);
		// Toggle onboard LED
		GPIOB->ODR ^= (1 << LED1_PIN);
		// increment cycles
		Cycles++;
	}
}


