.cpu cortex-m0
.thumb
.syntax unified
.fpu softvfp
.data

.text

.global twentyone
twentyone:
	movs r0,#21
  bx lr

.global first
first:
  movs r0, r0
  bx lr

.global comp4
comp4:
  muls r1, r0   //r1 = b * a
  muls r2, r0   //r2 = c * a
  muls r3, r0   //r3 = d * a
  subs r1, r2   //r1 = b*a - c*a
  adds r1, r3   //r1 = b*a + d*a
  movs r0, r1   //r0 = r1 //return value
  bx lr

.global or_into
or_into:
  ldr r2, [r0]  //load ptr value into r2
  orrs r2, r1   //*ptr | x
  str r2, [r0]  //store value back to *ptr
  bx lr

.global getbit
getbit:
  ldr r2, [r0]
  lsrs r2, r1
  movs r1, #1
  ands r2, r1
  movs r0, r2

  bx lr

.global setbit
setbit:
  ldr r2, [r0]
  movs r3, #1
  lsls r3, r1
  orrs r2, r3
  str r2, [r0]
  bx lr

.global clrbit
clrbit:
  movs r3, #1
  lsls r3, r1
  ldr r2, [r0]
  bics r2, r3
  str r2, [r0]
  bx lr

.global inner
inner:
  subs r0, #4
  bx lr

.global outer
outer: //r0 = x
  push {lr}
  movs r1, r0   //both r1 and r0 = x
  adds r0, #5   //r0 = x + 5
  bl inner      //returns r0
  movs r2, r0   //r2 stores return value from inner(x+5)
  movs r0, r1   //r0 = original x
  adds r0, #3   //r0 = x+3
  bl inner
  movs r3, r0   //r3 stores return value from inner(x+3)
  muls r2, r3
  muls r1, r2
  movs r0, r1
  pop  {pc}

.global set4
set4:
  push {lr}
  //ldr r2, [r0] //can i just use r0? but r0 is an address, but I cant do ldr r0, [r0]?
  //movs r0, r2 //confused about this
  movs r1, #4
  bl setbit
  pop {pc}

.global get6
get6:
  push {lr}
  movs r1, #6
  bl getbit
  pop {pc}

.global weird
weird:
  movs r3, #3
  muls r0, r3
  movs r3, #5
  muls r1, r3
  bics r0, r1
  bx lr

.global largest_weird
largest_weird:
  push {R4,R5,R6,R7, lr}
  //a, b, y, big
  //r0=x, r1 = y
  movs r7, #0   //big = 0
  movs r4, r0   //a = x
  movs r6, r1   //r6 = y
forloop1:
  cmp r4, r6    //a<=y
  bgt done      //if false then done
  movs r5, r4   //if a<=y, set b=a
forloop2:
  cmp r5, r6    //b<=y
  bgt endloop1
  movs r0, r4
  movs r1, r5
  bl weird
  movs r2, r0   //temp = weird(a,b)
if:
  cmp r2, r7    //temp > big
  ble endloop2
  movs r7, r2   //big = temp
endloop2:
  adds r5, #1
  b forloop2
endloop1:
  adds r4, #1
  b forloop1
done:
  movs r0, r7
  pop  {R4-R7, pc}

.global mulvec
mulvec:
	push {R4-R7, lr}
	//r0 = a[], r1= b[], r2=n, r3=m
	//r4= i*4, r5=addr of a[], r6=i, r7=sum
	movs r7, #0 //r7 = sum
	movs r6, #0 //r6 = i counter
loop:
	cmp r6, r2
	bge done		    //exit loop if i>=n

	//ldr r5, [r0]        //r5= array addr //gotta keep r0, r1, r2,r3
	movs r4, r6         //copy r6 value into r4
	lsls r4, r4, #2 	//r4 = i*4
	//ldr r5, [r0,r4] 	//r0 = a[i]
	ldr r5, [r1, r4]    //r1 = b[i]
	muls r5, r3			//m*b[i]
	//movs r0, r3			//r0 = m*b[i]
	str r5, [r0, r4]       //a[i] = m * b[i]; r4 is free
	adds r7, r5	        //sum += a[i]; r5 is free
endloop:
	adds r6, #1
	bl loop
done:
	movs r0, r7
	pop {R4,R5,R6,R7, pc}


// We put this down here just in case
// you forget to return from a subroutine.
// If you hit this, you'll know you forgot.
bkpt

// Because this array is "const", we can
// put it in the text segment (which is
// in the Flash ROM.  It cannot be modified.
// This is an example of a const array that the
// first few functions might refer to.
.global global_array
.align 4
global_array:
.word 2, 3, 5, 7, 11, 13, 17, 19, 23, 29
