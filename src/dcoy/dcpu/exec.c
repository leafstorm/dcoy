/**
 * dcoy/dcpu/exec.c
 *
 * The core of the interpreter loop - implementation
 *
 * (C) 2013, Matthew Frazier
 * Released under the MIT license - see LICENSE for details
 */

#include <stdbool.h>

#include "dcoy/code.h"
#include "dcoy/dcpu.h"
#include "dcoy/constants.h"
#include "dcoy/opcodes.h"

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
        default:                dcoy_dcpu_error(d, INVALID_ARG_TYPE, arg.type);
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
        default:                dcoy_dcpu_error(d, INVALID_ARG_TYPE, arg.type);
                                break;
    }
}


#define GUARD_ERROR if ((d)->error_code) return false

#define SIGN(word)  ((dcoy_sword) (word))

#define USE_A do {a = get(d, inst.a); GUARD_ERROR;} while (0)
#define USE_B do {b = get(d, inst.b); GUARD_ERROR;} while (0)

/* break_res sets b to res and breaks */
#define break_res set(d, inst.b, res); break
/* break_res sets b to res, then EX to ex, and breaks */
#define break_math set(d, inst.b, res); d->ex = ex; break

bool dcoy_dcpu_exec (dcoy_dcpu16 *d, dcoy_inst inst) {
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

            default:    dcoy_dcpu_error(d, INVALID_OPCODE, inst.opcode);
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

            default:    dcoy_dcpu_error(d, INVALID_SPEC_OPCODE, inst.opcode);
                        return false;
        }
        return true;
    }
}
