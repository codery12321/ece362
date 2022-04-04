#ifndef __FIFO_H__
#define __FIFO_H__

struct fifo {
    char buffer[128]; // circular buffer for the fifo
    volatile uint8_t head; // the first thing to remove from the fifo
    volatile uint8_t tail; // the next place to insert a new character
    volatile uint8_t newline; // offset of last-entered newline
};

int fifo_empty(const struct fifo *f);
int fifo_full(const struct fifo *f);
void fifo_insert(struct fifo *f, char ch);
char fifo_uninsert(struct fifo *f);
int fifo_newline(const struct fifo *f);
char fifo_remove(struct fifo *f);

#endif /* __FIFO_H__ */
