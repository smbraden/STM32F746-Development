#include "stm32f746xx.h"

// return 1 for high, 0 for low input
#define InputPinStatus(GPIOx, GPIO_Pin) ((~(GPIOx->IDR) & (1 << GPIO_Pin)) == 0 ? 0 : 1)

void RCC_GPIOB(void);
void initLED(uint8_t, GPIO_TypeDef*);
void initButton(uint8_t, GPIO_TypeDef*);