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

static dcoy_word get (dcoy_dcpu16 *d, dcoy_arg arg) {
    switch (arg.type) {
        case DCOY_ARG_RVALUE:   return d->reg[arg.reg];
        case DCOY_ARG_RLOOKUP:  return d->mem[d->mem[arg.reg]];
        case DCOY_ARG_ROFFSET:  return d->mem[(dcoy_word)(arg.reg + arg.data)];
        case DCOY_ARG_PUSHPOP:  return d->mem[d->sp++];
        case DCOY_ARG_PEEK:     return d->mem[d->sp];
        case DCOY_ARG_PICK:     return d->mem[(dcoy_word)(d->sp + arg.data)];
        case DCOY_ARG_SP:       return d->sp;
        case DCOY_ARG_PC:       return d->pc;
        case DCOY_ARG_EX:       return d->ex;
        case DCOY_ARG_LOOKUP:   return d->mem[arg.data];
        case DCOY_ARG_VALUE:    return arg.data;
        default:                dcoy_dcpu_error(d, "Invalid argument type %x",
                                                arg.type);
                                return 0;
    }
}


static void set (dcoy_dcpu16 *d, dcoy_arg arg, dcoy_word value) {
    switch (arg.type) {
        case DCOY_ARG_RVALUE:   d->reg[arg.reg] = value;                break;
        case DCOY_ARG_RLOOKUP:  d->mem[d->mem[arg.reg]] = value;        break;
        case DCOY_ARG_ROFFSET:  d->mem[(dcoy_word)(arg.reg + arg.data)] = value;
                                break;
        case DCOY_ARG_PUSHPOP:  d->mem[--d->sp] = value;                break;
        case DCOY_ARG_PEEK:     d->mem[d->sp] = value;                  break;
        case DCOY_ARG_PICK:     d->mem[(dcoy_word)(d->sp + arg.data)] = value;
                                break;
        case DCOY_ARG_SP:       d->sp = value;                          break;
        case DCOY_ARG_PC:       d->pc = value;                          break;
        case DCOY_ARG_EX:       d->ex = value;                          break;
        case DCOY_ARG_LOOKUP:   d->mem[arg.data] = value;               break;
        case DCOY_ARG_VALUE:    break;
        default:                dcoy_dcpu_error(d, "Invalid argument type %x",
                                                arg.type);
                                break;
    }
}


#define GUARD_ERROR if (dcoy_dcpu_flag(d, DCOY_FLAG_ERROR)) return false

#define SIGN(word)  ((dcoy_sword) (word))

#define USE_A do {a = get(d, inst.a); GUARD_ERROR;} while (0)
#define USE_B do {b = get(d, inst.b); GUARD_ERROR;} while (0)

/* break_res sets b to res and breaks */
#define break_res set(d, inst.b, res); break
/* break_res sets b to res, then EX to ex, and breaks */
#define break_math set(d, inst.b, res); d->ex = ex; break

bool dcoy_dcpu_step (dcoy_dcpu16 *d) {
    GUARD_ERROR;
    dcoy_inst inst;
    unsigned int inst_size = dcoy_dcpu_read_pc(&inst, d);
    d->pc += inst_size;

    if (!inst.special) {
        /* Standard opcode */
        dcoy_word a = 0, b = 0;
        dcoy_dword res = 0;
        dcoy_word ex = 0;

        switch (inst.opcode) {
            case SET:   USE_A;
                        set(d, inst.b, a);
                        break;

            case ADD:   USE_A; USE_B;
                        res = a + b;
                        ex = res >> 16;
                        break_math;

            case SUB:   USE_A; USE_B;
                        res = b - a;
                        ex = res >> 16;
                        break_math;

            case MUL:   USE_A; USE_B;
                        res = b * a;
                        ex = (res >> 16) & 0xffff;
                        break_math;

            case MLI:   USE_A; USE_B;
                        res = SIGN(b) * SIGN(a);
                        ex = (res >> 16) & 0xffff;
                        break_math;

            case DIV:   USE_A; USE_B;
                        if (a == 0) {
                            res = 0; ex = 0;
                        } else {
                            res = b / a;
                            ex = ((b << 16) / a) & 0xffff;
                        }
                        break_math;

            case DVI:   USE_A; USE_B;
                        if (a == 0) {
                            res = 0; ex = 0;
                        } else {
                            dcoy_sword sa = SIGN(a), sb = SIGN(b);
                            res = sb / sa;
                            ex = ((sb << 16) / sa) & 0xffff;
                        }
                        break_math;

            case MOD:   USE_A; USE_B;
                        res = a == 0 ? 0 : b % a;
                        break_res;

            case MDI:   USE_A; USE_B;
                        res = a == 0 ? 0 : SIGN(b) % SIGN(a);
                        break_res;

            case AND:   USE_A; USE_B;
                        res = b & a;
                        break_res;

            case BOR:   USE_A; USE_B;
                        res = b | a;
                        break_res;

            case XOR:   USE_A; USE_B;
                        res = b ^ a;
                        break_res;

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

            case STI:   USE_A;
                        set(d, inst.a, a);
                        d->reg[I]++;
                        d->reg[J]++;
                        break;

            case STD:   USE_A;
                        set(d, inst.b, a);
                        d->reg[I]--;
                        d->reg[J]--;
                        break;

            default:
                dcoy_dcpu_error(d, "Invalid opcode %x", inst.opcode);
                return false;
        }
        return true;

    } else {
        switch (inst.opcode) {
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
                dcoy_dcpu_error(d, "Invalid special opcode %x", inst.opcode);
                return false;
        }
        return true;
    }
}

