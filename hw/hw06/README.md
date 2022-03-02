# Homework 6: Fading Digit Clock

## Introduction
In this homework exercise, you'll get more practice with the Direct Memory Access (DMA) unit. This subsystem is sort of like a simple CPU that can only read from an address and write the same data to another address. The advantage is that it can be set up to do this without having to bother the real CPU while it's working.

Students are often asked to explain how DMA works when interviewing for jobs, so we like to make sure they're ready.

## Step 0: Physical circuitry
Use the multiplexed seven-segment display wiring you originally set up for lab 5, and used again for lab 6, and again for lab 7. Recall that the digit selection is done with PB[10:8] and the data to display on the selected digit must be output to PB[7:0];

## Step 1: Make the following initial program work
Try the following program. It uses the font array from lab 7, and a few subroutines to write things into a digit array. Note that this array has 8 entries of type uint16_t. This means that each entry is a 16-bit (two-byte) quantity.
```
#include "stm32f0xx.h"
#include <string.h>

const char font[] = {
        [' '] = 0x00,
        ['0'] = 0x3f,
        ['1'] = 0x06,
        ['2'] = 0x5b,
        ['3'] = 0x4f,
        ['4'] = 0x66,
        ['5'] = 0x6d,
        ['6'] = 0x7d,
        ['7'] = 0x07,
        ['8'] = 0x7f,
        ['9'] = 0x67,
        ['A'] = 0x77,
        ['B'] = 0x7c,
        ['C'] = 0x39,
        ['D'] = 0x5e,
        ['*'] = 0x49,
        ['#'] = 0x76,
        ['.'] = 0x80,
        ['?'] = 0x53,
        ['b'] = 0x7c,
        ['r'] = 0x50,
        ['g'] = 0x6f,
        ['i'] = 0x10,
        ['n'] = 0x54,
        ['u'] = 0x1c,
};

uint16_t digit[8];

void set_digit(int n, char c)
{
    digit[n] = (n<<8) | font[c];
}

void set_string(const char *s)
{
    for(int n=0; s[n] != '\0'; n++)
        set_digit(n,s[n]);
}

int main(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER |= 0x155555;
    set_string("running.");

    // display loop
    for(;;) {
        for(int x=0; x < 8; x++) {
            GPIOB->ODR = digit[x];
            for(int n=0; n < 100; n++);
        }
    }
}
```

The program calls set_string() to configure the digit array like so:
```
	0x050, 0x11c, 0x254, 0x354, 0x410, 0x554, 0x66f, 0x780
```

Successive iterations of the "display loop" will select digit 0 and write 0x50, select digit 1 and write 0x1c, select digit 2,, and write 0x54, and so on. If you run this program, you should see "running." shown on the seven-segment displays. No surprises.

There are two notes about this program that need explanation. First, it is copying a 16-bit value directly to the ODR even though not all pins are configured for output. This is, perhaps, bad form, but it will not cause problems for the pins that are still configured as input, by default. It would also work if PB[15:11] were configured for analog operation or for an alternate function. The only thing that could go wrong in this case is if we needed to use any of PB[15:11] as outputs.

Second, there is an apparently useless loop buried deep in the "display loop" where n counts from 0 to 99 doing nothing. Try commenting out that time-waster to see what happens. You should see the letters "running." blurred together and overlapping. As values are written simultaneously to PB[10:8] and PB[7:0] different propagation delays occur. PB[10:8] is decoded by the 74HC138 3-to-8 decoder, which has some propagation delay and also creates glitches on its output lines. Meanwhile, the data on PB[7:0] encounters much less of a propagation delay on the sink driver. As the encoder outputs are changing and glitching, the TLC59211 is driving the pattern on the constantly-changing selected digit. The "display loop" runs rapidly enough that the short duration of each glitch and delay is compounded by repetition. The time-waster loop is necessary to hold the pattern on a particular digit long enough that it dominates the noise of the decoder delay and glitches. Keep this in mind as you set up the display: If you use too fast of an update rate, it will cause the display to blur.

## Step 1: Use DMA instead
You saw in lab 6 that the code loop is easily replaced with DMA. A timer was set up to trigger the DMA channel at a 1 kHz rate so that each digit was displayed 1000/8 = 125 times per second. That's high enough that it does not appear to flicker, but low enough that the digits are not blurred together by the circuit delays.

For this exercise, you're going to use DMA **channel 2**. Enable the RCC clock to the DMA controller and configure **channel 2** in the following manner:

