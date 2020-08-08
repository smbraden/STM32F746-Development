// A set of functions implementing the General Purpose Timer peripherals on the STM32F746
#include "stm32f746xx.h"
#include "GPT.h"

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
