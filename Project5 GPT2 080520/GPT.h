// A set of functions implementing the General Purpose Timer peripherals on the STM32F746
#include "stm32f746xx.h"


#define stopGPT(TIMx) {\
	TIMx->CR1 &= ~(TIM_CR1_CEN);\
	TIMx->SR  &= ~(TIM_SR_UIF);\
}
#define startGPT(TIMx) {\
	TIMx->CR1  |= TIM_CR1_CEN;\
}


// For configuring the system clock to 48MHz with PLL as source
void clockConfig(void);

void enableGPT(TIM_TypeDef* TIMx);
void initGPT(TIM_TypeDef*, uint16_t, uint32_t);
