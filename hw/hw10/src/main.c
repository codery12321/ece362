#include "stm32f0xx.h"
#include <math.h>
#include <stdint.h>
#define SAMPLES 30
uint16_t array[SAMPLES];

void enable_ports(){
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= 0x300; //11 0000 0000
}
void init_timer2(float fre){
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->PSC = 0;
	TIM2->ARR = (48000000/(SAMPLES)/ fre) - 1;
	TIM2->CR2 &= ~TIM_CR2_MMS;
	TIM2->CR2 |= TIM_CR2_MMS_1; //enable TRGO to update: 010
	//TIM2->DIER |= TIM_DIER_UDE; uncomment for method 1
	TIM2->CR1 |= TIM_CR1_CEN;	//enable the timer
}
void setup_dma(void){
	RCC->AHBENR |= RCC_AHBENR_DMAEN;
	DMA1_Channel3->CCR &= ~DMA_CCR_EN;
	DMA1_Channel3->CMAR = (uint32_t)array;
	DMA1_Channel3->CPAR = (uint32_t)&DAC->DHR12R1;
	DMA1_Channel3->CNDTR = SAMPLES;
	DMA1_Channel3->CCR &= ~DMA_CCR_MSIZE;
	DMA1_Channel3->CCR &= ~DMA_CCR_PSIZE;
	DMA1_Channel3->CCR |= DMA_CCR_MSIZE_0;		//set msize to 16 bit
	DMA1_Channel3->CCR |= DMA_CCR_PSIZE_0;
	DMA1_Channel3->CCR |= DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_CIRC; //set direction transfer from mem to peripheral, memory address incremented after every tansfer, set for circular operation
	DMA1_Channel3->CCR |= DMA_CCR_EN;	//enable the DMA channel

}
/*
void setup_dma(void){
	RCC->AHBENR |= RCC_AHBENR_DMAEN;
	DMA1_Channel2 -> CCR &= ~DMA_CCR_EN;
	DMA1_Channel2->CMAR = (uint32_t)array;
	DMA1_Channel2->CPAR = (uint32_t)&DAC->DHR12R1;
	DMA1_Channel2->CNDTR = SAMPLES;
	DMA1_Channel2->CCR &= ~DMA_CCR_MSIZE;
	DMA1_Channel2->CCR &= ~DMA_CCR_PSIZE;
	DMA1_Channel2->CCR |= DMA_CCR_MSIZE_0;		//set msize to 16 bit
	DMA1_Channel2->CCR |= DMA_CCR_PSIZE_0;
	DMA1_Channel2->CCR |= DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_CIRC; //set direction transfer from mem to peripheral, memory address incremented after every tansfer, set for circular operation
	DMA1_Channel2->CCR |= DMA_CCR_EN;	//enable the DMA channel

}*/
void init_dac(void){
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;	//enable RCC clock for DAC
	DAC->CR &= ~DAC_CR_TSEL1;			//set TSEL1 for DAC to be triggered by the timer
	DAC->CR |= DAC_CR_TSEL1_2;
	DAC->CR |= DAC_CR_TEN1;				//enable the trigger for channel 1
	DAC->CR |= DAC_CR_EN1;				//enable channel 1
	DAC->CR |= DAC_CR_DMAEN1;
}

void setfreq(float fre)
{
   // All of the code for this exercise will be written here.
    for(int x=0; x < SAMPLES; x += 1)
        array[x] = 2048 + 1952 * sin(2 * M_PI * x / SAMPLES);
	//enable_ports();
	init_timer2(fre);
	setup_dma();
	init_dac();

}

int main(void)
{

    // Uncomment any one of the following calls ...
    setfreq(1920.81);
    //setfreq(1234.5);
    //setfreq(8529.48);
    //setfreq(11039.274);
    //setfreq(92816.14);

}
