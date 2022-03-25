#include "stm32f0xx.h"

// Be sure to change this to your login...
const char login[] = "chen3633";

void set_char_msg(int, char);
void nano_wait(unsigned int);


//===========================================================================
// Configure GPIOC
//===========================================================================
void enable_ports(void) {
    // Only enable port C for the keypad
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER &= ~0xffff;
    GPIOC->MODER |= 0x55 << (4*2);
    GPIOC->OTYPER &= ~0xff;
    GPIOC->OTYPER |= 0xf0;
    GPIOC->PUPDR &= ~0xff;
    GPIOC->PUPDR |= 0x55;
}

uint8_t col; // the column being scanned

void drive_column(int);   // energize one of the column outputs
int  read_rows();         // read the four row inputs
void update_history(int col, int rows); // record the buttons of the driven column
char get_key_event(void); // wait for a button event (press or release)
char get_keypress(void);  // wait for only a button press event.
float getfloat(void);     // read a floating-point number from keypad
void show_keys(void);     // demonstrate get_key_event()

//===========================================================================
// Configure timer 7 to invoke the update interrupt at 1kHz
// Copy from lab 6 or 7.
//===========================================================================
void init_tim7()
{
	RCC->APB1ENR |= 0x20;     	//TIM7EN bit 5 of RCC_APB1ENR
	TIM7->PSC = 4799;        	//48Mhz/(4800-1) = (10-1)*(1000)
	TIM7->ARR = 9;
	TIM7->DIER = TIM_DIER_UIE;
	TIM7->CR1 |= TIM_CR1_CEN; 	//bit 0 of TIM7_CR1
	NVIC->ISER[0] = 1<<TIM7_IRQn;

}

//===========================================================================
// Copy the Timer 7 ISR from lab 7
//===========================================================================
void TIM7_IRQHandler(void){
	// Remember to acknowledge the interrupt here!
	TIM7->SR &= ~TIM_SR_UIF;
	int rows = read_rows();
	update_history(col, rows);
	col = (col + 1) & 3;
	drive_column(col);
}

//===========================================================================
// 2.1 Bit Bang SPI LED Array
//===========================================================================
int msg_index = 0;
uint16_t msg[8] = { 0x0000,0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700 };
extern const char font[];

//===========================================================================
// Configure PB12 (NSS), PB13 (SCK), and PB15 (MOSI) for outputs
//===========================================================================
void setup_bb(void)
{
	// set pins PB12 (NSS), PB13 (SCK), and PB15 (MOSI) for general purpose output (not an alternate function)
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER &= ~0xcf000000;	//1100 1111 0000 0000 0000 0000 0000 0000
	GPIOB->MODER |= 0x45000000;		//0100 0101 0000 0000 0000 0000 0000 0000
	// Initialize the ODR so that NSS is high and SCK is low
	GPIOB->ODR &= ~GPIO_ODR_13;	//11 0000 0000 0000
	GPIOB->ODR |= GPIO_ODR_12; 			//01 0000 0000 0000
}

void small_delay(void) {
    //nano_wait(50000000);
}

//===========================================================================
// Set the MOSI bit, then set the clock high and low.
// Pause between doing these steps with small_delay().
//===========================================================================
void bb_write_bit(int val)
{
    // NSS (PB12)
    // SCK (PB13)
    // MOSI (PB15)

    // Set the MOSI pin to low if the value of the parameter is zero. (Otherwise, set it to high.)
	if(val == 0){
		GPIOB->ODR &= ~GPIO_ODR_15;
	}
	else{
		GPIOB->ODR |= GPIO_ODR_15;
	}
	small_delay();
	// Set the SCK pin to high.
	GPIOB->ODR |= GPIO_ODR_13;
	small_delay();
	// Set the SCK pin to low.
	GPIOB->ODR &= ~GPIO_ODR_13;
}

//===========================================================================
// Set NSS (PB12) low,
// write 16 bits using bb_write_bit,
// then set NSS high.
//===========================================================================
void bb_write_halfword(int halfword)
{
	GPIOB->ODR &= ~GPIO_ODR_12;
	for (int i = 15; i >= 0; i--){
		bb_write_bit((halfword>>i) & 1);
	}
	GPIOB->ODR |= GPIO_ODR_12;
}

//===========================================================================
// Continually bitbang the msg[] array.
//===========================================================================
void drive_bb(void) {
    for(;;)
        for(int d=0; d<8; d++) {
            bb_write_halfword(msg[d]);
            nano_wait(1000000); // wait 1 ms between digits
        }
}

