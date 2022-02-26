#include "stm32f0xx.h"

// This is used by the interrupt service routine.
volatile int counter = 0;

unsigned int recur(unsigned int);
void enable_portb(void);
void enable_portc(void);
void setup_pb3(void);
void setup_pb4(void);
void setup_pc8(void);
void setup_pc9(void);
void action8(void);
void action9(void);
void enable_exti(void);
void enable_tim3(void);

//====================================================================
// The fail subroutine is invoked when a test fails.
// The test variable indicates which test failed.
// Use the debugger to walk back up the stack to find
// the point at which fail() was called.
//====================================================================
void fail(int test) {
    for(;;)
        asm("bkpt");
}

//=========================================================================
// An inline assembly language version of nano_wait.
//=========================================================================
void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

//====================================================================
// This will try out all the subroutines you wrote in hw5.s.
//====================================================================
int main(void)
{
    int x;

    // Q1: recur
    x = recur(16);    // 8
    if (x != 8)
        fail(1);
    x = recur(35); // 10
    if (x != 10)
        fail(1);
    x = recur(1025); // 20
    if (x != 20)
        fail(1);
    x = recur(0x80000000); // 62
    if (x != 62)
        fail(1);
    x = recur(0xf0f0f0f0); // 67
    if (x != 67)
        fail(1);

    // Q2: enable_portb
    RCC->AHBENR &= ~RCC_AHBENR_GPIOBEN;
    enable_portb();
    if ((RCC->AHBENR & (RCC_AHBENR_GPIOBEN|RCC_AHBENR_GPIOBEN|RCC_AHBENR_GPIOCEN)) !=
            RCC_AHBENR_GPIOBEN)
        fail(2);
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    enable_portb();
    if ((RCC->AHBENR & (RCC_AHBENR_GPIOAEN|RCC_AHBENR_GPIOBEN|RCC_AHBENR_GPIOCEN)) !=
            (RCC_AHBENR_GPIOAEN|RCC_AHBENR_GPIOBEN|RCC_AHBENR_GPIOCEN))
        fail(2);

    // Q3: enable_portc
    RCC->AHBENR &= ~RCC_AHBENR_GPIOAEN;
    RCC->AHBENR &= ~RCC_AHBENR_GPIOBEN;
    RCC->AHBENR &= ~RCC_AHBENR_GPIOCEN;
    enable_portc();
    if ((RCC->AHBENR & (RCC_AHBENR_GPIOAEN|RCC_AHBENR_GPIOBEN|RCC_AHBENR_GPIOCEN)) !=
            RCC_AHBENR_GPIOCEN)
        fail(3);
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    enable_portc();
    if ((RCC->AHBENR & (RCC_AHBENR_GPIOAEN|RCC_AHBENR_GPIOBEN|RCC_AHBENR_GPIOCEN)) !=
            (RCC_AHBENR_GPIOBEN|RCC_AHBENR_GPIOCEN))
        fail(3);

    // Q4: setup_pb3
    GPIOB->MODER |= 3<<(2*3);  // Sometimes, we deliberately mess with the initial values
    GPIOB->PUPDR |= 0x55555555;// so that you get practice fully clearing/setting them.
    setup_pb3();
    if (GPIOB->MODER != 0x00000000)
        fail(4);
    if (GPIOB->PUPDR != 0x55555595)
        fail(4);
    GPIOB->MODER &= ~(3<<(2*4));
    GPIOB->PUPDR = 0;
    setup_pb3();
    if (GPIOB->MODER != 0x00000000)
        fail(4);
    if (GPIOB->PUPDR != 0x00000080)
        fail(4);

    // Q5: setup_pb4
    GPIOB->MODER = 0xffffffff;  // Again, messing up these values.
    GPIOB->PUPDR = 0x55555555;
    setup_pb4();
    if (GPIOB->MODER != 0xfffffcff)
        fail(5);
    if (GPIOB->PUPDR != 0x55555455)
        fail(5);
    GPIOB->MODER = 0x55555515;
    GPIOB->PUPDR = 2<<(2*4);
    setup_pb4();
    if (GPIOB->MODER != 0x55555415)
        fail(5);
    if (GPIOB->PUPDR != 0x00000000)
        fail(5);

    // Q6: setup_pc8
    GPIOC->MODER   |= 3<<(2*8);
    GPIOC->OSPEEDR |= 3<<(2*8);
    GPIOC->MODER |= 1;
    GPIOC->OSPEEDR |= 1;
    int old_moder = GPIOC->MODER;
    int old_ospeedr = GPIOC->OSPEEDR;
    int new_moder = (old_moder & ~(3<<(2*8))) | (1<<2*8);
    int new_ospeedr = old_ospeedr | (3<<2*8);
    setup_pc8();
    if (GPIOC->MODER != new_moder)
        fail(6);
    if (GPIOC->OSPEEDR != new_ospeedr)
        fail(6);
    GPIOC->MODER   &= ~(3<<(2*8));
    GPIOC->OSPEEDR &= ~(3<<(2*8));
    GPIOC->MODER &= ~1;
    GPIOC->OSPEEDR &= ~1;
    old_moder = GPIOC->MODER;
    old_ospeedr = GPIOC->OSPEEDR;
    new_moder = (old_moder & ~(3<<(2*8))) | (1<<2*8);
    new_ospeedr = old_ospeedr | (3<<2*8);
    setup_pc8();
    if (GPIOC->MODER != new_moder)
        fail(6);
    if (GPIOC->OSPEEDR != new_ospeedr)
        fail(6);

    // Q7: setup_pc9
    new_moder |= 1<<(2*9);
    new_ospeedr |= 1<<(2*9);
    setup_pc9();
    if (GPIOC->MODER != new_moder)
        fail(7);
    if (GPIOC->OSPEEDR != new_ospeedr)
        fail(7);
    GPIOC->MODER |= 3<<(2*9);
    GPIOC->OSPEEDR |= 3<<(2*9);
    setup_pc9();
    if (GPIOC->MODER != new_moder)
        fail(7);
    if (GPIOC->OSPEEDR != new_ospeedr)
        fail(7);

    // Q8: action8
    // You did all that work to configure PB3 and PB4 as inputs, and now
    // we're just going to reconfigure them as outputs just so we can test
    // action8 easily.  Remember that you can still read from the IDR when
    // the pin is configured as output.  Here, the action8 subroutine will
    // read the value that we write to the ODR.  This is our basic means of
    // evaluating the GPIO pins.
    GPIOB->MODER &= ~(3<<(2*3)) & ~(3<<(2*4));
    GPIOB->MODER |= (1<<(2*3)) | (1<<(2*4));
    for(x=0; x<4; x++) {
        GPIOB->BRR = 0x3<<3;
        GPIOB->BSRR = x<<3;
        nano_wait(100000000); // wait 100ms for lines to settle
        const static int check[] = { 1, 0, 1, 1 };
        action8();
        nano_wait(100000000); // wait 100ms for lines to settle
        if (((GPIOC->IDR >> 8) & 1) != check[x])
            fail(8);
    }

    // Q9: action9
    for(x=0; x<4; x++) {
        GPIOB->BRR = 0x3<<3;
        GPIOB->BSRR = x<<3;
        nano_wait(100000000); // wait 100ms for lines to settle
        const static int check[] = { 0, 0, 1, 0 };
        action9();
        nano_wait(100000000); // wait 100ms for lines to settle
        if (((GPIOC->IDR >> 9) & 1) != check[x])
            fail(9);
    }

    // Reset PA4 and PA5 to be inputs before anyone notices.
    setup_pb3();
    setup_pb4();

    // Q10: EXTI2_3_IRQHandler
    counter = 6;
    EXTI2_3_IRQHandler();
    if (counter != 7)
        fail(10);

    // Q11: enable_exti
    enable_exti();
    if ((RCC->APB2ENR & RCC_APB2ENR_SYSCFGEN) == 0)
        fail(11);
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[0] |= 0xf<<(4*(2));
    EXTI->RTSR &= ~(1<<2);
    EXTI->IMR &= ~(1<<2);
    NVIC->ICER[0] = 1<<EXTI2_3_IRQn;
    SYSCFG->EXTICR[0] = 0x7777;  // see if you clear the right things
    enable_exti();
    if (SYSCFG->EXTICR[0] != 0x7177)
        fail(11);
    if (EXTI->RTSR != 1<<2)
        fail(11);
    if ((EXTI->IMR & 0xffff) != 1<<2)
        fail(11);
    if (!(NVIC->ISER[0] & (1<<EXTI2_3_IRQn)))
        fail(11);
    SYSCFG->EXTICR[0] = 0x7654;  // see if you set the right things
    enable_exti();
    if (SYSCFG->EXTICR[0] != 0x7154)
        fail(11);
    counter = 0;
    // Try it: Post a pending interrupt for EXTI4.
    NVIC->ISPR[0] = 1<<EXTI2_3_IRQn;
    for(x=0; x<100; x++)
      GPIOC->BSRR = 0; // stall for a while to make sure ISR runs.
    if (counter != 1)
        fail(11);

    // Q12:
    GPIOC->BRR = 1<<9;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    NVIC->ISER[0] = 1<<TIM3_IRQn;
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->EGR |= TIM_EGR_UG;
    for(x=0; x<100; x++)
        GPIOC->BSRR = 0; // stall for a while to make sure ISR runs.
    if ((GPIOC->ODR & (1<<9)) == 0)
        fail(12);
    GPIOC->BSRR = 1<<9;
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->EGR |= TIM_EGR_UG;
    for(x=0; x<100; x++)
        GPIOC->BSRR = 0; // stall for a while to make sure ISR runs.
    if ((GPIOC->ODR & (1<<9)) != 0)
        fail(12);

    // Q13:
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->ARR = 1;
    TIM3->PSC = 1;
    TIM3->DIER = 0;
    NVIC->ICER[0] = 1<<TIM3_IRQn;
    RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN;
    enable_tim3();
    if ((RCC->APB1ENR & RCC_APB1ENR_TIM3EN) == 0)
        fail(13);
    if (TIM3->ARR == 0)
        fail(13);
    if ((TIM3->ARR+1)*(TIM3->PSC+1) != 12000000)
        fail(13);
    if ((TIM3->DIER & TIM_DIER_UIE) == 0)
        fail(13);
    if ((TIM3->CR1 & TIM_CR1_CEN) == 0)
        fail(13);
    if ((NVIC->ISER[0] & (1<<TIM3_IRQn)) == 0)
        fail(13);

    // Total success!
    // If you made it this far through the tests, the
    // subroutines you wrote in hw5.s are working properly.
    for(;;)
        asm("bkpt"); // Success.

    return 0;
}
