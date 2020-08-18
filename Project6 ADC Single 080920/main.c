/*
Description:	Basic implementation of the Nucleo's Analog to Digital Converter
				An LED display wired to the E port is utilized to represent 
				the voltage across the photoresistor element of a voltage divider
				relative to the supply (3.3V)
				
Author:			Sonja Braden
	
IDE:			Keil uVision 5

uVision 
Dependencies:	CMSIS Core, STM32F746xx Startup files
*/

#include "stm32f746xx.h"
#include "GPIO.h"
#include "pinDefines.h"


#define ADC_MAX 4096					// 256	| 512	| 1024	| 2048	| 4096
#define ADC_BITS 12					// 8	| 9		| 10	| 11	| 12


// Global variables
uint32_t ROW_MASK = (0xFF << 2);		// LED row bit mask
volatile uint32_t analogData = 0;		// 12-bit ADC data
volatile uint8_t voltBars = 0;			// 8-bit ADC data
volatile uint32_t msTicks = 0;			// store millisecond ticks


// Function prototypes
void configAWD(void);
void configEnableLEDS(void);
void configADC(void);
void test1(void);
void testLEDrow(void);


// SysTick function prototypes
void initSysTick(void);
void SysTick_Handler(void);
void delay_ms(uint32_t);


int main(void) {
	
	configEnableLEDS();						// Configure and enable LEDs
	//configAWD();
	configADC();							// Configure ADC
	
	ADC1->CR2 &= ~ADC_CR2_ADON;				// ADC in power down mode (reset)
	ADC1->CR2 |= ADC_CR2_ADON;				// Enable the ADC
	delay_ms(5);
	
	while (1) {
			
		
		ADC1->CR2 &= ~ADC_CR2_SWSTART;
		ADC1->CR2 |= ADC_CR2_SWSTART;		// For single conversion mode, conversion starts when SWSTART bit is set.

		//if(ADC1->SR & ADC_SR_AWD)			// Shut it down if voltage in danger zone
		//	break;							
		
		while(!(ADC1->SR & ADC_SR_EOC)){}	// wait for conversion
		analogData = ADC1->DR;				// get converted data		
		
		test1(); 
		testLEDrow(); 	
		delay_ms(10);
	}	
}

//------------------Testing---------------------//

void test1(void) {
	
	// The on-board LED that turns on indicates which range the reading is in
	if (analogData > 0 && analogData < (ADC_MAX/3)) {
		GPIOB->ODR |= (0x1UL << LED1_PIN);
		GPIOB->ODR &= ~(0x1UL << (LED2_PIN | LED3_PIN));
	}
	else if (analogData >= (ADC_MAX/3) && analogData < (2 * ADC_MAX/3)) {
		GPIOB->ODR |= (0x1UL << LED2_PIN);
		GPIOB->ODR &= ~(0x1UL << (LED1_PIN | LED3_PIN));
	}
	else if (analogData >= (2 * ADC_MAX/3) && analogData < (3 * ADC_MAX/3)) {
		GPIOB->ODR |= (0x1UL << LED3_PIN);
		GPIOB->ODR &= ~(0x1UL << (LED1_PIN | LED2_PIN));
	}
}


void testLEDrow(void) {
	
	// x-bit analog data shifted to 3-bit for 8 LEDs
	voltBars = (analogData >> (ADC_BITS - 3));
	
	// light up x out of 8 "bars", represent voltage across photoresistor relative to Vcc
	GPIOE->ODR &= ~ROW_MASK;
	for (uint8_t i = 0 ; i <= voltBars ; i++) { 
		GPIOE->ODR |= (1 << i);
	}
}


//-----------------Configure ADC-------------------//

