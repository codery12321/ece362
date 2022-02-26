.cpu cortex-m0
.thumb
.syntax unified
.fpu softvfp

.global login
login: .string "chen3633"
hello_str: .string "Hello, %s!\n"
.balign 2

//step1
.global hello
hello:
	push {lr}
	ldr r0, =hello_str
	ldr r1, =login
	bl printf
	pop  {pc}

//step2
.global showsub2
showsub2:
	push {lr}
	movs r3, r0 					//move a into r3 //free r0
	movs r2, r1 					//move b into r2 //free r1
	movs r1, r3 					//move a into r1 //free r3
	muls r3, r2 				//move a*b into r3
	ldr r0, =showsub2_str 		//load string into r0
	bl printf
	pop  {pc}
showsub2_str:
	.string "%d - %d = %d\n"
	.balign  2

// Add the rest of your subroutines below
//step3
.global showsub3
showsub3:
	push {r4-r7, lr}
	sub sp, #4 					//allocate space for 1 integer
	str r4, [sp, #0]			//store r4 onto stack
	movs r3, r2 				//move c into r3 //free r2
	movs r2, r1 				//move b into r2 //free r1
	movs r1, r0 				//move a into r1 //free r0
	ldr r0, =showsub3_str 		//load string into r0
	subs r4, r2, r3 			//r4 = b-c
	movs r5, r1 				//r5 = a
	subs r5, r5, r4 			//r5 = a-b-c
	movs r4, r5 				//r4 = a-b-c

	bl printf
	add sp, #4						//deallocate space for1 integer
	pop {r4-r7, pc}
showsub3_str:
	.string "%d - %d 0 %d = %d\n"
	.balign 2

//step4
.global listing
listing:
	push {r4-r7, lr}
	//input args: r0=*school, r1=course,r2=*verb,r3=enrollment,r4=*season, r5=year
	//for printf: r1 = school, r2=course, r3=verb, r4=enrollment, r5=season, r6=year
	ldr r4, [sp, #20]		//loading r4 of input to stack
	ldr r5, [sp, #24]		//loading r5 of input to stack
	sub sp, #12				//allocate space for 3 extra arguments
	str r3, [sp, #0]		//storing r3 of input to stack
	str r4, [sp, #4]		//storing r4 of input to stack
	str r5, [sp,#8]			//storing r5 of input to stack
	movs r3, r2
	movs r2, r1
	movs r1, r0
	ldr r0, =listing_str	//load string into r0
	bl printf				//call printf
	add sp, #12
	pop {r4-r7, pc}
listing_str:
	.string "%s %05d %s %d students in %s, %d\n"
	.balign 2

//step5
.global trivial
trivial:
	push {r4-r7, lr}
	sub sp, #400
	mov r7, sp				//r7 is start of array
	movs r6, #0				//r6: x=0
loop5:
	movs r5, #100			//sizeof tmp/sizeof tmp[0]
	cmp r6, r5				//x < sizeof tmp/sizeoftemp[0]
	bge if5

	lsls r5, r6, #2		//r5 is 4*x
	adds r3, r6, #1		//r3 = x+1
	str r3, [r7, r5]	//tmp[x] = x+1
	adds r6, #1				//x += 1
	bl loop5
if5:
	movs r5, #100			//r5 = sizeof tmp / sizeof tmp[0]
	cmp r0,r5					//n >= sizeof tmp / sizeof tmp[0]
	blt returntrivial

	subs r0, r5, #1		//r0 = n = sizeof tmp / sizeof tmp[0] - 1

returntrivial:
	lsls r0, #2
	ldr r0, [r7,r0]		//loading tmp[n] into r0
	//str r0, [r7, r0]	//storing n? in tmp[n] //this isnt right bruh
	//movs r0, r7
	add sp, #400
	pop {r4-r7, pc}

//step6
.global depth
depth:
	push {r4-r7,lr}
	//r0 is x r1 is *s
	movs r7, r0			//save original value of x into r7
	movs r5, r1			//save original value of s into r5
	movs r0, r5			//move s into r0 for function call
	bl strlen			//r0 = strlen(s)
	cmp r7, #0			//if (x == 0)
	bne callputs		//false, go to callputs
	bl returnlen		//true, return len
callputs:
	movs r6, r0			//move len into r6
	movs r0, r5			//move s into r0
	bl puts				//puts(s)
returnrecursive:
	movs r1, r5			//move s into arg2
	subs r0, r7, #1		//r0 = x-1
	bl depth			// goes back to depth, r0 is return value
	adds r0,r6 			//return len+depth(x-1,s)
returnlen:
	pop {r4-r7,pc}

//step7
.global collatz
collatz:
	push {r4-r7, lr}
	movs r7, r0			//store original value of n in r7
if1:
	cmp r7, #1			//n == 1 ?
	bne if2
	movs r0, #0
	bl done7

//go to if2 only if if1 is false. if if1 is true then return and exit function
if2:
	movs r1, #1			//r1 = 1
	ands r0, r1			//r0 = n & 1
	cmp r0, #0			//(n & 1) == 0 ?
	bne returncollatz			//go to return if if2 is false
	movs r6, r7
	lsrs r6,#1			//div by 2 so right shift by 1 //n/2
	movs r0, r6
	bl collatz			//collatz(n/2)
	adds r0, #1			//return 1+collatz(n/2)
	bl done7
//go to return only if if1 and if2 are both false
returncollatz:
	movs r5, #3
	movs r6, r7			//r6 = n
	muls r6, r5			//r6 = n*3
	adds r6, #1			//r6 = 3*n+1
	movs r0, r6
	bl collatz			//collatz(3*n+1)
	//do this
	//movs r7, r0
	//adds r0, r7, #1
	//or this?
	adds r0, #1		//r0 = 1 + collatz(3*n+1)
done7:
	pop  {r4-r7, pc}

//step8
.global permute
permute:
	push {r4-r7,lr}
	movs r7, r0			//store a in r7
	cmp r7, #0
	bgt return8
	adds r0, r5			//r0 = f
	adds r0, r4			//r0 = f+e
	adds r0, r3			//r0 = f+e+d
	adds r0, r2			//r0 = f+e+d+c
	adds r0, r1			//r0 = f+e+d+c+b
	adds r0, r7			//r0 = f+e+d+x+b+a
	bl done8			//return
return8:
	subs r0, r5, #1		//r0 = f-1
	movs r1, r7			//a
	movs r2, r1			//b
	movs r3, r2			//c
	movs r4, r3			//d
	movs r5, r4			//e
	bl permute			//call permute, result in r0
	adds r0, r0,#1		//return r0 + 1
done8:
	pop  {r4-r7,pc}

//step9
.global bizarre
bizarre:
	push {r4-r7, lr}
	ldr r3, =-800		//complaining about sub cos sub doesnt have sp register but add does
	add sp, r3			//allocate 200 integers which is #800
	mov r7, sp			//r7 is the beginning of "array"
	movs r4, #0			//r4 is x
loop9:
	cmp r4, #200
	bge callqsort		//if x !< 200 then go to callqsort
	adds r6, r4, #1		//r6 = x + 1
	adds r6, r0			//r6 = base + x+ 1 //r0 is free
	movs r5, #255
	muls r6, r5			//r6 = ((base+x+1)*255)	//r5 is free
	movs r5, #0xff
	ands r6, r5			//r6 = ((base+x+1) * 255) & 0xff //r5 is free
	lsls r5, r4, #2		//r5 = x*4
	str r6, [r7, r5]	//array[x] = ((base+x+1)*255) & 0xff
	adds r4, #1			//x++
	bl loop9
callqsort:
	movs r0, r7				//move or load? array into r0
	movs r5, r1				//move nth to r5
	movs r1, #200
	movs r2, #4
	ldr r3, =compare		//ta said
	//how to call compare using bx
	bl qsort
return9:
	//load array[nth]?
	lsls r5, #2
	ldr r0, [r7, r5]		//return array[nth]
done9:
	ldr r3, =800
	add sp,r3		//deallocate 200 integers
	pop {r4-r7, pc}
.type compare, %function  // You know why you need this line, right?
compare:
    ldr  r0,[r0]		//load value of a
    ldr  r1,[r1]		//load value of b
    subs r0,r1			//r0 = a-b
    bx lr

//step10
.global easy
easy:
	push {r4-r7, lr}
	ldr r4, [sp, #20]
	sub sp, #256			//allocate space for 64 integers
	sub sp, #12				//allocating space for r2,r3,r4 input arguments //if i want the value the just do ldr r0, [sp, #0]
	str r2, [sp, #0]		//store shift onto the stack
	str r3, [sp, #4]		//store mask onto the stack
	str r4, [sp, #8]		//store skip onto the stack
	mov r7, sp				//r7 is save array
	movs r6, #0				//r6 is result
	movs r4, #1				//r4 is 1
	str r4, [r7]			//save[0] = 1 //r4 is free
	movs r5, #1				//x = 1
loopEasy1:
	cmp r5, #64				//x < sizeof save / sizeof save[0]?
	bge loopEasy2
	movs r4, r5				//r4 = x //r5 is original value for index
	subs r4, #1				//r4 = x-1
	lsls r4, #2				//r4 = (x-1)*4
	ldr r4, [r7, r4]		//r4 = save[x-1]
	ldr r3, [sp, #260]		//load mask from stack // 256+4 = 260
	ands r3, r5				//r3 = mask & x
	eors r4, r3				//r4 = (save[x-1] ^ (x & mask)) //r3 is free
	ldr r3, [sp, #256]		//loading shift from stack
	lsls r4, r3				//r4 = (save[x-1] ^ (x & mask)) << shift
	movs r3, r5				//duplicate x into r3
	lsls r3, #2				//r3 = x*4
	str r4, [r7, r3]		//save[x] = (save[x-1] ^ (x & mask)) << shift
	adds r5, #1				//x += 1
	bl loopEasy1

loopEasy2:
	movs r5, r0				//x = a //r0 is free
forloopEasy2:
	cmp r5, r1				//x != b //r1 is free
	beq returneasy
	movs r4, r5				//keep original value of x in r5
	lsls r4, #2				//r4 = x*4
	ldr r3, [r7, r4]		//r3 = save[x]
	eors r6, r3				//r6 is result //r3 is free
	//incrementation for loop 	//x = (x*5+skip) & 63)
	movs r3, #5				//r0 = 5
	movs r4, r5				//move original value of x into r4
	muls r4, r3				//r4 = x*5	//r3 is free
	ldr r3, [sp, #264]		//load skip from stack
	adds r4, r3				//r4 = (x*5+skip)
	movs r3, #63
	ands r4, r3				//r4 = (x*5+skip) & 63
	movs r5, r4				//movs value back into x
	bl forloopEasy2

returneasy:
	movs r0, r6				//move result into return value
	add sp, #268				//deallocate space for array
	pop {r4-r7, pc}
