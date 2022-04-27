#include "stm32f0xx.h"
#include "7-segment.h"
#include <math.h>


//===========================================================================
// 2.1 Bit Bang SPI LED Array
//===========================================================================
int msg_index = 0;
uint16_t msg[8] = { 0x0000,0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700};
const char font[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x00, // 32: space
    0x86, // 33: exclamation
    0x22, // 34: double quote
    0x76, // 35: octothorpe
    0x00, // dollar
    0x00, // percent
    0x00, // ampersand
    0x20, // 39: single quote
    0x39, // 40: open paren
    0x0f, // 41: close paren
    0x49, // 42: asterisk
    0x00, // plus
    0x10, // 44: comma
    0x40, // 45: minus
    0x80, // 46: period
    0x00, // slash
    // digits
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67,
    // seven unknown
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    // Uppercase
    0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x6f, 0x76, 0x30, 0x1e, 0x00, 0x38, 0x00,
    0x37, 0x3f, 0x73, 0x7b, 0x31, 0x6d, 0x78, 0x3e, 0x00, 0x00, 0x00, 0x6e, 0x00,
    0x39, // 91: open square bracket
    0x00, // backslash
    0x0f, // 93: close square bracket
    0x00, // circumflex
    0x08, // 95: underscore
    0x20, // 96: backquote
    // Lowercase
    0x5f, 0x7c, 0x58, 0x5e, 0x79, 0x71, 0x6f, 0x74, 0x10, 0x0e, 0x00, 0x30, 0x00,
    0x54, 0x5c, 0x73, 0x7b, 0x50, 0x6d, 0x78, 0x1c, 0x00, 0x00, 0x00, 0x6e, 0x00
};

//===========================================================================
// Configure PB12 (NSS), PB13 (SCK), and PB15 (MOSI) for outputs
//===========================================================================
// NSS (PB12) -> PC0
// SCK (PB13) -> PC1
// MOSI (PB15) -> PC2
// PA15 SPI1_NSS, PA5 SP1_SCK, PA7 SPI1_MOSI
/*void setup_bb(void)
{
	RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
	// set up PB12, 13, 15 for output
	GPIOA -> MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER7 | GPIO_MODER_MODER15);
	GPIOA -> MODER |= (GPIO_MODER_MODER5_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER15_0);

	// initialize odr so that NSS high (PB12) and SCK low (PB13)
	GPIOA -> BSRR |= GPIO_BSRR_BS_15;
	GPIOA -> BSRR |= GPIO_BSRR_BR_5;
}
*/
void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void small_delay(int del) {
    nano_wait(del);
}

//===========================================================================
// Set the MOSI bit, then set the clock high and low.
// Pause between doing these steps with small_delay().
//===========================================================================
/*void bb_write_bit(int val)
{
    // NSS (PB12) -> PC0
    // SCK (PB13) -> PC1
    // MOSI (PB15) -> PC2
    // PB15 -> PC2, PB13 -> PC1, PB12 -> PC0
	// set PB15 low if val zero
	// PA15 SPI1_NSS, PA5 SP1_SCK, PA7 SPI1_MOSI
	if (val == 0)
	{
		GPIOA -> BSRR |= GPIO_BSRR_BR_7;
	}
	// otherwise set to high
	else
	{
		GPIOA -> BSRR |= GPIO_BSRR_BS_7;
	}

	// delay
	small_delay();

	// Set SCK (PC1) high
	GPIOA -> BSRR |= GPIO_BSRR_BS_5;

	// delay
	small_delay();

	// Set SCK(PC1) low
	GPIOA -> ODR &= ~(GPIO_ODR_5);

}

//===========================================================================
// Set NSS (PC0) low,
// write 16 bits using bb_write_bit,
// then set NSS high.
//===========================================================================
void bb_write_halfword(int halfword)
{
	// set PC0 low
	GPIOA -> ODR &= ~(GPIO_ODR_15);

	// call bb_write_bit to write bit 15 to 0 of the argument
	for (int i = 15; i >= 0; i--)
	{
		int bit = (halfword >> i) & 0x1;
		bb_write_bit(bit);
	}

	// set PC0 high
	GPIOA -> ODR |= GPIO_ODR_15;
}

//===========================================================================
// Continually bitbang the msg[] array.
//===========================================================================
void drive_bb(void) {

	for(int d=0; d<8; d++) {
		bb_write_halfword(msg[d]);
		nano_wait(100000); // wait 1 ms between digits
	}
}
*/
//============================================================================
// setup_dma()
// Copy this from lab 6 or lab 7.
// Write to SPI1->DR instead of GPIOB->ODR.
//============================================================================
void setup_dma(void)
{
	// Enable RCC Clock to the DMA controller
	RCC -> AHBENR |= RCC_AHBENR_DMA1EN;

	// Turn off enable bit for the channel 3
	DMA1_Channel3 -> CCR &= ~(0x1);

	// Set CPAR to SPI1_DR
	DMA1_Channel3 -> CPAR = (uint32_t) &SPI1->DR;

	// Set CMAR to the msg array
	DMA1_Channel3 -> CMAR = (uint32_t) msg;

	// Set CNDTR to 8
	DMA1_Channel3 -> CNDTR = 8;

	DMA1_Channel3 -> CCR |= DMA_CCR_PL_1 | DMA_CCR_PL_0;

	// Set DIR for mem -> per bit 4 1
	// Set MINC to increment CMAR for every transfer bit 7 1
	// Memory Datum = 16-bit 11:10 01
	// Periph Datum = 16-bit 9:8 01
	// CIRC operation 5 1
	DMA1_Channel3 -> CCR &= ~(0xf00);
	DMA1_Channel3 -> CCR |= 0x5b0;
}

