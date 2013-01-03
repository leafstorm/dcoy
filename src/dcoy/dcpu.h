/**
 * dcoy/dcpu.h
 *
 * The primary data structures and emulator loop - header
 *
 * (C) 2013, Matthew Frazier
 * Released under the MIT license - see LICENSE for details
 */

#ifndef _dcoy_dcpu_h
#define _dcoy_dcpu_h

#include <stdbool.h>
#include "dcoy/code.h"
#include "dcoy/specs.h"

/* Execution flags */

#define DCOY_FLAG_QUEUE_INTS    (1 << 0)
#define DCOY_FLAG_HALT          (1 << 4)
#define DCOY_FLAG_ON_FIRE       (1 << 5)
#define DCOY_FLAG_ERROR         (1 << 8)

#define dcoy_dcpu_flag(d, flag)         ((d)->flags & (flag))
#define dcoy_dcpu_flag_set(d, flag)     ((d)->flags |= (flag))
#define dcoy_dcpu_flag_unset(d, flag)   ((d)->flags &= ~(flag))


/* DCPU emulator structure */

#define DCOY_DCPU_ERROR_MAXLEN   64

typedef struct dcoy_dcpu16 {
    unsigned int cycles;
    unsigned int flags;
    char error[DCOY_DCPU_ERROR_MAXLEN];

    dcoy_word reg[DCOY_REG_COUNT];
    dcoy_word pc;
    dcoy_word sp;
    dcoy_word ex;
    dcoy_word ia;

    dcoy_word mem[DCOY_MEM_WORDS];

    dcoy_word iq[DCOY_INT_QUEUE_SIZE];
    unsigned int iq_front;
    unsigned int iq_back;

} dcoy_dcpu16;


/* Instance management functions */

dcoy_dcpu16 *dcoy_dcpu_create ();
void dcoy_dcpu_reset (dcoy_dcpu16 *d);

void dcoy_dcpu_error (dcoy_dcpu16 *d, const char *format, ...);
void dcoy_dcpu_recover (dcoy_dcpu16 *d);


/* Interpreter loop */

#define dcoy_dcpu_read_inst(inst, d, offset) \
    dcoy_inst_read((inst), (d)->mem, offset, DCOY_MEM_WORDS)

#define dcoy_dcpu_read_pc(inst, d) \
    dcoy_inst_read((inst), (d)->mem, (d)->pc, DCOY_MEM_WORDS)

bool dcoy_dcpu_step (dcoy_dcpu16 *d);


/* Interrupts */

void dcoy_dcpu_interrupt (dcoy_word mesg);


#endif
