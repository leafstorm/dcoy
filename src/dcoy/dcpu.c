/**
 * dcoy/dcpu.c
 *
 * The primary data structures and emulator loop - implementation
 *
 * (C) 2012, Matthew Frazier
 * Released under the MIT license - see LICENSE for details
 */

#include <stdlib.h>
#include <stdbool.h>
#include "dcoy/dcpu.h"
#include "dcoy/_constants.h"

/* Instance management */

static void init_state (dcoy_dcpu16 *d) {
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
    init_state(d):
}


void dcoy_dcpu_error (dcoy_dcpu16 *d, const char *format, ...) {
    va_list ap;
    dcoy_dcpu_flag_set(DCOY_FLAG_ERROR);
    va_start(ap, format);
    vsnprintf(d->error, DCOY_ERROR_MAXLEN, format, ap);
    va_end(ap);
}


void dcoy_dcpu_recover (dcoy_dcpu16 *d) {
    dcoy_dcpu_flag_unset(DCOY_FLAG_ERROR);
    d->error[0] = '\0';
}


/* Interpreter loop */

#define NEXT_WORD (d->cycles++, d->mem[d->pc++])

static dcoy_word process_a (dcoy_dcpu16 *d, int arg) {
    if (ARG_IS_LITERAL(arg)) {
        return ARG_GET_LITERAL(arg);
    }
    switch (ARG_GET_LEAD(arg)) {
        case ARG_LEAD_RVALUE:
            return d->reg[ARG_GET_REGISTER(arg)];

        case ARG_LEAD_RPOINTER:
            return d->mem[d->reg[ARG_GET_REGISTER(arg)]];

        case ARG_LEAD_ROFFSET:
            return d->mem[d->reg[ARG_GET_REGISTER(arg)] + NEXT_WORD];

        default:
            switch (arg) {
                case ARG_PUSHPOP:       return d->mem[d->sp++];
                case ARG_PEEK:          return d->mem[d->sp];
                case ARG_PICK:          return d->mem[d->sp + NEXT_WORD];
                case ARG_SP:            return d->sp;
                case ARG_PC:            return d->pc;
                case ARG_EX:            return d->ex;
                case ARG_NEXTPOINTER:   return d->mem[NEXT_WORD];
                case ARG_NEXTVALUE:     return NEXT_WORD;
                default:
                    dcoy_dcpu_error(d, "Invalid a argument %x", arg);
                    return 0;
            }
    }
}


static dcoy_word process_b (dcoy_dcpu16 *d, int arg, dcoy_word **dest) {
    *dest = NULL;

    if (ARG_IS_LITERAL(arg)) {
        return ARG_GET_LITERAL(arg);
    }
    switch (ARG_GET_LEAD(arg)) {
        case ARG_LEAD_RVALUE:
            *dest = d->reg + ARG_GET_REGISTER(arg);
            break;

        case ARG_LEAD_RPOINTER:
            *dest = d->mem + d->reg[ARG_GET_REGISTER(arg)];
            break;

        case ARG_LEAD_ROFFSET:
            *dest = d->mem + d->reg[ARG_GET_REGISTER(arg)] + NEXT_WORD;
            break;

        default:
            switch (arg) {
                case ARG_PUSHPOP:       *dest = d->mem + (--(d->sp)); break;
                case ARG_PEEK:          *dest = d->mem + d->sp; break;
                case ARG_PICK:          *dest = d->mem + d->sp + NEXT_WORD;
                                        break;
                case ARG_SP:            *dest = &d->sp; break;
                case ARG_PC:            *dest = &d->pc; break;
                case ARG_EX:            *dest = &d->ex; break;
                case ARG_NEXTPOINTER:   *dest = d->mem + NEXT_WORD; break;
                case ARG_NEXTVALUE:     d->cycles++; *dest = d->mem + d->pc++;
                                        break;
                default:
                    dcoy_dcpu_error(d, "Invalid b argument %x", arg);
                    return 0;
            }
    }
    if (*dest != NULL) return **dest;
    return 0;       /* this should probably never happen */
}


#define GUARD_ERROR \
        if (dcoy_dcpu_flag(DCOY_FLAG_ERROR)) return false

bool dcoy_dcpu_step (dcoy_dcpu16 *d) {
    GUARD_ERROR;
    dcoy_word inst = NEXT_WORD;

    int op = INST_GET_OP(inst);

    if (op != OP_SPEC) {
        /* Standard opcode */
        dcoy_word a, b, *dest;
        dcoy_dword res = 0;
        a = process_a(d, OP_GET_A(inst)); GUARD_ERROR;
        b = process_b(d, OP_GET_B(inst), &dest); GUARD_ERROR;

        switch (op) {
            case OP_SET:    res = a; break;
            case OP_ADD:    res = b + a;
                            d->ex = res >> 16;
                            break;
            case OP_SUB:    res = b - a;
                            d->ex = res >> 16;
                            break;

            case OP_MUL:    res = b * a;
                            d->ex = (res >> 16) & 0xffff;
                            break;
            case OP_MLI:    res = SIGN(b) / SIGN(a);
                            d->ex = (res >> 16) & 0xffff;
                            break;

            case OP_DIV:    if (a == 0) {
                                res = 0; d->ex = 0;
                            } else {
                                res = b / a;
                                d->ex = ((b << 16) / a) & 0xffff;
                            }
                            break;
            case OP_DVI:    if (a == 0) {
                                res = 0; d->ex = 0;
                            } else {
                                dcoy_sword sa = SIGN(a), sb = SIGN(b);
                                res = sb / sa;
                                d->ex = ((sb << 16) / sa) & 0xffff;
                            }
                            break;

            case OP_MOD:    res = a == 0 ? 0 : b % a; break;
            case OP_MDI:    break;

            case OP_AND:    res = b & a; break;
            case OP_BOR:    res = b | a; break;
            case OP_XOR:    res = b ^ a; break;
            case OP_SHR:    break;
            case OP_ASR:    break;
            case OP_SHL:    break;

            case OP_IFB:    break;
            case OP_IFC:    break;
            case OP_IFE:    break;
            case OP_IFN:    break;
            case OP_IFG:    break;
            case OP_IFA:    break;
            case OP_IFL:    break;
            case OP_IFU:    break;

            case OP_ADX:    break;
            case OP_SBX:    break;
            case OP_STI:    res = a; d->reg[I]++; d->reg[J]++; break;
            case OP_STD:    res = a; d->reg[I]--; d->reg[J]--; break;

            default:
                dcoy_dcpu_error(d, "Invalid opcode %x", arg);
                return false;
        }
        if (dest != NULL) *dest = res;
        return true;

    } else {
        /* Special opcode */
        int sop = OP_GET_B(inst);
        dcoy_word a = process_a(d, OP_GET_A(inst));

        switch (sop) {
            case SOP_JSR:   break;

            case SOP_INT:   break;
            case SOP_IAG:   break;
            case SOP_IAS:   break;
            case SOP_RFI:   break;
            case SOP_IAQ:   break;

            case SOP_HWN:   break;
            case SOP_HWQ:   break;
            case SOP_HWI:   break;

            default:
                dcoy_dcpu_error(d, "Invalid special opcode %x", sop);
                return false;
        }
        return true;
    }
}

