.cpu cortex-m0
.thumb
.syntax unified

// RCC configuration registers
.equ  RCC,      0x40021000
.equ  AHBENR,   0x014
.equ  GPIOCEN,  0x080000
.equ  GPIOBEN,  0x040000
.equ  GPIOAEN,  0x020000
.equ  APB1ENR,  0x01c
.equ  TIM6EN,   1<<4
.equ  TIM7EN,   1<<5
.equ  TIM14EN,  1<<8

// NVIC configuration registers
.equ NVIC, 0xe000e000
.equ ISER, 0x0100
.equ ICER, 0x0180
.equ ISPR, 0x0200
.equ ICPR, 0x0280
.equ IPR,  0x0400
.equ TIM6_DAC_IRQn, 17
.equ TIM7_IRQn,     18
.equ TIM14_IRQn,    19

// Timer configuration registers
.equ TIM6,   0x40001000
.equ TIM7,   0x40001400
.equ TIM14,  0x40002000
.equ TIM_CR1,  0x00
.equ TIM_CR2,  0x04
.equ TIM_DIER, 0x0c
.equ TIM_SR,   0x10
.equ TIM_EGR,  0x14
.equ TIM_CNT,  0x24
.equ TIM_PSC,  0x28
.equ TIM_ARR,  0x2c

// Timer configuration register bits
.equ TIM_CR1_CEN,  1<<0
.equ TIM_DIER_UDE, 1<<8
.equ TIM_DIER_UIE, 1<<0
.equ TIM_SR_UIF,   1<<0

// GPIO configuration registers
.equ  GPIOC,    0x48000800
.equ  GPIOB,    0x48000400
.equ  GPIOA,    0x48000000
.equ  MODER,    0x0
.equ  PUPDR,    0xc
.equ  IDR,      0x10
.equ  ODR,      0x14
.equ  BSRR,     0x18
.equ  BRR,      0x28

