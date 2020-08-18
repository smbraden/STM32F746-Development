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

#define ADC_MAX 4096		// 8-bit = 256, 9-bit = 512, 10-bit = 1024, 12-bit = 4096
#define ADC_BITS 9
#define ROW_Pos (2U)
#define ROW_Msk (0xFF << ROW_Pos)		// LED row bit mask


// Global variables
volatile uint32_t analogData = 0;		// 9-bit ADC
volatile uint32_t voltBars = 0;			// 3-bit ADC
volatile uint32_t msTicks = 0;			// store millisecond ticks


// Function prototypes
void ADC_IRQHandler(void);				// This example not best used of this interupt, just a test
void configEnableLEDS(void);
void configADC(void);
void test1(void);
void testLEDrow(void);
// void configAWD(void);
// [AWD-triggered interrupt prototype]

// SysTick function prototypes
void initSysTick(void);
void SysTick_Handler(void);
void delay_ms(uint32_t);


int main(void) {
	
	initSysTick();
	configEnableLEDS();
//	configAWD();
	configADC();
		
	while(1) {
		
	}
}


//-----------------ADC-------------------//

void configADC(void) {
	
	/*
		f_ADC = ADC clock frequency:
		VDDA = 1.7V to 2.4V	-----	Max: 18MHz
		VDDA = 2.4V to 3.6V -----	Max: 36MHz
	*/
	
	//Set prescalar for ADC clock (PCLK2), PCLK2 = APB2
	RCC->CFGR &= ~RCC_CFGR_PPRE2;
//	RCC->CFGR |= RCC_CFGR_PPRE2_2 | RCC_CFGR_PPRE2_0;
	
	/*
		0xx: AHB clock not divided
		100: AHB clock divided by 2
		101: AHB clock divided by 4
		110: AHB clock divided by 8
		111: AHB clock divided by 16
	*/	
	
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;		// enable peripheral clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	// using GPIO on PA0
	
	
	// set ADC Pin to analog mode
	GPIOA->OTYPER &= ~(0x1UL << ADC_PIN);
	GPIOA->PUPDR &= ~(0x3UL << (ADC_PIN * 2));
	GPIOA->OSPEEDR &= ~(0x3UL << (ADC_PIN * 2));
	GPIOA->MODER &= ~(0x3UL << (ADC_PIN * 2));
	GPIOA->MODER |= (0x3UL << (ADC_PIN * 2));
	
	// set L[3:0] to 0b0000 for 1 conversion
	ADC1->SQR1 &= ~ADC_SQR1_L;
	// Configure the first (and only) step in the sequence to read channel 10
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	ADC1->SQR3 |= (0xAUL << ADC_SQR3_SQ1_Pos);	// (ADC_SQR3_SQ1_3 | ADC_SQR3_SQ1_1);	|= (10 << 0); 

	// set the sampling rate for channel 10 to 480 cycles
	ADC1->SMPR1 &= ~ADC_SMPR1_SMP10;
	ADC1->SMPR1 |= ADC_SMPR1_SMP10_2;
	
	/*
		000: 3 cycles
		001: 15 cycles		ADC_SMPR1_SMP10_0
		010: 28 cycles		ADC_SMPR1_SMP10_1
		011: 56 cycles
		100: 84 cycles		ADC_SMPR1_SMP10_2
		101: 112 cycles
		110: 144 cycles
		111: 480 cycles		*/
	
	// enable End of Conversion interrupt
	ADC1->CR1 |= ADC_CR1_EOCIE;
	NVIC_SetPriority(ADC_IRQn, 0x0UL);
	NVIC_EnableIRQ(ADC_IRQn);
	
	
	// Continuous conversion mode
	ADC1->CR2 |= ADC_CR2_CONT;		// This bit is set and cleared by software. If set, 
									// conversion takes place continuously until it is cleared
									
	ADC1->CR2 &= ~ADC_CR1_SCAN;		// Scan mode off

	ADC1->CR2 |= (ADC_CR2_ADON);	// Power up the ADC. When the ADON bit is set for the first time, 
	delay_ms(1);					// it wakes up the ADC from the Power-down mode.
	
	
	//  Start conversion of regular channels
	ADC1->CR2 &= ~ADC_CR2_SWSTART;
	ADC1->CR2 |= ADC_CR2_SWSTART;	// This bit is set by software to start conversion
									// and cleared by hardware as soon as the conversion starts.
}


