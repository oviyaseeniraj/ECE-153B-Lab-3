/*
 * ECE 153B
 *
 * Name(s): Oviya Seeniraj, Lily Chen
 * Section: Tues 7 pm
 * Lab: 2B
 */

#include "LED.h"

void LED_Init(void) {
	// Enable GPIO Clocks
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Enable Clock for GPIO Port A (LED)
	
	// Initialize Green LED
	GPIOA->MODER &= ~(GPIO_MODER_MODE5); // Clear MODER5[1:0] for PA5
  GPIOA->MODER |= (GPIO_MODER_MODE5_0);  // Set PA5 as General purpose output mode
  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT5);      // Set PA5 to push-pull
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD5); // No pull-up, no pull-down
}

void Green_LED_Off(void) {
	GPIOA->ODR &= ~GPIO_ODR_OD5; // Turn on the LED, 5 b/c pin 5 maps there
}

void Green_LED_On(void) {
	GPIOA->ODR |= GPIO_ODR_OD5; // Turn off the LED, 5 b/c pin 5 maps there
}

void Green_LED_Toggle(void){
	GPIOA->ODR ^= GPIO_ODR_OD5; // Toggle = XOR w itself
}
