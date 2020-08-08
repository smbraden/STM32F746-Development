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
#include "GPT.h"
#include "pinDefines.h"

// Macro defines
//#define GPIOx GPIOE


// Global variables
uint32_t ROW_MASK = (0xFF << 2);		// LED row bit mask
volatile uint32_t analogData = 0;
volatile uint32_t msTicks = 0;			// store millisecond ticks


// Function prototypes
void ADC_IRQHandler(void);


int main(void) {
	
	/*	f_ADC = ADC clock frequency:
		VDDA = 1.7V to 2.4V	-----	Max: 18MHz
		VDDA = 2.4V to 3.6V -----	Max: 36MHz
		
		In the clear with default 16MHz	*/
	
	// As a precaution, set prescalar to halve the ADCCLK,
	// generated from the APB2 clock
	RCC->CFGR &= ~RCC_CFGR_PPRE2;
	RCC->CFGR |= RCC_CFGR_PPRE2_2;
	
	// enable GPIOE peripheral clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	// enable GPIOB peripheral clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	// Set the wired LED pins to push-pull low-speed output.
	for (uint8_t PINx = 2; PINx < 10; PINx++) {
		configLED(PINx, GPIOE);
	}
	// set the onboard LED likewise
	configLED(LED1_PIN, GPIOB);
	configLED(LED2_PIN, GPIOB);
	
	
	// enable peripheral clock
	RCC->AHB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	
	// set pin to analog mode
	GPIOA->OTYPER &= ~(0x1UL << ADC_PIN);
	GPIOA->PUPDR &= ~(0x3UL << (ADC_PIN * 2));
	GPIOA->OSPEEDR &= ~(0x3UL << (ADC_PIN * 2));
	GPIOA->MODER &= ~(0x3UL << (ADC_PIN * 2));
	GPIOA->MODER |= (0x3UL << (ADC_PIN * 2));
	
	// ADC's are "Additional functions," not "Alternate functions"
	// Functions directly selected/enabled through peripheral registers
	
	
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
	ADC1->SQR3 |= (10 >> ADC_SQR3_SQ1_Pos);	// (ADC_SQR3_SQ1_3 | ADC_SQR3_SQ1_1);	|= (10 << 0); 

	// enable the ADC and using continuous mode
	ADC1->CR2 |= (ADC_CR2_ADON | ADC_CR2_CONT);
	
	// For single conversion mode, where the ADC does one conversion
	// The conversion starts when either the SWSTART or the JSWSTART bit is set.
	// ADC1->CR2 |= ADC_CR2_SWSTART;
	
	
	// Test GPIOB
	GPIOB->ODR |= (0x1UL << LED2_PIN);
	// main event loop
	while (1) {
		uint32_t voltBars = analogData/512;		// 12-bit analog data, 8 LEDs
											// (2^12)/8 = 512	
		
		// light up x out of 8 "bars", which represent
		// voltage across photoresistor relative to Vcc
		for (uint8_t i = 0 ; i <= voltBars ; i++) { 
			GPIOE->ODR |= (1 << i);
		}
		GPIOE->ODR &= ~ROW_MASK;
		
		// For debugging:
		// on-board LED1 should turn on if recieving analog data
		if (analogData > 0) {
			GPIOB->ODR |= (0x1UL << LED1_PIN);
		}
		else {
			GPIOB->ODR &= ~(0x1UL << LED1_PIN);
		}
		
		GPIOB->ODR |= (0x1UL << LED2_PIN);
		delay_ms(1000);
		// just test the new dely function
	}
}


void ADC_IRQHandler(void) {
	
	if(ADC1->SR & ADC_SR_EOC) {
		analogData = ADC1->DR;
	}
}

