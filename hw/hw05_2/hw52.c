#include "stm32f0xx.h"
//===========================================================
//Q1: recur [1 point]
//===========================================================
unsigned int recur(unsigned int x) {
        if (x < 3)
                return x;
        if ((x & 0xf) == 0)
                return 1 + recur(x - 1);
        return recur(x >> 1) + 2;
}
//===========================================================
//Q2: enable_portb [1 point]
//===========================================================
void enable_portb(void){
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN
}
//===========================================================
//Q3: enable_portc [1 point]
//===========================================================
void enable_portb(void){
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN
}
//===========================================================
//Q4: setup_pb3 [1 point]
//===========================================================
void setup_pb3(void){
    GPIOB->MODER &= ~0xc0
    GPIOB->PUPDR &= ~0xc0
    GPIOB->PUPDR |= 0x80
}
//===========================================================
//Q5: setup_pb4 [1 point]
//===========================================================
void setup_pb4(void){
    GPIOB->MODER &= ~0x300
    GPIOB->PUPDR &= ~0x300
}
//===========================================================
//Q6: setup_pc8 [1 point]
//===========================================================
void setup_pc8(void){
    GPIOC->MODER &= ~0x30000
    GPIOC->MODER |= 0x10000
    GPIOC->OSPEEDR |= 0x30000
}
//===========================================================
//Q7: setup_pc9 [1 point]
//===========================================================
void setup_pc9(void){
    GPIOC->MODER &= ~0xc0000
    GPIOC->MODER |= 0x40000
    GPIOC->OSPEEDR &= ~0xc0000
    GPIOC->OSPEEDR |= 0x40000
}
//===========================================================
//Q8: action8 [1 point]
//===========================================================
void action8(void){
    GPIOB->IDR &= 0x18
    if (GPIOB->IDR == 0x8){
        GPIOC->BSRR |= 0x1000000
    }
    else{
        GPIOC->BSRR |= 0x100
    }
}
//===========================================================
//Q9: action9 [1 point]
//===========================================================
void action9(void){
    GPIOB->IDR &= 0x18
    if (GPIOB->IDR == 0x10){
        GPIOC->BSRR |= 0x200
    }
    else{
        GPIOC->BSRR |= 0x4000000
    }
}
//===========================================================
//Q10: External Interrupt Handler [1 point]
//===========================================================
void EXTI2_3_IRQHandler(void){
    EXTI->PR |= 1<<2
    extern int counter += 1
}
//===========================================================
//Q11: enable_exti [1 point]
//===========================================================
void enable_exti(void){
    RCC->APB2ENR |= SYSCFGEN
    SYSCFG->EXTICR1 &= ~0xf00
    SYSCFG->EXTICR1 |= 0x100
    EXTI->RTSR |= 0x4
    EXTI->IMR |= 0x4
    NVIC->ISER = 1<<6
}
//===========================================================
//Q12: (the interrupt handler for Timer 3) [1 point]
//===========================================================
void TIM3_IRQHandler(void){
    TIM3->SR &= ~TIM_SR_UIF
    GPIOC->ODR &= 1<<9
    if(GPIOC->ODR != 0){
        GPIOC->BRR = 1<<9
    }
    else{
        GPIOC->BSRR = 1<<9
    }
}
//===========================================================
//Q13: enable_tim3 [1 point]
//===========================================================
void enable_tim3(void){
    RCC->APB1ENR |= TIM3EN
    TIM3->PSC = 48000-1
    TIM3->ARR = 250-1
    TIM3->DIER |= 1<<0
    NVIC->ISER[0] = 1<<16
    TIM3->CR1 |= 1<<0
}
