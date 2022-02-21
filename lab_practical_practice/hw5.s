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
  movs r1, #3
  cmp r0, r1
  bhs if2
  bl done
if2:
  ldr r1, =0xf
  ands r1, r0
  cmp r1, #0
  bne returnRecur
  subs r0, #1
  bl recur
  adds r0, #1
  bl done
returnRecur:
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
  orrs r1, r2
  str r1, [r0, #AHBENR]
  pop {pc}
//====================================================================
// Q3
//====================================================================
.global enable_portc
enable_portc:
  push {lr}
   ldr r0, =RCC
   ldr r1, [r0, #AHBENR]
   ldr r2, = IOPCEN
   orrs r1, r2
   str r1, [r0, #AHBENR]
  pop {pc}
//====================================================================
// Q4
//====================================================================
.global setup_pb3
setup_pb3:
push {lr}
ldr r0, =GPIOB
ldr r1, [r0, #MODER]
ldr r2, =0xc0       //1100 0000
bics r1, r2
str r1, [r0, #MODER]

ldr r1, [r0, #PUPDR]
ldr r2, =0xc0
bics r1, r2
str r1, [r0, #PUPDR]
ldr r2, =0x40       //0100 0000
orrs r1, r2
str r1, [r0, #PUPDR]
pop {pc}
//====================================================================
// Q5
//====================================================================
.global setup_pb4
setup_pb4:
push {lr}
ldr r0, =GPIOC
ldr r1, [r0, #MODER]
ldr r2, =0x300     //0011 0000 0000
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
ldr r0, =GPIOC
ldr r1, [r0, #MODER]
ldr r2, =0x30000
bics r1, r2
str r1, [r0, #MODER]

ldr r1, [r0, #OSPEEDR]
ldr r2, =0x30000     //11 0000 0000 0000 0000
bics r1, r2
str r1, [r0, #OSPEEDR]
ldr r2, =0x30000
orrs r1, r2
str r1, [r0, #OSPEEDR]
pop {pc}
//====================================================================
// Q7
//====================================================================
.global setup_pc9
setup_pc9:
push {lr}
ldr r0, =GPIOC
ldr r1, [r0, #MODER]
ldr r2, =0x30000
bics r1, r2
str r1, [r0, #MODER]

ldr r1, [r0, #OSPEEDR]
ldr r2, =0x30000
bics r1, r2
str r1, [r0, #OSPEEDR]
ldr r2, =0x10000
orrs r1, r2
str r1, [r0, #OSPEEDR]
pop {pc}
//====================================================================
// Q8
//====================================================================
.global action8
action8:
push {lr}

pop {pc}
//====================================================================
// Q9
//====================================================================
.global action9
action9:
push {lr}

pop {pc}
//====================================================================
// Q10
//====================================================================
// Do everything needed to write the ISR here...
.global EXTI2_3_IRQHandler
.type EXTI2_3_IRQHandler, %function
EXTI2_3_IRQHandler:
push {lr}
//acknowledge the interrupt
ldr r0, =EXTI
ldr r1, [r0, #PR]
ldr r2, =1<<2
orrs r1, r2
str r1, [r0, #PR]
ldr r1, =counter
ldr r2, [r1]
adds r2, #1
str r2, [r1]
pop {pc}
//====================================================================
// Q11
//====================================================================
.global enable_exti
enable_exti:
push {lr}
//enable the system clock to the SYSCFG subsystem
ldr r0, =RCC
ldr r1, [r0, #APB2ENR]
ldr r2, =SYSCFGCOMPEN
orrs r1, r2
str r1, [r0, #APB2ENR]

ldr r0, =SYSCFG
ldr r1, [r0, #EXTICR1]
ldr r2, =0xff00
bics r1, r2
str r1, [r0, #EXTICR1]
ldr r2, =0x100
orrs r1, r2
str r1, [r0, #EXTICR1]

//configure EXTI_RTSR
ldr r0, =EXTI
ldr r1, [r0, #RTSR]
ldr r2, =0x4
orrs r1, r2
str r1, [r0, #RTSR]

//set EXTI_IMR to not ignore pin #2
ldr r0, =EXTI
ldr r1, [r0, #IMR]
ldr r2, =0x4
orrs r1, r2
str r1, [r0, #IMR]

//configure NVIC to enable the interrupt for the ISR
ldr r0, =NVIC
ldr r1, = ISER
ldr r2, =1<<EXTI2_3_IRQn
orrs r1, r2
str r1, [r0, r1]
pop {pc}
//====================================================================
// Q12
//====================================================================
// Do everything needed to write the ISR here...
.global TIM3_IRQHandler
.type TIM3_IRQHandler, %function
TIM3_IRQHandler:
push {lr}
ldr r0, =TIM3
ldr r1, [r0, #TIMSR]
ldr r2, =TIM_SR_UIF
bics r1, r2
str r1, [r0, #TIMSR]

//toggle PC9
ldr r0, =GPIOC
ldr r1, [r0, #ODR]
movs r2, #1
cmp r1, r2
lsls r2, #9
ands r1, r2
cmp r1, #0
beq elsetim3        //if odr == 0 means its off
str r2, [r0, #BRR]
bl donetime3
elsetime3:
str r1, [r0, #BSRR]
donetim3:
pop {pc}
//====================================================================
// Q13
//====================================================================
.global enable_tim3
enable_tim3:
push {lr}
ldr r0, =RCC
ldr r1, [r0, #APB1ENR]
ldr r2, =TIM3EN
orrs r1, r2
str r1, [r0, #APB1ENR]

ldr r0, =TIM3
ldr r1, = 48000-1
ldr r2, =250-1
str r1, [r0, #PSC]
str r2, [r0, #ARR]

ldr r1, [r0, #DIER]
ldr r2, =1<<0
orrs r1, r2
str r1, [r0, #DIER]

ldr r0, =NVIC
ldr r1, =ISER
ldr r2, =1<<TIM3_IRQn
str r2, [r0, r1]

ldr r0, =TIM3
ldr r1, [r0, #TIMCR1]
ldr r2, =1<<0
orrs r1, r2
str r1, [r0, #TIMCR1]
pop {pc}
