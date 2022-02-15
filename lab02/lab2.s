.cpu cortex-m0
.thumb
.syntax unified
.fpu softvfp

.data
.balign 4
// Your global variables go here
.global arr
arr: .word 23, 11, 23, 27, 19, 14, 13, 13, 13, 12, 17, 24, 21, 16, 19, 22
.global value
value: .word 0
.global str
str: .string "jKfFgP+ , oGuUcIgU - 01234 Kp . VjKu % UvTkPi!"

.text
.global intsub
intsub:

    // Your code for intsub goes here
    PUSH {R4-R7, LR}
for1:
	movs r0, #0 //i
	ldr r7, =value //load address of value
	ldr r6, [r7] //load value into r6
	movs r1, #15 //size

check1:
	cmp r0, r1
	bge done1

body1:
if1:
	asrs r4, r6, #4 	//r6 = value >> 4
	cmp r4, #5 			//is (value >> 4) < 5
	bge else1 			//if not, go to else
then1:
	ldr r2, =arr 		//r2 = arr address
	lsls r3, r0, #2 	//r3 = i*4
	ldr r4, [r2,r3] 	//r4 = arr[i]
	adds r3, #4 		// r3 = (i+1)*4
	ldr r5, [r2, r3] 	// r5 = arr[i+1]
	movs r3, #2 		//r3 = 2
	muls r4, r3 		//r4 = arr[i] * 2
	adds r4, r4, r5 	//r4 = (arr[i]*2) + (arr[i+1])
	adds r6, r6, r4		//r6 = value + (arr[i]*2 + arr[i+1])
	ldr r5, =value
	str r6, [r5]
	b endif1

else1:
	ldr r2, =arr 		//r2 = arr address
	lsls r3, r0, #2 	//r3 = i*4
	ldr r4, [r2,r3] 	//r4 = arr[i]
	movs r3, #3 		//r3 = 3
	muls r4, r3 		//r4 = arr[i] * 3
	subs r6, r6, r4		//r6 = value - arr[i]*3
	ldr r5, =value
	str r6, [r5]
	b endif1

endif1:
	ldr r2, =arr		//r2 = arr address
	lsls r3,r0,#2 		//r3 = i*4
	ldr r4, [r2, r3]
	movs r5, #2
	muls r4, r5
	adds r3, r3, #4
	ldr r5, [r2, r3]
	adds r4, r4, r5
	subs r3, r3, #4
	str r4, [r2, r3]
	//str r4, [r2, r3]	//arr[i] = r4

next1:
	adds r0, #1 //i++
	b check1
done1:
    // You must terminate your subroutine with bx lr
    // unless you know how to use PUSH and POP.
    //bx lr
	POP {R4-R7, PC}



.global charsub
charsub:

    // Your code for charsub goes here
    PUSH {R4-R7, LR}
for2:
	movs r0, #0 //r0 = x

check2:
	ldr r1, =str //r2 = str address
	ldrb r2, [r1,r0] //r2 = str[x]
	cmp r2, #0 //str[x] == 0
	beq endfor2

body2:
if2:
	ldr r1, =str //r1 = string address
	ldrb r2, [r1,r0] //r2 = str[x]
	//If r2 < x61 or r2 > 0x7a, go to next2
	cmp r2, #0x61
	blt next2
	cmp r2, #0x7a
	bgt if3
then3:
	movs r4, #0x20
	bics r2, r4 //str[x] & ~0x20
	strb r2, [r1,r0] // str[x] = str[x] & ~0x20

if3:
	ldr r1, =str //r1 = string address
	ldrb r2, [r1,r0] //r2 = str[x]
	//If r2 < 0x41 or r2 > 0x5a, go to next2
	cmp r2, #0x41
	blt next2
	cmp r2, #0x5a
	bgt next2
then4:
	movs r4, #2
	subs r2, r4 //str[x] - 2
	strb r2, [r1,r0] // str[x] = str[x] - 2
next2:
	adds r0, #1 //x= x+1
	b check2
endfor2:
	bkpt
    // You must terminate your subroutine with bx lr
    // unless you know how to use PUSH and POP.
    //bx lr
    POP {R4-R7, PC}
.global login
login: .string "chen3633" // Make sure you put your login here.
.balign 2
.global main
main:
    bl autotest // uncomment AFTER you debug your subroutines
    bl intsub
    bl charsub
    bkpt
