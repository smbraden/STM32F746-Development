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
void TIM2_IRQHandler(void);


int main(void) {
	
	
	// Configure the clock for PLL at 48 MHz
	clockConfig();
	
	// Enable the timer 2 peripheral
	enableGPT(TIM2);
	
	// enable clock for GPIOB port
	RCC_GPIOB();
	
	// Enable the TIM2 clock.
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	// Set LED pin to push-pull low-speed output.
	initLED(LED1_PIN, GPIOB);
	initLED(LED2_PIN, GPIOB);
	
	// Test timer-independent GPIO
	GPIOB->ODR |= (1 << LED2_PIN);
	
	// Enable the NVIC interrupt for TIM2.
	NVIC_SetPriority(TIM2_IRQn, 0x0);
	NVIC_EnableIRQ(TIM2_IRQn);

	// Start the timer.
	initGPT(TIM2, timer_ms, core_clock_hz);
	
	// main event loop
	while (1) {
		
	}
}


void clockConfig() {
	
	// set the Flash Latency to 1 wait state in order to access Flash memory at 48 MHz
	// The default boots to zero wait state.
	// LATENCY[3:0]	lives in the FLASH->ACR register
	/*
		These bits represent the ratio of the CPU clock period to the Flash memory access time.
		0000: Zero wait state
		0001: One wait state
		0010: Two wait states
		...
		...		
		1110: Fourteen wait states
		1111: Fifteen wait states
	*/

	// Also, enable the prefetch buffer in order to help compensate for slower reading
	FLASH->ACR |= FLASH_ACR_LATENCY_1WS | FLASH_ACR_PRFTEN;
	
	// SYSCLK will be derived from PLLCLK
	// HSI = 16MHz is the clock source at bootup
	// Configure the PLL to (HSI / PLLM) * PLLN / PLLP = 48 MHz.
	//						(16 / 16) * 192 / 4 = 48 MHz
    
	// Keep the PLL off while configuraing M, N, P
    RCC->CR &= ~(RCC_CR_PLLON);
	
    // reset/configure PLLM to 16		 		PLLM[5:0] = RCC_CFGR[5:0]
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_4;		//	= (0x10UL << RCC_PLLCFGR_PLLM_Pos)     /*!< 0x00000010 */
	
	// rest/configure PLLN to 192				PLLN[8:0] = RCC_CFGR[14:6]	
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;
	RCC->PLLCFGR |= (192 << 6);				// 	|=(0xC0 << 6)	|= 0b11000000
	//RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_7;
		
	// reset/configure PLLP to 4				PLLP[1:0] = RCC_CFGR[17:16]
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;		//	= (0x1UL << RCC_PLLCFGR_PLLP_Pos)      /*!< 0x00010000 */
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0;
	
    // Turn the PLL on and wait for it to be ready.
    RCC->CR |= (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}

	// Select the PLL as the system clock source.
    RCC->CFGR  &= ~(RCC_CFGR_SW);
    RCC->CFGR  |=  (RCC_CFGR_SW_PLL);
	while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {}
    
	// Set the global clock speed variable.
    core_clock_hz = 48000000;
	
		
		
	// A last-ditch effort to get the application working as desired...
		
	// Set APB1 prescalar as 2, resulting in (48/2) = 24 MHz
	// Just to be cautious, because APB1 bus runs at max 54MHz:
	// "The software has to set these bits correctly not to exceed 54 MHz on this domain."
	// PPRE1[2:0] lives in RCC_CFGR[12:10]
	// Set these as 100, resulting in AHB clock divided by 2
	RCC->CFGR &= RCC_CFGR_PPRE1;
	RCC->CFGR |= RCC_CFGR_PPRE1_2;			//	(0x4UL << RCC_CFGR_PPRE1_Pos)        /*!< 0x00001000 */
	
	
	/*	0xx: AHB clock not divided
		100: AHB clock divided by 2
		101: AHB clock divided by 4
		110: AHB clock divided by 8
		111: AHB clock divided by 16	*/
	
		
}


void TIM2_IRQHandler(void) {
	// Handle a timer 'update' interrupt event
	if (TIM2->SR & TIM_SR_UIF) {
		// Toggle the LED output pin.
		GPIOB->ODR ^= (1 << LED1_PIN);
		// Reset the Update Interrupt Flag (UIF)
		TIM2->SR &= ~(TIM_SR_UIF);
	}
}


