.syntax unified
.cpu cortex-m0
.fpu softvfp
.thumb

//==================================================================
// ECE 362 Lab Experiment 3
// General Purpose I/O
//==================================================================

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
.equ  OUTO8_11,	0x00550000
.equ  OUTO4_7,  0x00005500
.equ  OUTO7, 	0x00004000

//==========================================================
// initb:
// Enable Port B in the RCC AHBENR register and configure
// the pins as described in section 2.1 of the lab
// No parameters.
// No expected return value.

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
	ldr r2, =0x00ff0303
	bics r1, r2
	str r1, [r0, #MODER] //r1 newest already
	ldr r2, =OUTO8_11 //load value to enable PB8-PB11 into r2
	orrs r1, r2
	str r1, [r0, #MODER] //store new bits into GPIOB_MODER

    // End of student code
    pop     {pc}

//==========================================================
// initc:
// Enable Port C in the RCC AHBENR register and configure
// the pins as described in section 2.2 of the lab
// No parameters.
// No expected return value.
.global initc
initc:
    push    {lr}
    // Student code goes here
    //enable GPIOC's clock
	ldr r0, =RCC
	ldr r1,[r0,#AHBENR]
	ldr r2, =GPIOCEN
	orrs r1, r2
	str r1,[r0,#AHBENR]

	//configure Port C pins 4, 5, 6, 7 as outputs
	ldr r0, =GPIOC
	ldr r1, [r0,#MODER]
	ldr r2, =0x0000ffff
	bics r1, r2
	str r1, [r0, #MODER]
	ldr r2, =OUTO4_7
	orrs r1, r2
	str r1, [r0,#MODER]

	//lab explanation says: a similar process can be
	//used to read and set values in the PUPDR register
	//WHAT DOES THAT MEAN?!!?!?!?
	//PUPDR section
	ldr r0, =GPIOC
	ldr r1, [r0, #PUPDR]
	ldr r2, =0x000000ff
	bics r1, r2
	str r1, [r0, #PUPDR]
	ldr r2, =0x000000aa
	orrs r1, r2
	str r1, [r0,#PUPDR]

    // End of student code
    pop     {pc}

//==========================================================
// setn:
// Set given pin in GPIOB to given value in ODR
// Param 1 - pin number
// param 2 - value [zero or non-zero]
// No expected retern value.
.global setn
setn:
    push    {R4, lr}
    // Student code goes here
    //r0= pin # r1 = value to set the pin to
    ldr r4, =GPIOB
    movs r1, r1  //set flags
    beq clear
set:
	movs r2, #1			//ldr r2, =0x1
	lsls r2, r2, r0
	//ldr r4, =GPIOB
	str r2, [r4, #BSRR]
	b doneSetn
clear:
	movs r2, #1
	lsls r2,r2, r0
	//ldr r4, =GPIOB
	str r2, [r4, #BRR]
    // End of student code

doneSetn:
    pop     {R4, pc}

//==========================================================
// readpin:
// read the pin given in param 1 from GPIOB_IDR
// Param 1 - pin to read
// No expected return value.
.global readpin
readpin:
    push    {lr}
    // Student code goes here
	//r0 = bit number r1 = value in register r0
	ldr r1, =GPIOB
	ldr r1, [r1, #IDR]
	//movs r2, #1
	ldr r2, =0x1 		//use mov to save memory
	lsls r2, r2, r0
	ands r1, r2
	lsrs r1, r1, r0
	movs r0, r1

    // End of student code
    pop     {pc}

//==========================================================
// buttons:
// Check the pushbuttons and turn a light on or off as
// described in section 2.6 of the lab
// No parameters.
// No return value
.global buttons
buttons:
    push    {lr}
    // Student code goes here
	movs r0, #0
	bl readpin
	movs r1, r0		//r1 = readpin return value
	movs r0, #8
	bl setn

	movs r0, #4
	bl readpin
	movs r1, r0		//r1 = readpin return value
	movs r0, #9
	bl setn

    // End of student code
    pop     {pc}

//==========================================================
// keypad:
// Cycle through columns and check rows of keypad to turn
// LEDs on or off as described in section 2.7 of the lab
// No parameters.
// No expected return value.
.global keypad
keypad:
    push    {R4, R5, lr}
    movs r3, #8  		//c = 8
    // Student code goes here
loop:
	cmp r3, #0			//c>0
	ble done			//check for loop condition

	ldr r5, =GPIOC		//load address of GPIOC
	lsls r1, r3, #4		//r1 = c << 4
	str r1, [r5, #ODR]  //GPIOC->ODR = c << 4
	bl mysleep			//mysleep()

	movs r2, #0xf
	ldr r4,[r5, #IDR]	//r4 = GPIOC->IDR
	ands r2, r4			//r2 = 0xf & GPIOC->IDR
	//movs r2, r4		//r2 = GPIOC->IDR & 0xf

	cmp r3, #8			//if(c == 8)
	bne elseif1
if:
	movs r4, #1
	ands r2, r4			//r2 & 1
	movs r0, #8
	movs r1, r2			//make parameter 1, 2 => r0, r1
	bl setn
	b doneloop
elseif1:
	cmp r3, #4
	bne elseif2			//else if (c == 4)

	movs r4, #2
	ands r2, r4			//r2 & 2
	movs r1, r2
	movs r0, #9			//r0, #9
	bl setn
	b doneloop
elseif2:
	cmp r3, #2
	bne else			//else if (c == 2)

	movs r4, #4
	ands r2, r4			//r1 = r2 &
	movs r1, r2
	movs r0, #10
	bl setn
	b doneloop
else:
	movs r4, #8
	ands r2, r4
	movs r1, r2
	movs r0, #11
	bl setn
	b doneloop
doneloop:
	lsrs r3, #1  		//c = c >> 1
	b loop
done:
    // End of student code
    pop     {R4, R5, pc}

//==========================================================
// mysleep:
// a do nothing loop so that row lines can be charged
// as described in section 2.7 of the lab
// No parameters.
// No expected return value.
.global mysleep
mysleep:
    push    {lr}
    // Student code goes here
	movs r0, #0
	ldr r1, =0x3e8 //dec 1000
mysleeploop: //loop and compare and increment
	cmp r0, r1
	bge donemysleep
	adds r0, #1
	b mysleeploop
donemysleep:
    // End of student code
    pop     {pc}


//==========================================================
// The main subroutine calls everything else.
// It never returns.
.global main
main:
    push {lr}
    bl   autotest // Uncomment when most things are working
    bl   initb
    bl   initc
// uncomment one of the loops, below, when ready
//loop1:
//    bl   buttons
//    b    loop1
//loop2:
//    bl   keypad
//    b    loop2

    wfi
    pop {pc}
