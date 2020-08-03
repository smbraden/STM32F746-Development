#include "EXTI.h"

void initLine9_5(uint8_t BUTTON_PIN) {
	
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	SYSCFG->EXTICR[(BUTTON_PIN/4)] &= ~(0xF << ((BUTTON_PIN % 4) * 4));
	SYSCFG->EXTICR[(BUTTON_PIN/4)] |=  (0x1 << ((BUTTON_PIN % 4) * 4));

	// Setup the button's EXTI line as an interrupt.
	EXTI->IMR  |=  (1 << BUTTON_PIN);
	// Disable the 'rising edge' trigger (button release).
	EXTI->RTSR &= ~(1 << BUTTON_PIN);
	// Enable the 'falling edge' trigger (button press).
	EXTI->FTSR |=  (1 << BUTTON_PIN);
	
	// Enable the NVIC interrupt for EXTI5 through EXTI9, inclusive (button on PB9)
	NVIC_SetPriority(EXTI9_5_IRQn, 0x03);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
}


