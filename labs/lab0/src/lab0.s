  .cpu cortex-m0
  .thumb
  .syntax unified
  .fpu softvfp

  .global main
  main:
  movs r0,#4
  movs r1,#6
  movs r2,#5
  movs r3,#9
  adds r0,r3,r2
  subs r1,r3,r1
  bl autotest
  bkpt

  .global login
  login:
  .asciz "chen3633"
  .align 2
