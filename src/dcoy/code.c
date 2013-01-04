/**
 * dcoy/code.c
 *
 * Code for parsing and describing assembly code - implementation
 *
 * (C) 2013, Matthew Frazier
 * Released under the MIT license - see LICENSE for details
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "dcoy/code.h"
#include "dcoy/opcodes.h"
#include "dcoy/specs.h"


/* Instruction parsing */

/* Returns true if the next_word was used, false if not */
static bool read_arg (dcoy_arg *arg, unsigned int lead, dcoy_word next_word) {
    if (lead >= 0x20) {
        /* Inline immediate */
        arg->type = DCOY_ARG_IVALUE;
        arg->reg = 0;
        arg->data = (lead & 0x1f) - 1;
        return false;
    } else if (lead >= 0x18) {
        /* Non-register-linked argument */
        arg->type = lead;
        arg->reg = 0;
        switch (lead) {
            case DCOY_ARG_PICK:
            case DCOY_ARG_LOOKUP:
            case DCOY_ARG_VALUE:
                arg->data = next_word;
                return true;
            default:
                arg->data = 0;
                return false;
        }
    } else {
        /* Register-linked argument */
        arg->type = lead & 0x18;
        arg->reg = lead & 0x07;
        if (arg->type == DCOY_ARG_ROFFSET) {
            arg->data = next_word;
            return true;
        } else {
            arg->data = 0;
            return false;
        }
    }
}


unsigned int dcoy_inst_read (dcoy_inst *inst, dcoy_word *buf,
                             unsigned int offset, unsigned int bufsize) {
    unsigned int size = 1;
    dcoy_word lead = buf[offset];
    offset = bufsize ? (offset + 1) % bufsize : offset + 1;

    int opcode = dcoy_inst_opcode(lead);

    if (opcode == SPEC) {
        inst->special = true;
        inst->opcode = dcoy_inst_arg_b(lead);

        if (read_arg(&inst->a, dcoy_inst_arg_a(lead), buf[offset])) {
            size++;
            offset = bufsize ? (offset + 1) % bufsize : offset + 1;
        }
    } else {
        inst->special = false;
        inst->opcode = opcode;

        if (read_arg(&inst->a, dcoy_inst_arg_a(lead), buf[offset])) {
            size++;
            offset = bufsize ? (offset + 1) % bufsize : offset + 1;
        }

        if (read_arg(&inst->b, dcoy_inst_arg_b(lead), buf[offset])) {
            size++;
            offset = bufsize ? (offset + 1) % bufsize : offset + 1;
        }
    }

    return size;
}


/* Instruction costing */

unsigned int dcoy_inst_base_cost (dcoy_inst inst) {
    unsigned int base = 0;

    if (inst.special) {
        base = inst.opcode < 0x20
             ? dcoy_special_opcode_base_costs[inst.opcode]
             : 0;
        return base ? base + dcoy_arg_base_cost(inst.a) : 0;
    } else {
        base = inst.opcode < 0x20
             ? dcoy_opcode_base_costs[inst.opcode]
             : 0;
        return base ? base + dcoy_arg_base_cost(inst.a) +
                             dcoy_arg_base_cost(inst.b)
                    : 0;
    }
}


unsigned int dcoy_arg_base_cost (dcoy_arg arg) {
    switch (arg.type) {
        case DCOY_ARG_ROFFSET:
        case DCOY_ARG_PICK:
        case DCOY_ARG_LOOKUP:
        case DCOY_ARG_VALUE:
            return 1;

        default:
            return 0;
    }
}


const unsigned int dcoy_opcode_base_costs[] = {
    0,  1,  2,  2,  2,  2,  3,  3,
    3,  3,  1,  1,  1,  1,  1,  1,
    2,  2,  2,  2,  2,  2,  2,  2,
    0,  0,  3,  3,  0,  0,  2,  2
};

