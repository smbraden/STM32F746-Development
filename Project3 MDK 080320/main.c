#include "stm32f746xx.h"

#include "pinDefines.h"
#include "GPIO.h"
#include "EXTI.h"

// Function prototypes
void EXTI9_5_IRQ_handler(uint8_t);

// Global variables
volatile uint8_t led_on;

int main(void) {
	// initialize global variables
	led_on = 0;

	RCC_GPIOB();
	// configure and enable bread board button 
	initButton(BUTTON_BB, GPIOB);
	// configure and enable Nucleo user LED pin
	initLED(LED1_PIN, GPIOB);
	initLED(LED2_PIN, GPIOB);
	
	// Test Nucleo user LEDs 2 and 3
	GPIOB->ODR |= (1 << LED2_PIN);
	
	// configure and enable interrupt
	initLine9_5(BUTTON_BB);
	
	while (1) {
		if (led_on) {
			GPIOB->ODR |= (1 << LED1_PIN);
		}
		else {
			GPIOB->ODR &= ~(1 << LED1_PIN);
		}
	}
}

void EXTI9_5_IRQ_handler(uint8_t BUTTON_PIN) {
	if (EXTI->PR & (1 << BUTTON_PIN)) {
		// Clear the EXTI status flag.
		EXTI->PR |= (1 << BUTTON_PIN);
		// Toggle the global 'led on?' variable.
		led_on = !led_on;
	}
}
