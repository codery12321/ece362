//============================================================================
// lcd.h: Adapted from the lcdwiki.com examples.
//============================================================================

#ifndef __LCD_H
#define __LCD_H
#include "stdlib.h"

// shorthand notation for 8-bit and 16-bit unsigned integers
typedef uint8_t u8;
typedef uint16_t u16;
typedef int16_t s16;

// The LCD device structure definition.
//
typedef struct
{
    u16 width;
    u16 height;
    u16 id;
    u8  dir;
    u16  wramcmd;
    u16  setxcmd;
    u16  setycmd;
    void (*reset)(int);
    void (*select)(int);
    void (*reg_select)(int);
} lcd_dev_t;

// The LCD device.
// This will be initialized by LCD_direction() so that the
// width and height will be appropriate for the rotation.
// The setxcmd and setycmd will be set so that cursor selection
// is defined properly for the rotation.
extern lcd_dev_t lcddev;

// Rotation:
// 0: rotate 0
// 1: rotate: 90
// 2: rotate: 180
// 3: rotate 270
#define USE_HORIZONTAL       0

// The dimensions of the display.
#define LCD_W 240
#define LCD_H 320

// Some popular colors
#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define YELLOW      0XFFE0
#define GBLUE       0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define BROWN       0XBC40
#define BRRED       0XFC07
#define GRAY        0X8430
#define DARKBLUE    0X01CF
#define LIGHTBLUE   0X7D7C
#define GRAYBLUE    0X5458
#define LIGHTGREEN  0X841F
#define LIGHTGRAY   0XEF5B
#define LGRAY       0XC618
#define LGRAYBLUE   0XA651
#define LBBLUE      0X2B12

void LCD_Setup(void);
void LCD_Init(void (*reset)(int), void (*select)(int), void (*reg_select)(int));
void LCD_Clear(u16 Color);
void LCD_DrawPoint(u16 x,u16 y,u16 c);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 c);
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 c);
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 c);
void LCD_Circle(u16 xc, u16 yc, u16 r, u16 fill, u16 c);
void LCD_DrawTriangle(u16 x0,u16 y0, u16 x1,u16 y1, u16 x2,u16 y2, u16 c);
void LCD_DrawFillTriangle(u16 x0,u16 y0, u16 x1,u16 y1, u16 x2,u16 y2, u16 c);
void LCD_DrawChar(u16 x,u16 y,u16 fc, u16 bc, char num, u8 size, u8 mode);
void LCD_DrawString(u16 x,u16 y, u16 fc, u16 bg, const char *p, u8 size, u8 mode);

//===========================================================================
// C Picture data structure.
//===========================================================================
typedef struct {
    unsigned int   width;
    unsigned int   height;
    unsigned int   bytes_per_pixel; // 2:RGB16, 3:RGB, 4:RGBA
    union {
    unsigned char  pixel_data[0]; // variable length array
    unsigned short pix2[0];
    };
} Picture;

void LCD_DrawPicture(int x0, int y0, const Picture *pic);

#endif
