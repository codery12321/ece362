.syntax unified
.cpu cortex-m0
.fpu softvfp
.thumb

//==================================================================
// ECE 362 Lab Experiment 4
// Interrupts
//==================================================================

// RCC config registers
.equ  RCC,      0x40021000
.equ  AHBENR,   0x014
.equ  GPIOAEN,  0x20000
.equ  GPIOBEN,  0x40000
.equ  GPIOCEN,  0x80000
.equ  APB2ENR,  0x018
.equ  SYSCFGCOMPEN, 1

// GPIO config registers
.equ  GPIOA,    0x48000000
.equ  GPIOB,    0x48000400
.equ  GPIOC,    0x48000800
.equ  MODER,    0
.equ  PUPDR,    0x0c
.equ  IDR,      0x10
.equ  ODR,      0x14
.equ  BSRR,     0x18
.equ  BRR,      0x28

// SYSCFG config registers
.equ SYSCFG, 0x40010000
.equ EXTICR1, 0x08
.equ EXTICR2, 0x0c
.equ EXTICR3, 0x10
.equ EXTICR4, 0x14

// External interrupt config registers
.equ EXTI,  0x40010400
.equ IMR,   0x00
.equ EMR,   0x04
.equ RTSR,  0x08
.equ FTSR,  0x0c
.equ SWIER, 0x10
.equ PR,    0x14

// Variables to register things for EXTI on pin 0
.equ EXTI_RTSR_TR0, 1<<0
.equ EXTI_IMR_MR0,  1<<0
.equ EXTI_PR_PR0,   1<<0
// Variables to register things for EXTI on pin 1
.equ EXTI_RTSR_TR1, 1<<1
.equ EXTI_IMR_MR1,  1<<1
.equ EXTI_PR_PR1,   1<<1
// Variables to register things for EXTI on pin 2
.equ EXTI_RTSR_TR2, 1<<2
.equ EXTI_IMR_MR2,  1<<2
.equ EXTI_PR_PR2,   1<<2
// Variables to register things for EXTI on pin 3
.equ EXTI_RTSR_TR3, 1<<3
.equ EXTI_IMR_MR3,  1<<3
.equ EXTI_PR_PR3,   1<<3
// Variables to register things for EXTI on pin 4
.equ EXTI_RTSR_TR4, 1<<4
.equ EXTI_IMR_MR4,  1<<4
.equ EXTI_PR_PR4,   1<<4

// SysTick counter variables....
.equ STK, 0xe000e010
.equ CSR, 0x00
.equ RVR, 0x04
.equ CVR, 0x08

// NVIC config registers
.equ NVIC, 0xe000e000
.equ ISER, 0x0100
.equ ICER, 0x0180
.equ ISPR, 0x0200
.equ ICPR, 0x0280
.equ IPR,  0x0400
.equ EXTI0_1_IRQn,5  // External interrupt number for pins 0 and 1 is IRQ 5.
.equ EXTI2_3_IRQn,6  // External interrupt number for pins 2 and 3 is IRQ 6.
.equ EXTI4_15_IRQn,7 // External interrupt number for pins 4 - 15 is IRQ 7.

// GPIO config registers
.equ  GPIOC,    0x48000800
.equ  GPIOB,    0x48000400
.equ  GPIOA,    0x48000000
.equ  MODER,    0x00
.equ  PUPDR,    0x0c
.equ  IDR,      0x10
.equ  ODR,      0x14
.equ  BSRR,     0x18
.equ  BRR,      0x28

.equ CLR4_9, 0xfff00
.equ OUTO4_9, 0x55500
.equ IN0_3,  0xff
.equ PD0_3,  0xaa
.equ CLR8_11,  0xff0000
.equ OUTO8_11,  0x550000
.equ CLR0_2_4, 0x3f3
.equ CLR2_3,   0xf0
.equ PD2_3,    0xa0
.equ CLR024, 0xfff0f
.equ B024,   0x11101
//==========================================================
// nano_wait
// Loop for approximately the specified number of nanoseconds.
// Write the entire subroutine below.

.global nano_wait
nano_wait:
	subs r0, #83
	bgt nano_wait
	bx lr

