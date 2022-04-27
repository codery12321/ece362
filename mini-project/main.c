

#include "stm32f0xx.h"
#include <math.h>
#include <stdint.h>
#include "game.h"
#include "init_periph.h"
#include "7-segment.h"

extern int firsttime;


int row;

void timer16_init()
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;

    TIM16->ARR = 400-1;
    TIM16->PSC = 60-1;
    TIM16->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] |= 1<<TIM16_IRQn;
    NVIC -> IP[5] |= (0x4f << 8);
    TIM16->CR1 |= TIM_CR1_CEN;
}

void TIM16_IRQHandler()
{
    TIM16->SR &= ~1;
    int rgb[3];
    for(int col = 0; col < 64; col++){
        if(scr[row][col] == wall){ // green
            rgb[0] = 0;
            rgb[1] = 1;
            rgb[2] = 0;
        }else if(scr[row][col] == playerBeak){ //red
            rgb[0] = 1;
            rgb[1] = 0;
            rgb[2] = 0;
        }else if(scr[row][col] == playerBody){ // yellow
            rgb[0] = 1;
            rgb[1] = 1;
            rgb[2] = 0;
        }else{ // black
            rgb[0] = 0;
            rgb[1] = 0;
            rgb[2] = 0;
        }

        GPIOB->BRR |= 7 << 4;
        GPIOB->BRR |= 7 << 8;

        if(row > 15){
            GPIOB->BSRR |= rgb[0] << 8;
            GPIOB->BSRR |= rgb[1] << 9;
            GPIOB->BSRR |= rgb[2] << 10;
        }else{
            GPIOB->BSRR |= rgb[0] << 4;
            GPIOB->BSRR |= rgb[1] << 5;
            GPIOB->BSRR |= rgb[2] << 6;
        }
        GPIOB->BSRR |= 1<<7;
        GPIOB->BRR |= 1<<7;

    }

    GPIOB->BSRR |= 1<<12;

    GPIOB->BRR |= 15;
    GPIOB->BSRR |= row%16;

    GPIOB->BSRR |= 1<<11;
    GPIOB->BRR |= 1<<11;

    GPIOB->BRR |= 1<<12;

    row = (row+1)%32;
}


void init_gpiob(){
    //Enable GPIOB RCC
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
//    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
//    GPIOC->MODER |= GPIO_MODER_MODER9_0;

    //Configure MODER set PB0~PB12 as output
    GPIOB->MODER |= GPIO_MODER_MODER0_0;
    GPIOB->MODER |= GPIO_MODER_MODER1_0;
    GPIOB->MODER |= GPIO_MODER_MODER2_0;
    GPIOB->MODER |= GPIO_MODER_MODER3_0;
    GPIOB->MODER |= GPIO_MODER_MODER4_0;
    GPIOB->MODER |= GPIO_MODER_MODER5_0;
    GPIOB->MODER |= GPIO_MODER_MODER6_0;
    GPIOB->MODER |= GPIO_MODER_MODER7_0;
    GPIOB->MODER |= GPIO_MODER_MODER8_0;
    GPIOB->MODER |= GPIO_MODER_MODER9_0;
    GPIOB->MODER |= GPIO_MODER_MODER10_0;
    GPIOB->MODER |= GPIO_MODER_MODER11_0;
    GPIOB->MODER |= GPIO_MODER_MODER12_0;
}
void timer6_init(){
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

    TIM6->ARR = 400-1;
    TIM6->PSC = 60-1;
    TIM6->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] |= 1<<TIM6_DAC_IRQn;
    NVIC -> IP[4] |= (0x4f << 8);
    TIM6->CR1 |= TIM_CR1_CEN;
}

