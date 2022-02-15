.syntax unified
.cpu cortex-m0
.fpu softvfp
.thumb

.equ  RCC,      0x40021000
.equ  AHBENR,   0x014
.equ  GPIOAEN,  0x20000
.equ  GPIOBEN,  0x40000
.equ  GPIOCEN,  0x80000
.equ  GPIOA,    0x48000000
.equ  GPIOB,    0x48000400
.equ  GPIOC,    0x48000800
.equ  MODER,    0x00
.equ  PUPDR,    0x0c
.equ  IDR,      0x10
.equ  ODR,      0x14
.equ  BSRR,     0x18
.equ  BRR,      0x28
.equ STK, 0xe000e010
.equ CSR, 0x0
.equ CVR, 0x08
.equ RVR, 0x4


.global initc
initc:
    push    {lr}
    // Student code goes here
    //enable GPIOC's clock
	ldr r0, =RCC
	ldr r1,[r0,#AHBENR]
	ldr r2, =0x80000
	orrs r1, r2
	str r1,[r0,#AHBENR]

	//configure Port C pins 4, 5, 6, 7 as outputs
	ldr r0, =GPIOC
	ldr r1, [r0,#MODER]
	ldr r2, =0x00003000
	bics r1, r2

	ldr r2, =0x00001000
	orrs r1, r2

	str r1, [r0, #MODER]
	bx lr
    // End of student code
    pop     {pc}
.type SysTick_Handler, %function
.global SysTick_Handler
SysTick_Handler:
	ldr r1, =GPIOC
	ldr r2, [r1, #ODR]
	ldr r3, =0x40
	eors r2, r3
	str r2, [r1, #ODR]

.global systick
systick:
	ldr r3, =STK
	ldr r0, =4499999
	str r0, [r3, #CVR]
	ldr r0, =2999999
	str r0, [r3, #RVR]

	movs r0, #3
	str r0, [r3, #CSR]
endless:
	b endless


.global main
main:

	bl initc
	bl systick
