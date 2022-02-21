.cpu cortex-m0
.thumb
.syntax unified
.fpu softvfp

.global login
login: .string "xyz"
hello_str: .string "Hello, %s!\n"
.balign  2
.global hello
hello:
	push {lr}

	pop  {pc}

showsub2_str: .string "%d * %d = %d\n"
.balign  2
.global showstr2
showsub2:
	push {lr}

	pop  {pc}

// Add the rest of your subroutines below
