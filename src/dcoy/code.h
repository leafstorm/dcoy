/**
 * dcoy/code.h
 *
 * Code for parsing and describing assembly code - header
 *
 * (C) 2013, Matthew Frazier
 * Released under the MIT license - see LICENSE for details
 */

#ifndef _dcoy_code_h
#define _dcoy_code_h

#include "dcoy/specs.h"

/* Instruction data structure */

typedef struct dcoy_arg {
    uint8_t type;
    uint8_t reg;
    dcoy_word data;
} dcoy_arg;


typedef struct dcoy_inst {
    bool special;
    uint8_t opcode;
    dcoy_arg a;
    dcoy_arg b;
} dcoy_inst;


/* Instruction parsing */

unsigned int dcoy_inst_read (dcoy_inst *inst, dcoy_word *buf,
                             unsigned int offset, unsigned int bufsize);

#define dcoy_inst_opcode(inst)  ((inst) & 0x1f)
#define dcoy_inst_arg_a(inst)   (((inst) & 0xfc00) >> 10)
#define dcoy_inst_arg_b(inst)   (((inst) & 0x3e0) >> 5)


/* Argument types */
/* R = reg is set, D = data is set */

#define DCOY_ARG_RVALUE     0x00    /* R-: register */
#define DCOY_ARG_RLOOKUP    0x08    /* R-: [register] */
#define DCOY_ARG_ROFFSET    0x10    /* RD: [register + next word] */

#define DCOY_ARG_PUSHPOP    0x18    /* --: [SP++] for read, [--SP] for write */
#define DCOY_ARG_PEEK       0x19    /* --: [SP] */
#define DCOY_ARG_PICK       0x1a    /* -D: [SP + immediate value] */

#define DCOY_ARG_SP         0x1b    /* -- */
#define DCOY_ARG_PC         0x1c    /* -- */
#define DCOY_ARG_EX         0x1d    /* -- */

#define DCOY_ARG_LOOKUP     0x1e    /* -D: [immediate value] */
#define DCOY_ARG_VALUE      0x1f    /* -D: immediate value */


/* Opcodes */

#define DCOY_OP_SPEC 0x00   /* SPECial, not an actual mnemonic */
#define DCOY_OP_SET  0x01   /* SET */
#define DCOY_OP_ADD  0x02   /* ADD */
#define DCOY_OP_SUB  0x03   /* SUBtract */

#define DCOY_OP_MUL  0x04   /* MULtiply */
#define DCOY_OP_MLI  0x05   /* MuLtiply sIgned */
#define DCOY_OP_DIV  0x06   /* DIVide */
#define DCOY_OP_DVI  0x07   /* DiVide sIgned */
#define DCOY_OP_MOD  0x08   /* MODulo */
#define DCOY_OP_MDI  0x09   /* MoDulo sIgned */

#define DCOY_OP_AND  0x0a   /* bitwise AND */
#define DCOY_OP_BOR  0x0b   /* Bitwise OR */
#define DCOY_OP_XOR  0x0c   /* bitwise XOR */
#define DCOY_OP_SHR  0x0d   /* SHift Right */
#define DCOY_OP_ASR  0x0e   /* Arithmetic (signed) Shift Right */
#define DCOY_OP_SHL  0x0f   /* SHift Left */

#define DCOY_OP_IFB  0x10   /* IF all Bits are set */
#define DCOY_OP_IFC  0x11   /* IF all bits are Clear */
#define DCOY_OP_IFE  0x12   /* IF Equal */
#define DCOY_OP_IFN  0x13   /* IF Not equal */
#define DCOY_OP_IFG  0x14   /* IF Greater */
#define DCOY_OP_IFA  0x15   /* IF greater signed ("Above")*/
#define DCOY_OP_IFL  0x16   /* IF Less */
#define DCOY_OP_IFU  0x17   /* IF less signed ("Under") */

#define DCOY_OP_ADX  0x1a   /* add with carry (EX) */
#define DCOY_OP_SBX  0x1b   /* subtract with borrow (EX) */

#define DCOY_OP_STI  0x1e   /* set then increment I, J */
#define DCOY_OP_STD  0x1f   /* set then decrement I, J */


/* Special opcodes */

#define DCOY_SOP_JSR 0x01   /* jump and set return */

#define DCOY_SOP_INT 0x08   /* trigger interrupt */
#define DCOY_SOP_IAG 0x09   /* get interrupt address */
#define DCOY_SOP_IAS 0x0a   /* set interrupt address */
#define DCOY_SOP_RFI 0x0b   /* return from interrupt */
#define DCOY_SOP_IAQ 0x0c   /* enable/disable interrupt queue */

#define DCOY_SOP_HWN 0x10   /* hardware count ("number") */
#define DCOY_SOP_HWQ 0x11   /* hardware query */
#define DCOY_SOP_HWI 0x12   /* hardware interrupt */


/* Disassembly */

void dcoy_inst_write (dcoy_inst inst, char *out);

extern const char *dcoy_opcode_names[];
extern const char *dcoy_spec_opcode_names[];
extern char dcoy_register_names[];

#endif