const unsigned int dcoy_special_opcode_base_costs[] = {
    0,  3,  0,  0,  0,  0,  0,  0,
    4,  1,  1,  3,  2,  0,  0,  0,
    2,  4,  4,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0
};


/* Disassembly */

const char *dcoy_opcode_names[] = {
    "SPEC", "SET",  "ADD",  "SUB",  "MUL",  "MLI",  "DIV",  "DVI",
    "MOD",  "MDI",  "AND",  "BOR",  "XOR",  "SHR",  "ASR",  "SHL",
    "IFB",  "IFC",  "IFE",  "IFN",  "IFG",  "IFA",  "IFL",  "IFU",
    NULL,   NULL,   "ADX",  "SBX",  NULL,   NULL,   "STI",  "STD"
};

const char *dcoy_special_opcode_names[] = {
    NULL,   "JSR",  NULL,   NULL,   NULL,   NULL,   NULL,   NULL,
    "INT",  "IAG",  "IAS",  "RFI",  "IAQ",  NULL,   NULL,   NULL,
    "HWN",  "HWQ",  "HWI",  NULL,   NULL,   NULL,   NULL,   NULL,
    NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL
};

char dcoy_register_names[] = "ABCXYZIJ";


void dcoy_arg_write (dcoy_arg arg, bool set_ctx, char *out) {
    switch (arg.type) {
        case DCOY_ARG_RVALUE:   sprintf(out, "%c",
                                             dcoy_register_names[arg.reg]);
                                break;
        case DCOY_ARG_RLOOKUP:  sprintf(out, "[%c]",
                                             dcoy_register_names[arg.reg]);
                                break;
        case DCOY_ARG_ROFFSET:  sprintf(out, "[%c + %d]",
                                             dcoy_register_names[arg.reg],
                                             arg.data);
                                break;
        case DCOY_ARG_PUSHPOP:  strcpy(out,  set_ctx ? "PUSH" : "POP"); break;
        case DCOY_ARG_PEEK:     strcpy(out,  "PICK");                   break;
        case DCOY_ARG_PICK:     sprintf(out, "PICK %d", arg.data);      break;
        case DCOY_ARG_SP:       strcpy(out,  "SP");                     break;
        case DCOY_ARG_PC:       strcpy(out,  "PC");                     break;
        case DCOY_ARG_EX:       strcpy(out,  "EX");                     break;
        case DCOY_ARG_LOOKUP:   sprintf(out, "[0x%04x]", arg.data);     break;
        case DCOY_ARG_VALUE:
        case DCOY_ARG_IVALUE:   if (arg.data == 0xffff || arg.data < 10) {
                                    sprintf(out, "%d", (dcoy_sword)(arg.data));
                                } else {
                                    sprintf(out, "0x%04x", arg.data);
                                }
                                break;
        default:                sprintf(out, "<0x%02x>", arg.type);     break;
    }
}


void dcoy_inst_write (dcoy_inst inst, char *out) {
    unsigned int op = inst.opcode;

    if (inst.special) {
        const char *mnemonic = op < 0x20 ? dcoy_special_opcode_names[op] : NULL;

        char a_dis[32];
        dcoy_arg_write(inst.a, false, a_dis);

        if (mnemonic) {
            sprintf(out, "%s %s", mnemonic, a_dis);
        } else {
            sprintf(out, "%02x? %s", op, a_dis);
        }
    } else {
        const char *mnemonic = op < 0x20 ? dcoy_opcode_names[op] : NULL;

        char a_dis[32];
        char b_dis[32];
        dcoy_arg_write(inst.a, false, a_dis);
        dcoy_arg_write(inst.b, true, b_dis);

        if (mnemonic) {
            sprintf(out, "%s %s, %s", dcoy_opcode_names[op], b_dis, a_dis);
        } else {
            sprintf(out, "%02x? %s, %s", op, b_dis, a_dis);
        }
    }
}
