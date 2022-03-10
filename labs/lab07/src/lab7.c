#include "stm32f0xx.h"
#include <math.h>   // for M_PI

void nano_wait(int);

// 16-bits per digit.
// The most significant 8 bits are the digit number.
// The least significant 8 bits are the segments to illuminate.
uint16_t msg[8] = { 0x0000,0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700 };
extern const char font[];
// Print an 8-character string on the 8 digits
void print(const char str[]);
// Print a floating-point value.
void printfloat(float f);


//============================================================================
// enable_ports()
//============================================================================
void enable_ports(void)
{
    //enable RCC clock for GPIOB and GPIOC
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    //Configures pins PB0 ? PB10 to be outputs
    GPIOB->MODER &= ~0x3fffff;     //0011 1111 1111 1111 1111 1111
    GPIOB->MODER |= 0x155555;     //0001 0101 0101 0101 0101 0101
    //Configures pins PC4 ? PC7 to be outputs
    GPIOC->MODER &= ~0xffff;       //1111 1111 1111 1111
    GPIOC->MODER |= 0x5500;       //0101 0101 0000 0000
    //Configures pins PC4 ? PC7 to have output type open-drain (using the OTYPER)
    //GPIOC->OTYPER &= ~0xffff;
    GPIOC->OTYPER |= 0xf0;        //1111 0000
    //Configures pins PC0 ? PC3 to be inputs
    //GPIOC->MODER &= ~0xff;          //1111 1111
    //Configures pins PC0 ? PC3 to be internally pulled high
    GPIOC->PUPDR &= ~0xff;
    GPIOC->PUPDR |= 0x55;         //0101 0101
}

//============================================================================
// setup_dma()
//============================================================================
void setup_dma(void) {
	//DMA channel Timer 15 can trigger: channel 5
	//Enables the RCC clock to the DMA controller and configures the following channel parameters:
	    RCC->AHBENR |= RCC_AHBENR_DMAEN;            //bit 0 of RCC_AHBENR
	    //Turn off the enable bit for the channel.
	    DMA1_Channel5->CCR &= ~DMA_CCR_EN;              //bit 0 of DMA->CCR5
	    //Set CPAR to the address of the GPIOB_ODR register.
	    DMA1_Channel5->CPAR = (uint32_t)(&(GPIOB->ODR)); 	//addresses are 32 bits
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
}

//============================================================================
// enable_dma()
//============================================================================
void enable_dma(void) {
	 DMA1_Channel5->CCR |= DMA_CCR_EN;    //bit 0 of DMA->CCR5
}

//============================================================================
// init_tim15()
//============================================================================
void init_tim15(void)
{
    //enable TIM15's clock in RCC
    //trigger a DMA request at a rate of 1 kHz
    //Do ? by setting the UDE bit in the DIER.
    //Do not set the UIE bit in the DIER this time.
    RCC->APB2ENR |= 0x10000;     //TIM15EN bit 16 of RCC_APB2ENR
    TIM15->PSC = 4800-1;        //48Mhz/(4800-1) = (10-1)*(1000)
    TIM15->ARR = 10-1;
    TIM15->DIER |= TIM_DIER_UDE;       //UDE is bit 8 of TIM15_DIER
    TIM15->CR1 |= TIM_CR1_CEN; //bit 0 of TIM15_CR1
    //do we need to set NVIC_ISER? Ans: it doesn't need to be unmasked
}

//=============================================================================
// Part 2: Debounced keypad scanning.
//=============================================================================

uint8_t col; // the column being scanned

void drive_column(int);   // energize one of the column outputs
int  read_rows();         // read the four row inputs
void update_history(int col, int rows); // record the buttons of the driven column
char get_key_event(void); // wait for a button event (press or release)
char get_keypress(void);  // wait for only a button press event.
float getfloat(void);     // read a floating-point number from keypad
void show_keys(void);     // demonstrate get_key_event()

