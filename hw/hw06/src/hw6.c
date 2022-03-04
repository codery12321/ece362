#include "stm32f0xx.h"
#include <string.h>

const char font[] = {
        [' '] = 0x00,
        ['0'] = 0x3f,
        ['1'] = 0x06,
        ['2'] = 0x5b,
        ['3'] = 0x4f,
        ['4'] = 0x66,
        ['5'] = 0x6d,
        ['6'] = 0x7d,
        ['7'] = 0x07,
        ['8'] = 0x7f,
        ['9'] = 0x67,
        ['A'] = 0x77,
        ['B'] = 0x7c,
        ['C'] = 0x39,
        ['D'] = 0x5e,
        ['*'] = 0x49,
        ['#'] = 0x76,
        ['.'] = 0x80,
        ['?'] = 0x53,
        ['b'] = 0x7c,
        ['r'] = 0x50,
        ['g'] = 0x6f,
        ['i'] = 0x10,
        ['n'] = 0x54,
        ['u'] = 0x1c,
};

//uint16_t digit[8];
uint16_t digit[8*4];
//declare global variable
int hrs = 12;
int min = 06;
int sec = 30;
int eighth;

void set_digit(int n, char c)
{
    digit[n] = (n<<8) | font[c];
}

void set_string(const char *s)
{
    for(int n=0; s[n] != '\0'; n++)
        set_digit(n,s[n]);
}
//===================================================================
// Step 1 Use DMA instead
//===================================================================
void setup_dma(void){
	//Enable the RCC clock to the DMA controller
	RCC->AHBENR |= 0x1;    //bit 0 of AHBENR
	//disable DMA to configure the channel
	DMA1_Channel2->CCR &= ~DMA_CCR_EN;
	//Set CMAR to the address of the beginning of the digit array
    DMA1_Channel2->CMAR &= ~(uint32_t) digit;
	DMA1_Channel2->CMAR = (uint32_t) digit;
	//Set CPAR to the address of the Port B ODR
    DMA1_Channel2->CPAR &= ~(uint32_t) (&(GPIOB->ODR));
	DMA1_Channel2->CPAR = (uint32_t) (&(GPIOB->ODR));
	//Set CNDTR to 8 to indicate there are 8 things to transfer
	//DMA1_Channel2->CNDTR = 8;
	//STEP 3
	DMA1_Channel2->CNDTR = 8*4;
//in the CCR register
    //Set the DIR bit to indicate from the "memory" address to the "peripheral" address. 	Bit 4: 1
    //Set the memory datum size (in the MSIZE field) to 16 bits. 							Bit 11:10: 01
    //Set the peripheral datum size (in the PSIZE field) to 16 bits.						Bit 9:8: 01
    //Set the MINC bit so that the memory address is incremented by 2 after each transfer.	Bit 7: 1
    //Set the CIRC bit so that, once all 8 transfers complete, the entire transaction is restarted, over and over again. Bit 5: 1
    //Last: Set the EN bit to enable the DMA channel. 										Bit 0: 1
    DMA1_Channel2->CCR &= ~0xfb1;               //1111 1011 0001
    DMA1_Channel2->CCR |= 0x5b1;			//0101 1011 0001
}
void init_tim2(void){
	RCC->APB1ENR |= 0x1;		//bit 0 of APB1ENR
	TIM2->PSC = 4800-1;			//48Mhz/4800-1 = (10-1) * (1000)
	TIM2->ARR = 10-1;
	TIM2->DIER |= TIM_DIER_UDE;
	TIM2->CR1 |= TIM_CR1_CEN;
}

//===================================================================
// Step 2 Create a clock
//===================================================================
void TIM7_IRQHandler(void){
	// Acknowledge the interrupt here
	TIM7->SR &= ~TIM_SR_UIF;

    eighth += 1;
    if (eighth >= 8) { eighth -= 8; sec += 1; }
    if (sec >= 60)   { sec -= 60;   min += 1; }
    if (min >= 60)   { min -= 60;   hrs += 1; }
    if (hrs >= 24)   { hrs -= 24; }
    char time[8];
    sprintf(time, "%02d%02d%02d  ", hrs, min, sec);
    set_string(time);

    //STEP 3
    if (eighth > 0 && eighth < 4) {
        memcpy(&digit[8*eighth], digit, 2*8);
    }
}

int main(void){
	/*
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER |= 0x155555;
    set_string("running.");
	*/
	setup_dma();
	init_tim2();
    //enable the RCC clock to the selected timer
    RCC->APB1ENR |= 0x20;     	//TIM7EN bit 5 of RCC_APB1ENR
    //configure it to have an update event 8 times per second.
    TIM7->PSC = 48000-1;        //48Mhz/(48000-1) = (125-1)*(8)
    TIM7->ARR = 125-1;
    // display loop
    for(;;) {
        for(int x=0; x < 8; x++) {
            GPIOB->ODR = digit[x];
            for(int n=0; n < 100; n++);
        }
    }
}