//============================================================================
// enable_ports() {
// Set up the ports and pins exactly as directed.
// }
.global enable_ports
enable_ports:
  // Enable the RCC clock for GPIOC and configure pins
  push    {lr}
  // Student code goes here
  .equ CLR0_10,   0x003fffff
  .equ OUTOB0_10, 0x00155555
  .equ CLR4_8,    0x0003ff00
  .equ OUTO4_8,   0x00015500
  .equ IN0_3,     0x000000ff
  .equ PD0_3,     0x000000aa

  //enable GPIOC and GPIOBs clock
	ldr r0, =RCC
	ldr r1,[r0,#AHBENR]
	ldr r2, =GPIOCEN
  ldr r3, =GPIOBEN
	orrs r1, r2
	str r1,[r0,#AHBENR]
  orrs r1, r3
  str r1, [r0, #AHBENR]

  //configure port B pins 0-10 as outputs
  ldr r0, =GPIOB
  ldr r1,[r0,#MODER]
  ldr r2, =CLR0_10
  bics r1, r2               //clear the bits
  str r1, [r0, #MODER]
  ldr r2, =OUTOB0_10        //load value to enable PB0 - PB10 into r2
  orrs r1, r2
  str r1, [r0, #MODER]      //store new bits into GPIOB_MODER

	//configure Port C pins 4-8 as outputs
  //pins 0-3 as inputs and internally pulled low
	ldr r0, =GPIOC
	ldr r1, [r0,#MODER]
	ldr r2, =CLR4_8        //clear bits for pins 4-8
	bics r1, r2
	ldr r2, =OUTO4_8       //load MODER bits as 01 to configure it as output
	orrs r1, r2
	ldr r2, =IN0_3         //clear bits 0-8 for pins 0-3 too
	bics r1, r2
	str r1, [r0, #MODER]
	ldr r1, [r0, #PUPDR]   //pull down configuration
	ldr r2, =IN0_3         //clear bits 0-8
	bics r1, r2
	ldr r2, =PD0_3         //enable pulldown for bits 0-8 (pins 0-3)
	orrs r1, r2
	str r1, [r0,#PUPDR]    //store it back to PUPDR
  // End of student code
  pop     {pc}

//============================================================================
// TIM6_ISR() {
//   TIM6->SR &= ~TIM_SR_UIF
//   if (GPIOC->ODR & (1<<8))
//     GPIOC->BRR = 1<<8;
//   else
//     GPIOC->BSRR = 1<<8;
// }
.global TIM6_DAC_IRQHandler
.type TIM6_DAC_IRQHandler, % function
TIM6_DAC_IRQHandler:
	push {lr}
  //acknowledge the interrupt
  ldr r0, =TIM6
  ldr r1, [r0, #TIM_SR]
  ldr r2, =TIM_SR_UIF
  bics r1, r2             //turn off UIF
  str r1, [r0, #TIM_SR]

  //Toggle the PC8 bit
  ldr r0, =GPIOC
  ldr r1, [r0, #ODR]      //load ODR into r1
  movs r2, #1
  lsls r2, #8            	//r2 = 1<<8
  ands r1, r2
  cmp r1, #0
  beq else
  str r2, [r0, #BRR]
  bl done
else:
  str r2, [r0, #BSRR]
done:
  pop {pc}

//============================================================================
// Implement the setup_tim6 subroutine below.  Follow the instructions in the
// lab text.
.global setup_tim6
setup_tim6:
  push {lr}
  //enable RCC clock for TIM6
  ldr r0, =RCC
  ldr r1, [r0, #APB1ENR]
  ldr r2, =TIM6EN         //TIM6EN is bit 4 of APB1ENR
  orrs r1, r2
  str r1, [r0, #APB1ENR]

  //Configure TIM6_PSC to prescale the system clock by 48000
  ldr r0, =TIM6
  ldr r2, =48000-1  		//PSC value
  str r2, [r0, #TIM_PSC]
  ldr r2, =500-1 				//ARR value
  str r2, [r0, #TIM_ARR]

  //Configure the Timer 6 DMA/Interrupt Enable Register (TIM6_DIER) to enable the UIE flag
  ldr r2, =TIM_DIER_UIE
  str r2, [r0, #TIM_DIER]

  //Enable Timer 6 to start counting by setting the CEN bit in the Timer 6 Control Register 1.
  ldr r1, [r0, #TIM_CR1]
  ldr r2, =TIM_CR1_CEN
  orrs r2, r1
  str r2, [r0, #TIM_CR1]

  //Enable the interrupt for Timer 6 in the NVIC ISER
  ldr r0, =NVIC
  ldr r1, =ISER
  ldr r2, =1<<TIM6_DAC_IRQn
  str r2, [r0, r1]
  pop {pc}

//============================================================================
// void show_char(int col, char ch) {
//   GPIOB->ODR = ((col & 7) << 8) | font[ch];
// }
.global show_char
show_char:
  push {r4-r7, lr}
  ldr r4, =GPIOB      //r4 is address of GPIOB
  movs r2, #7         //r2 = 7
  ands r2, r0         //col & 7
  lsls r2, #8         //r2 = ((col & 7) << 8)
  //lsls r1, #2         //ch*4
  ldr r5, =font
  //ldr r5, [r5]      //load font from read-only mem
  ldrb r6, [r5, r1]   //r6 = font[ch]
  orrs r2, r6
  str r2, [r4, #ODR]
  pop {r4-r7, pc}

//============================================================================
// nano_wait(int x)
// Wait the number of nanoseconds specified by x.
.global nano_wait
nano_wait:
	subs r0,#83
	bgt nano_wait
	bx lr

//============================================================================
// This function is provided for you to fill the LED matrix with AbCdEFg.
// It is a very useful function.  Study it carefully.
.global fill_alpha
fill_alpha:
	push {r4,r5,lr}
	movs r4,#0
fillloop:
	movs r5,#'A' // load the character 'A' (integer value 65)
	adds r5,r4
	movs r0,r4
	movs r1,r5
	bl   show_char
	adds r4,#1
	movs r0,#7
	ands r4,r0
	ldr  r0,=1000000
	bl   nano_wait
	b    fillloop
	pop {r4,r5,pc} // not actually reached

//============================================================================
// void drive_column(int c) {
//   c = c & 3;
//   GPIOC->BSRR = 0xf00000 | (1 << (c + 4));
// }
.global drive_column
drive_column:
	push {r4-r7, lr}
  movs r1, #3
  ands r1, r0       //r1 = c&3
  ldr r2, =GPIOC
  adds r1, #4
  movs r4, #1
  lsls r4, r1
  ldr r3, =0xf00000
  orrs r3, r4
  str r3, [r2, #BSRR]
  	pop {r4-r7, pc}

//============================================================================
// int read_rows(void) {
//   return GPIOC->IDR & 0xf;
// }
.global read_rows
read_rows:
	push {lr}
  ldr r0, =GPIOC
  ldr r1, [r0, #IDR]
  movs r2, 0xf
  ands r1, r2
  movs r0, r1
  	pop {pc}

//============================================================================
// char rows_to_key(int rows) {
//   int n = (col & 0x3) * 4; // or int n = (col << 30) >> 28;
//   do {
//     if (rows & 1)
//       break;						//breaks while loop
//     n ++;
//     rows = rows >> 1;
//   } while(rows != 0);
//   char c = keymap[n];
//   return c;
// }
.global rows_to_key
rows_to_key:
	push {r4-r7, lr}
	ldr r1, =col
	ldrb r1, [r1]
	movs r2, #0x3
	ands r1, r2
	movs r2, #4
	muls r1, r2						//r1 = n
do:
	movs r3, #1
	movs r4, r0
	ands r3, r4						//rows & 1
	cmp r3, #0						//if condition
	bne charc							//breaks while loop
	adds r1, #1						//n++
	lsrs r0, r0, #1				//rows = rows >> 1
while:
	cmp r0, #0
	bne do
charc:
	ldr r4, =keymap
	//lsls r1, #2						//n*4
	ldrb r5,[r4,r1]				//r5 = keymap[n]
	movs r0, r5						//return c
donerows:
	pop {r4-r7, pc}

//============================================================================
// TIM7_ISR() {
//    TIM7->SR &= ~TIM_SR_UIF
//    int rows = read_rows();
//    if (rows != 0) {
//        char key = rows_to_key(rows);
//        handle_key(key);
//    }
//    char ch = disp[col];
//    show_char(col, ch);
//    col = (col + 1) & 7;
//    drive_column(col);
// }
.global TIM7_IRQHandler
.type TIM7_IRQHandler, % function
TIM7_IRQHandler:
	push {r4-r7, lr}
	//TIM7->SR &= ~TIM_SR_UIF
	ldr r0, =TIM7
	ldr r1, [r0, #TIM_SR]
	ldr r2, =TIM_SR_UIF
	bics r2, r1
	str r2, [r0, #TIM_SR]

	bl read_rows			//r0 is return value
	cmp r0, #0
	beq charch
	bl rows_to_key
	//r0 = key
	bl handle_key
charch:
	ldr r0, =disp
	ldr r1, =col
	ldrb r1,[r1]
	//lsls r1, #2				//col*4
	ldrb r2, [r0, r1]	//char ch = disp[col]
	movs r0, r1
	movs r1, r2
	bl show_char		//show_char(col,ch)

	ldr r3, =col		//r3 is address of col
	ldrb r4, [r3]		//r4 is value of col
	adds r4, #1			//r4 = col +1
	movs r2, #7
	ands r4, r2
	ldr r5, =0			//offset of 0
	strb r4, [r3, r5]	//col = (col+1) & 7
	ldrb r0, [r3]
	bl drive_column

	pop {r4-r7, pc}
//============================================================================
// Implement the setup_tim7 subroutine below.  Follow the instructions
// in the lab text.
.global setup_tim7
setup_tim7:
	push {lr}
	//Enable the RCC clock for TIM7
	ldr r0, =RCC
	ldr r1, [r0, #APB1ENR]
	ldr r2, =TIM7EN
	orrs r1, r2
	str r1, [r0, #APB1ENR]

	//Set the Prescaler and Auto-Reload Register to result in a frequency of 1 kHz)
	ldr r0, =TIM7
	ldr r2, =4800-1    //PSC value
	ldr r3, =10-1			//ARR value
	str r2, [r0, #TIM_PSC]
	str r3, [r0, #TIM_ARR]

	//Enable the UIE bit in the DIER.
	ldr r2, =TIM_DIER_UIE
	str r2, [r0, #TIM_DIER]

	//Enable the Timer 7 interrupt in the NVIC ISER
	ldr r0, =NVIC
	ldr r1, =ISER
	ldr r2, =1<<TIM7_IRQn
	str r2, [r0, r1]

	//Set the CEN bit in TIM7_CR1
	ldr r0, =TIM7
	ldr r1, [r0, #TIM_CR1]
	ldr r2, =TIM_CR1_CEN
	orrs r2, r1
	str r2, [r0, #TIM_CR1]

	pop {pc}
//============================================================================
// void handle_key(char key)
// {
//     if (key == 'A' || key == 'B' || key == 'D')
//         mode = key;
//     else if (key >= '0' && key <= '9')
//         thrust = key - '0';
// }
.global handle_key
handle_key:
	push {lr}
	if:
		cmp r0, #'A'
		bne or2
		beq dostuff
	or2:
		cmp r0, #'B'
		bne or3
		beq dostuff
	or3:
		cmp r0, 'D'
		bne elseifhandle
		beq dostuff
	dostuff:
		ldr r1, =mode
		ldr r2, =0x0
		//ldrb r1, [r1]
		strb r0, [r1, r2]				//r1 = mode = key
		bl donekey
	elseifhandle:
		cmp r0, #'0'
		blt donekey
		cmp r0, #'9'
		bgt donekey
		movs r2, #'0'
		//subs r0, #'0'
		subs r0, r2
		movs r3, #0
		ldr r1, =thrust
		//ldrb r1, [r1, r3]
		strb r0, [r1, r3]				//r2 = thrust = key - '0'
	donekey:
		pop {pc}

//============================================================================
// void write_display(void)
// {
//     if (mode == 'C')
//         snprintf(disp, 9, "Crashed");
//     else if (mode == 'L')
//         snprintf(disp, 9, "Landed "); // Note the extra space!
//     else if (mode == 'A')
//         snprintf(disp, 9, "ALt%5d", alt);
//     else if (mode == 'B')
//         snprintf(disp, 9, "FUEL %3d", fuel);
//     else if (mode == 'D')
//         snprintf(disp, 9, "Spd %4d", velo);
// }
.global write_display
write_display:
	push {r4-r7, lr}
	ldr r6, =mode
	ldrb r6, [r6]
	ldr r7, =disp
	ldr r4, =0
ifwrite:
	cmp r6, #'C'
	bne elseif1
	movs r0, r7
	movs r1, #9
	ldr r2, =crashed_str		//how to load string
	bl snprintf
	b donewrite
elseif1:
	cmp r6, #'L'
	bne elseif2
	movs r0, r7
	movs r1, #9
	ldr r2, =landed_str
	bl snprintf
	b donewrite
elseif2:
	cmp r6, #'A'
	bne elseif3
	movs r0, r7
	movs r1, #9
	ldr r2, =alt_str
	ldr r3, =alt
	ldrh r3, [r3, r4]			//take care of them properly??
	bl snprintf
	b donewrite
elseif3:
	cmp r6, #'B'
	bne elseif4
	movs r0, r7
	movs r1, #9
	ldr r2, =fuel_str
	ldr r3, =fuel
	ldrh r3, [r3, r4]
	bl snprintf
	b donewrite
elseif4:
	cmp r6, #'D'
	bne donewrite
	movs r0, r7
	movs r1, #9
	ldr r2, =spd_str
	ldr r3, =velo
	ldrsh r3, [r3, r4]			//signed halfword load
	bl snprintf
donewrite:
	pop {r4-r7, pc}
//declare strings
crashed_str:
	.string "Crashed"
	.balign 2						//need this?
landed_str:
	.string "Landed "
	.balign 2
alt_str:
	.string "ALt%5d"
	.balign 2
fuel_str:
	.string "FUEL %3d"
	.balign 2
spd_str:
	.string "Spd %4d"
	.balign 2

//============================================================================
// void update_variables(void)
// {
//     fuel -= thrust;
//     if (fuel <= 0) {
//         thrust = 0;
//         fuel = 0;
//     }
//     alt += velo;
//     if (alt <= 0) { // weve reached the surface
//         if (-velo < 10)
//             mode = 'L'; // soft landing
//         else
//             mode = 'C'; // crash landing
//         return;
//     }
//     velo += thrust - 5;
// }
.global update_variables
update_variables:
	push {r4-r7, lr}
	/*
	ldrb, mode
	ldrb, thrust 	//r1
	ldrh, fuel 		//r0
	ldrh, alt		//r3
	ldrsh, velo		//r4
	*/
	ldr r0, =fuel
	ldr r5, =0
	ldrsh r0, [r0, r5]
	ldr r1, =thrust
	ldrb r1, [r1]
	subs r2, r0, r1					//r2 = fuel-thrust
	ldr r0, =fuel
	strh r2, [r0]					//fuel -= thrust
if1:
	cmp r2, #0							//fuel <= 0?
	bgt endif1
	movs r5, #0
	ldr r1, =thrust
	strb r5, [r1]						//thrust = 0
	ldr r0, =fuel
	strh r5, [r0]						//fuel = 0
endif1:
	ldr r3, =alt
	ldr r5, =0
	ldrsh r3, [r3, r5]		//value of alt
	ldr r4, =velo
	ldrsh r4, [r4, r5]		//r4 = velo
	adds r3, r4				//r3 = alt + velo
	ldr r6, =alt
	strh r3, [r6]			//this would work?
if2:
	cmp r3, #0				//alt <= 0?
	bgt endif2
nestedif:
	ldr r4, =velo
	ldr r5, =0
	ldrsh r4, [r4, r5]		//value of velo
	rsbs r5, r4, #0			//r5 = -velo
	cmp r5, #10				//-velo < 10 ?
	bge nestedelse
	movs r6, #'L'
	ldr r7, =mode
	strb r6, [r7]			//mode = 'L'
	bl doneupdate
nestedelse:
	movs r6, #'C'
	ldr r7, =mode
	strb r6, [r7]			//mode = 'C'
	bl doneupdate
endif2:
	ldr r1, =thrust
	ldrb r1, [r1]
	subs r1, #5				//thrust-5
	ldr r4, =velo
	ldr r5, =0
	ldrsh r6, [r4, r5]		//r6 = velo
	adds r6, r1				//velo + thrust-5
	strh r6, [r4]
doneupdate:
	pop {r4-r7, pc}
//============================================================================
// TIM14_ISR() {
//    // acknowledge the interrupt
//    update_variables();
//    write_display();
// }
.global TIM14_ISR
TIM14_ISR:
	push {lr}
	ldr r0, =TIM14
	ldr r1, [r0, #TIM_SR]
	ldr r2, =TIM_SR_UIF
	orrs r2,r1
	str r2, [r0, #TIM_SR]

	bl update_variables
	bl write_display
	pop {pc}

//============================================================================
// Implement setup_tim14 as directed.
.global setup_tim14
setup_tim14:
	push {lr}
	//enable RCC clock
	ldr r0, =RCC
	ldr r1, [r0, #APB1ENR]
	ldr r2, =TIM14EN
	orrs r2, r1
	str r2, [r0, #APB1ENR]

	//set up PSC and ARR
	ldr r0, =TIM14
	ldr r1, =48000-1
	ldr r2, =500-1
	str r1, [r0, #TIM_PSC]
	str r2, [r0, #TIM_ARR]

	//enable UIE bit in the DIER
	ldr r2, =TIM_DIER_UIE
	str r2, [r0, #TIM_DIER]

	//set the CEN in CR1
	ldr r1, [r0, #TIM_CR1]
	ldr r2, =TIM_CR1_CEN
	orrs r2, r1
	str r2, [r0, #TIM_CR1]

	//enable interrupt in the NVIC ISER
	ldr r0, =NVIC
	ldr r1, =ISER
	ldr r2, =1<<TIM14_IRQn
	str r2, [r0, r1]
	pop {pc}

//============================================================================
//
.global login
login: .string "chen3633" // Replace with your login.
.balign 2

.global main
main:
	//bl check_wiring
	//bl fill_alpha
	bl autotest
	bl enable_ports
	bl setup_tim6
	bl setup_tim7
	bl setup_tim14
snooze:
	wfi
	b  snooze
	// Does not return.

//============================================================================
// Map the key numbers in the history array to characters.
// We just use a string for this.
.global keymap
keymap:
.string "DCBA#9630852*741"

//============================================================================
// This table is a *font*.  It provides a mapping between ASCII character
// numbers and the LED segments to illuminate for those characters.
// For instance, the character '2' has an ASCII value 50.  Element 50
// of the font array should be the 8-bit pattern to illuminate segments
// A, B, D, E, and G.  Spread out, those patterns would correspond to:
//   .GFEDCBA
//   01011011 = 0x5b
// Accessing the element 50 of the font table will retrieve the value 0x5b.
//
.global font
font:
.space 32
.byte  0x00 // 32: space
.byte  0x86 // 33: exclamation
.byte  0x22 // 34: double quote
.byte  0x76 // 35: octothorpe
.byte  0x00 // dollar
.byte  0x00 // percent
.byte  0x00 // ampersand
.byte  0x20 // 39: single quote
.byte  0x39 // 40: open paren
.byte  0x0f // 41: close paren
.byte  0x49 // 42: asterisk
.byte  0x00 // plus
.byte  0x10 // 44: comma
.byte  0x40 // 45: minus
.byte  0x80 // 46: period
.byte  0x00 // slash
.byte  0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07
.byte  0x7f, 0x67
.space 7
// Uppercase alphabet
.byte  0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x6f, 0x76, 0x30, 0x1e, 0x00, 0x38, 0x00
.byte  0x37, 0x3f, 0x73, 0x7b, 0x31, 0x6d, 0x78, 0x3e, 0x00, 0x00, 0x00, 0x6e, 0x00
.byte  0x39 // 91: open square bracket
.byte  0x00 // backslash
.byte  0x0f // 93: close square bracket
.byte  0x00 // circumflex
.byte  0x08 // 95: underscore
.byte  0x20 // 96: backquote
// Lowercase alphabet
.byte  0x5f, 0x7c, 0x58, 0x5e, 0x79, 0x71, 0x6f, 0x74, 0x10, 0x0e, 0x00, 0x30, 0x00
.byte  0x54, 0x5c, 0x73, 0x7b, 0x50, 0x6d, 0x78, 0x1c, 0x00, 0x00, 0x00, 0x6e, 0x00
.balign 2

//============================================================================
// Data structures for this experiment.
//
.data
.global col
.global disp
.global mode
.global thrust
.global fuel
.global alt
.global velo
disp: .string "Hello..."
col: .byte 0
mode: .byte 'A'
thrust: .byte 0
.balign 4
.hword 0 // put this here to make sure next hword is not word-aligned
fuel: .hword 800
.hword 0 // put this here to make sure next hword is not word-aligned
alt: .hword 4500
.hword 0 // put this here to make sure next hword is not word-aligned
velo: .hword 0
