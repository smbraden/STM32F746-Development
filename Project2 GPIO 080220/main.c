#include "stm32f746xx.h"

//#define BUTTON_NUCLEO 13
#define BUTTON_BB 9
#define LED1_PIN 0
#define LED2_PIN 7
#define LED3_PIN 14

// return 1 for high, 0 for low input
#define InputPinStatus(GPIOx, GPIO_Pin) ((~(GPIOx->IDR) & (1 << GPIO_Pin)) == 0 ? 0 : 1)

void RCC_GPIOB(void);
void initLED(uint8_t, GPIO_TypeDef*);
void initButton(uint8_t, GPIO_TypeDef*);

int main(void) {
	
	// Enable clock for 
	RCC_GPIOB();
	
	// Configure PB9 for input with pull-up resistors
	// PB9 wired to a button with deboincing circuit on breadboard
	initButton(BUTTON_BB, GPIOB);
	
	// enable all Nucleo user LED pins, configured for output
	initLED(LED1_PIN, GPIOB);
	initLED(LED2_PIN, GPIOB);
	initLED(LED3_PIN, GPIOB);
		
	// Test Nucleo user LEDs 2 and 3
	GPIOB->ODR |= (1 << LED2_PIN);
	GPIOB->ODR |= (1 << LED3_PIN);
		
	while(1) {
		if (InputPinStatus(GPIOB, BUTTON_BB)) {		// The button is pressed;
			GPIOB->ODR |= (0x1UL << LED1_PIN);
		}
		else {
			GPIOB->ODR &= ~(0x1UL << LED1_PIN);
		}
	}
	
}

void RCC_GPIOB() {
	
	// Enable the GPIOB peripheral
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
}

void initLED(uint8_t LED_PIN, GPIO_TypeDef* GPIOx) {
	
	// Set LED pin to push-pull low-speed output.
	GPIOx->MODER  &= ~(0x3UL << (LED_PIN*2));	// input mode (reset state)
	GPIOx->MODER  |=  (0x1UL << (LED_PIN*2));	// output mode
	GPIOx->OTYPER &= ~(0x1UL << LED_PIN);			// Push-pull output
}

void initButton(uint8_t BUTTON_PIN, GPIO_TypeDef* GPIOx) {

	//Initialize the GPIOC pins.
	//PC13 should be set to 'input' mode with pull-up.
	GPIOx->MODER &= ~(0x3UL << (BUTTON_PIN*2));	// input mode (reset state)
	GPIOx->PUPDR &= ~(0x3UL << (BUTTON_PIN*2));	// no pull-up or pull-down (reset)
	GPIOx->PUPDR |= (0x1UL << (BUTTON_PIN*2));	// pull-up activated
	
}

