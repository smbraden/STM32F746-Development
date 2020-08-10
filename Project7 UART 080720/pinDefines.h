#ifndef PINDEFINES_H
#define PINDEFINES_H

// on-board user button
#define BUTTON1 13	// PC13

// breadboard wired buttons
#define BUTTON2 9	// PB9
#define BUTTON3 4	// PB4

// On-board user LEDs
#define LED1_PIN 0	// PB0
#define LED2_PIN 7	// PB7
#define LED3_PIN 14 // PB14

// LED Row
#define E2 2
#define E3 3
#define E4 4
#define E5 5
#define E6 6
#define E7 7
#define E8 8
#define E9 9

// Analog Digital Converter
#define ADC_PIN 3 // PA3

// Universal Synchronous/Asynchronous Receiver/Transmitter
//#define TX4 0 // PA0 
//#define RX4 1 // PA1

// Use of PC10 and PC11 for UART4 would require remapping
#define TX4 10 // PC10 
#define RX4 11 // PC11


#endif
