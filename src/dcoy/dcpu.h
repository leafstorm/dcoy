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

#define DCOY_DCPU_FLAG_QUEUE_INTS       (1 << 0)
#define DCOY_DCPU_FLAG_HALT             (1 << 4)
#define DCOY_DCPU_FLAG_ON_FIRE          (1 << 5)

#define dcoy_dcpu_flag(d, flag)         ((d)->flags & (flag))
#define dcoy_dcpu_flag_set(d, flag)     ((d)->flags |= (flag))
#define dcoy_dcpu_flag_unset(d, flag)   ((d)->flags &= ~(flag))


/* DCPU emulator structure */

typedef struct dcoy_dcpu16 {
    unsigned int cycles;
    unsigned int flags;

    dcoy_word reg[DCOY_REG_COUNT];
    dcoy_word pc;
    dcoy_word sp;
    dcoy_word ex;
    dcoy_word ia;

    dcoy_word mem[DCOY_MEM_WORDS];

    dcoy_word iq[DCOY_INT_QUEUE_SIZE];
    unsigned int iq_front;
    unsigned int iq_back;

    unsigned int error_code;
    const char *error_message;
    dcoy_word error_data;
    dcoy_word error_pc;

} dcoy_dcpu16;


/* Instance management functions */

dcoy_dcpu16 *dcoy_dcpu_create ();
void dcoy_dcpu_initialize (dcoy_dcpu16 *d);


/* Errors */

#include "dcoy/dcpu/errors.h"

void dcoy_dcpu_error_set (dcoy_dcpu16 *d, unsigned int code,
                          const char *message, dcoy_word data);
void dcoy_dcpu_error_clear (dcoy_dcpu16 *d);

#define dcoy_dcpu_error(d, error, data) dcoy_dcpu_error_set( \
    d, DCOY_DCPU_ERROR_##error, DCOY_DCPU_ERROR_MSG_##error, data \
)


/* Instruction execution */

#define dcoy_dcpu_read_inst(inst, d, offset) \
    dcoy_inst_read((inst), (d)->mem, offset, DCOY_MEM_WORDS)

#define dcoy_dcpu_read_pc(inst, d) \
    dcoy_inst_read((inst), (d)->mem, (d)->pc, DCOY_MEM_WORDS)

/* implemented in dcoy/dcpu/exec.c */
bool dcoy_dcpu_exec (dcoy_dcpu16 *d, dcoy_inst inst);


/* Interpreter loop */

bool dcoy_dcpu_step (dcoy_dcpu16 *d);


/* Interrupts */

void dcoy_dcpu_interrupt (dcoy_word mesg);


#endif
