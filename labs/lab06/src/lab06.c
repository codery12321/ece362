#include "stm32f0xx.h"
#include <math.h>   // for M_PI

void nano_wait(int);
//=============================================================================
// Part 1: 7-segment display update with DMA
//=============================================================================
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
void setup_dma(void)
{
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
void enable_dma(void)
{
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
// Write the Timer 7 ISR here.  Be sure to give it the right name.
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
uint32_t volume = 2048;

//============================================================================
// setup_adc()
//============================================================================
void setup_adc(void)
{
    // Enable the clock to GPIO Port A
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    // Set the configuration for analog operation only for the appropriate pins
	//ADC_IN1 is PA1 so configure GPIO Port B and set MODER 1 to high
	GPIOA->MODER |= 0x2;    //10
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
// Write the Timer 2 ISR here.  Be sure to give it the right name.
void TIM2_IRQHandler(void){
	// Acknowledge the interrupt.
	TIM2->SR &= ~TIM_SR_UIF;
	// Start the ADC by turning on the ADSTART bit in the CR.
	ADC1->CR |= ADC_CR_ADSTART;
	// Wait until the EOC bit is set in the ISR.
	while(!(ADC1->ISR & ADC_ISR_ADRDY));
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

    NVIC_SetPriority(TIM2_IRQn, 3);
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
// setup_dac()
//============================================================================
void setup_dac(void)
{
	//Enable the RCC clock for the DAC
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;
	//Select a TIM6 TRGO trigger for the DAC with the TSEL field of the CR register
	DAC->CR &= ~380000;		//11 1000 0000 0000 0000 0000
	//Enable the trigger for the DAC
	DAC->CR |= DAC_CR_TEN1;
	//Enable the DAC
	DAC->CR |= DAC_CR_EN1;
}

//============================================================================
// Timer 6 ISR
//============================================================================
// Write the Timer 6 ISR here.  Be sure to give it the right name.
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
    samp = samp >> 17;
    samp += 2048;
    DAC->DHR12R1 = samp;
}

//============================================================================
// init_tim6()
//============================================================================
void init_tim6(void)
{
    RCC->APB1ENR |= 0x1;     	//TIM6EN bit 0 of RCC_APB1ENR
    TIM6->ARR = 5-1;
    TIM6->PSC = 4800-1;        //48Mhz/(48000-1) = (100-1)*(10)
    TIM6->DIER = TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_CEN; 	//bit 0 of TIM6_CR1
    NVIC->ISER[0] = 1<<TIM6_DAC_IRQn;

    TIM6->CR2 |= 0x20;

}

//============================================================================
// All the things you need to test your subroutines.
//============================================================================
int main(void)
{
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

    // Demonstrate part 1
//#define SCROLL_DISPLAY
#ifdef SCROLL_DISPLAY
    for(;;)
        for(int i=0; i<8; i++) {
            print(&"Hello...Hello..."[i]);
            nano_wait(250000000);
        }
#endif

    init_tim7();

    // Demonstrate part 2
//#define SHOW_KEY_EVENTS
#ifdef SHOW_KEY_EVENTS
    show_keys();
#endif

    setup_adc();
    init_tim2();

    // Demonstrate part 3
//#define SHOW_VOLTAGE
#ifdef SHOW_VOLTAGE
    for(;;) {
        printfloat(2.95 * volume / 4096);
    }
#endif

    init_wavetable();
    setup_dac();
    init_tim6();

//#define ONE_TONE
#ifdef ONE_TONE
    for(;;) {
        float f = getfloat();
        set_freq(0,f);
    }
#endif

    // demonstrate part 4
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

    // Have fun.
    dialer();
}
