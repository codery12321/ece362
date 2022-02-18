.cpu cortex-m0
.thumb
.syntax unified
.fpu softvfp

.equ RCC,       0x40021000
.equ GPIOA,     0x48000000
.equ GPIOB,     0x48000400
.equ GPIOC,     0x48000800
.equ AHBENR,    0x14
.equ APB2ENR,   0x18
.equ APB1ENR,   0x1c
.equ IOPAEN,    0x20000
.equ IOPBEN,    0x40000
.equ IOPCEN,    0x80000
.equ SYSCFGCOMPEN, 1
.equ TIM3EN,    2
.equ MODER,     0
.equ OSPEEDR,   8
.equ PUPDR,     0xc
.equ IDR,       0x10
.equ ODR,       0x14
.equ BSRR,      0x18
.equ BRR,       0x28
.equ PC8,       0x100

// SYSCFG control registers
.equ SYSCFG,    0x40010000
.equ EXTICR1,   0x8
.equ EXTICR2,   0xc
.equ EXTICR3,   0x10
.equ EXTICR4,   0x14

// NVIC control registers
.equ NVIC,      0xe000e000
.equ ISER,      0x100

// External interrupt control registers
.equ EXTI,      0x40010400
.equ IMR,       0x00
.equ RTSR,      0x08
.equ PR,        0x14

.equ TIM3,      0x40000400
.equ TIMCR1,    0x00
.equ DIER,      0x0c
.equ TIMSR,     0x10
.equ PSC,       0x28
.equ ARR,       0x2c

// Popular interrupt numbers
.equ EXTI0_1_IRQn,   5
.equ EXTI2_3_IRQn,   6
.equ EXTI4_15_IRQn,  7
.equ EXTI4_15_IRQn,  7
.equ TIM2_IRQn,      15
.equ TIM3_IRQn,      16
.equ TIM6_DAC_IRQn,  17
.equ TIM7_IRQn,      18
.equ TIM14_IRQn,     19
.equ TIM15_IRQn,     20
.equ TIM16_IRQn,     21
.equ TIM17_IRQn,     22

//====================================================================
// Q1
//====================================================================
.global recur
recur:
  push {lr}
if1:
  movs r1, #3
  cmp r0, r1
  bhs if2
  bl done
if2:
  movs r1, #0xf
  movs r2, r0
  ands r2, r1
  movs r1, #0
  cmp r2, r1
  bne return
  subs r0, #1
  bl recur
  adds r0, #1
  bl done
return:
  lsrs r0, #1
  bl recur
  adds r0, #2
done:
  pop {pc}