//============================================================================
// setup_dma()
// Copy this from lab 6 or lab 7.
// Write to SPI2->DR instead of GPIOB->ODR.
//============================================================================
void setup_dma(void)
{
	RCC->AHBENR |= RCC_AHBENR_DMAEN;            //bit 0 of RCC_AHBENR
	//Turn off the enable bit for the channel.
	DMA1_Channel5->CCR &= ~DMA_CCR_EN;              //bit 0 of DMA->CCR5
	//Set CPAR to the address of the GPIOB_ODR register.
	DMA1_Channel5->CPAR = (uint32_t)(&(SPI2->DR));
	//Set CMAR to the msg array base address
	DMA1_Channel5->CMAR = (uint32_t) msg;             //addresses are 32 bits
	//Set CNDTR to 8
	DMA1_Channel5->CNDTR = 8;
	//Set the DIRection for copying from-memory-to-peripheral.  bit 4: 1
	//Set the MINC to increment the CMAR for every transfer.    bit 7: 1
	//Set the memory datum size to 16-bit.                      bit 11:10: 01
	//Set the peripheral datum size to 16-bit.                  bit 9:8: 01
	//Set the channel for CIRCular operation.                   bit 5: 1
	DMA1_Channel5->CCR &= ~0xff0;
	DMA1_Channel5->CCR |= 0x5b0;    //0101 1011 0000;
	// SPI1->CR2 |= SPI_CR2_TXDMAEN;
}

//============================================================================
// enable_dma()
// Copy this from lab 6 or lab 7.
//============================================================================
void enable_dma(void)
{
	DMA1_Channel5->CCR |= DMA_CCR_EN;
}

//============================================================================
// Configure Timer 15 for an update rate of 1 kHz.
// Trigger the DMA channel on each update.
// Copy this from lab 6 or lab 7.
//============================================================================
void init_tim15(void)
{
	RCC->APB2ENR |= 0x10000;     //TIM15EN bit 16 of RCC_APB2ENR
	TIM15->PSC = 4800-1;        //48Mhz/(4800-1) = (10-1)*(1000)
	TIM15->ARR = 10-1;
	TIM15->DIER |= TIM_DIER_UDE;       //UDE is bit 8 of TIM15_DIER
	TIM15->CR1 |= TIM_CR1_CEN; //bit 0 of TIM15_CR1

}

//===========================================================================
// Initialize the SPI2 peripheral.
//===========================================================================
void init_spi2(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER &= ~0xcf000000;	//1100 1111 0000 0000 0000 0000 0000 0000
	GPIOB->MODER |= 0x8a000000;		//1000 1010 0000 0000 0000 0000 0000 0000
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	// Ensure that the CR1_SPE bit is clear. Many of the bits set in the control registers require that the SPI channel is not enabled.
	SPI2->CR1 &= ~SPI_CR1_SPE;
	// Set the baud rate as low as possible (maximum divisor for BR).
	SPI2->CR1 |= SPI_CR1_BR;
	// Configure the interface for a 16-bit word size.
	SPI2->CR2 = SPI_CR2_DS;
	// Configure the SPI channel to be in "master mode".
	SPI2->CR1 |= SPI_CR1_MSTR;
	// Set the SS Output enable bit and enable NSSP.
	SPI2->CR2 |= SPI_CR2_SSOE;
	SPI2->CR2 |= SPI_CR2_NSSP;
	// Set the TXDMAEN bit to enable DMA transfers on transmit buffer empty
	SPI2->CR2 |= SPI_CR2_TXDMAEN;
	// Enable the SPI channel.
	SPI2->CR1 |= SPI_CR1_SPE;
}

//===========================================================================
// Configure the SPI2 peripheral to trigger the DMA channel when the
// transmitter is empty.
//===========================================================================
void spi2_setup_dma(void) {
    setup_dma();
    SPI2->CR2 |= SPI_CR2_TXDMAEN;// Transfer register empty DMA enable
}

//===========================================================================
// Enable the DMA channel.
//===========================================================================
void spi2_enable_dma(void) {
    enable_dma();
}

//===========================================================================
// 2.4 SPI OLED Display
//===========================================================================
void init_spi1() {
    // PA5  SPI1_SCK
    // PA6  SPI1_MISO
    // PA7  SPI1_MOSI
    // PA15 SPI1_NSS
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER &= ~0xc000fc00;		//1100 0000 0000 0000 1111 1100 0000 0000
	GPIOA->MODER |= 0x8000a800;			//1000 0000 0000 0000 1010 1000 0000 0000
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	// Ensure that the CR1_SPE bit is clear. Many of the bits set in the control registers require that the SPI channel is not enabled.
	SPI1->CR1 &= ~SPI_CR1_SPE;
	// Set the baud rate as low as possible (maximum divisor for BR).
	SPI1->CR1 |= SPI_CR1_BR;
	// Configure the interface for a 10-bit word size. (1001)
	SPI1->CR2 = SPI_CR2_DS_3 | SPI_CR2_DS_0;
	// Configure the SPI channel to be in "master mode".
	SPI1->CR1 |= SPI_CR1_MSTR;
	// Set the SS Output enable bit and enable NSSP.
	SPI1->CR2 |= SPI_CR2_SSOE;
	SPI1->CR2 |= SPI_CR2_NSSP;
	// Set the TXDMAEN bit to enable DMA transfers on transmit buffer empty
	SPI1->CR2 |= SPI_CR2_TXDMAEN;
	// Enable the SPI channel.
	SPI1->CR1 |= SPI_CR1_SPE;
}

