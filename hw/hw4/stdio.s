.cpu cortex-m0
.thumb
.syntax unified
.fpu softvfp

.equ RCC,     0x40021000
.equ GPIOCEN, 0x00080000
.equ GPIOC,   0x48000800
.equ GPIODEN, 0x00100000
.equ GPIOD,   0x48000c00
.equ USART5,  0x40005000
.equ AHBENR,    0x14
.equ APB1ENR,   0x1c
.equ MODER,     0x00
.equ AFRL,      0x20
.equ AFRH,      0x24
.equ USART_CR1, 0x00
.equ USART_CR2, 0x04
.equ USART_CR3, 0x08
.equ USART_BRR, 0x0c
.equ USART_ISR, 0x1c
.equ USART_ICR, 0x20
.equ USART_RDR, 0x24
.equ USART_TDR, 0x28

// After we extract the location of stdin, stdout, and stderr
// from the impure_data region, put them in these global variables.
.data
.align 4
.global stdin_ptr
stdin_ptr:
	.word 0
.global stdout_ptr
stdout_ptr:
	.word 0
.global stderr_ptr
stderr_ptr:
	.word 0

.text

//========================================================================
// Turn off stdio buffering (so we don't malloc() buffers,
// set up USART5, configure PD2 as the receiver, configure
// PC12 as the transmitter, set the baud rate to 115200, 8 bits, no parity.
//========================================================================
.global serial_init
serial_init:
	push {lr}
	// Find the stdin, stdout, stderr pointers...
	ldr r0,=_impure_ptr
	ldr r0,[r0] // get the _reent ptr
	ldr r1,[r0,#4] // stdin
	ldr r2,=stdin_ptr
	str r1,[r2]
	ldr r1,[r0,#8] // stdout
	ldr r2,=stdout_ptr
	str r1,[r2]
	ldr r1,[r0,#12] // stderr
	ldr r2,=stderr_ptr
	str r1,[r2]

	// Turn off buffer allocation for stdin, stdout, and stderr.
	ldr r0,=stdin_ptr
	ldr r0,[r0]
	movs r1,#0
	bl setbuf
	ldr r0,=stdout_ptr
	ldr r0,[r0]
	movs r1,#0
	bl setbuf
	ldr r0,=stderr_ptr
	ldr r0,[r0]
	movs r1,#0
	bl setbuf

	// Turn on the clock to Ports C and D
	ldr  r0,=RCC
	ldr  r1,[r0,#AHBENR]
	ldr  r2,=GPIOCEN|GPIODEN
	orrs r1,r2
	str  r1,[r0,#AHBENR]

	// Turn on the clock to USART5 in APB1ENR
	ldr  r1,[r0,#APB1ENR]
	ldr  r2,=1<<20
	orrs r1,r2
	str  r1,[r0,#APB1ENR]

	// Clear/set the MODER entry for PC12
	ldr  r0,=GPIOC
	ldr  r1,[r0,#MODER]
	ldr  r2,=0x03000000
	bics r1,r2
	ldr  r2,=0x02000000
	orrs r1,r2
	str  r1,[r0,#MODER]

	// Set the AFRH entry for PC12
	ldr  r1,[r0,#AFRH]
	ldr  r2,=0x000f0000
	bics r1,r2
	ldr  r2,=0x00020000
	orrs r1,r2
	str  r1,[r0,#AFRH]

	// Clear/set the MODER entry for PD2
	ldr  r0,=GPIOD
	ldr  r1,[r0,#MODER]
	ldr  r2,=0x00000030
	bics r1,r2
	ldr  r2,=0x00000020
	orrs r1,r2
	str  r1,[r0,#MODER]

	// Set the AFRL entry for PD2
	ldr  r1,[r0,#AFRL]
	ldr  r2,=0x00000f00
	bics r1,r2
	ldr  r2,=0x00000200
	orrs r1,r2
	str  r1,[r0,#AFRL]

	// Set USART5 for 8 bits, one stop, no parity,
	// 16x oversampling, 115.2kbaud.
	// Clear M1,OVER8,M0,PCE,UE
	ldr  r0,=USART5
	ldr  r1,[r0,#USART_CR1]
	ldr  r2,=0x10009401
	bics r1,r2
	str  r1,[r0,#USART_CR1]
	// Clear STOP
	ldr  r1,[r0,#USART_CR2]
	ldr  r2,=0x3000
	bics r1,r2
	str  r1,[r0,#USART_CR2]
	// Set baud rate divisor
	ldr  r1,=0x1a1 // divisor for 115.2kbaud
	str  r1,[r0,USART_BRR]
	// Enable TX,RX,UE
	ldr  r1,[r0,#USART_CR1]
	movs r2,#0xd
	orrs r1,r2
	str  r1,[r0,#USART_CR1]
	// Wait for TEACK/REACK
	ldr  r2,=0x00600000
wait_for_acks:
	ldr  r1,[r0,#USART_ISR]
	ands r1,r2
	cmp  r1,r2
	bne  wait_for_acks
	// Serial port is ready.
	pop {pc}

//========================================================================
// This is the low-level driver function to output a character to USART5.
// When printing a newline, print an extra carriage return just before.
//========================================================================
.global __io_putchar
__io_putchar:
	ldr  r1,=USART5
	cmp  r0,#10 // is this a newline?
	bne  putchar_wait2
putchar_wait1:
	ldr  r2,[r1,#USART_ISR]
	movs r3,#0x80
	ands r2,r3
	cmp  r2,r3
	bne  putchar_wait1
	movs r2,#13 // send a carriage return first
	str  r2,[r1,#USART_TDR]
putchar_wait2:
	ldr  r2,[r1,#USART_ISR]
	movs r3,#0x80
	ands r2,r3
	cmp  r2,r3
	bne  putchar_wait2
	str  r0,[r1,#USART_TDR]
	bx lr

//========================================================================
// This is the low-level I/O driver get get a character from USART5.
// If the character is carriage-return, turn it into a newline.
// Otherwise, just pass any other character through to the caller.
//========================================================================
.global __io_getchar
__io_getchar:
	push {lr}
	ldr  r1,=USART5
	ldr  r0,[r1,#USART_ISR]
	movs r2,#8
	ands r0,r2
	cmp  r0,r2
	bne  wait_for_rxne
	// Clear the overrun flag.
	ldr  r0,[r1,#USART_ICR]
	bics r0,r2
	str  r0,[r1,#USART_ICR]
wait_for_rxne:
	ldr  r0,[r1,#USART_ISR]
	movs r2,#0x20
	ands r0,r2
	cmp  r0,r2
	bne  wait_for_rxne
	// Read a character.
	ldr  r0,[r1,#USART_RDR]
	cmp  r0,#13  // Is it a carriage return?
	bne  igcdone // If not, return it.
	movs r0,#10  // change CR into LF
igcdone:
	bl __io_putchar
	pop {pc}
