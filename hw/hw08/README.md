# Homework 8: Using Color Graphic Displays
In this homework exercise, you'll set up the TFT LCD display. The MSP2202 in your lab kit is an SPI device. Like most SPI devices, it has a few other pins and a complicated initialization procedure.

You should read the documentation for the display. In the References section of the course web page, you will find the User Manual, Specifications, Bottom-Side Technical Diagram, Schematic, and ILI9341 Datasheet. This might take you days or weeks to go through carefully enough to do interesting work. In particular, it would take a great deal of effort to find out how to drive the interface at high speed.

To hurry things along, we give you the initialization software as well as a library of graphics software that you can use to build interesting pictures and interactions. It is typical for most complex devices to have much of the software provided for you. The goal of the class is to understand and demonstrate the interfacing — not come up with the software to do everything.

## Step 0: Physical circuitry
Wire the display as show in Figure 1. You may still have Port B pins 0 - 10 wired to the 7-segment display. By now, you've switched over to using the SPI interface for that. **Remove the old connections for PB0 - PB10.** You won't use them for the 7-segment display any longer.
![Figure 1: TFT LCD and SD card interface wiring](figure1.png)  

Figure 1 includes wiring for both the LCD display (9-pin connector) and the SDcard interface (5-pin connector). Don't wire anything to the five-pin connector for this exercise.

When you wire the power (+3V and Gnd), make sure to connect them as close as possible to the microcontroller. This does not matter much for the LCD itself, but it will be important in the future when we start to use the SDcard interface.

## Step 1: Software setup
Create a project in System Workbench named 'hw8'. There are several support files provided for you. Look at the References page again. In the "Project Guides" section, there is a link for "Code libraries we provide for you". In that section, there is a link to TFT LCD examples. Click on the image of the checkerboard bounce game to see the description and the files that it uses.

From the bounce game page, download the following files:
- [background.c](src/background.c): the background image
- [ball.c](src/ball.c): the 19x19 purple ball image
- [lcd.c](src/lcd.c): the LCD graphics library
- [lcd.h](src/lcd.h): the LCD graphics library header file

Combine them with the [main.c](src/main.c) and [support.c](src/support.c) files to get started with this homework.
To get the system to work, all you need to do is complete the `init_lcd_spi()` and `setup_buttons()` in main.c.

The lcd.c file is specifically set up to work with the TFT LCD connected to the pins shown in Figure 1. If you implement a mini-project with the TFT LCD you can use whatever pins you prefer. Just be sure to change the definitions in lcd.c to match your changes.

### setup_buttons
The `setup_buttons()` subroutine is exactly the same as the one you used for labs 7 and 8. It should do the following:
- Enable the RCC clock to GPIO Port C
- Configure PC4 - PC7 to be outputs
- Configure PC4 - PC7 to be open-drain
- Configure PC0 - PC3 to be inputs
- Configure PC0 - PC3 to enable internal pull-up resistors

### init_lcd_spi()
The `init_lcd_spi()` subroutine should do the following:
- Enable the RCC clock to GPIO Port B
- Set PB8, PB11, and PB14 to be outputs
- Set the ODR value for PB8, PB11, and PB14 to be 1 (logic high)
- Configure PB3 to be alternate function 0
- Configure PB5 to be alternate function 0

And to set up the SPI1 peripheral:
- Enable the RCC clock to SPI1
- Turn off the SPI1 peripheral (clear the SPE bit)
- Set the baud rate to be as high as possible
- Configure the SPI1 peripheral for "master mode"
- Set the word size to be 8-bit
- Set the SSM and SSI bits
- Enable the SPI1 peripheral (set the SPE bit)

This is vaguely similar to what you did in lab 8, however this time you should not configure automatic NSS protocol (NSSP) or the NSS output enable (SSOE). The MSP2202 LCD is a device that expects its CS (chip select) to go low and stay low for many transactions.

Also note that SPI1 was configured to work with PA4,5,6,7,15 in lab 8. Here, we're using different pins for it. You do not need to remove your OLED LCD display. In fact, if you're clever, you can change the SPI configuration back and forth between the TFT LCD and OLED LCD to use both devices.

## Try it
If you have configured everything correctly, the `basic_drawing()` subroutine will display a variety of shapes and text strings on the LCD. Feel free to change the subroutine to do interesting things. (You will not turn in the support.c file.)

