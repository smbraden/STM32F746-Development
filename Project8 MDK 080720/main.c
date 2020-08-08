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

// Type defines
typedef enum {
	msec = 1000,
	usec = 1000000,
} order;


// Global variables
static volatile uint32_t msTicks = 0;			// store millisecond ticks


// Function prototypes
void SysTick_Handler(void);
void initSysTick(order);
void delay(uint32_t);
 

int main(void) {
	
	// enable GPIOB peripheral clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	// configure the GPIO
	configLED(LED2_PIN, GPIOB);
	configLED(LED1_PIN, GPIOB);
	
	initSysTick(msec);

	while (1) {
		
		// just test the new delay function
		GPIOB->ODR ^= (0x1UL << LED2_PIN);
		delay(1000);			
	}
}




void SysTick_Handler(void)  {                               /* SysTick interrupt Handler. */
	msTicks++;
}

void initSysTick(order Divisor) {
	
	// Configure SysTick to generate 1ms interrupts
	uint32_t returnStatus = SysTick_Config(SystemCoreClock / Divisor);	

	// Check return code for errors
	if (returnStatus != 0)  {   
		GPIOB->ODR ^= (0x1UL << LED1_PIN); 
	}							
}

void delay(uint32_t delayTime) {
	
	uint32_t curTicks;
	curTicks = msTicks;
	while ((msTicks - curTicks) < delayTime) {}
}