//============================================================================
// enable_dma()
// Copy this from lab 6 or lab 7.
//============================================================================
void enable_dma(void)
{
	// Enable the Channel
	DMA1_Channel3 -> CCR |= DMA_CCR_EN;
}

//===========================================================================
// Initialize the SPI1 peripheral.
//===========================================================================
void init_spi2(void)
{
	RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA -> MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER7 | GPIO_MODER_MODER15);
	GPIOA -> MODER |= GPIO_MODER_MODER5_1 | GPIO_MODER_MODER7_1 | GPIO_MODER_MODER15_1;

	RCC -> APB2ENR |= RCC_APB2ENR_SPI1EN;
	// clear CR1_SPE bit
	SPI1 -> CR1 &= ~SPI_CR1_SPE;
	// Set baud rate as low as possible (divided by 256)
	SPI1 -> CR1 |= SPI_CR1_BR;

	// Configure interface for 16-bit word size
	SPI1 -> CR2 = SPI_CR2_DS; // have to do =

	// Configure the SPI channel to be in master mode
	SPI1 -> CR1 |= SPI_CR1_MSTR;

	// Set the SSOE bit and NSSP bit
	SPI1 -> CR2 |= SPI_CR2_SSOE | SPI_CR2_NSSP;

	// Set the TXDMAEN to enable DMA transfers on transmit buffer empty
	SPI1 -> CR2 |= SPI_CR2_TXDMAEN;

	// Enable SPI by setting SPE bit
	SPI1 -> CR1 |= SPI_CR1_SPE;
}

//===========================================================================
// Configure the SPI1 peripheral to trigger the DMA channel when the
// transmitter is empty.
//===========================================================================
void spi2_setup_dma(void) {
    setup_dma();
    SPI1->CR2 |= SPI_CR2_TXDMAEN;// Transfer register empty DMA enable
}

//===========================================================================
// Enable the DMA channel.
//===========================================================================
void spi2_enable_dma(void) {
    enable_dma();
}


//===========================================================================
// Score Setup
//===========================================================================
void setup_score(int score)
{
    // Message Array
    msg[0] |= font['P'];
    msg[1] |= font['T'];
    msg[2] |= font['S'];
    msg[3] |= font[' '];
    msg[4] |= font[' '];
    msg[5] |= font[(int)((char)(score / pow(10, 2)) + '0')];
    msg[6] |= font[(int)((char)((int)(score / pow(10, 1))%10) + '0')];
    msg[7] |= font[(int)((char)(score % 10) + '0')];


    // Set up and Drive Bit Banging
    //setup_bb();
    //drive_bb();

    init_spi2();
    spi2_setup_dma();
    spi2_enable_dma();
}

void update_score(int score)
{
    msg[5] &= ~font[(int)((char)((score - 1) / pow(10, 2)) + '0')];
    msg[6] &= ~font[(int)((char)((int)((score - 1) / pow(10, 1))%10) + '0')];
    msg[7] &= ~font[(int)((char)((score - 1) % 10) + '0')];

    msg[5] |= font[(int)((char)(score / pow(10, 2)) + '0')];
    msg[6] |= font[(int)((char)((int)(score / pow(10, 1))%10) + '0')];
    msg[7] |= font[(int)((char)(score % 10) + '0')];

    //drive_bb();
}

//#define TESTING
#if defined(TESTING)
int main(void)
{

	your stuff back
}
#endif
