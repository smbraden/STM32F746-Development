// A set of functions implementing the General Purpose Timer peripherals on the STM32F746
#include "stm32f746xx.h"

// For configuring the system clock to 48MHz to PLL as source
void clockConfig(void);

void enableGPT(TIM_TypeDef* TIMx);
void initGPT(TIM_TypeDef*, uint16_t, uint32_t);
void stopGPT(TIM_TypeDef* TIMx);
