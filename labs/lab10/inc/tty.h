#ifndef __TTY_H__
#define __TTY_H__

#include "fifo.h"

extern struct fifo input_fifo;

int  tty_input_available(void);
void raw_mode(void);
void cooked_mode(void);
int line_buffer_getchar(void);
void insert_echo_char(char ch);

#endif /* __TTY_H__ */
