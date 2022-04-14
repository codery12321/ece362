#include "stm32f0xx.h"


void init_usart5(){
	// USART1/6/7/8 clocks enabled on RCC_APB2ENR
	// USART2/3/4/5 clocks enabled on RCC_APB1ENR
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	RCC->AHBENR |= RCC_AHBENR_GPIODEN;
	//Bc datasheet
	// configure pin PC12 to be routed to USART5_TX.
	GPIOC->MODER |= GPIO_MODER_MODER12_1;
	GPIOC->AFR[1] |= 0x20000;
	// configure pin PD2 to be routed to USART5_RX.
	GPIOD->MODER |= GPIO_MODER_MODER2_1;
	GPIOD->AFR[0] |= 0x200;
	// Enable the RCC clock to the USART5 peripheral
	RCC->APB1ENR = RCC_APB1ENR_USART5EN;
	// disable it by turning off its UE bit
	USART5->CR1 &= ~USART_CR1_UE;
	// Set a word size of 8 bits.
	USART5->CR1 &= ~0x10000000;
	USART5->CR1 &= ~USART_CR1_M;
	// Set it for one stop bit.
	USART5->CR2 &= ~USART_CR2_STOP;
	// Set it for no parity.
	USART5->CR1 &= ~USART_CR1_PCE;
	// Use 16x oversampling.
	USART5->CR1 &= ~USART_CR1_OVER8;
	// Use a baud rate of 115200 (115.2 kbaud).
	USART5->BRR |= 0x1A1;
	// Enable the transmitter and the receiver by setting the TE and RE bits.
	USART5->CR1 |= USART_CR1_TE;
	USART5->CR1 |= USART_CR1_RE;
	//Enable the USART.
	USART5->CR1 |= USART_CR1_UE;
	while (!(USART5->ISR & USART_ISR_TEACK));
	while(!(USART5->ISR & USART_ISR_REACK));
}

//#define STEP21
#if defined(STEP21)
int main(void)
{
	init_usart5();
	for(;;) {
		while (!(USART5->ISR & USART_ISR_RXNE)) { }
		char c = USART5->RDR;
		while(!(USART5->ISR & USART_ISR_TXE)) { }
		USART5->TDR = c;
	}
}
#endif

//#define STEP22
#if defined(STEP22)
#include <stdio.h>
int __io_putchar(int c) {
    if(c == '\n'){
        while(!(USART5->ISR & USART_ISR_TXE)) { }
        USART5->TDR = '\r';
    }
    while(!(USART5->ISR & USART_ISR_TXE)) { }
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
     while (!(USART5->ISR & USART_ISR_RXNE)) { }
     char c = USART5->RDR;
     if(c == '\r'){
    	 c = '\n';
     }
     __io_putchar(c);
     return c;
}

int main() {
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

//#define STEP23
#if defined(STEP23)
#include <stdio.h>
#include "fifo.h"
#include "tty.h"

int __io_putchar(int c) {
    if(c == '\n'){
        while(!(USART5->ISR & USART_ISR_TXE)) { }
        USART5->TDR = '\r';
    }
    while(!(USART5->ISR & USART_ISR_TXE)) { }
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
	int val = line_buffer_getchar();
    return val;
}

int main() {
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

#define STEP24
#if defined(STEP24)
#include <stdio.h>
#include "fifo.h"
#include "tty.h"

#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0;

int __io_putchar(int c) {
    if(c == '\n'){
        while(!(USART5->ISR & USART_ISR_TXE)) { }
        USART5->TDR = '\r';
    }
    while(!(USART5->ISR & USART_ISR_TXE)) { }
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
	int val =  interrupt_getchar();
    return val;
}

int main() {
    init_usart5();
    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif
void enable_tty_interrupt(){
	NVIC->ISER[0] = 1<<USART3_8_IRQn;
	USART5->CR1 |= USART_CR1_RXNEIE;
	USART5->CR3 |= USART_CR3_DMAR;
	// enable the RCC clock for DMA Controller 2
    RCC->AHBENR |= RCC_AHBENR_DMA2EN;
    DMA2->RMPCR |= DMA_RMPCR2_CH2_USART5_RX;
    DMA2_Channel2->CCR &= ~DMA_CCR_EN;  // First make sure DMA is turned off
    DMA2_Channel2->CMAR = (uint32_t)serfifo;
    DMA2_Channel2->CPAR = (uint32_t)&USART5->RDR;
    DMA2_Channel2->CNDTR = FIFOSIZE;
    DMA2_Channel2->CCR &= ~0xffff;
    DMA2_Channel2->CCR |= 0x30a1;//011 0000 1010 0001
    		/*
    		bit 14: 0
    		bit 13:12: 11
    		bit 11:10: 00
			bit 9:8: 00
			bit 7: 1
			bit 6: 0
			bit 5: 1
			bit 4: 0
			bit 0: 1 */
    DMA2_Channel2->CCR |= DMA_CCR_EN;
}
int interrupt_getchar(){
    while(fifo_newline(&input_fifo) == 0) {
    	asm volatile ("wfi"); // wait for an interrupt
    }
    // Return a character from the line buffer.
    char ch = fifo_remove(&input_fifo);
    return ch;
}

void USART3_4_5_6_7_8_IRQHandler(void) {
	while(DMA2_Channel2->CNDTR != sizeof serfifo - seroffset) {
		if (!fifo_full(&input_fifo))
			insert_echo_char(serfifo[seroffset]);
		seroffset = (seroffset + 1) % sizeof serfifo;
	}
}
