#include "stm32f0xx.h"
#include <math.h>
#include <stdint.h>
#define SAMPLES 30
uint16_t array[SAMPLES];

// Method 1, Timer 2
void setup_tim2(float fre){
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 0;
    TIM2->ARR = (48000000 / SAMPLES / fre ) - 1;
    TIM2->CR2 &= ~TIM_CR2_MMS;
    TIM2->CR2 |= TIM_CR2_MMS_1;
    TIM2->DIER |= TIM_DIER_UDE;
    TIM2->TIM_CR1_CEN;
}
void setup_dma(){
    RCC->AHBENR |= RCC_AHBENR_DMAEN;
    DMA1_Channel2->CCR &= ~DMA_CCR_EN;
    DMA1_Channel2->CMAR = array;
    DMA1_Channel2->CPAR = &DAC->DHR12R1;
    DMA1_Channel2->CNDTR = SAMPLES;
    DMA1_Channel2->CCR &= ~DMA_CCR_MSIZE;
    DMA1_Channel2->CCR &= ~DMA_CCR_PSIZE;
    DMA1_Channel2->CCR |= DMA_CCR_MSIZE_0;
    DMA1_Channel2->CCR |= DMA_CCR_PSIZE_0;
    DMA1_Channel2->CCR |= DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_CIRC;
    DMA1_Channel2->CCR | DMA_CCR_EN;

}

void setup_dac(){
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    DAC->CR &= ~DAC_CR_TSEL1;
    DAC->CR |= DAC_CR_TSEL1_2;
    DAC->CR |= DAC_CR_TEN1;
    DAC->CR |= DAC_CR_EN1;
    //DAC->CR |= DAC_CR_DMAEN1; uncomment if using method 2
}
void setfreq(float fre)
{
   // All of the code for this exercise will be written here.
   for(int x=0; x < SAMPLES; x += 1)
        array[x] = 2048 + 1952 * sin(2 * M_PI * x / SAMPLES);
    init_tim2(fre);
    setup_dma();
    setup_dac();
}

int main(void)
{
    // Uncomment any one of the following calls ...
    //setfreq(1920.81);
    //setfreq(1234.5);
    //setfreq(8529.48);
    //setfreq(11039.274);
    //setfreq(92816.14);
}
