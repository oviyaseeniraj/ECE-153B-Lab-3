/*
 * ECE 153B
 * 
 * Name(s): Oviya Seeniraj, Lily Chen
 * Section: Tues 7 pm
 * Lab: 3B
 */

#include "stm32l476xx.h"

void LED_Pin_Init(void);
void TIM2_CH1_Init(void);
void SERVO_Pin_Init(void);
void TIM5_CH1_Init(void);

void LED_Pin_Init(void){
  // Enable GPIO Clocks
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // Enable Clock for GPIO Port A (LED)
	
	// Initialize Green LED
	GPIOA->MODER &= ~(GPIO_MODER_MODE5); // Clear MODER5[1:0] for PA5
  GPIOA->MODER |= (GPIO_MODER_MODE5_1);  // Set PA5 as Alternative Function (10 -> 8)
	
	//GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL5_0;

	//Set I/O output speed value as very high speed
	//GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR5;
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5_0;
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5_1;
	
	// GPIO Push-Pull: No pull-up, no pull-down
	//GPIOA->OTYPER &= ~(GPIO_OTYPER_OT5);      // Set PA5 to push-pull
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD5); // No pull-up, no pull-down
}
	

void TIM2_CH1_Init(void){
	// Enable the timer clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	
	// Counting direction: 0 = up-counting, 1 = down-counting
	TIM2->CR1 &= ~TIM_CR1_DIR;
	
  // Prescaler
	TIM2->PSC |= 100; //set prescaler to 100;
	
  // Auto-reload
	TIM2->ARR &= ~TIM_ARR_ARR;
	TIM2->ARR |= 999; // set ARR to 999
	
	// Disable output compare mode
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M_1;
	
	// PWM mode 1
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	TIM2->CCMR1 &= ~TIM_CCMR1_OC1M_3;
	
	// Output 1 preload enable
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE; // output channel 1 preload enable
	
	// Select output polarity: active high
	TIM2->CCER &= ~TIM_CCER_CC1P;
	
  // Enable output for ch1
	TIM2->CCER |= TIM_CCER_CC1E;
	
	// Output Compare Register for channel 1 
	TIM1->CCR1 |= 500; //500 so duty cycle is 50%
	
	// Enable counter
	TIM2->CR1 |= TIM_CR1_CEN;
}

void SERVO_Pin_Init(void){
	//RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	// Set mode as Alternative Function 1
	GPIOA->MODER &= ~(GPIO_MODER_MODE0); // Clear MODER5[1:0] for PA0
  GPIOA->MODER |= (GPIO_MODER_MODE0_1);
	
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL0;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL0_1;
	
	
	//Set I/O output speed value as very high speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED0_0;
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED0_1;
	
	//Set I/O as no pull-up pull-down 
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD0);
}

void TIM5_CH1_Init(void){
	// Enable the timer clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
	// Counting direction: 0 = up-counting, 1 = down-counting
	TIM5->CR1 &= ~TIM_CR1_DIR;
	
  // Prescaler
	TIM5->PSC |= 39; //set prescaler to 100;
	
  // Auto-reload
	TIM5->ARR &= ~TIM_ARR_ARR;
	TIM5->ARR |= 2000; // set ARR to 999
	
	
	// Disable output compare mode
	TIM5->CCMR1 &= ~TIM_CCMR1_OC1M_1;
	
	// PWM mode 1
	TIM5->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	TIM5->CCMR1 &= ~TIM_CCMR1_OC1M_3;
	
	// Output 1 preload enable
	TIM5->CCMR1 |= TIM_CCMR1_OC1PE;
		
	// Select output polarity: active high
	TIM2->CCER &= ~TIM_CCER_CC1P;
		
  // Enable output for ch1
	TIM5->CCER |= TIM_CCER_CC1E;
		
	// Output Compare Register for channel 1 
	TIM5->CCR1 |= 1000;

	// Enable counter
	TIM5->CR1 |= TIM_CR1_CEN;
}

int main(void) {
	//int i;
	int led = 0;
	int dire = 1;
	int servo_position = 0;
	
	LED_Pin_Init();
	SERVO_Pin_Init();
	TIM2_CH1_Init();
	TIM5_CH1_Init();
		
    while(1) {

      // Update servo position based on LED brightness
      /*
			servo_position = 2000 - led * 2; // Map LED brightness to servo position
      if (servo_position < 0) {
        servo_position = 0;
      }
			else if (servo_position > 0) {
				servo_position = 2000;
      }
			*/
      //TIM5->CCR1 = servo_position;

      // Update LED brightness for next iteration
      
			led += dire;
			servo_position+=dire;
			
      if (led >= 2000 || led <= 0) {
        dire = -dire;
			}
			
			TIM2->CCR1 = led;
			TIM5->CCR1 = servo_position;

			// Delay
      for (int i = 0; i < 300; i++);
	}
}
