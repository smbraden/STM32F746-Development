/*
Description:	Basic implementation of the Nucleo's Analog to Digital Converter
				An LED display wired to the E port will be utilized 
				to represent the voltage across a photoresistor relative to the supply (3.3V)
				
Author:			Sonja Braden
	
IDE:			Keil uVision 5

uVision 
Dependencies:	CMSIS Core, STM32F746xx Startup files
*/

#include "stm32f746xx.h"
#include "GPIO.h"
#include "GPT.h"			// For  void initSysTick(), SysTick_Handler(), delay_ms(uint32_t);
#include "pinDefines.h"		// For configLED(LEDx, GPIOx), OutputHigh(GPIOx, PINx)

// Global variables
volatile uint32_t msTicks = 0;			// store millisecond ticks

// Function prototypes
void initSysTick(void);
void SysTick_Handler(void);
void delay_ms(uint32_t);


int main(void) {
	
	// enable GPIOB peripheral clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	// configure the GPIO
	configLED(LED2_PIN, GPIOB);
	configLED(LED1_PIN, GPIOB);
	
	initSysTick();

	while (1) {
		
		// just test the new delay function
		Toggle(GPIOB, LED2_PIN);
		delay_ms(1000);			
	}
}


// Configures SysTick to generate 1 ms interrupts
void initSysTick(void) {
	
	// 1 ms interrupts
	uint32_t returnStatus = SysTick_Config(SystemCoreClock / 1000);	// generating 16,000 interupts per second

	/*
	
	// Check return code for errors
	if (returnStatus != 0)  {   
		GPIOB->ODR ^= (0x1UL << LED1_PIN); 
	}

	*/
	
}

// SysTick interrupt Handler
// Will only response to its interrupt if initSysTick() is called first 
void SysTick_Handler(void)  {
	msTicks++;
}


// Can only be called if initSysTick() is called first
void delay_ms(uint32_t delayTime) {
	
	uint32_t curTicks;
	curTicks = msTicks;
	while ((msTicks - curTicks) < delayTime) {}
}