//============================================================================
// The Timer 7 ISR
//============================================================================
void TIM7_IRQHandler(void){
	// Remember to acknowledge the interrupt here!
	TIM7->SR &= ~TIM_SR_UIF;
	int rows = read_rows();
	update_history(col, rows);
	col = (col + 1) & 3;
	drive_column(col);
}

//============================================================================
// init_tim7()
//============================================================================
void init_tim7(void)
{
    RCC->APB1ENR |= 0x20;     	//TIM7EN bit 5 of RCC_APB1ENR
    TIM7->PSC = 4799;        	//48Mhz/(4800-1) = (10-1)*(1000)
    TIM7->ARR = 9;
    TIM7->DIER = TIM_DIER_UIE;
    TIM7->CR1 |= TIM_CR1_CEN; 	//bit 0 of TIM7_CR1
    NVIC->ISER[0] = 1<<TIM7_IRQn;
}

//=============================================================================
// Part 3: Analog-to-digital conversion for a volume level.
//=============================================================================
int volume = 2048;

//============================================================================
// setup_adc()
//============================================================================
void setup_adc(void)
{
    // Enable the clock to GPIO Port A
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    // Set the configuration for analog operation only for the appropriate pins
	//ADC_IN1 is PA1 so configure GPIO Port A and set MODER 1 to high
	GPIOA->MODER |= 0xC;    //11
    // Enable the clock to the ADC peripheral
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    // Turn on the "high-speed internal" 14 MHz clock (HSI14)
	RCC->CR2 |= RCC_CR2_HSI14ON;
    // Wait for the 14 MHz clock to be ready
	while(!(RCC->CR2 & RCC_CR2_HSI14RDY));
    // Enable the ADC by setting the ADEN bit in the CR register
	ADC1->CR |= ADC_CR_ADEN;
    // Wait for the ADC to be ready
	while(!(ADC1->ISR & ADC_ISR_ADRDY));
    // Select the corresponding channel for ADC_IN1 in the CHSELR
	ADC1->CHSELR = 0;
	ADC1->CHSELR |= 1<<1;
    // Wait for the ADC to be ready
	while(!(ADC1->ISR & ADC_ISR_ADRDY));
}

//============================================================================
// Variables for boxcar averaging.
//============================================================================
#define BCSIZE 32
int bcsum = 0;
int boxcar[BCSIZE];
int bcn = 0;
//============================================================================
// Timer 2 ISR
//============================================================================
void TIM2_IRQHandler(void){
	// Acknowledge the interrupt.
	TIM2->SR &= ~TIM_SR_UIF;
	// Start the ADC by turning on the ADSTART bit in the CR.
	ADC1->CR |= ADC_CR_ADSTART;
	// Wait until the EOC bit is set in the ISR.
	while(!(ADC1->ISR & ADC_ISR_EOC));
	// Implement boxcar averaging
    bcsum -= boxcar[bcn];
    bcsum += boxcar[bcn] = ADC1->DR;
    bcn += 1;
    if (bcn >= BCSIZE)
        bcn = 0;
    volume = bcsum / BCSIZE;
}

//============================================================================
// init_tim2()
//============================================================================
void init_tim2(void)
{
    RCC->APB1ENR |= 0x1;     	//TIM2EN bit 0 of RCC_APB1ENR
    TIM2->PSC = 48000-1;        //48Mhz/(48000-1) = (100-1)*(10)
    TIM2->ARR = 100-1;
    TIM2->DIER = TIM_DIER_UIE;
    TIM2->CR1 |= TIM_CR1_CEN; 	//bit 0 of TIM2_CR1
    NVIC->ISER[0] = 1<<TIM2_IRQn;

    //NVIC_SetPriority(TIM2_IRQn, 3);
}

//===========================================================================
// Part 4: Create an analog sine wave of a specified frequency
//===========================================================================
void dialer(void);