void configADC(void) {
	
	
	/*	f_ADC = ADC clock frequency:
	VDDA = 1.7V to 2.4V	-----	Max: 18MHz
	VDDA = 2.4V to 3.6V -----	Max: 36MHz
	
	In the clear with default 16MHz	*/
	
	// This would set prescalar to halve the ADCCLK:
	// RCC->CFGR &= ~RCC_CFGR_PPRE2;
	// RCC->CFGR |= RCC_CFGR_PPRE2_2;
	
	
	// enable peripheral clock
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	// using ADC on PA0
		
	ADC1->CR2 &= ~ADC_CR2_CONT;					// single, not continuous mode
	ADC1->CR1 &= ~ADC_CR1_SCAN;					// Single channel, not scan mode
	
	// set ADC GPIO Pin to analog mode
	GPIOA->PUPDR &= ~(0x3UL << (ADC_PIN * 2));
	GPIOA->MODER &= ~(0x3UL << (ADC_PIN * 2));
	GPIOA->MODER |= (0x3UL << (ADC_PIN * 2));
	GPIOA->OTYPER &= ~(0x1UL << ADC_PIN);
	GPIOA->OSPEEDR &= ~(0x3UL << (ADC_PIN * 2));

	
	// set L[3:0] to 0b0000 for 1 conversion
	ADC1->SQR1 &= ~ADC_SQR1_L;
	// Configure the first (and only) step in the sequence to read channel 10
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;				// Reset
	ADC1->SQR3 |= (0xAUL << ADC_SQR3_SQ1_Pos);	// |= (ADC_SQR3_SQ1_3 | ADC_SQR3_SQ1_1);	|= (10 << 0); 

	// set the sampling rate for channel 10 to 480 cycles
	ADC1->SMPR1 &= ~ADC_SMPR1_SMP10;
	ADC1->SMPR1 |= ADC_SMPR1_SMP10; 			// 144 cycles: ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_1;
	
	/*
		000: 3 cycles
		001: 15 cycles
		010: 28 cycles
		011: 56 cycles
		100: 84 cycles
		101: 112 cycles
		110: 144 cycles
		111: 480 cycles		*/
		

}


//---------------- ADC Watchodg-----------------//

void configAWD(void) {
	
	// Analog watchdog channel select bits
	// These bits are set and cleared by software. 
	// They select the input channel to be guarded by the analog watchdog.
	ADC1->CR1 &= ~ADC_CR1_AWDCH;
	ADC1->CR1 |= (10 << ADC_CR1_AWDCH_Pos);
	
	// Configure ADC watchdog lower threshold to 0
	ADC1->LTR &= ~ADC_LTR_LT;
	
	// Configure ADC watchdog higher threahold to 3.6V
	ADC1->HTR &= ~ADC_HTR_HT;
	ADC1->HTR |= (ADC_MAX << ADC_HTR_HT_Pos); 	// 9-bit

	// Analog watchdog enable on regular channels
	ADC1->CR1 &= ~ADC_CR1_AWDEN;
	ADC1->CR1 |= ADC_CR1_AWDEN;

	// Analog watchdog interrupt enable
	// ADC1->CR1 |= ADC_CR1_AWDIE;
}

//-----------------LEDs------------------------//

void configEnableLEDS(void) {
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;			// enable GPIOE peripheral clock
	for (uint8_t PINx = 2; PINx < 10; PINx++) {		// Set the wired LED pins to push-pull low-speed output.
		configLED(PINx, GPIOE);
	}
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;			// enable GPIOB peripheral clock
	configLED(LED1_PIN, GPIOB);						// set the onboard LED likewise
	configLED(LED2_PIN, GPIOB);						// set the onboard LED likewise
	configLED(LED3_PIN, GPIOB);						// set the onboard LED likewise
}


//---------SysTick Functions------------------//

// Configures SysTick to generate 1 ms interrupts
void initSysTick(void) {
	
	// 1 interrupt per millisecond
	SysTick_Config(SystemCoreClock / 1000);	
	// generating 1 interupt per (SystemCoreClock / 1000) 'ticks'

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



	/* 
		Discontinuous mode used to convert a short sequence 
		of n conversions (n = 8) that is part of the sequence of
		conversions selected in the ADC_SQRx registers. 
		
		ADC1->CR1 |= ADC_CR1_DISCEN;
		ADC1->CR1 &= ~ADC_CR1_DISCNUM; 
	*/
