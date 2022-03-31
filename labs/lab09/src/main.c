#include "stm32f0xx.h"
#include <string.h> // for memcpy() declaration

void nano_wait(unsigned int);
extern const char font[128];

//===========================================================================
// Debouncing a Keypad
//===========================================================================

void drive_column(int);
int read_rows();
void update_history(col, rows);
//============================================================================
// enable_dma()
//============================================================================

uint8_t col;
void TIM7_IRQHandler(void) {
  // copy from lab 8
	// Remember to acknowledge the interrupt here!
	TIM7->SR &= ~TIM_SR_UIF;
	int rows = read_rows();
	update_history(col, rows);
	col = (col + 1) & 3;
	drive_column(col);
}

void init_tim7() {
  // copy from lab 8
	RCC->APB1ENR |= 0x20;     	//TIM7EN bit 5 of RCC_APB1ENR
	TIM7->PSC = 4799;        	//48Mhz/(4800-1) = (10-1)*(1000)
	TIM7->ARR = 9;
	TIM7->DIER = TIM_DIER_UIE;
	TIM7->CR1 |= TIM_CR1_CEN; 	//bit 0 of TIM7_CR1
	NVIC->ISER[0] = 1<<TIM7_IRQn;
}

//===========================================================================
// SPI DMA LED Array
//===========================================================================
uint16_t msg[8] = { 0x0000,0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700 };