//====================================================================
// Q2
//====================================================================
.global enable_portb
enable_portb:
  push {lr}
  ldr r0, =RCC
  ldr r1, [r0, #AHBENR]
  ldr r2, =IOPBEN
  orrs r2, r1
  str r2, [r0, #AHBENR]
  pop {pc}
//====================================================================
// Q3
//====================================================================
.global enable_portc
enable_portc:
  push {lr}
  ldr r0, =RCC
  ldr r1, [r0, #AHBENR]
  ldr r2, =IOPCEN
  orrs r2, r1
  str r2, [r0, #AHBENR]
  pop {pc}
//====================================================================
// Q4
//====================================================================
.global setup_pb3
setup_pb3:
  //push {lr}
  //.equ CLR3, 0xc0  //1100 0000
  //.equ IN3,  0x100
  //.equ PD3, 0x80

  ldr r0, =GPIOB
  ldr r1, [r0, #MODER]
  ldr r2, =0xc0
  bics r1, r2
  str r1, [r0, #MODER]

  ldr r1, [r0, #PUPDR]
  ldr r2, =0xc0
  bics r1, r2
  ldr r2, =0x80
  orrs r1, r2
  str r1, [r0, #PUPDR]
	bx lr
  //pop {pc}
//====================================================================
// Q5
//====================================================================
.global setup_pb4
setup_pb4:
  push {lr}
  ldr r0, =GPIOB
  ldr r1, [r0, #MODER]
  ldr r2, =0x300
  bics r1, r2
  str r1, [r0, #MODER]
  ldr r1, [r0, #PUPDR]
  ldr r2, =0x300
  bics r1, r2
  str r1, [r0, #PUPDR]
  pop {pc}
//====================================================================
// Q6
//====================================================================
.global setup_pc8
setup_pc8:
  push {lr}
  //.equ CLR8, 0x30000
  //.equ OUT8, 0x10000
  //.equ SPD8, 0x30000
  ldr r0, =GPIOC
  ldr r1, [r0, #MODER]
  ldr r2, =0x30000
  bics r1, r2
  str r1, [r0, #MODER]
  ldr r2, =0x10000
  orrs r1, r2
  str r1, [r0, #MODER]      //is this necessary? store 2 times?
  ldr r1, [r0, #OSPEEDR]
  ldr r2, =0x30000
  orrs r1, r2
  str r1, [r0, #OSPEEDR]
  pop {pc}
//====================================================================
// Q7
//====================================================================
.global setup_pc9
setup_pc9:
  //push {lr}
  //.equ CLR9, 0xc0000 //1100 0000 0000 0000 0000
  //.equ OUT9, 0x40000
  //.equ SPD9, 0x80000
  ldr r0, =GPIOC
  ldr r1, [r0, #MODER]
  ldr r2, =0xc0000
  bics r1, r2
  str r1, [r0, #MODER]
  ldr r2, =0x40000
  orrs r1, r2
  str r1, [r0, #MODER]
  ldr r1, [r0, #OSPEEDR]
  ldr r2, =0xc0000
  bics r1, r2
  ldr r2, =0x40000
  orrs r1, r2
  str r1, [r0, #OSPEEDR]
  bx lr
  //pop {pc}
//====================================================================
// Q8
//====================================================================
//c code
/*
if (pb3 == 1 && pb4 == 0)
  set pc8 = 0
else
  set pc8 = 1
*/
.global action8
action8:
  ldr r0, =GPIOB
  ldr r1, [r0, #IDR]
  ldr r3, =0x8 //pb3 is bit 3 of odr binary: 1000
  cmp r1, r3
  bne else
  ldr r3, =0x10	//binary: 1 0000
  cmp r1, r3
  beq else
  //set pc8 to 0
  ldr r2, =GPIOC
  ldr r3, =0x100
  bics r3, r1         //set bit 8 to 0
  str r3, [r2, #ODR]
  bl doneaction8
else:
  ldr r3, =0x100
  orrs r3, r1         //set pc8 to 1
  str r3, [r2, #ODR]
doneaction8:
  pop {pc}

//====================================================================
// Q9
//====================================================================
/*
if (pb3 == 0 && pb4 == 1)
  set pc9 = 1
else
  set pc9 = 0
*/
.global action9
action9:
  ldr r0, =GPIOB
  ldr r1, [r0, #ODR]
  //check if pb3 == 0
  ldr r2, =0x4        //pb3 is bit 3 of ODR
  cmp r1, r2
  beq else9
  ldr r2, =0x8        //pb4 is bit 4 of ODR
  cmp r1, r2
  bne else9
  ldr r2, =0x200
  orrs r2, r1         //set bit 9 to 1
  str r2, [r0, #ODR]
  bl doneaction9
else9:
  ldr r2, =0x200
  bics r2, r1         //set pc9 to 0
  str r2, [r0, #ODR]
doneaction9:
  pop {pc}
//====================================================================
// Q10
//====================================================================
// Do everything needed to write the ISR here...
.global EXTI2_3_IRQHandler
.type EXTI2_3_IRQHandler, %function
EXTI2_3_IRQHandler:
  push {lr}
  .equ EXTI_PR_PR2,   1<<2
  ldr r0, =EXTI
  ldr r1, [r0, #PR]
  ldr r2, =EXTI_PR_PR2
  orrs r1, r2
  str r1, [r0, #PR]
  //counter
  ldr r3, =counter
  ldr r3, [r3]
  adds r2, r3, #1
  str r2, [r3]
  pop {pc}
//====================================================================
// Q11
//====================================================================
.global enable_exti
enable_exti:
  push {lr}
  .equ CLR2, 0xf00
  .equ B2, 0x100

  //enable system clock to the SYSCFG subsystem
  ldr r0, =RCC
  ldr r1, [r0, #APB2ENR]
  ldr r2, =SYSCFGCOMPEN
  orrs r1, r2
  str r1, [r0, APB2ENR]

  //set up SYSCFG external interrupt configuration registers
  ldr r0, =SYSCFG
  ldr r1, [r0, EXTICR1]
  ldr r2, =CLR2
  bics r2, r1
  ldr r2, =B2
  orrs r1, r2
  str r1, [r0, EXTICR1]

  //configure the EXTI_RTSR
  ldr r0, =EXTI
  ldr r1, [r0, #RTSR]
  ldr r2, =0x4
	orrs r1, r2
  str r1, [r0, #RTSR]

  //set the EXTI_IMR to not ignore pin number 2
  ldr r0, =EXTI
  ldr r1, [r0, #IMR]
  ldr r2, =0x4
  orrs r2, r1
  str r2, [r0, #IMR]

  //configure the NVIC to enable the interrupt for the ISR
  ldr r0, =1<<EXTI2_3_IRQn
  ldr r1, =NVIC
  ldr r2, =ISER
  str r0, [r1, r2]
  pop {pc}
//====================================================================
// Q12
//====================================================================
// Do everything needed to write the ISR here...
.global TIM3_IRQHandler
.type TIM3_IRQHandler, %function
TIM3_IRQHandler:
  push {lr}
  .equ TIM_SR_UIF,   1<<0
  ldr r0, =TIM3
  ldr r1, [r0, #TIMSR]
  ldr r2, =TIM_SR_UIF
  bics r2, r1
  str r2, [r0, #TIMSR]

  //toggle PC9
  ldr r0, =GPIOC
  ldr r1, [r0, ODR]
  movs r2, #1
  lsls r2, #9
  ands r1, r2
  cmp r1, #0
  beq elsetim3
  str r2, [r0, #BRR]
  bl donetim3
elsetim3:
  str r2, [r0, #BSRR]
donetim3:
  pop {pc}
//====================================================================
// Q13
//====================================================================
.global enable_tim3
enable_tim3:
  push {lr}
  .equ TIM3_DIER_UIE, 1<<0
  .equ TIM_CR1_CEN,  1<<0
  //Enables the system clock to the timer 3 subsystem.
  ldr r0, =RCC
  ldr r1, [r0, #APB1ENR]
  ldr r2, =TIM3EN
  orrs r2, r1
  str r2, [r0, #APB1ENR]

  //Configures the Auto-Reload Register and Prescaler of Timer 3 so that an update event occurs exactly four times per second.
  ldr r0, =TIM3
  ldr r1, =4800-1   //PSC
  ldr r2, =2500-1 //assuming its 48Mhz clock, ARR=(10000/4)-1
  str r1, [r0, #PSC]
  str r2, [r0, #ARR]

  //Set the DIER of Timer 3 so that an interrupt occurs on an update event
  ldr r1, [r0, #DIER]
  ldr r2, =TIM3_DIER_UIE
  orrs r2, r1
  str r2, [r0, #DIER]

  //Write the appropriate bit to the NVIC ISER so that the interrupt for the Timer 3 update event is enabled.
  ldr r0, =NVIC
  ldr r1, =ISER
  ldr r2, =(1<<TIM3_IRQn)
  str r2, [r0, r1]

  //Enable the counter for Timer 3.
  ldr r1, [r0, #TIMCR1]
  ldr r2, =TIM_CR1_CEN
  orrs r2, r1
  str r2, [r0, #TIMCR1]
  pop {pc}
