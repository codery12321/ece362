#include <stdint.h>
#include "fifo.h"

//============================================================================
// Return 1 if the fifo holds no characters to remove.  Otherwise 0.
//============================================================================
int fifo_empty(const struct fifo *f) {
    if (f->head == f->tail)
        return 1;
    else
        return 0;
}

//============================================================================
// Return 1 if the fifo cannot hold any more characters.  Otherwise 0.
//============================================================================
int fifo_full(const struct fifo *f) {
    uint8_t next = (f->tail + 1) % sizeof f->buffer;
    // We can't let the tail reach the head.
    if (next == f->head)
        return 1;
    else
        return 0;
}

//============================================================================
// Append a character to the tail of the fifo.
// If the fifo is already full, drop the character.
//============================================================================
void fifo_insert(struct fifo *f, char ch) {
    if (fifo_full(f))
        return; // FIFO is full.  Just drop the new character.
    uint8_t next = (f->tail + 1) % sizeof f->buffer;
    if (ch == '\n')
        f->newline = f->tail; // a newline has been inserted
    else if (f->newline == f->tail)
        f->newline = next; // no newline detected yet
    f->buffer[f->tail] = ch;
    f->tail = next;
}

//============================================================================
// Remove a character from the *tail* of the fifo.
// In other words, undo the last insertion.
//============================================================================
char fifo_uninsert(struct fifo *f) {
    if (fifo_empty(f))
        return '$'; // something unexpected
    int prev = (f->tail == 0) ? sizeof f->buffer - 1 : f->tail - 1;
    if (f->newline == f->tail)
        f->newline = prev;
    f->tail = prev;
    char ch = f->buffer[f->tail];
    f->buffer[f->tail] = '@'; // something unexpected
    return ch;
}

//============================================================================
// Return 1 if the fifo contains at least one newline.  Otherwise 0.
//============================================================================
int fifo_newline(const struct fifo *f) {
    if (fifo_empty(f))
        return 0;
    if (f->newline != f->tail)
        return 1;
    return 0;
}

//============================================================================
// Remove a character from the head of the fifo.
// If the fifo is empty, you get a tilde (~).
//============================================================================
char fifo_remove(struct fifo *f) {
    if (fifo_empty(f))
        return '!'; // something unexpected.
    char ch = f->buffer[f->head];
    if (f->newline == f->head)
        f->newline = f->tail; // We just read the last newline.  Clear nl.
    f->buffer[f->head] = '~'; // overwrite removed char with something weird
    uint8_t next = (f->head + 1) % sizeof f->buffer;
    f->head = next;
    return ch;
}
