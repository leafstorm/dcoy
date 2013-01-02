/**
 * dcoy/dcpu.c
 *
 * The primary data structures and emulator loop - implementation
 *
 * (C) 2013, Matthew Frazier
 * Released under the MIT license - see LICENSE for details
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "dcoy/dcpu.h"
#include "dcoy/code.h"
#include "dcoy/constants.h"
#include "dcoy/opcodes.h"

/* Instance management */

static void init_state (dcoy_dcpu16 *d) {
    (void) d;
    /* Assumes that our DCPU is blank */
    /* We don't have anything to init yet... */
}


dcoy_dcpu16 *dcoy_dcpu_create () {
    void *ptr = calloc(1, sizeof(dcoy_dcpu16));
    if (ptr == NULL) return ptr;
    init_state(ptr);
    return ptr;
}


void dcoy_dcpu_reset (dcoy_dcpu16 *d) {
    memset(d, 0, sizeof(dcoy_dcpu16));
    init_state(d);
}


void dcoy_dcpu_error (dcoy_dcpu16 *d, const char *format, ...) {
    va_list ap;
    dcoy_dcpu_flag_set(d, DCOY_FLAG_ERROR);
    va_start(ap, format);
    vsnprintf(d->error, DCOY_DCPU_ERROR_MAXLEN, format, ap);
    va_end(ap);
}


void dcoy_dcpu_recover (dcoy_dcpu16 *d) {
    dcoy_dcpu_flag_unset(d, DCOY_FLAG_ERROR);
    d->error[0] = '\0';
}


/* Interpreter loop */

#define NEXT_WORD (d->cycles++, d->mem[d->pc++])

static dcoy_word process_a (dcoy_dcpu16 *d, int arg) {
    switch (dcoy_arg_type(arg)) {
        case DCOY_ARG_TYPE_INLINE:
            return dcoy_arg_inline(arg);

        case DCOY_ARG_TYPE_RVALUE:
            return d->reg[dcoy_arg_register(arg)];

        case DCOY_ARG_TYPE_RLOOKUP:
            return d->mem[d->reg[dcoy_arg_register(arg)]];

        case DCOY_ARG_TYPE_ROFFSET:
            return d->mem[d->reg[dcoy_arg_register(arg)] + NEXT_WORD];

        default:
            switch (arg) {
                case DCOY_ARG_PUSHPOP:  return d->mem[d->sp++];
                case DCOY_ARG_PEEK:     return d->mem[d->sp];
                case DCOY_ARG_PICK:     return d->mem[d->sp + NEXT_WORD];
                case DCOY_ARG_SP:       return d->sp;
                case DCOY_ARG_PC:       return d->pc;
                case DCOY_ARG_EX:       return d->ex;
                case DCOY_ARG_NLOOKUP:  return d->mem[NEXT_WORD];
                case DCOY_ARG_NVALUE:   return NEXT_WORD;
                default:
                    dcoy_dcpu_error(d, "Invalid a argument %x", arg);
                    return 0;
            }
    }
}


static dcoy_word process_b (dcoy_dcpu16 *d, int arg, dcoy_word **dest) {
    *dest = NULL;

    switch (dcoy_arg_type(arg)) {
        case DCOY_ARG_TYPE_INLINE:
            return dcoy_arg_inline(arg);

        case DCOY_ARG_TYPE_RVALUE:
            *dest = d->reg + dcoy_arg_register(arg);
            break;

        case DCOY_ARG_TYPE_RLOOKUP:
            *dest = d->mem + d->reg[dcoy_arg_register(arg)];
            break;

        case DCOY_ARG_TYPE_ROFFSET:
            *dest = d->mem + d->reg[dcoy_arg_register(arg)] + NEXT_WORD;
            break;

        default:
            switch (arg) {
                case DCOY_ARG_PUSHPOP:  *dest = d->mem + (--(d->sp)); break;
                case DCOY_ARG_PEEK:     *dest = d->mem + d->sp; break;
                case DCOY_ARG_PICK:     *dest = d->mem + (d->sp + NEXT_WORD);
                                        break;
                case DCOY_ARG_SP:       *dest = &d->sp; break;
                case DCOY_ARG_PC:       *dest = &d->pc; break;
                case DCOY_ARG_EX:       *dest = &d->ex; break;
                case DCOY_ARG_NLOOKUP:  *dest = d->mem + NEXT_WORD; break;
                case DCOY_ARG_NVALUE:   d->cycles++;
                                        *dest = d->mem + d->pc++;
                                        break;
                default:
                    dcoy_dcpu_error(d, "Invalid b argument %x", arg);
                    return 0;
            }
    }
    if (*dest != NULL) return **dest;
    return 0;       /* this should probably never happen */
}