void init_spi2(void) {
  // copy from lab 8
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
void enable_dma(void)
{
	DMA1_Channel5->CCR |= DMA_CCR_EN;
}

void setup_spi2_dma(void) {
  // copy from lab 8
    setup_dma();
    SPI2->CR2 |= SPI_CR2_TXDMAEN;// Transfer register empty DMA enable
}

void enable_spi2_dma(void) {
  // copy from lab 8
	enable_dma();
}

//===========================================================================
// 2.1 Initialize I2C
//===========================================================================
#define GPIOEX_ADDR 0x20  // ENTER GPIO EXPANDER I2C ADDRESS HERE
#define EEPROM_ADDR 0x57  // ENTER EEPROM I2C ADDRESS HERE

void init_i2c(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= 2<<(2*6) | 2<<(2*7);
	GPIOB->AFR[0] |= 1<<(4*6) | 1<<(4*7);

	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	// First, disable the PE bit in CR1 before making the following configuration changes.
	// Turn off the ANFOFF bit (turn on the analog noise filter).
	// Disable the error interrupt.
	// Turn off the NOSTRETCH bit in CR1 to enable clock stretching.
	I2C1->CR1 &= ~I2C_CR1_PE;
	I2C1->CR1 &= ~I2C_CR1_ANFOFF;
	I2C1->CR1 &= ~I2C_CR1_ERRIE;
	I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;
	// Set the TIMINGR register as follows:
	//(Note that these are configurations found on Table 83 of the Family Reference Manual. Why is I2C1's clock 8 MHz? See Figure 209 of the Reference Manual for a hint).
	I2C1->TIMINGR = 0;
	I2C1->TIMINGR &= ~I2C_TIMINGR_PRESC;
	I2C1->TIMINGR |= 0 << 28;		// Set the prescaler to 0.
	I2C1->TIMINGR |= 3 << 20;		// Set the SCLDEL field to 3.
	I2C1->TIMINGR |= 1 << 16;		// Set the SDADEL field to 1.
	I2C1->TIMINGR |= 3 << 8;		// Set the SCLH field to 3.
	I2C1->TIMINGR |= 9 << 0;

	// Disable both of the "own addresses", OAR1 and OAR2.
	I2C1->OAR1 &= ~I2C_OAR1_OA1EN;
	I2C1->OAR1 =   I2C_OAR1_OA1EN | 0x2;
	I2C1->OAR2 &= ~I2C_OAR2_OA2EN;
	// Configure the ADD10 field of CR2 for 7-bit mode.
	I2C1->CR2 &= ~I2C_CR2_ADD10;

	// Turn on the AUTOEND setting to enable automatic end.
	I2C1->CR2 |= I2C_CR2_AUTOEND;
	//I2C1->CR2 |= I2C_CR2_NACK;
	// Enable the channel by setting the PE bit in CR1.
	I2C1->CR1 |= I2C_CR1_PE;

}


//===========================================================================
// 2.2 I2C helpers
//===========================================================================

void i2c_waitidle(void) {
	while((I2C1->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);
}

void i2c_start(uint32_t devaddr, uint8_t size, uint8_t dir) {
	//dir: 0 = master requests a write transfer
	//dir: 1 = master requests a read transfer
	uint32_t tmpreg = I2C1->CR2;
	tmpreg &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES |
				I2C_CR2_RELOAD | I2C_CR2_AUTOEND |
				I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP);
	if(dir == 1)
		tmpreg |= I2C_CR2_RD_WRN; 	//read from slave
	else
		tmpreg &= ~I2C_CR2_RD_WRN;	//write to slave
	tmpreg |= ((devaddr<<1) & I2C_CR2_SADD) | ((size << 16) & I2C_CR2_NBYTES);
	tmpreg |= I2C_CR2_START;
	I2C1->CR2 = tmpreg;
}

void i2c_stop(void) {
	if(I2C1->ISR & I2C_ISR_STOPF)
			return;
	//Master: Generate STOP bit after current byte has been transferred.
	I2C1->CR2 |= I2C_CR2_STOP;
	// wait until STOPF flag is reset
	while((I2C1->ISR & I2C_ISR_STOPF) == 0);
	I2C1->ICR |= I2C_ICR_STOPCF; //write to clear STOPF flag
}

int i2c_checknack(void) {		//not same as lecture notes
	int check_bit4 = I2C1->ISR >>4;
	check_bit4 = check_bit4 & 1;
	return check_bit4;
}

void i2c_clearnack(void) {		//not same as lecture notes
	I2C1->ISR |= I2C_ISR_NACKF;
}

int i2c_senddata(uint8_t devaddr, const void *data, uint8_t size) {
	int i;
	if(size <= 0 || data == 0) return -1;
	uint8_t *udata = (uint8_t*)data;
	i2c_waitidle();
	//last argument is dir: 0 = sending data to the slave device
	i2c_start(devaddr, size, 0);
	for(i = 0; i<size; i++){
		int count= 0;
		while((I2C1->ISR & I2C_ISR_TXIS) == 0){
			count += 1;
			if (count > 1000000) return -1;
			if(i2c_checknack()) { i2c_clearnack(); i2c_stop(); return -1; }
		}
		//TXIS is cleared
		I2C1->TXDR = udata[i] & I2C_TXDR_TXDATA;
	}
	while((I2C1->ISR & I2C_ISR_TC) == 0 && (I2C1->ISR & I2C_ISR_NACKF) == 0);
	if((I2C1->ISR & I2C_ISR_NACKF) != 0)
		return -1;
	i2c_stop();
	return 0;
}

int i2c_recvdata(uint8_t devaddr, void *data, uint8_t size) {
	int i;
	if (size <= 0 || data == 0) return -1;
	uint8_t *udata = (uint8_t*)data;
	i2c_waitidle();
	i2c_start(devaddr, size, 1);
	for(i=0; i<size; i++) {
		int count = 0;
		while((I2C1->ISR & I2C_ISR_RXNE) == 0) {
			count += 1;
			if(count > 1000000) return -1;
			if(i2c_checknack()) {i2c_clearnack(); i2c_stop(); return -1; }
		}
		udata[i] = I2C1->RXDR;
	}
	while((I2C1->ISR & I2C_ISR_TC) == 0 && (I2C1->ISR & I2C_ISR_NACKF) == 0);
	if((I2C1->ISR & I2C_ISR_NACKF) != 0)
		return -1;
	i2c_stop();
	return 0;
}
// ===================
/* 2.2 check 
// ===================
i2c_init();
for(;;) {
    i2c_waitidle();
    i2c_start(0x20,0,0);
    i2c_clearnack();
    i2c_stop();
}
*/
//===========================================================================
// 2.4 GPIO Expander
//===========================================================================
void gpio_write(uint8_t reg, uint8_t val) {
	//Set the MCP23008 register number reg to the val in the second argument.
	//To do this, set up a two-byte array with the two parameters and
	//use the i2c_senddata() subroutine to send them to the MCP23008.

	i2c_senddata(uint8_t devaddr, const void *data, uint8_t size);
}

uint8_t gpio_read(uint8_t reg) {
	// Get the value of the MCP23008 register number reg. Return this value.
	// To do this, set up a one-byte array with the register number, send it with i2c_senddata().
	// Reuse the one-byte array to read one byte from the MCP23008 with i2c_recvdata().

}

void init_expander(void) {
	// uses void gpio_write(uint8_t reg, uint8_t value) to configure the GPIO Expander.
	// It should configure the IODIR register so that GP0-3 are outputs and GP4-7 are inputs.
	// It should also turn on the internal pull ups on pins GP4-7 and reverse their polarity.
}

void drive_column(int c) {
    gpio_write(10, ~(1 << (3 - c)) );
}

int read_rows() {
    uint8_t data = gpio_read(9);
    data &= 0xf0;
    for (int i = 0; i < 4; i++) {
        uint8_t bit = data & (1 << (4 + i));
        bit >>= (4 + i);
        bit <<= (3 - i);
        data |= bit;
    }
    return data & 0xf;
}


//===========================================================================
// 2.4 EEPROM functions
//===========================================================================
void eeprom_write(uint16_t loc, const char* data, uint8_t len) {
}

int eeprom_write_complete(void) {
}

void eeprom_read(uint16_t loc, char data[], uint8_t len) {
    TIM7->CR1 &= ~TIM_CR1_CEN; // Pause keypad scanning.

    // ... your code here

    TIM7->CR1 |= TIM_CR1_CEN; // Resume keypad scanning.
}


void eeprom_blocking_write(uint16_t loc, const char* data, uint8_t len) {
    TIM7->CR1 &= ~TIM_CR1_CEN; // Pause keypad scanning.
    eeprom_write(loc, data, len);
    while (!eeprom_write_complete());
    TIM7->CR1 |= TIM_CR1_CEN; // Resume keypad scanning.
}

//===========================================================================
// Main and supporting functions
//===========================================================================

void serial_ui(void);
void show_keys(void);

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


    // LED array SPI
    setup_spi2_dma();
    enable_spi2_dma();
    init_spi2();

    // This LAB

    // 2.1 Initialize I2C
    init_i2c();

    // 2.2 Example code for testing
#define STEP22
#if defined(STEP22)
    for(;;) {
        i2c_waitidle();
        i2c_start(GPIOEX_ADDR,0,0);
        i2c_clearnack();
        i2c_stop();
    }
#endif

    // 2.3 Example code for testing
#define STEP23
#if defined(STEP23)
    for(;;) {
        uint8_t data[2] = { 0x00, 0xff };
        i2c_senddata(0x20, data, 2);
    }
#endif

    // 2.4 Expander setup
    init_expander();
    init_tim7();

    // 2.5 Interface for reading/writing memory.
    serial_ui();

    show_keys();
}
