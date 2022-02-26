"GPIOC
PC5 -> output led green
PC6 -> input button black

GPIOB
PB4 -> output led yellow
PB11 -> input button red
"
//============================================================
//Q1 set up GPIO
//============================================================
.global setup_GPIO
setup_GPIO:
push {lr}
//port b
ldr r0, =RCC
ldr r1, [r0, #AHBENR]
ldr r2, =GPIOBEN
orrs r1, r2
str r1, [r0, #AHBENR]

//port c
ldr r0, =RCC
ldr r1, [r0, #AHBENR]
ldr r2, = GPIOCEN
orrs r1, r2
str r1, [r0, #AHBENR]

//set up pc5 as output
ldr r0, =GPIOC
ldr r1, [r0, #MODER]
ldr r2, =0xc00      //1100 0000 0000
bics r1, r2
ldr r2, =0x400
orrs r1, r2
str r1, [r0, #MODER]

//setup pc6 as input and pull down
ldr r0, =GPIOC
ldr r1, [r0, #MODER]
ldr r2, =0x3000     //11 0000 0000 0000
bics r1, r2
str r1, [r0, #MODER]
ldr r1, [r0, #PUPDR]
ldr r2, =0x3000
bics r1, r2
ldr r2, =0x2000
orrs r1, r2
str r1, [r0, #PUPDR]

//setup pb4 as output
ldr r0, =GPIOB
ldr r1, [r0, #MODER]
ldr r2, =0x30
bics r1, r2
ldr r2, =0x10
orrs r1, r2
str r1, [r0, #MODER]

//setup pb11 as input and pull up
ldr r0, =GPIOB
ldr r1, [r0, #MODER]
ldr r2, =0xc00000   // 1100 0 0 0 0 0
bics r1, r2
str r1, [r0, #MODER]
ldr r1, [r0, #PUPDR]
ldr r2, =0xc00000
bics r1, r2
ldr r2, =0x400000
orrs r1, r2
str r1, [r0, #PUPDR]

pop {pc}
//============================================================
//Q2 given parameter x, set green->1 if x is 1, vice versa
//============================================================
.global set_green //pc5
set_green:
    push {lr}
    cmp r0, #0
    beq greenlow
    ldr r1, =GPIOC
    ldr r2, =0x20
    str r2, [r1, #BSRR]
    bl doneGreen
greenlow:
    ldr r1, =GPIOC
    ldr r2, =0x20     //10 0000
    str r2, [r1, #BRR]
doneGreen:
    pop {pc}

//============================================================
//Q3 given parameter x, set yellow->1 if x is 1, vice versa
//============================================================
.global set_yellow //pb4
set_yellow:
    push {lr}
    cmp r0, #0
    beq yellowLow
    ldr r1, =GPIOB
    ldr r2, =0x10       //1 0000
    str r2, [r1, #BSRR]
    bl yellowDone
yellowLow:
    ldr r1, =GPIOB
    ldr r2, =0x10
    str r2, [r1, #BRR]
yellowDone:
    pop {pc}

//============================================================
//Q4 read from button return LED
//============================================================
.global get_black  //pc6
get_black:
    push {lr}
    ldr r0, =GPIOC
    ldr r1, [r0, #IDR]
    lsrs r1, #6
    ldr r2, =0x1
    ands r1, r2
    movs r0, r1
    pop {pc}

//============================================================
//Q5 read from button return LED
//============================================================
.global get_red //pb11
get_red:
    push {lr}
    ldr r0, =GPIOB
    ldr r1, [r0, #IDR]
    lsrs r1, #11
    ldr r2, =0x1
    ands r1, r2
    movs r0, r1
    pop {pc}

//============================================================
//Q6 set TIMER_ISR then toggle LED if both buttons are pressed
//============================================================
/*
if get_black == 0
    bl done
if get red == 0
    bl done
else: turn on toggle stuff
toggle function
*/
.global TIM17_IRQHandler
.type TIM17_IRQHandler, % function
TIM17_IRQHandler:
    push {lr}
    ldr  r0, =TIM17
    ldr r1, [r0, #SR]
    ldr r2, =TIM_SR_UIF
    bics r1, r2             //remember to bics
    str r1, [r0, #SR]
    pop {pc}

    //toggle LED
    push {r4-r7, lr}
    bl get_black
    movs r7, r0
    cmp r7, #0
    beq doneToggle
    bl get_red
    movs r6, r0
    cmp r6, #0
    beq doneToggle

    //start toggling LED //ex PC9
    ldr r0, =GPIOC
    ldr r1, [r0, #ODR]
    ldr r2, =0x1
    lsls r2, #9
    ands r1, r2
    cmp r1, #0
    beq elseToggle
    str r2, [r0, #BRR]
    bl doneToggle
elseToggle:
    str r2, [r0, #BSRR]
doneToggle:
    pop {r4-r7, pc}


//============================================================
//Q7 enable timer
//============================================================
.gloabl enab_timer
enab_timer:
    ldr r0, =RCC
    ldr r1, [r0, #APB1ENR]
    ldr r2, =TIM17EN
    orrs r1, r2
    str r1, [r0, #APB1ENR]

    ldr r0, =TIM17
    ldr r1, =PSC-1
    ldr r2, =ARR-1
    str r1, [r0, #PSC]
    str r2, [r0, #ARR]

    ldr r1, [r0, #DIER]
    ldr r2, =UIE
    orrs r1, r2
    str r1, [r0, #DIER]

    ldr r0, =NVIC
    ldr r1, =ISER
    ldr r2, =1<<TIM17_IRQn
    str r2, [r0, r1]

    ldr r0, =TIM17
    ldr r1, [r0, #TIMCR1]
    ldr r2, =TIM_CR1_CEN
    orrs r1, r2
    str r1, [r0, #TIMCR1]

//============================================================
//Q8 Recursion
//============================================================
/*
if (x >5)
    return 3*(recurse(x>>3) + 2*(x&2))
else
    return x+2
*/
.global recursion
recursion:
    push {r4-r7, lr}
    cmp r0, #5
    bls else
    movs r7, r0
    lsrs r0, #3
    bl recur
    movs r1, #2
    ands r7, r1
    movs r1, #2
    muls r1, r7
    adds r0, r1
    movs r1, #3
    muls r0, r1
    bl doneRecur
else:
    adds r0, #2
doneRecur:
    pop {r4-r7, pc}
