/**
 * dcoy/dcpu.c
 *
 * The primary data structures and emulator loop - implementation
 *
 * (C) 2013, Matthew Frazier
 * Released under the MIT license - see LICENSE for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "dcoy/dcpu.h"
#include "dcoy/code.h"
#include "dcoy/constants.h"

/* Instance management */

static void initialize (dcoy_dcpu16 *d) {
    /* Assumes that our DCPU is blank */
    d->error_message = NULL;
}


dcoy_dcpu16 *dcoy_dcpu_create () {
    void *ptr = calloc(1, sizeof(dcoy_dcpu16));
    if (ptr == NULL) return ptr;
    initialize(ptr);
    return ptr;
}


void dcoy_dcpu_initialize (dcoy_dcpu16 *d) {
    memset(d, 0, sizeof(dcoy_dcpu16));
    initialize(d);
}


/* Errors */

void dcoy_dcpu_error_set (dcoy_dcpu16 *d, unsigned int code,
                          const char *message, dcoy_word data) {
    d->error_code = code;
    d->error_message = message;
    d->error_data = data;
    d->error_pc = d->pc;

    /* in the future, error interceptors will be invoked here */

    dcoy_dcpu_halt(d);
}


void dcoy_dcpu_error_clear (dcoy_dcpu16 *d) {
    d->error_code = DCOY_DCPU_ERROR_NONE;
    d->error_message = NULL;
    d->error_data = 0;
    d->error_pc = 0;
}


/* Interpreter loop */

unsigned int dcoy_dcpu_step (dcoy_dcpu16 *d) {
    /* Check that the DCPU is still online. */
    if (dcoy_dcpu_flag(d, DCOY_DCPU_FLAG_HALT)) {
        return false;
    }

    /* Read an instruction and increment PC accordingly. */
    dcoy_inst inst;
    unsigned int inst_size = dcoy_dcpu_read_pc(&inst, d);
    d->pc += inst_size;

    /* Run the instruction and incur the cost. */
    unsigned int cost = dcoy_dcpu_exec(d, inst);
    d->cycles += cost;

    /* Trigger one interrupt after each instruction.
     * This provides the most predictable behavior, since it means
     * INT instructions take effect immediately, and the host will
     * be able to see the interrupted state. */
    dcoy_dcpu_interrupt_trigger(d);

    return cost;
}


/* Interrupts */

bool dcoy_dcpu_interrupt (dcoy_dcpu16 *d, dcoy_word message) {
    if (d->int_queue_count == DCOY_INT_QUEUE_SIZE) {
        /* queue's already full - ignite the DCPU instead */
        dcoy_dcpu_flag_set(d, DCOY_DCPU_FLAG_ON_FIRE);
        return false;
    } else {
        /* store it at the end of the queue */
        unsigned int end = (d->int_queue_start + d->int_queue_count)
                                % DCOY_INT_QUEUE_SIZE;
        d->int_queue[end] = message;
        d->int_queue_count++;
        return true;
    }
}

unsigned int dcoy_dcpu_interrupt_trigger (dcoy_dcpu16 *d) {
    if (dcoy_dcpu_flag(d, DCOY_DCPU_FLAG_IAQ)) {
        return DCOY_DCPU_INT_IAQ_ON;
    }

    if (d->int_queue_count == 0) {
        return DCOY_DCPU_INT_NONE_QUEUED;
    }

    /* dequeue the next interrupt to process */
    dcoy_word message = d->int_queue[d->int_queue_start];
    d->int_queue_start = (d->int_queue_start + 1) % DCOY_INT_QUEUE_SIZE;
    --d->int_queue_count;

    /* if no IA is set, we don't have to actually deliver interrupts */
    if (d->ia) {
        /* block interrupt delivery */
        dcoy_dcpu_flag_set(d, DCOY_DCPU_FLAG_IAQ);
        /* push PC and A to the stack, in that order */
        d->mem[--d->sp] = d->pc;
        d->mem[--d->sp] = d->reg[A];
        /* set A to the message and jump to IA */
        d->reg[A] = message;
        d->pc = d->ia;
        /* the interrupt will be handled when the interpreter resumes */
        return DCOY_DCPU_INT_TRIGGERED;
    } else {
        return DCOY_DCPU_INT_NO_HANDLER;
    }
}