- Set CMAR to the address of the beginning of the digit array. (In C, an array reference is indistinguishable from a pointer to the starting element, so you don't need to use the address-of operator. Just refer to the array. You do, however, need to cast it to a uint32_t though.
- Set CPAR to the address of the Port B ODR. Remember to cast it.
- Set CNDTR to 8 to indicate there are 8 things to transfer.
- In the CCR register:
    - Set the DIR bit to indicate the direction of copying is from the "memory" address to the "peripheral" address.
    - Set the memory datum size (in the MSIZE field) to 16 bits.
    - Set the peripheral datum size (in the PSIZE field) to 16 bits.
    - Set the MINC bit so that the memory address is incremented by 2 after each transfer.
    - Set the CIRC bit so that, once all 8 transfers complete, the entire transaction is restarted, over and over again.
    - Last: Set the EN bit to enable the DMA channel.
There are lots of other DMA flags. Unless otherwise instructed above, you can leave them at their default values.
Once a DMA channel is configured and enabled, the work is not quite done. A DMA channel must be triggered. Depending on the type of peripheral involved, this is sometimes automated. Here, we wan to transfer to one of the Port B registers. Port B is a very simple peripheral that lacks any kind of DMA triggering capability. For this reason, we need to set up a timer to periodically trigger the DMA channel and tell it to transfer a datum from memory to peripheral.

Recall from the DMA lecture that a particular DMA channel can only be triggered by a limited set of devices which have a hardware notification path for that channel. We've chosen DMA channel 1 to do the work, so we need something that can trigger it. The first section in Table 32 of the FRM shows the different things that can trigger channel 1 on an STM32F09 microcontroller.

Column 2 shows only three things that can trigger DMA channel 2. They are TIM2_UP, TIM3_CH3, and TIM1_CH1. The "UP"date event is easier to configure than the "CH"annel events, so we'll continue to keep things simple and choose TIM2_UP. All that is needed to make this work is configuration of Timer 2, and enable a DMA trigger on an update event (the counter being set back to zero upon reaching the ARR value).

Turn on the RCC clock to TIM2, and configure the PSC and ARR to have an update event at a rate of 1000 Hz. You know that a TIM2_IRQn interrupt can be raised by setting the UIE bit in the TIM2 DIER. This time, we don't want to use interrupts with Timer 2. Enabling a DMA trigger is just as easy. Instead, set only the UDE bit in the TIM2 DIER. On doing so a DMA trigger is invoked on each Update is Enabled. Finally, remember to set the CEN bit of TIM2 CR1 to let the counter run.

Run the program, and you should see "running." printed on the seven-segment displays. Notice that the `main()` subroutine no longer has anything to do, and it returns to the caller. The caller is in startup_stm32.s, and it will continue in the LoopForever infinite loop. While the CPU is busy in an endless loop, the DMA will continually copy values to the Port B output to drive the displays. This also means that the CPU could be doing something more useful instead. The use of DMA frees the CPU to do other things.

## Step 2: Create a clock
Now that the `digit` array is continually copied to the display. To make the display change, all we need to do is write new values into the array. They will be copied by the next DMA transfer cycle. An obvious thing to try with a display like this is to create a clock. The `main()` subroutine is free to do interesting things, but it is difficult to time events precisely with just software. Instead, set up a second timer to invoke an ISR with each update. You've done this a few times already.

Create the following global variables:
```
int hrs = 12;
int min = 06;
int sec = 30;
int eighth;
```

The hrs/min/sec variables can be initialized as shown to set the time to those values when the reset button is pressed.
Choose any timer other than Timer 2. In the main() subroutine, enable the RCC clock to the selected timer, configure it to have an update event 8 times per second. In its ISR, you should acknowledge the interrupt, and insert the following code:
```
    eighth += 1;
    if (eighth >= 8) { eighth -= 8; sec += 1; }
    if (sec >= 60)   { sec -= 60;   min += 1; }
    if (min >= 60)   { min -= 60;   hrs += 1; }
    if (hrs >= 24)   { hrs -= 24; }
    char time[8];
    sprintf(time, "%02d%02d%02d  ", hrs, min, sec);
    set_string(time);
```

When you run it, you should see a clock. The rightmost digit should change once per second.

## Step 3: Making the digits slowly fade
The digital clock you just implemented should seem commonplace and familiar. It looks like almost any other digital clock. When you become adept at using microcontrollers, you do things that disguise the "digital" aspects of a design. To demonstrate this, we'll use DMA to make the digits seem to "fade" into others as they change. This is a contrived example, but it illustrates a useful technique.

To do this, we're going to replicate the DMA region that is repeatedly copied to the displays. To do so, make two initial changes:

- Quadruple the size of the digit array:
    `uint16_t digit[8*4];`  
- Change the CNDTR value to 8*4.

Now, when you run the program, it should act almost as it did before, but the digits are dimmer. Why? The DMA channel is now copying 32 two-byte values to Port B. The first 8 values illuminate the digits, but the next 24 values send zeros. Any global variable is automatically initialized to zero, and the set_string() subroutine only sets the first eight entries. The effect is that the displays are only illuminated 1/4 as often as they were before. This makes them dimmer.
Effectively, what is being displayed is:
```
	121314
		(blank)
		(blank)
		(blank)
```

The observation to make is that if the first 8 entries of the digit array were replicated into the second, third, and fourth sets of 8 entries, then the display would appear as bright as it did before because it would look like this:
```
	121314
	121314
	121314
	121314
```

If the four 8-entry chunks were replicated, but updated one at a time, they would appear to be mixed together. For instance, when the time was updated from 121314 to 121315, the array could be update first to:
```
	121315
	121314
	121314
	121314
```
Next, it would be updated to:
```
	121315
	121315
	121314
	121314
```
Next, it would be updated to:
```
	121315
	121315
	121315
	121314
```
Finally, it would be updated to:
```
	121315
	121315
	121315
	121315
```
Since the display update is so rapid, the change of one digit from 4 to 5 will give the appearance of fading from one to the other.
To implement this, make a third change of adding the following code at the end of the ISR to incrementally replicate the first 8-entry chunk to the second, third, and fourth chunks:
```
    if (eighth > 0 && eighth < 4) {
        memcpy(&digit[8*eighth], digit, 2*8);
    }
```

## Step 4: Making it look good
You might notice that the fading digits are not so smooth. The segments appear to flicker as they fade out. One way to correct this is to increase the update rate of Timer 2. Remember: If you speed it up too much, you'll see the effects of delay and glitches with the 74HC138. A recommended rate is 4–8 times as fast as the original rate of 1 kHz. Test this. Try setting Timer 2 to update at 24 MHz. (It won't work at all.) Then gradually lower the rate to 8 kHz.