//---------ADC Interrupt Handler--------------//

// ADC status register (ADC_SR)
// EOC: Regular channel end of conversion
// This bit is set by hardware at the end of the conversion of a regular group of channels.
// It is cleared by software or by reading the ADC_DR register.

void ADC_IRQHandler(void) {
	
	// ADC1->SR &= ~ADC_SR_EOC;
	analogData = ADC1->DR;
	voltBars = (analogData >> (ADC_BITS - 3));				// 9-bit analog data, to 3-bit for 8 LEDs
	
	testLEDrow();
	test1();
	delay_ms(5);
	ADC1->CR2 |= ADC_CR2_SWSTART;							// Start a new conversion
}

//-----------------Tests-------------------//

void test1(void) {
	
	// on-board LED1 should turn on if recieving analog data
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

	GPIOE->BSRR &= ~ROW_Msk;						// light up x out of 8 "bars", representing voltage
	for (uint8_t i = 0 ; i <= voltBars ; i++) { 
		GPIOE->ODR |= (1 << i);
	}

}



//---------------- ADC Watchodg-----------------//

void configAWD(void) {
	
	// Configure ADC watchdog lower threshold to 0
	ADC1->LTR &= ~ADC_LTR_LT;
	
	// Configure ADC watchdog higher threahold to max potential bits, ADC_MAX
	ADC1->HTR &= ~ADC_HTR_HT;
	ADC1->HTR |= (ADC_MAX << ADC_HTR_HT_Pos);

	// Analog watchdog enable on regular channels
	ADC1->CR1 &= ~ADC_CR1_AWDEN;
	ADC1->CR1 |= ADC_CR1_AWDEN;

	// Interupts enable
	ADC1->CR1 &= ~ADC_CR1_AWDIE;
	ADC1->CR1 |= ADC_CR1_AWDIE;
	
//	set AWD-triggered interrupt... in progress
//	NVIC_SetPriority(AWD1_IRQn, 0x03UL);
//	NVIC_EnableIRQ(AWD1_IRQn);

}

//-----------ADC Watchodg Inerrupt------------//
/*

if(ADC1->SR & ADC_SR_AWD) {					// Analog watchdog, shut it down if danger zone
	
	// GPIOB->ODR |= (0x1UL << LED2_PIN);									
	ADC1->CR2 &= ~ADC_CR2_SWSTART;			// stop conversions
	ADC1->CR2 &= ~(ADC_CR2_ADON);			// power off the ADC

	ADC1->SR &= ~ADC_SR_AWD					// clear the interrupt flag

	GPIOB->ODR &= ~(0x1UL << (LED2_PIN | LED2_PIN | LED3_PIN));
	GPIOE->ODR &= ~(ROW_Msk);
	
	for (int i = 0; i < 8; i++) {			// flashing indicates conversions halted
		GPIOE->ODR ^= ROW_Msk;
		delay_ms(1000);
	}

}
*/


//-----------------LEDs------------------------//

void configEnableLEDS(void) {
	
	// enable GPIOE peripheral clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	// enable GPIOB peripheral clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	
	// Set the wired LED pins to push-pull low-speed output.
	for (uint8_t PINx = 2; PINx < 10; PINx++) {
		configLED(PINx, GPIOE);
	}
	// set the onboard LEDs likewise
	configLED(LED1_PIN, GPIOB);
	configLED(LED2_PIN, GPIOB);
	configLED(LED3_PIN, GPIOB);
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




// ADC's are "Additional functions," not "Alternate functions"
// Functions directly selected/enabled through peripheral registers
