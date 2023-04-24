/*
 * Jackson Newman
 */
 
#include <stdio.h> 
 
#include "stm32l476xx.h"

uint32_t volatile currentValue = 0;
uint32_t volatile lastValue = 0;
uint32_t volatile overflowCount = 0;
uint32_t volatile timeInterval = 0;
uint32_t volatile count = 0;
uint32_t volatile distance = 0;

void Input_Capture_Setup() {
	// [TODO]
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	
	GPIOB->MODER &= ~GPIO_MODER_MODE6;
	GPIOB->MODER |= GPIO_MODER_MODE6_1;
	
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL6;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_1;
	
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD6;
	
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
	
	TIM4->PSC &= ~TIM_PSC_PSC;
	TIM4->PSC = 15;

	TIM4->CR1 |= TIM_CR1_ARPE;
	TIM4->ARR &= ~TIM_ARR_ARR;
	TIM4->ARR |= TIM_ARR_ARR;
	
	TIM4->CCMR1 &= ~TIM_CCMR1_CC1S;
	TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;
	
	TIM4->CCER |= TIM_CCER_CC1NP;
	TIM4->CCER |= TIM_CCER_CC1P;
	TIM4->CCER |= TIM_CCER_CC1NE;
	TIM4->CCER |= TIM_CCER_CC1E;
	
	TIM4->DIER |= TIM_DIER_CC1IE;
	TIM4->DIER |= TIM_DIER_CC1DE;
	TIM4->DIER |= TIM_DIER_UIE;
	
	TIM4->EGR |= TIM_EGR_UG;
	
	TIM4->SR &= ~TIM_SR_UIF;
	
	TIM4->CR1 &= ~TIM_CR1_DIR;
	TIM4->CR1 |= TIM_CR1_CEN;
	
	NVIC_EnableIRQ(TIM4_IRQn);
	NVIC_SetPriority(TIM4_IRQn, 2);
}

void TIM4_IRQHandler(void) {
	// [TODO]
	if((TIM4 -> SR) & TIM_SR_CC1IF) {
		count++;
		if((GPIOB -> IDR) & GPIO_IDR_ID6) {
			lastValue = (TIM4 -> CCR1) & TIM_CCR1_CCR1;
			overflowCount = 0;
		}
		else {
			currentValue = (TIM4 -> CCR1) & TIM_CCR1_CCR1;
			timeInterval = currentValue + (65535*overflowCount) - lastValue;
		}
	}
	if((TIM4 -> SR) & TIM_SR_UIF) {
		overflowCount++;
		TIM4 -> SR &= ~TIM_SR_UIF;
	}
}

void Trigger_Setup() {
	// [TODO]
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	GPIOA->MODER &= ~GPIO_MODER_MODE9;
	GPIOA->MODER |= GPIO_MODER_MODE9_1;
	
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL9;
	GPIOA->AFR[1] |= GPIO_AFRH_AFSEL9_0;
	
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD9;
	
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT9;
	
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9;
	
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	TIM1->PSC &= ~TIM_PSC_PSC;
	TIM1->PSC = 15;
	
	TIM1->CR1 |= TIM_CR1_ARPE;
	
	TIM1->ARR &= ~TIM_ARR_ARR;
	TIM1->ARR |= TIM_ARR_ARR;
	
	TIM1->CCR2 &= ~TIM_CCR2_CCR2;
	TIM1->CCR2 = 10;
	
	TIM1->CCMR1 &= ~TIM_CCMR1_OC2M;
	TIM1->CCMR1 |= (TIM_CCMR1_OC2M_1) | (TIM_CCMR1_OC2M_2);
	TIM1->CCMR1 |= TIM_CCMR1_OC2PE;
	
	TIM1->CCER |= TIM_CCER_CC2E;
	
	TIM1->BDTR |= (TIM_BDTR_MOE) | (TIM_BDTR_OSSR);
	
	TIM1->EGR |= TIM_EGR_UG;
	
	TIM1->DIER |= TIM_DIER_UIE;
	TIM1->SR &= ~TIM_SR_UIF;
	
	TIM1->CR1 &= ~TIM_CR1_DIR;
	TIM1->CR1 |= TIM_CR1_CEN;
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

		if (timeInterval / 58 > 400) {
			distance = 0;
		} else {
			distance = timeInterval / 58;
		}
	}
}