If the rate is a little too high, you will see ghosting in the unused digits. This is, again, due to the decoder delays and glitches. The following video shows the end result of your work. The DMA trigger rate is 120 kHz. The first half of the video uses red 7-segment displays, and the second half uses green 7-segment displays (because they're easier to see in lower light.) Notice how digit 6, the blank digit just to the right of the seconds, seems to dimly replicate the least significant digit of the seconds. By lowering the update rate to around 8 kHz, this ghosting is no longer noticable.

## Is there a name for this technique?
Yes. This technique is often referred to as pulse-width modulation. We're not using the PWM mode of any of the timers, but the "fade" involved with turning segments on and off is effectively like changing the pulse-width of the "on-time" of the controlling signal. That's how it achieves the analog quality of fading.

The same technique is also useful for color displays to implement half-toning. The "HUB75"-style display (look it up) is a popular options for students when constructing mini-projects Students read that they can be controlled with "PWM" signals. Then they ask the instructor about this, and find out it's all wrong, and it's a trap to get them to learn a lot of details.

You've now learned the basic mechanism for only one color. When dealing a Red/Green/Blue system, various shades of colors can be made by turning on the elements of each pixel for a variable duration. Avoiding flicker is a matter of making sure that the update rate is high enough. Avoiding ghosting is a matter of making sure that the update rate is not too high. DMA makes the process much easier.

## Other things to try
Once you succeed in configuring DMA a few times, it is easy to adapt your code to change things. Try changing the code to:

- use DMA channel 1 with the Timer 17 update -OR-
- use DMA channel 3 with the Timer 3, 6, or 16 update -OR-
- use DMA channel 4 with the Timer 7 update -OR-
- use DMA channel 5 with the Timer 1 or 15 update -OR-
- use any of the DMA channels with Timer channel events. The DMA trigger is enabled with the CCxDE bits in a timer's DIER register.
- use any of the DMA channels with Timer TRIGger events. The trigger for a timer is configured by setting the MMS field of the CR2 register.
You can *try* these things, but don't turn them in below.

## Step 5: Turn it in
When you have a working clock with fading digits, submit your main.c file. It will be graded by briefly looking it over and then watching the results of running it. Your clock should increment seconds at the correct rate, and there should be a half-second fade for each digit as shown in the video.

>Questions or comments about the course and/or the content of these webpages should be sent to the Course Webmaster. All the materials on this site are intended solely for the use of students enrolled in ECE 362 at the Purdue University West Lafayette Campus. Downloading, copying, or reproducing any of the copyrighted materials posted on this site (documents or videos) for anything other than educational purposes is forbidden.
