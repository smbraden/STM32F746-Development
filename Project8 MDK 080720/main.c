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
#include "pinDefines.h"

// Macro defines

// Global variables
volatile uint32_t msTicks = 0;			// store millisecond ticks
volatile uint32_t time_ms = 1000;

// Function prototypes
void SysTick_Handler(void);
void initSysTick(void);
void delay_ms(void);

int main(void) {
	
	// enable GPIOB peripheral clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	// set the onboard LED likewise
	configLED(LED1_PIN, GPIOB);
	configLED(LED2_PIN, GPIOB);

	initSysTick();

	while (1) {
		
		// just test the new delay function
		GPIOB->ODR |= (0x1UL << LED2_PIN);
		delay_ms();
		
	}
}


void SysTick_Handler(void)  {                               /* SysTick interrupt Handler. */
	msTicks = (msTicks == time_ms)? 0 : (msTicks + 1); 
}

void initSysTick(void) {
	
	uint32_t returnStatus;
	returnStatus = SysTick_Config(SystemCoreClock / 1000);	// Configure SysTick to generate an interrupt every millisecond */

	//if (returnStatus != 0)  {Error Handling}   // Check return code for errors 
}

void delay_ms(void) {
	
	while (msTicks <= time_ms) {}
	msTicks = 0;
}
