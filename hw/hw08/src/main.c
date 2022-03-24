#include "stm32f0xx.h"
#include "lcd.h"

void init_lcd_spi(void)
{

    // Enable the RCC clock to GPIO Port B
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	// Set PB8, PB11, and PB14 to be outputs
	GPIOB->MODER &= ~0x30c30000;  	//0011 0000 1100 0011 0000 0000 0000 0000
	GPIOB->MODER |= 0x10410000;     //0001 0000 0100 0001 0000 0000 0000 0000
	// Set the ODR value for PB8, PB11, and PB14 to be 1 (logic high)
	GPIOB->ODR |= 0x4900;			//0100 1001 0000 0000
	// Configure PB3 to be alternate function 0      //WHAT DOES IT MEAN TO BE ALTERNATE FUNCTION 0?????????????????
	GPIOB->MODER &= ~0xc0;        //1100 0000
	GPIOB->MODER |= 0x80;         //1000 0000
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFR3;
	// Configure PB5 to be alternate function 0
	GPIOB->MODER &= ~0xc00;        //1100 0000 0000
	GPIOB->MODER |= 0x800;         //1000 0000 0000
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFR5;

	// Enable the RCC clock to SPI1
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	// Turn off the SPI1 peripheral (clear the SPE bit)
	SPI1->CR1 &= ~SPI_CR1_SPE;
	// Set the baud rate to be as high as possible 			//means setting BR[2:0] to 000????????????
	SPI1->CR1 &= ~SPI_CR1_BR;
	// Configure the SPI1 peripheral for "master mode"
	SPI1->CR1 |= SPI_CR1_MSTR;
	// Set the word size to be 8-bit		//0111
	SPI1->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;
	// Set the SSM and SSI bits
	SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
	// Enable the SPI1 peripheral (set the SPE bit)
	SPI1->CR1 |= SPI_CR1_SPE;
}

void setup_buttons(void)
{
    // Enable the RCC clock to GPIO Port C
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	// Configure PC4 - PC7 to be outputs
	GPIOC->MODER &= ~0xffff;       //1111 1111 1111 1111
	GPIOC->MODER |= 0x5500;       //0101 0101 0000 0000
	// Configure PC4 - PC7 to be open-drain
	GPIOC->OTYPER |= 0xf0;        //1111 0000
	// Configure PC0 - PC3 to be inputs
	GPIOC->MODER &= ~0xff;          //1111 1111
	// Configure PC0 - PC3 to enable internal pull-up resistors
    GPIOC->PUPDR &= ~0xff;
    GPIOC->PUPDR |= 0x55;         //0101 0101
}

void basic_drawing(void);
void move_ball(void);

int main(void)
{
    setup_buttons();
    LCD_Setup(); // this will call init_lcd_spi()
    basic_drawing();
    move_ball();
}