#define GUARD_ERROR if (dcoy_dcpu_flag(d, DCOY_FLAG_ERROR)) return false

bool dcoy_dcpu_step (dcoy_dcpu16 *d) {
    GUARD_ERROR;
    dcoy_word inst = NEXT_WORD;

    int op = dcoy_inst_opcode(inst);

    if (op != SPEC) {
        /* Standard opcode */
        dcoy_word a, b, *dest;
        dcoy_dword res = 0;

        a = process_a(d, dcoy_inst_arg_a(inst)); GUARD_ERROR;
        b = process_b(d, dcoy_inst_arg_b(inst), &dest); GUARD_ERROR;

        switch (op) {
            case SET:   res = a; break;
            case ADD:   res = b + a;
                        d->ex = res >> 16;
                        break;
            case SUB:   res = b - a;
                        d->ex = res >> 16;
                        break;

            case MUL:   res = b * a;
                        d->ex = (res >> 16) & 0xffff;
                        break;
            case MLI:   res = SIGN(b) * SIGN(a);
                        d->ex = (res >> 16) & 0xffff;
                        break;

            case DIV:   if (a == 0) {
                            res = 0; d->ex = 0;
                        } else {
                            res = b / a;
                            d->ex = ((b << 16) / a) & 0xffff;
                        }
                        break;
            case DVI:   if (a == 0) {
                            res = 0; d->ex = 0;
                        } else {
                            dcoy_sword sa = SIGN(a), sb = SIGN(b);
                            res = sb / sa;
                            d->ex = ((sb << 16) / sa) & 0xffff;
                        }
                        break;

            case MOD:   res = a == 0 ? 0 : b % a; break;
            case MDI:   break;

            case AND:   res = b & a; break;
            case BOR:   res = b | a; break;
            case XOR:   res = b ^ a; break;
            case SHR:   break;
            case ASR:   break;
            case SHL:   break;

            case IFB:   break;
            case IFC:   break;
            case IFE:   break;
            case IFN:   break;
            case IFG:   break;
            case IFA:   break;
            case IFL:   break;
            case IFU:   break;

            case ADX:   break;
            case SBX:   break;
            case STI:   res = a; d->reg[I]++; d->reg[J]++; break;
            case STD:   res = a; d->reg[I]--; d->reg[J]--; break;

            default:
                dcoy_dcpu_error(d, "Invalid opcode %x", op);
                return false;
        }
        if (dest != NULL) *dest = res;
        return true;

    } else {
        /* Special opcode */
        int sop = dcoy_inst_arg_b(inst);
        dcoy_word a = process_a(d, dcoy_inst_arg_a(inst));
        (void) a;

        switch (sop) {
            case JSR:   break;

            case INT:   break;
            case IAG:   break;
            case IAS:   break;
            case RFI:   break;
            case IAQ:   break;

            case HWN:   break;
            case HWQ:   break;
            case HWI:   break;

            default:
                dcoy_dcpu_error(d, "Invalid special opcode %x", sop);
                return false;
        }
        return true;
    }
}