// Parameters for the wavetable size and expected synthesis rate.
#define N 1000
#define RATE 20000
short int wavetable[N];
int step0 = 0;
int offset0 = 0;
int step1 = 0;
int offset1 = 0;

//===========================================================================
// init_wavetable()
// Write the pattern for a complete cycle of a sine wave into the
// wavetable[] array.
//===========================================================================
void init_wavetable(void)
{
    for(int i=0; i < N; i++)
        wavetable[i] = 32767 * sin(2 * M_PI * i / N);
}
//============================================================================
// set_freq()
//============================================================================
void set_freq(int chan, float f) {
    if (chan == 0) {
        if (f == 0.0) {
            step0 = 0;
            offset0 = 0;
        } else
            step0 = (f * N / RATE) * (1<<16);
    }
    if (chan == 1) {
        if (f == 0.0) {
            step1 = 0;
            offset1 = 0;
        } else
            step1 = (f * N / RATE) * (1<<16);
    }
}

//============================================================================
// Timer 6 ISR
//============================================================================
void TIM6_DAC_IRQHandler(){
	// Acknowledge the interrupt.
	TIM6->SR &= ~TIM_SR_UIF;

    offset0 += step0;
    offset1 += step1;
    if (offset0 >= (N << 16))
        offset0 -= (N << 16);
    if (offset1 >= (N << 16))
        offset1 -= (N << 16);
    int samp = wavetable[offset0>>16] + wavetable[offset1>>16];
    samp = samp * volume;
    samp = samp >> 18;
    samp += 1200;
    TIM1->CCR4 = samp;
}

//============================================================================
// init_tim6()
//============================================================================
void init_tim6(void)
{
    RCC->APB1ENR |= 0x10;     	//TIM6EN bit 4 of RCC_APB1ENR
    TIM6->PSC = 480-1;        //48Mhz/(4800-1) = (100-1)*(RATE)
    TIM6->ARR = (100000 / RATE)-1;
    TIM6->DIER = TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_CEN; 	//bit 0 of TIM6_CR1
    NVIC->ISER[0] = 1<<TIM6_DAC_IRQn;
    //tim6 configured to trigger the DAC
    //TIM6->CR2 &= ~0x70;
    //TIM6->CR2 |= 0x20;    //10 0000

}

void setup_tim3(void)
{
	//enable RCC clock
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	//set mode for PA8-PA11 for "alternate function"
	GPIOC->MODER &= ~0xff000;
	GPIOC->MODER |= 0xaa000;	//11 11 11 11 00 00 00 0 0 0
	GPIOC->AFR[0] &= ~0xff000000;
	GPIOC->AFR[1] &= ~0xff;

	//enable RCC clock for Timer 3
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;	//bit 1 of APB1ENR

	//configure timer 3 PSC to divide by 48000
	TIM3->PSC = 48000-1;
	TIM3->ARR = 1000-1;  //48M/48000/ARR-1 = 1
	TIM3->CCMR1 &= ~0x7070;
	TIM3->CCMR1 |= 0x6060;
	TIM3->CCMR2 &= ~0x7070;
	TIM3->CCMR2 |= 0x6060;

	//Enable the four channel outputs in the TIM3_CCER register
	TIM3->CCER |= TIM_CCER_CC1E;
	TIM3->CCER |= TIM_CCER_CC2E;
	TIM3->CCER |= TIM_CCER_CC3E;
	TIM3->CCER |= TIM_CCER_CC4E;

	//enable counter
	TIM3->CR1 |= TIM_CR1_CEN;

	TIM3->CCR1 = 800;
	TIM3->CCR2 = 400;
	TIM3->CCR3 = 200;
	TIM3->CCR4 = 100;
}