Once it works, uncomment the call to the `move_ball()` subroutine. This subroutine does the following:
- Uses the LCD_DrawPicture() subroutine to put the image encoded in background.c on the screen at coordinates 0,0.
- Calls the update() subroutine to draw the image encoded in ball.c in the middle of the screen.
- Scans the keypad using drive_column() and read_rows() — sort of like labs 7 and 8, only there is no debouncing needed here.
- When a button is pressed, it changes the x and y variables and redraws the ball.

Note that pressing the '5' button will move the ball back to the center of the screen. The other buttons work like so:

	1	|2  ^	|3
	up/left	|   |	|up/right
		|   |	|
	--------+-------+--------
	4	|5	|6
	<---	|center	| ---->
		|	|
	--------+-------+--------
	7	|8  |	|9
	 down/	|   |	| down/
	 left	|   v	| right

### Improving the drawing
Notice that when you move the ball image, it's only redrawing the image at a new location. It's not erasing the previous image. It's also drawing a white square around the ball rather than a round object. This does not look good at all.

Consider the code in the `erase()` subroutine in the support.c file. To erase the ball, we really want to just redraw the background. Redrawing the entire background would take a long time. Instead, we'd rather just draw enough of the background to cover the ball. The `erase()` subroutine does this by allocating a temporary image of size 29x29. It uses the `pic_subset()` subroutine to copy the background image at the coordinates of the ball into the `tmp` picture. Then it draws that temporary image to the screen.

Next, look at the `update2()` subroutine. It does something similar to `erase()`. It creates a temporary picture, copies the background into it, and then it calls `pic_overlay()` to copy the ball image into it. It accepts a transparency color that is used to filter out some pixels. The ball is purple on a white square. By passing white (0xffff) as the transparency color, only the non-white pixels of the ball are copied onto the temporary image. The result is that the temporary image contains only the portion of the background image and the non-white portions of the ball image. When it is copied to the screen, it will show only the ball.

Furthermore, the temporary image is 29x29, while the ball is only 19x19. In this way, when the ball is moved, the 5 pixels on either side will overwrite the old ball image with the appropriate background image.

Replace the two calls to `update()` with calls to `update2()`. Look at the excellent results.

These are all the basic steps needed to create animated images on the LCD display. At this point, you might look at the bounce game more to see how it is organized. Try to make it work on your own time.

### Submit your code
When you can make the ball move around on the screen by holding the buttons, you can turn in your main.c file. Remember that the only things you needed to change in main.c were the `setup_buttons()` and `init_lcd_spi()` subroutines (and uncomment the call to `move_ball()`).

### When it doesn't work...
There are so many things that can go wrong. This is the nature of embedded system design. Solving problems requires patience and determination. Here are the steps you should take to get it working:

First, check that your wiring is correct.

Next, connect the AD2 to your development system and configure the Logic Tool like so:
- SPI Select: DIO 0: PB8
- SPI Clock: DIO 1: PB3
- SPI Data: DIO 2: PB5
- GPIO DC: DIO 3: PB14
- GPIO nRESET: DIO 4: PB11

Set the Logic Tool to trigger on the falling edge of SPI Select. Make sure that you are seeing patterns that make sense. The SPI Select line should go low for many transitions of the SPI Clock signal. The SPI Data line should go high and low to indicate the bit patterns. The DC signal should sometimes be high and sometimes be low. It will only vary between entire 8-bit chunks of data. The nRESET line should be high most of the time.

If one or more of the signals do not appear correct, use the System Workbench I/O Register debugger to examine configuration for each of the Port B pins. Are they configured for proper SPI operation similar to lab 8? (Remember that we're not using automatic NSS generation. NSS will stay low for many transactions.)

Finally, is the SPI peripheral configured correctly? Use the I/O Register debugger to check. Try depositing a value in the SPI->DR. Do you see that value on the AD2 Logic Tool trace?

>Questions or comments about the course and/or the content of these webpages should be sent to the Course Webmaster. All the materials on this site are intended solely for the use of students enrolled in ECE 362 at the Purdue University West Lafayette Campus. Downloading, copying, or reproducing any of the copyrighted materials posted on this site (documents or videos) for anything other than educational purposes is forbidden.
