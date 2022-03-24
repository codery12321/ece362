#include "stm32f0xx.h"
#include <stdint.h>
#include <stdlib.h>
#include "lcd.h"

static void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void drive_column(int c)
{
    GPIOC->BSRR = 0xf00000 | ~(1 << (c + 4));
}

int read_rows()
{
    return (~GPIOC->IDR) & 0xf;
}

// Copy a subset of a large source picture into a smaller destination.
// sx,sy are the offset into the source picture.
void pic_subset(Picture *dst, const Picture *src, int sx, int sy)
{
    int dw = dst->width;
    int dh = dst->height;
    for(int y=0; y<dh; y++) {
        if (y+sy < 0)
            continue;
        if (y+sy >= src->height)
            break;
        for(int x=0; x<dw; x++) {
            if (x+sx < 0)
                continue;
            if (x+sx >= src->width)
                break;
            dst->pix2[dw * y + x] = src->pix2[src->width * (y+sy) + x + sx];
        }
    }
}

// Overlay a picture onto a destination picture.
// xoffset,yoffset are the offset into the destination picture that the
// source picture is placed.
// Any pixel in the source that is the 'transparent' color will not be
// copied.  This defines a color in the source that can be used as a
// transparent color.
void pic_overlay(Picture *dst, int xoffset, int yoffset, const Picture *src, int transparent)
{
    for(int y=0; y<src->height; y++) {
        int dy = y+yoffset;
        if (dy < 0)
            continue;
        if (dy >= dst->height)
            break;
        for(int x=0; x<src->width; x++) {
            int dx = x+xoffset;
            if (dx < 0)
                continue;
            if (dx >= dst->width)
                break;
            unsigned short int p = src->pix2[y*src->width + x];
            if (p != transparent)
                dst->pix2[dy*dst->width + dx] = p;
        }
    }
}

extern const Picture background; // A 240x320 background image
extern const Picture ball; // A 19x19 purple ball with white boundaries

// This C macro will create an array of Picture elements.
// Really, you'll just use it as a pointer to a single Picture
// element with an internal pix2[] array large enough to hold
// an image of the specified size.
// BE CAREFUL HOW LARGE OF A PICTURE YOU TRY TO CREATE:
// A 100x100 picture uses 20000 bytes.  You have 32768 bytes of SRAM.
#define TempPicturePtr(name,width,height) Picture name[(width)*(height)/6+2] = { {width,height,2} }

void erase(int x, int y)
{
    TempPicturePtr(tmp,29,29); // Create a temporary 29x29 image.
    pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
    //pic_overlay(tmp, 5,5, &ball, 0xffff); // Overlay the ball
    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
}

void update(int x, int y)
{
    LCD_DrawPicture(x-ball.width/2,y-ball.height/2, &ball); // Draw the ball
}

void update2(int x, int y)
{
    TempPicturePtr(tmp,29,29); // Create a temporary 29x29 image.
    pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
    pic_overlay(tmp, 5,5, &ball, 0xffff); // Overlay the ball
    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
}

void move_ball(void)
{
    // Draw the background.
    LCD_DrawPicture(0,0,&background);
    int x = 120;
    int y = 160;
    update(x,y);
    for(;;)
        for(int c=0; c<4; c++) {
            int dx=0;
            int dy=0;
            drive_column(c);
            nano_wait(1000000); // wait 1 ms
            int r = read_rows();
            if (c==3) { // leftmost column
                if (r & 8) { // '1'
                    dy -= 1; dx -= 1;
                }
                if (r & 4) { // '4'
                    dx -= 1;
                }
                if (r & 2) { // '7'
                    dy += 1; dx -= 1;
                }
            } else if (c == 2) { // column 2
                if (r & 8) { // '2'
                    dy -= 1;
                }
                if (r & 4) { // '5' re-center the ball
                    erase(x,y);
                    dx = 1; dy = 1;
                    x = 119; y = 159;
                }
                if (r & 2) { // '8'
                    dy += 1;
                }
            } else if (c == 1) { // column 3
                if (r & 8) { // '3'
                    dy -= 1; dx += 1;
                }
                if (r & 4) { // '6'
                    dx += 1;
                }
                if (r & 2) { // '9'
                    dy += 1; dx += 1;
                }
            }
            if (dx !=0 || dy != 0) {
                x += dx;
                y += dy;
                update(x,y);
            }
        }
}

void basic_drawing(void)
{
    LCD_Clear(0);
    LCD_DrawRectangle(10, 10, 30, 50, GREEN);
    LCD_DrawFillRectangle(50, 10, 70, 50, BLUE);
    LCD_DrawLine(10, 10, 70, 50, RED);
    LCD_Circle(50, 90, 40, 1, CYAN);
    LCD_DrawTriangle(90,10, 120,10, 90,30, YELLOW);
    LCD_DrawFillTriangle(90,90, 120,120, 90,120, GRAY);
    LCD_DrawFillRectangle(10, 140, 120, 159, WHITE);
    LCD_DrawString(20,141, BLACK, WHITE, "Test string!", 16, 0); // opaque background
}
