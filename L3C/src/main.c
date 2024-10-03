/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 3C
 */
 
#include <stdio.h>
 
#include "stm32l476xx.h"

uint32_t volatile currentValue = 0;
uint32_t volatile lastValue = 0;
uint32_t volatile overflowCount = 0;
uint32_t volatile timeInterval = 0;
volatile float localDistanceCm;

void Input_Capture_Setup() {
// [TODO]
RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

GPIOB->MODER &= ~GPIO_MODER_MODE6 ;        // Clear mode bits for PB6
GPIOB->MODER |= GPIO_MODER_MODE6_1;         // Set alternate function mode for PB6
GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_1;  // Set alternate function to AF2 for TIM4

GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6); // No pull-up, no pull-down

RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;

TIM4->PSC = 15;

TIM4->CR1 |= TIM_CR1_ARPE; // Enable auto-reload preload
TIM4->ARR = 0xFFFF;        // Set auto-reload to maximum

TIM4->CCMR1 &= ~TIM_CCMR1_CC1S;      // Clear CC1S bits
TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;      // Set CC1S bits to 01 to select TI1 as the input

TIM4->CCER |= TIM_CCER_CC1E; // Enable capture for channel 1
TIM4->CCER |= TIM_CCER_CC1P | TIM_CCER_CC1NP; // Capture on both rising and falling edges

TIM4->DIER |= TIM_DIER_CC1IE; // Enable capture/compare interrupt for channel 1
TIM4->DIER |= TIM_DIER_UIE;   // Enable update interrupt

TIM4->DIER |= TIM_DIER_CC1DE; // Enable DMA request for capture/compare channel 1
TIM4->DIER |= TIM_DIER_UDE;   // Enable DMA request for update event

TIM4->EGR |= TIM_EGR_UG; // Generate an update event to update the registers

TIM4->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag

TIM4->CR1 &= ~TIM_CR1_DIR; // Set to count up
TIM4->CR1 |= TIM_CR1_CEN;  // Enable the counter

NVIC_EnableIRQ(TIM4_IRQn);      // Enable interrupt from TIM4
NVIC_SetPriority(TIM4_IRQn, 2); // Set priority to 2


}


void TIM4_IRQHandler(void) {
// [TODO]
 // Check if the update interrupt flag is set (indicates overflow)
    if ((TIM4->SR & TIM_SR_UIF) ==TIM_SR_UIF)
    {
        overflowCount++; // Increment the overflow counter
        TIM4->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag
    }

    // Check if the capture/compare interrupt flag is set
    if ((TIM4->SR & TIM_SR_CC1IF)== TIM_SR_CC1IF)
    {
        currentValue = TIM4->CCR1; // Read the current CCR value

        // Check if we have captured at least one rising edge before
        if (lastValue != 0)
        {
            // If an overflow has occurred, adjust the time interval calculation
            if (overflowCount > 0)
            {
                timeInterval = (0xFFFF - lastValue) + currentValue + (overflowCount - 1) * 0x10000;
                overflowCount = 0; // Reset the overflow counter
            }
            else
            {
                timeInterval = currentValue - lastValue; // Calculate the time interval
            }
        }

        lastValue = currentValue; // Update the lastValue with the current capture

        TIM4->SR &= ~TIM_SR_CC1IF; // Clear the capture/compare interrupt flag
    }

}

void Trigger_Setup() {
// [TODO]
RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

GPIOA->MODER &= ~(GPIO_MODER_MODE9);
GPIOA->MODER |= GPIO_MODER_MODE9_1;
GPIOA->AFR[1] |= GPIO_AFRH_AFSEL9_0;

GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD9); // No pull-up, no pull-down

GPIOA->OTYPER &= ~(GPIO_OTYPER_OT9); // Set to push-pull (reset state)

GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED9); // Very high speed

RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

TIM1->PSC = 15;

TIM1->CR1 |= TIM_CR1_ARPE; // Enable auto-reload preload
TIM1->ARR = 0xFFFF; // Set auto-reload to maximum

TIM1->CCR2 = 10;

TIM1->CCMR1 |= TIM_CCMR1_OC2PE; // Enable output compare preload for channel 2

TIM1->CCMR1 &= ~(TIM_CCMR1_OC2M);
TIM1->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2; // PWM mode 1 for channel 2

TIM1->CCER |= TIM_CCER_CC2E; // Enable the output for channel 2

TIM1->BDTR |= TIM_BDTR_MOE | TIM_BDTR_OSSR | TIM_BDTR_OSSI;

TIM1->EGR |= TIM_EGR_UG; // Enable update generation

TIM1->DIER |= TIM_DIER_UIE; // Enable update interrupt
TIM1->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag

TIM1->CR1 &= ~TIM_CR1_DIR; // Clear DIR bit for upcounting (if needed)
TIM1->CR1 |= TIM_CR1_CEN; // Enable the counter

}

int main(void) {

// Enable High Speed Internal Clock (HSI = 16 MHz)
RCC->CR |= RCC_CR_HSION;
while ((RCC->CR & RCC_CR_HSIRDY) == 0); // Wait until HSI is ready

// Select HSI as system clock source
RCC->CFGR &= ~RCC_CFGR_SW;
RCC->CFGR |= RCC_CFGR_SW_HSI;
while ((RCC->CFGR & RCC_CFGR_SWS) == 0); // Wait until HSI is system clock source
 
// Input Capture Setup
Input_Capture_Setup();

// Trigger Setup
Trigger_Setup();


while(1) {
// [TODO] Store your measurements on Stack
/*
if (timeInterval > 0) {
            // Convert the timeInterval to a distance in centimeters using the formula provided
            float distanceCm = timeInterval / 58.0f;

            // If the pulse width corresponds to no object in range, set the distance to 0
           if (timeInterval >= 38000) { // Assuming 38 ms is equivalent to 38000 us
                distanceCm = 0.0f;
            }

            // Assign the distance to a local variable to store on the stack
            localDistanceCm = distanceCm;

            // Reset timeInterval for the next measurement
            timeInterval = 0;
        } else {
            // If no new measurement is ready, set the value to 0
           localDistanceCm = 0.0f;
        }
*/


		if (localDistanceCm> 49){
				localDistanceCm = 0;
		}
		else {
			localDistanceCm = timeInterval/58.0f;
		}
	}
}
