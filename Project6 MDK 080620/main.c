/*
Description:	Basic implementation of the Nucleo's Analog to Digital Converter functionallity
				An LED display wired to the E port will be utilized 
				to display the relative magnitude of the varying voltage input
				as produced by a photoresistor
				
Author:			Sonja Braden

Reference:		
	
IDE:			Keil uVision 5

uVision 
Dependencies:	CMSIS Core, STM32F746xx Startup files
*/

#include "stm32f746xx.h"
#include "GPIO.h"
#include "GPT.h"
#include "pinDefines.h"

// Macro defines
//#define GPIOx GPIOE


// Global variables
uint32_t ROW_MASK = (0xFF << 2);		// LED row bit mask
uint32_t analogData = 0;


// Function prototypes
void ADC_IRQHandler(void);


int main(void) {
	
	// enable GPIOE peripheral clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;
	
	// Set the wired LED pins to push-pull low-speed output.
	for (uint8_t PINx = 2; PINx < 10; PINx++) {
		configLED(PINx, GPIOE);
	}
	
	// Enable clock for ADC
	/*	f_ADC = ADC clock frequency:
		VDDA = 1.7V to 2.4V	-----	Max: 18MHz
		VDDA = 2.4V to 3.6V -----	Max: 36MHz
		
		In the clear with 16MHz		*/
	
	// enablePeriphClock(AHB2ENR, RCC_APB2ENR_ADC1EN)
	RCC->AHB2ENR |= RCC_APB2ENR_ADC1EN;
	//RCC->CCIPR   &= ~( RCC_CCIPR_ADCSEL );
	//RCC->CCIPR   |=  ( 3 << RCC_CCIPR_ADCSEL_Pos );
	
	// enable End of Conversion interrupt in NVIC
	ADC1->CR1 |= ADC_CR1_EOCIE;
	NVIC_EnableIRQ(ADC_IRQn);
	
	
	// set the sampling rate for channel 10 to 480 cycles
	ADC1->SMPR1 &= ~ADC_SMPR1_SMP10;
	ADC1->SMPR1 |= ADC_SMPR1_SMP10;
	
	/*
		000: 3 cycles
		001: 15 cycles
		010: 28 cycles
		011: 56 cycles
		100: 84 cycles
		101: 112 cycles
		110: 144 cycles
		111: 480 cycles		*/

	ADC1->SQR1 &= ~ADC_SQR1_L;
	// Configure the first (and only) step in the sequence to read channel 10
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	ADC1->SQR3 |= (ADC_SQR3_SQ1_Pos | ADC_SQR3_SQ1_Pos);	//	|= (10 << 0); 

	// enable the ADC and using continuous mode
	ADC1->CR2 |= (ADC_CR2_ADON | ADC_CR2_CONT);
	
	// main event loop
	while (1) {
		uint8_t bars = analogData/512;		// (2^12)/8 = 512
		for (uint8_t i = 0 ; i <= bars ; i++) { 
			GPIOE->ODR |= (1 << i);
		}
		GPIOE->ODR &= ~ROW_MASK;
	}
}


void ADC_IRQHandler(void) {
	
	if(ADC1->SR & ADC_SR_EOC) {
		analogData = ADC1->DR;
	}
}

/*
void delay_ms(int time_ms) {
	msTicks = 0;
	while(msTicks < time_ms) {}
}
*/