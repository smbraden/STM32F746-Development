/*
Description:

	A simple implementation of GPIO interrupts on STM32F746xx
	A button wired to pin 9 enables users LED1.
	
References:
		
	https://vivonomicon.com/2018/04/28/bare-metal-stm32-programming-part-4-intro-to-hardware-interrupts/
	https://stm32f4-discovery.net/2014/08/stm32f4-external-interrupts-tutorial/
	
*/


#include "stm32f746xx.h"

#include "pinDefines.h"
#include "GPIO.h"
#include "EXTI.h"

// Function prototypes
void EXTI4_IRQHandler(void);

// Global variables
volatile uint8_t led_on;	// LED status

int main(void) {
	// initialize global variables
	led_on = 0;

	// Enable the GPIOB peripheral
	RCC_GPIOB();
	
	// configure and enable bread board button
	initButton(BUTTON_B4, GPIOB);
	
	// configure and enable Nucleo user LED pins
	initLED(LED1_PIN, GPIOB);
	initLED(LED2_PIN, GPIOB);
	
	// Test Nucleo user LED 2
	GPIOB->ODR |= (1 << LED2_PIN);
	
	// configure and enable interrupt line
	// if different button pin used in code, EXTIx_IQRn line must be adjusted
	// (eg, EXTI3_IRQn, EXTI9_5_IRQn ...)
	initLine(BUTTON_B4, EXTI4_IRQn);
	
	while (1) {
		if (led_on) {
			GPIOB->ODR |= (1 << LED1_PIN);
		}
		else {
			GPIOB->ODR &= ~(1 << LED1_PIN);
		}
	}
}


void EXTI4_IRQHandler(void) {
	if (EXTI->PR & (1 << BUTTON_B4)) {
		// Clear the EXTI status flag.
		EXTI->PR |= (1 << BUTTON_B4);
		// Toggle the global 'led on?' variable.
		led_on = !led_on;
	}
}
