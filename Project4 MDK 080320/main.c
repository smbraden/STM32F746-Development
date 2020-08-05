/*
Description:	Basic implementation of timer peripherals in the STM32F7 board.
	
Author:			Sonja Braden

Reference:		https://vivonomicon.com/2018/05/20/bare-metal-stm32-programming-part-5-timer-peripherals-and-the-system-clock/
	
IDE:			Keil uVision 5

uVision 
Dependencies:	CMSIS Core, STM32F746xx Startup files
*/

#include "stm32f746xx.h"
#include "GPIO.h"
#include "EXTI.h"
#include "GPT.h"
#include "pinDefines.h"


// Global variables
volatile uint8_t core_clock_hz;
uint16_t timer_ms = 1000;			// timer milliseconds

// Function prototypes
void clockConfig(void);
void TIM2_IRQ_handler(void);


int main(void) {
	
	// Configure the clock for PLL at 48 MHz
	clockConfig();
	
	// Enable the timer 2 peripheral
	enableGPT(TIM2);
	
	// enable clock for GPIOB port
	RCC_GPIOB();
	
	// Set LED pin to push-pull low-speed output.
	initLED(LED1_PIN, GPIOB);
	
	// Enable the NVIC interrupt for TIM2.
	// (Timer peripheral initialized and used elsewhere)
	NVIC_SetPriority(TIM2_IRQn, 0x0);
	NVIC_EnableIRQ(TIM2_IRQn);

	// Start the timer.
	initGPT(TIM2, timer_ms, core_clock_hz);
	
	// main event loop
	while (1) {
		
	}
}


void clockConfig() {
	
	// SYSCLK will be derived from PLLCLK
	// HSI = 16MHz is the clock source at bootup
	// Configure the PLL to (HSI / PLLM) * PLLN / PLLP = 48 MHz.
	//						(16 / 16) * 192 / 4 = 48 MHz
    
	
	// Pseudocode:
    // configure PLLM to 16
	// configure PLLN to 192
	// configure PLLP to 4
	
	
    // Turn the PLL on and wait for it to be ready.
    RCC->CR |= (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}

	// Select the PLL as the system clock source.
    RCC->CFGR  &= ~(RCC_CFGR_SW);
    RCC->CFGR  |=  (RCC_CFGR_SW_PLL);
	while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {}
    
	// Set the global clock speed variable.
    core_clock_hz = 48000000;
	
}





void TIM2_IRQ_handler(void) {
	// Handle a timer 'update' interrupt event
	if (TIM2->SR & TIM_SR_UIF) {
		TIM2->SR &= ~(TIM_SR_UIF);
		// Toggle the LED output pin.
		OutputToggle(GPIOB, LED1_PIN);
	}
}