//==========================================================
// initc
// Enable the RCC clock for GPIO C and configure pins as
// described in section 2.2.1.
// Do not modify any other pin configuration.
// Parameters: none
// Write the entire subroutine below.
.global initc
initc:
    push    {lr}
    // Student code goes here
    //enable GPIOCs clock
	ldr r0, =RCC
	ldr r1,[r0,#AHBENR]
	ldr r2, =GPIOCEN
	orrs r1, r2
	str r1,[r0,#AHBENR]

	//configure Port C pins 4, 5, 6, 7 as outputs
	ldr r0, =GPIOC
	ldr r1, [r0,#MODER]
	ldr r2, =CLR4_9
	bics r1, r2
	ldr r2, =OUTO4_9
	orrs r1, r2
	ldr r2, =IN0_3
	bics r1, r2
	str r1, [r0, #MODER]
	ldr r1, [r0, #PUPDR]
	ldr r2, =IN0_3
	bics r1, r2
	ldr r2, =PD0_3
	orrs r1, r2
	str r1, [r0,#PUPDR]

    // End of student code
    pop     {pc}

//==========================================================
// initb
// Enable the RCC clock for GPIO B and configure pins as
// described in section 2.2.2
// Do not modify any other pin configuration.
// Parameters: none
// Write the entire subroutine below.

.global initb
initb:
    push    {lr}
    // Student code goes here
	//enable RCC clock for Port B
	ldr r0, =RCC
	ldr r1, [r0,#AHBENR]
	ldr r2, =GPIOBEN
	orrs r1, r2
	str r1,[r0,#AHBENR]

	//set pins PB8-PB11 as outputs
	//set pins PB0 & PB4 as inputs
	ldr r0, =GPIOB
	ldr r1,[r0,#MODER]
	ldr r2, =CLR8_11
	bics r1, r2
	ldr r2, =OUTO8_11 //load value to enable PB8-PB11 into r2
	orrs r1, r2
	ldr r2, =CLR0_2_4
	bics r1, r2
	str r1, [r0, #MODER] //store new bits into GPIOB_MODER

	//do i even need PUPDR here?
	ldr r1, [r0, #PUPDR]
	ldr r2, =CLR2_3
	bics r1, r2
	ldr r2, =PD2_3
	orrs r1, r2
	str r1, [r0, #PUPDR]

    // End of student code
    pop     {pc}

//==========================================================
// togglexn
// Change the ODR value from 0 to 1 or 1 to 0 for a specified
// pin of Port C.
// Parameters: r0 holds the base address of the GPIO port
//                to use
//             r1 holds the pin number to toggle
// Write the entire subroutine below.
.global togglexn
togglexn:
	push {lr}
	ldr r2, [r0, #ODR]
	movs r3, #1
	lsls r3, r1
	eors r2, r3
	str r2, [r0, #ODR]
	pop {pc}

//==========================================================
// Write the EXTI interrupt handler for pins 0 and 1 below.
// Copy the name from startup/startup_stm32.s, create a label
// of that name below, declare it to be global, and declare
// it to be a function.
// It acknowledge the pending bit for pin 0, and it should
// call togglexn(GPIOB, 8).
.global EXTI0_1_IRQHandler
.type EXTI0_1_IRQHandler, %function
EXTI0_1_IRQHandler:
	push {lr}
	ldr r0, =EXTI					//r0= address of EXTI
	ldr r1, [r0, #PR]			//r1 = offset address of PR
	ldr r2, =EXTI_PR_PR0	//r2 is address of EXIT_PR_PRO
	orrs r1, r2
	str r1, [r0, #PR]			//store value back to PR
	ldr r0, =GPIOB				//load GPIOB to pass into togglexn
	movs r1, #8						//arg 2
	bl togglexn						//call togglexn
	pop {pc}

//==========================================================
// Write the EXTI interrupt handler for pins 2-3 below.
// It should acknowledge the pending bit for pin2, and it
// should call togglexn(GPIOB, 9).
.global EXTI2_3_IRQHandler
.type EXTI2_3_IRQHandler, %function
EXTI2_3_IRQHandler:
	push {lr}
	ldr r0, =EXTI 				//r0 = address of EXTI
	ldr r1, [r0, #PR]
	ldr r2, =EXTI_PR_PR2
	orrs r1, r2
	str r1, [r0, #PR]
	ldr r0, =GPIOB
	movs r1, #9
	bl togglexn
	pop {pc}
//==========================================================
// Write the EXTI interrupt handler for pins 4-15 below.
// It should acknowledge the pending bit for pin4, and it
// should call togglxn(GPIOB, 10).
.global EXTI4_15_IRQHandler
.type EXTI4_15_IRQHandler, %function
EXTI4_15_IRQHandler:
	push {lr}
	ldr r0, =EXTI 				//r0 = address of EXTI
	ldr r1, [r0, #PR]
	ldr r2, =EXTI_PR_PR4
	orrs r1, r2
	str r1, [r0, #PR]
	ldr r0, =GPIOB
	movs r1, #10
	bl togglexn
	pop {pc}

//==========================================================
// init_exti
// (1) Enable the SYSCFG subsystem, and select Port B for
//     pins 0, 2, 3, and 4.
// (2) Configure the EXTI_RTSR register so that an EXTI
//     interrupt is generated on the rising edge of
//     pins 0, 2, 3, and 4.
// (3) Configure the EXTI_IMR register so that the EXTI
//     interrupts are unmasked for pins 2, 3, and 4.
// (4) Enable the three interupts for EXTI pins 0-1, 2-3 and
//     4-15. Dont enable any other interrupts.
// Parameters: none
.global init_exti
init_exti:
	push {lr}
	// Student code goes below
	//2.4.1 enable the SYSCFG subsystem
	ldr r0, =RCC
	ldr r1, [r0, #APB2ENR]
	ldr r2, =SYSCFGCOMPEN
	orrs r1, r2						//r2 is free
	str r1, [r0, #APB2ENR]

	//2.4.2 configure the port-to-pin assignment
	ldr r0, =SYSCFG
	ldr r1, [r0, EXTICR1]	//r1 is offset address of EXTICR1
	ldr r2, =CLR024				//load clr024
	bics r1,r2						//r2 is free
	ldr r3, =B024					//load b024
	orrs r1, r3						//r3 is free
	str r1, [r0, EXTICR1]	//r1 is free
	ldr r3, [r0, EXTICR2]	//EXTICR2 address
	ldr r2, =CLR024
	bics r3, r2						//r2 is free
	ldr r2, = B024				//load address of B024 into r2
	orrs r3, r2						//r2 is free
	str r3, [r0, EXTICR2]	//store r1 back to address

	//2.4.3 initialize the rtsr
	ldr r0, =EXTI
	ldr r1, [r0, #RTSR]										//r1 = offset address
	ldr r2, =EXTI_RTSR_TR0
	orrs r1, r2														//r2 is free
	ldr r2, =EXTI_RTSR_TR2								//load address
	orrs r1, r2
	ldr r2, =EXTI_RTSR_TR3
	orrs r1, r2
	ldr r2, =EXTI_RTSR_TR4
	orrs r1, r2
	str r1, [r0, #RTSR]										//store back to address

	//2.4.4 initialize the IMR
	ldr r0, =EXTI						//load address
	ldr r1, [r0, #IMR]			//load offset
	ldr r2, =EXTI_IMR_MR0		//load value of MR0
	orrs r1, r2							//r2 is free
	ldr r2, =EXTI_IMR_MR2
	orrs r1, r2
	ldr r2, =EXTI_IMR_MR3
	orrs r1, r2
	ldr r2, =EXTI_IMR_MR4
	orrs r1, r2
	str r1, [r0, #IMR]			//store back to IMR address
	//2.4.5 initialize the ISER
	ldr r3, =1<<EXTI0_1_IRQn //left shift 1 by EXTI0_1_IRQn //equal sign is op
	ldr r0, =NVIC
	ldr r1, =ISER
	str r3, [r0, r1]
	ldr r3, =1<<EXTI2_3_IRQn
	ldr r0, =NVIC
	ldr r1, =ISER
	str r3,[r0,r1]
	ldr r3, =1<<EXTI4_15_IRQn
	ldr r0, =NVIC
	ldr r1, =ISER
	str r3,[r0,r1]
	// Student code goes above
	pop  {pc}


//==========================================================
// set_col
// Set the specified column level to logic "high".
// Set the other three three columns to logic "low".
//r0 is input DONT CHANGE IT
.global set_col
set_col:
	push {lr}
	ldr r3, =GPIOC
	movs r1, #4
	adds r1, #16
	movs r2, #0xf
	lsls r2, r1
	str r2, [r3, #BSRR]

	ldr r3, =GPIOC
	movs r1, #8
	subs r1, r0
	movs r2, #1
	lsls r2, r1
	str r2, [r3, #BSRR]
	pop {pc}

//==========================================================
// The current_col variable.
.data
.global current_col
current_col:
        .word 1
.text


//==========================================================
// SysTick_Handler
// The ISR for the SysTick interrupt.
//
.global SysTick_Handler
.type SysTick_Handler, %function
SysTick_Handler:
	push {lr}
	// Student code goes below
	ldr r0, =GPIOC
	ldr r1,[r0, #IDR]			//GPIOC->IDR
	movs r2, #0xf
	ands r2, r1					//row_val = r2 = GPIOC->IDR & 0xf
	ldr r0, =current_col
	ldr r3, [r0]				//r3 = current_col
if1:
	cmp r3, #1					//is current_col == 1
	bne elif1					//if false, go to else if 1
	movs r1, #0x8
	ands r1, r2				//row_val & 0x8
	cmp r1, #0				//(row_val & 0x8) != 0
	beq elif1				//if false then go to else if 1
	ldr r0, =GPIOB			//arg 1 of togglexn
	movs r1, #8				//arg 2 of togglexn
	push {r3}
	bl togglexn				//togglexn is modifying r3 so have to push and pop everytime I call it
	pop {r3}
	b doneif1
elif1:
	cmp r3, #2				//is current_col == 2
	bne elif2				//if false, go to else if 2
	movs r1, #0x4
	ands r1, r2
	cmp r1, #0
	beq elif2
	ldr r0, =GPIOB
	movs r1, #9
	push {r3}
	bl togglexn
	pop {r3}
	b doneif1
elif2:
	cmp r3, #3				//is current_col == 3
	bne elif3				//if false, go to else if 3
	movs r1, #0x2
	ands r1, r2
	cmp r1, #0
	beq elif3
	ldr r0, =GPIOB
	movs r1, #10
	push {r3}
	bl togglexn
	pop {r3}
	b doneif1
elif3:
	cmp r3, #4				//is current_col == 4
	bne doneif1				//if false, go to doneif
	movs r1, #0x1
	ands r1, r2
	cmp r1, #0
	beq doneif1
	ldr r0, =GPIOB
	movs r1, #11
	push {r3}
	bl togglexn
	pop {r3}
	b doneif1
doneif1:
	adds r3, #1			//current_col +=1
if2:
	cmp r3, #4			//is current_col > 4 ?
	ble callset_col		//
	movs r3, #1
callset_col:
	ldr r0, =current_col	//get address of current_col
	str r3, [r0]			//store current_col value into r3
	movs r0, r3				//move value of current_col into arg 1 of set_col
	bl set_col
	// Student code goes above
	pop  {pc}

//==========================================================
// init_systick
// Enable the SysTick interrupt to occur every 0.5 seconds.
// Parameters: none
.global init_systick
init_systick:
	push {lr}
	// Student code goes below
	ldr r0, =STK
	ldr r1, =0x5B8D7
	str r1, [r0, #RVR]
	ldr r2, =0x3
	str r2, [r0, #CSR]
	// Student code goes above
	pop  {pc}


//==========================================================
// adjust_priorities
// Set the priority for EXTI pins 2-3 interrupt to 192.
// Set the priority for EXTI pins 4-15 interrupt to 128.
// Do not adjust the priority for any other interrupts.
.global adjust_priorities
adjust_priorities:
	push {lr}
	// Student code goes below

	// Student code goes above
	pop  {pc}

//==========================================================
// The main subroutine will call everything else.
// It will never return.
.global main
main:
	bl autotest // Uncomment when most things are working
	bl initb
	bl initc
	bl init_exti
	bl init_systick
	bl adjust_priorities

endless_loop:
	ldr  r0,=GPIOC
	movs r1,#9
	bl   togglexn
	ldr  r0,=500000000
	bl   nano_wait
	b    endless_loop