void spi_cmd(unsigned int data) {
    while(!(SPI1->SR & SPI_SR_TXE)) {}
    SPI1->DR = data;
}
void spi_data(unsigned int data) {
    spi_cmd(data | 0x200);
}
void spi1_init_oled() {
    nano_wait(1000000);
    spi_cmd(0x38);
    spi_cmd(0x08);
    spi_cmd(0x01);
    nano_wait(2000000);
    spi_cmd(0x06);
    spi_cmd(0x02);
    spi_cmd(0x0c);
}
void spi1_display1(const char *string) {
    spi_cmd(0x02);
    while(*string != '\0') {
        spi_data(*string);
        string++;
    }
}
void spi1_display2(const char *string) {
    spi_cmd(0xc0);
    while(*string != '\0') {
        spi_data(*string);
        string++;
    }
}

//===========================================================================
// This is the 34-entry buffer to be copied into SPI1.
// Each element is a 16-bit value that is either character data or a command.
// Element 0 is the command to set the cursor to the first position of line 1.
// The next 16 elements are 16 characters.
// Element 17 is the command to set the cursor to the first position of line 2.
//===========================================================================
uint16_t display[34] = {
        0x002, // Command to set the cursor at the first position line 1
        0x200+'E', 0x200+'C', 0x200+'E', 0x200+'3', 0x200+'6', + 0x200+'2', 0x200+' ', 0x200+'i',
        0x200+'s', 0x200+' ', 0x200+'t', 0x200+'h', + 0x200+'e', 0x200+' ', 0x200+' ', 0x200+' ',
        0x0c0, // Command to set the cursor at the first position line 2
        0x200+'c', 0x200+'l', 0x200+'a', 0x200+'s', 0x200+'s', + 0x200+' ', 0x200+'f', 0x200+'o',
        0x200+'r', 0x200+' ', 0x200+'y', 0x200+'o', + 0x200+'u', 0x200+'!', 0x200+' ', 0x200+' ',
};

//===========================================================================
// Configure the proper DMA channel to be triggered by SPI1_TX.
// Set the SPI1 peripheral to trigger a DMA when the transmitter is empty.
//===========================================================================
void spi1_setup_dma(void)
{
	RCC->AHBENR |= RCC_AHBENR_DMAEN;            //bit 0 of RCC_AHBENR
	//Turn off the enable bit for the channel.
	DMA1_Channel3->CCR &= ~DMA_CCR_EN;              //bit 0 of DMA->CCR5
	//Set CPAR to the address of the GPIOB_ODR register.
	DMA1_Channel3->CPAR = (uint32_t)(&(SPI1->DR));
	//Set CMAR to the display array base address
	DMA1_Channel3->CMAR = (uint32_t) display;             //addresses are 32 bits
	//Set CNDTR to 8
	DMA1_Channel3->CNDTR = 34;
	//Set the DIRection for copying from-memory-to-peripheral.  bit 4: 1
	//Set the MINC to increment the CMAR for every transfer.    bit 7: 1
	//Set the memory datum size to 16-bit.                      bit 11:10: 01
	//Set the peripheral datum size to 16-bit.                  bit 9:8: 01
	//Set the channel for CIRCular operation.                   bit 5: 1
	DMA1_Channel3->CCR &= ~0xff0;
	DMA1_Channel3->CCR |= 0x5b0;    //0101 1011 0000;
	SPI1->CR2 |= SPI_CR2_TXDMAEN;
}

//===========================================================================
// Enable the DMA channel triggered by SPI1_TX.
//===========================================================================
void spi1_enable_dma(void)
{
	DMA1_Channel3->CCR |= DMA_CCR_EN;
}

//===========================================================================
// Main function
//===========================================================================

int main(void)
{
    msg[0] |= font['E'];
    msg[1] |= font['C'];
    msg[2] |= font['E'];
    msg[3] |= font[' '];
    msg[4] |= font['3'];
    msg[5] |= font['6'];
    msg[6] |= font['2'];
    msg[7] |= font[' '];

    // This time, autotest always runs as an invisible aid to you.
    autotest();

    // GPIO enable
    enable_ports();
    // setup keyboard
    init_tim7();

    // LED array Bit Bang
//#define BIT_BANG
#if defined(BIT_BANG)
    setup_bb();
    drive_bb();
#endif

    // Direct SPI peripheral to drive LED display
//#define SPI_LEDS
#if defined(SPI_LEDS)
    init_spi2();
    setup_dma();
    enable_dma();
    init_tim15();
    show_keys();
#endif

    // LED array SPI
//#define SPI_LEDS_DMA
#if defined(SPI_LEDS_DMA)
    init_spi2();
    spi2_setup_dma();
    spi2_enable_dma();
    show_keys();
#endif

    // SPI OLED direct drive
//#define SPI_OLED
#if defined(SPI_OLED)
    init_spi1();
    spi1_init_oled();
    spi1_display1("Hello again,");
    spi1_display2(login);
#endif

    // SPI
#define SPI_OLED_DMA
#if defined(SPI_OLED_DMA)
    init_spi1();
    spi1_init_oled();
    spi1_setup_dma();
    spi1_enable_dma();
#endif

    // Game on!  The goal is to score 100 points.
    game();
}
