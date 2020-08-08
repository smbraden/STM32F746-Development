// A set of functions implementing the General Purpose Timer peripherals on the STM32F746
#include "stm32f746xx.h"
#include "GPT.h"





// For configuring the system clock to 48MHz to PLL as source
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
	// (16 / 16) * 192 / 4 = 48 MHz
    
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
    //core_clock_hz = 48000000;
	
	 
/*	APB1 bus runs at max 54MHz.	Reference Manual, pg 149: 
	"The software has to set these bits correctly not to exceed 54 MHz on this domain."
	
	Safe at 48MHz, but could set APB1 prescalar to 2, resulting in (48/2) = 24 MHz
	PPRE1[2:0] lives in RCC_CFGR[12:10]
	Set these as 100, resulting in AHB clock divided by 2
	uncomment the following 2 lines in order to do so:		*/

	//RCC->CFGR &= RCC_CFGR_PPRE1;
	//RCC->CFGR |= RCC_CFGR_PPRE1_2;			//	= 0x00001000
	
	
	/*	0xx: AHB clock not divided
		100: AHB clock divided by 2
		101: AHB clock divided by 4
		110: AHB clock divided by 8
		111: AHB clock divided by 16	*/
	
		
}



void enableGPT(TIM_TypeDef* TIMx) {
	
	// Enable the clock for the General Purpose TIMx 
	if (TIMx == TIM2) {
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if (TIMx == TIM3) {
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if (TIMx == TIM4) {
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	else if (TIMx == TIM5) {
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}
}



// GPT = "General Purpose Timer" 
// For STM32F746, these include TIM2, TIM3, TIM4, TIM5
void initGPT(TIM_TypeDef* TIMx, uint16_t ms, uint32_t coreClock_hz) {
	
	// Timer's counter off
	TIMx->CR1 &= ~(TIM_CR1_CEN);

	// Reset the peripheral
	if (TIMx == TIM2) {
		RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM2RST);
		RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);
	}
	else if (TIMx == TIM3) {
		RCC->APB1RSTR |= (RCC_APB1RSTR_TIM3RST);
		RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM3RST);
	}
	else if (TIMx == TIM4) {
		RCC->APB2RSTR |=  (RCC_APB1RSTR_TIM4RST);
		RCC->APB2RSTR &= ~(RCC_APB1RSTR_TIM4RST);
	}
	else if (TIMx == TIM5) {
		RCC->APB2RSTR |=  (RCC_APB1RSTR_TIM5RST);
		RCC->APB2RSTR &= ~(RCC_APB1RSTR_TIM5RST);
	}
	

	// Set the timer prescaler/autoreload timing registers.
	TIMx->PSC   = coreClock_hz / 1000;
	TIMx->ARR   = ms;
	// Send an update event to reset the timer and apply settings.
	TIMx->EGR  |= TIM_EGR_UG;
	// Enable the hardware interrupt.
	TIMx->DIER |= TIM_DIER_UIE;
	// Enable the timer.
	TIMx->CR1  |= TIM_CR1_CEN;

}


void stopGPT(TIM_TypeDef* TIMx) {
	// Turn off the timer.
	TIMx->CR1 &= ~(TIM_CR1_CEN);
	// Clear the 'pending update interrupt' flag, just in case.
	TIMx->SR  &= ~(TIM_SR_UIF);
}




