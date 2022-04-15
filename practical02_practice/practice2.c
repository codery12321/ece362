//method 1
void init_timer2(){
    RCC->APB1ENR_TIM2EN |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 0;
    TIM2->ARR = (48000000 / SAMPLES / fre) - 1;
    TIM2->CR2 &= ~TIM_CR2_MMS;
    TIM2->CR2 |= TIM_CR2_MMS_1;
    TIM2->DIER |= TIM_DIER_UDE;
    TIM2->CR1 |= TIM_CR1_CEN;
}

void init_dma(){
    RCC->AHBENR |= RCC_AHBENR_DMAEN;
    DMA1_Channel2->CCR &= ~DMA_CCR_EN;
    DMA1_Channel2->CMAR = array;
    DMA1_Channel2->CPAR = &DAC->DHR12R1;
    DMA1_Channel2->CNDTR = SAMPLES;
    DMA1_Channel2->CCR &= ~DMA_CCR_MSIZE;
    DMA1_Channel2->CCR &= ~DMA_CCR_PSIZE;
    DMA1_Channel2->CCR |= DMA_CCR_MSIZE_0;
    DMA1_Channel2->CCr |= DMA_CCR_PSIZE_0;
    DMA1_Channle2->CCR |= DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_CIRC;
    DMA1_Channel2->CCR |= DMA_CCR_EN;

}

void init_dac(){
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    DAC->CR &= ~DAC_CR_TSEL1;
    DAC->CR |= DAC_CR_TSEL1_2;
    DAC->CR |= DAC_CR_TEN1;
    DAC->CR |= DAC_CR_EN1;
    //DAC->CR |= DAC_CR_DMAEN;
}
