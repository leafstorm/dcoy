/**
 * dcoy/code.c
 *
 * Code for parsing and describing assembly code - implementation
 *
 * (C) 2012, Matthew Frazier
 * Released under the MIT license - see LICENSE for details
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "dcoy/code.h"
#include "dcoy/opcodes.h"
#include "dcoy/specs.h"

const char *dcoy_opcode_names[] = {
    "SPEC", "SET",  "ADD",  "SUB",  "MUL",  "MLI",  "DIV",  "DVI",
    "MOD",  "MDI",  "AND",  "BOR",  "XOR",  "SHR",  "ASL",  "SHL",
    "IFB",  "IFC",  "IFE",  "IFN",  "IFG",  "IFA",  "IFL",  "IFU",
    "18?",  "19?",  "ADX",  "SBX",  "1C?",  "1D?",  "STI",  "STD"
};

const char *dcoy_special_opcode_names[] = {
    "00?",  "JSR",  "02?",  "03?",  "04?",  "05?",  "06?",  "07?",
    "INT",  "IAG",  "IAS",  "RFI",  "IAQ",  "0D?",  "0E?",  "0F?",
    "HWN",  "HWQ",  "HWI",  "13?",  "14?",  "15?",  "16?",  "17?",
    "18?",  "19?",  "1A?",  "1B?",  "1C?",  "1D?",  "1E?",  "1F?"
};

char dcoy_register_names[] = "ABCXYZIJ";


static dcoy_word *dcoy_arg_disassemble (dcoy_word arg, bool is_b, dcoy_word *code, char *out) {
    switch (dcoy_arg_type(arg)) {
        case DCOY_ARG_TYPE_INLINE:
            sprintf(out, "0x%04x", dcoy_arg_inline(arg));
            break;

        case DCOY_ARG_TYPE_RVALUE:
            sprintf(out, "%c", dcoy_register_names[dcoy_arg_register(arg)]);
            break;

        case DCOY_ARG_TYPE_RLOOKUP:
            sprintf(out, "%c", dcoy_register_names[dcoy_arg_register(arg)]);
            break;

        case DCOY_ARG_TYPE_ROFFSET:
            sprintf(out, "[%c + %d]",
                    dcoy_register_names[dcoy_arg_register(arg)], *code++);
            break;

        default:
            switch (arg) {
                case DCOY_ARG_PUSHPOP:  strcpy(out,  is_b ? "push" : "pop"); break;
                case DCOY_ARG_PEEK:     strcpy(out,  "peek"); break;
                case DCOY_ARG_PICK:     sprintf(out, "pick %d", *code++); break;
                case DCOY_ARG_SP:       strcpy(out,  "SP"); break;
                case DCOY_ARG_PC:       strcpy(out,  "PC"); break;
                case DCOY_ARG_EX:       strcpy(out,  "EX"); break;
                case DCOY_ARG_NLOOKUP:  sprintf(out, "[0x%04x]", *code++); break;
                case DCOY_ARG_NVALUE:   sprintf(out, "0x%04x", *code++); break;
            }
    }
    return code;
}


void dcoy_inst_disassemble (dcoy_word *code, char *out) {
    int inst = *code++;
    int op = dcoy_inst_opcode(inst);
    int a = dcoy_inst_arg_a(inst);
    int b = dcoy_inst_arg_b(inst);

    if (op != SPEC) {
        char a_dis[32];
        char b_dis[32];
        code = dcoy_arg_disassemble(a, false, code, a_dis);
        code = dcoy_arg_disassemble(b, true, code, b_dis);

        sprintf(out, "%s %s, %s", dcoy_opcode_names[op], b_dis, a_dis);
    } else {
        char a_dis[32];
        code = dcoy_arg_disassemble(a, false, code, a_dis);

        sprintf(out, "%s %s", dcoy_special_opcode_names[b], a_dis);
    }
}