void TIM6_DAC_IRQHandler(){
    TIM6->SR &= ~1;
    int rgb[3];
    for(int col = 0; col < 64; col++){
        if(dispMap[row][col] == wall){ // green
            rgb[0] = 0;
            rgb[1] = 1;
            rgb[2] = 0;
        }else if(dispMap[row][col] == playerBeak){ //red
            rgb[0] = 1;
            rgb[1] = 0;
            rgb[2] = 0;
        }else if(dispMap[row][col] == playerBody){ // yellow
            rgb[0] = 1;
            rgb[1] = 1;
            rgb[2] = 0;
        }else{ // black
            rgb[0] = 0;
            rgb[1] = 0;
            rgb[2] = 0;
        }

        GPIOB->BRR |= 7 << 4;
        GPIOB->BRR |= 7 << 8;

        if(row > 15){
            GPIOB->BSRR |= rgb[0] << 8;
            GPIOB->BSRR |= rgb[1] << 9;
            GPIOB->BSRR |= rgb[2] << 10;
        }else{
            GPIOB->BSRR |= rgb[0] << 4;
            GPIOB->BSRR |= rgb[1] << 5;
            GPIOB->BSRR |= rgb[2] << 6;
        }
        GPIOB->BSRR |= 1<<7;
        GPIOB->BRR |= 1<<7;

    }

    GPIOB->BSRR |= 1<<12;

    GPIOB->BRR |= 15;
    GPIOB->BSRR |= row%16;

    GPIOB->BSRR |= 1<<11;
    GPIOB->BRR |= 1<<11;

    GPIOB->BRR |= 1<<12;

    row = (row+1)%32;

}

void init_tim15(){
    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    TIM15->PSC = 12000-1;
    TIM15->ARR = 1000-1;
    TIM15->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] |= 1<<TIM15_IRQn;
    //NVIC_SetPriority(TIM15_IRQn, 3);
    NVIC -> IP[5] |= (0xff);//
    TIM15->CR1 |= TIM_CR1_CEN;
}

void TIM15_IRQHandler(){
    TIM15->SR &= ~1;
    if(started){
        update();
        int speed = score * speedMultiplier;
        if(speed > speedMax){
            speed = speedMax;
        }
        TIM15->PSC = 12000-speed;
    }
}

void reset_tim15()
{
	TIM15 -> CR1 &= ~(TIM_CR1_CEN);
	TIM15 -> PSC = 12000 - 1;
	TIM15-> CR1 |= TIM_CR1_CEN;
}

void init_gpio(){
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER &= ~0xc0000;
    GPIOC->MODER |= 0x40000;
}

/*void timer14_init(){
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
    TIM14->ARR = 800-1;
    TIM14->PSC = 800-1;
    TIM14->DIER |= TIM_DIER_UIE;
    NVIC->ISER[0] |= 1<<TIM14_IRQn;
    TIM14->CR1 |= TIM_CR1_CEN;
}

void TIM14_IRQHandler(){
    TIM14->SR &= ~1;
    drive_bb();
}*/

void setup()
{
	init_gpio();
	    init_tim15();

		row = 0;

	    score = 0;
	    gap = 14;
	    started = 0;
	    gapSize = 8;
	    speedMultiplier = 800;
	    speedMax = 8000;
	    playerMovement = 2;

	    space = gap;

	    player[0][0] = 16;
	    player[0][1] = 4;
	    player[1][0] = 16;
	    player[1][1] = 3;
	    player[2][0] = 16;
	    player[2][1] = 2;
	    player[3][0] = 17;
	    player[3][1] = 3;
	    player[4][0] = 17;
	    player[4][1] = 2;

	    white = ' ';
	    playerBeak = '>';
	    playerBody = 'n';
	    wall = '#';
	    wallGap = '|';
	    open = '.';
	    init_gpiob();
		replace_startscr(0);//change screen
		timer16_init();//show it
	    timer6_init();

	    init_buttons();
	//    while(firsttime == 0){
	//
	//    }
	//    TIM16->CR1 &= ~TIM_CR1_CEN; //disable start screen





}
int main(void){
	/*
    init_gpio();
    init_tim15();

	row = 0;

    score = 0;
    gap = 14;
    started = 0;
    gapSize = 8;
    speedMultiplier = 800;
    speedMax = 8000;
    playerMovement = 2;

    space = gap;

    player[0][0] = 16;
    player[0][1] = 4;
    player[1][0] = 16;
    player[1][1] = 3;
    player[2][0] = 16;
    player[2][1] = 2;
    player[3][0] = 17;
    player[3][1] = 3;
    player[4][0] = 17;
    player[4][1] = 2;

    white = ' ';
    playerBeak = '>';
    playerBody = 'n';
    wall = '#';
    wallGap = '|';
    open = '.';
    init_gpiob();
//	replace_startscr(firsttime);//change screen
//	timer16_init();//show it
    timer6_init();

    init_buttons();
//    while(firsttime == 0){
//
//    }
//    TIM16->CR1 &= ~TIM_CR1_CEN; //disable start screen






    //timer14_init();
//    for (;;)
//    {
//        loop();
////        drive_bb();
//    }
 *
 */
	setup();
    return 0;
}
