/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "init_periph.h"
#include "stm32f0xx.h"
#include "7-segment.h"
#include "midi.h"
#include "midiplay.h"
#include "game.h"

// Actual Implementation
#define NOT_TESTING
#if defined(NOT_TESTING)
extern int started;
void EXTI4_15_IRQHandler ()
{
	if (started == 0)
	{
		TIM16 -> CR1 &= ~(TIM_CR1_CEN); // disable start screen
//	    TIM6->CR1 |= TIM_CR1_CEN;
//	    TIM15->CR1 |= TIM_CR1_CEN;
	    //DAC -> CR |= DAC_CR_EN1;

		//setup();
		score = 0;
		setup_score(0);

		//reset_tim15();
		start();
		init_audio();

	}
	if ((EXTI -> PR & EXTI_PR_PR8) != 0)
	{
		//GPIOC -> BSRR |= GPIO_BSRR_BS_9;
		small_delay(4000000);
	    moveUp();
	}
	else if ((EXTI -> PR & EXTI_PR_PR9) != 0)
	{
		// INSERT FUNCTION TO UPDATE DISPLAY AFTER "DOWN" BUTTON HAS BEEN PRESSED
		//GPIOC -> BSRR |= GPIO_BSRR_BS_8;
		small_delay(3000000);
	    moveDown();
	}
    // Acknowledge Interrupt
	 EXTI -> PR |= EXTI_PR_PR8 | EXTI_PR_PR9;


}
#endif

void endscreen()
{
	TIM6->CR1 &= ~TIM_CR1_CEN;
	TIM15->CR1 &= ~TIM_CR1_CEN;
	timer16_init();
}

void reset()
{
	//SCB->AIRCR = 0x05fa0004;
	NVIC_SystemReset();
}

void reset_screen()
{
	GPIOB -> BSRR |= 0xffff0000;
}

void setup_porta()
{
	RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA -> MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9); // input
	GPIOA -> PUPDR &= ~(GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR9); // set up pull down resistor
	GPIOA -> PUPDR |= GPIO_PUPDR_PUPDR8_1 | GPIO_PUPDR_PUPDR9_1;

}

void enable_exti()
{
    // Enable System clock
    RCC -> APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;

    // Set up PC8 & PC9 interrupt source
    SYSCFG -> EXTICR[2] &= ~(SYSCFG_EXTICR3_EXTI8 | SYSCFG_EXTICR3_EXTI9);

    // Configure EXTI_RTSR to trigger on rising edge of PC8 & PC9
    EXTI -> RTSR |= EXTI_RTSR_TR8 | EXTI_RTSR_TR9;
    //EXTI -> FTSR  |= EXTI_FTSR_TR8 | EXTI_FTSR_TR9;
    // EXTI_IMR to not ignore pin number 8 & 9
    EXTI -> IMR |= EXTI_IMR_MR8 | EXTI_IMR_MR9;

    // NVIC enable interrupt
    NVIC -> ISER[0] |= 1 << EXTI4_15_IRQn;
}

void test(){
	// Test by lighting up microcontroller LEDs
	RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
	// Pressing Button 5 (Up PB should turn on blue LED)
	GPIOC -> MODER &= ~(0xc0000);
	GPIOC -> MODER |= 0x40000;
	GPIOC -> OSPEEDR &= ~(0xc0000);
	GPIOC -> OSPEEDR |= 0x40000;
	// Pressing Button 6 (Down PB should turn on green LED)
	GPIOC -> MODER &= ~(0x30000);
	GPIOC -> MODER |= 0x10000;
	GPIOC -> OSPEEDR |= 0x30000;

}

void init_buttons()
{
	//test();
	setup_porta();
	enable_exti();
}


//#define TESTING
#if defined(TESTING)
// Testing Module
int main(void)
{
	RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
	// Pressing Button 5 (Up PB should turn on blue LED)
    GPIOC -> MODER &= ~(0xc0000);
    GPIOC -> MODER |= 0x40000;
    GPIOC -> OSPEEDR &= ~(0xc0000);
    GPIOC -> OSPEEDR |= 0x40000;
    // Pressing Button 6 (Down PB should turn on green LED)
    GPIOC -> MODER &= ~(0x30000);
    GPIOC -> MODER |= 0x10000;
    GPIOC -> OSPEEDR |= 0x30000;

	init_buttons();
}


#endif