void setup_tim1(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	//set mode for PA8-PA11 for "alternate function"
	GPIOA->MODER &= ~0xff0000;
	GPIOA->MODER |= 0xaa0000;	//1010 1010 0 0 0 0
	GPIOA->AFR[1] &= ~0xffff;
	GPIOA->AFR[1] |= 0x2222;

	//Activate the RCC clock to Timer 1.
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

	// enable the MOE bit of the BDTR
	TIM1->BDTR |= TIM_BDTR_MOE;

	TIM1->PSC = 1-1;
	TIM1->ARR = 2400-1;	//update event occurs 20000 times per second

	//Configure the "capture/compare mode registers" for PWM mode 1
	TIM1->CCMR1 &= ~0x7070;
	TIM1->CCMR1 |= 0x6060;
	TIM1->CCMR2 &= ~0x7070;
	TIM1->CCMR2 |= 0x6060;

	//Configure the CCMR2 register to set the "output compare preload enable" bit only for channel 4.
	TIM1->CCMR2 &= ~TIM_CCMR2_OC4PE;
	TIM1->CCMR2 |= TIM_CCMR2_OC4PE;

	//Enable the four channel outputs in the TIM1_CCER register
	TIM1->CCER |= TIM_CCER_CC1E;
	TIM1->CCER |= TIM_CCER_CC2E;
	TIM1->CCER |= TIM_CCER_CC3E;
	TIM1->CCER |= TIM_CCER_CC4E;

	//enable counter
	TIM1->CR1 |= TIM_CR1_CEN;
}

int getrgb(void);

void setrgb(int rgb)
{
	int tensRED = rgb>>20 & 0xf;
	int onesRED = rgb>>16 & 0xf;
	int percentageRED = 10 * tensRED + onesRED;
	TIM1->CCR1 = (2400 * (100 - percentageRED))/100;

	int tensGREEN = rgb>>12 & 0xf;
	int onesGREEN = rgb>>8 & 0xf;
	int percentageGREEN = 10 * tensGREEN + onesGREEN;
	TIM1->CCR2 = (2400 * (100 - percentageGREEN))/100;

	int tensBLUE = rgb>>4 & 0xf;
	int onesBLUE = rgb>>0 & 0xf;
	int percentageBLUE = 10 * tensBLUE + onesBLUE;
	TIM1->CCR3 = (2400 * (100 - percentageBLUE))/100;
}

//============================================================================
// All the things you need to test your subroutines.
//============================================================================
int main(void)
{

    // Demonstrate part 1
//#define TEST_TIMER3
#ifdef TEST_TIMER3
    setup_tim3();
    for(;;) { }
#endif

    // Initialize the display to something interesting to get started.
    msg[0] |= font['E'];
    msg[1] |= font['C'];
    msg[2] |= font['E'];
    msg[3] |= font[' '];
    msg[4] |= font['3'];
    msg[5] |= font['6'];
    msg[6] |= font['2'];
    msg[7] |= font[' '];

    enable_ports();
    setup_dma();
    enable_dma();
    init_tim15();
    init_tim7();
    setup_adc();
    init_tim2();
    init_wavetable();
    init_tim6();

    setup_tim1();

    // demonstrate part 2
//#define TEST_TIM1
#ifdef TEST_TIM1
    for(;;) {
        for(float x=10; x<2400; x *= 1.1) {
            TIM1->CCR1 = TIM1->CCR2 = TIM1->CCR3 = 2400-x;
            nano_wait(100000000);
        }
    }
#endif

    // demonstrate part 3
//#define MIX_TONES
#ifdef MIX_TONES
    for(;;) {
        char key = get_keypress();
        if (key == 'A')
            set_freq(0,getfloat());
        if (key == 'B')
            set_freq(1,getfloat());
    }
#endif

    // demonstrate part 4
#define TEST_SETRGB
#ifdef TEST_SETRGB
    for(;;) {
        char key = get_keypress();
        if (key == 'A')
            set_freq(0,getfloat());
        if (key == 'B')
            set_freq(1,getfloat());
        if (key == 'D')
            setrgb(getrgb());
    }
#endif

    // Have fun.
    dialer();
}
