/**
 * dcoy/_constants.h
 *
 * Constants describing various system features of the DCPU-16
 *
 * (C) 2012, Matthew Frazier
 * Released under the MIT license - see LICENSE for details
 */

#ifndef _dcoy_specs_h
#define _dcoy_specs_h

#include <stdint.h>

/* System architecture */

typedef uint16_t    dcoy_word;


/* Registers */

#define DCOY_REGISTERS   8
#define DCOY_A   0
#define DCOY_B   1
#define DCOY_C   2
#define DCOY_X   3
#define DCOY_Y   4
#define DCOY_Z   5
#define DCOY_I   6
#define DCOY_J   7


/* Memory */

#define DCOY_MEM_WORDS   0x10000


/* Instructions */

#define DCOY_INST_GET_OP(inst)   ((inst) & 0x1f)
#define DCOY_INST_GET_A(inst)    (((inst) & 0xfc00) >> 10)
#define DCOY_INST_GET_B(inst)    (((inst) & 0x3e0) >> 5)


/* Opcodes */

#define DCOY_OP_SPEC 0x00
#define DCOY_OP_SET  0x01
#define DCOY_OP_ADD  0x02
#define DCOY_OP_SUB  0x03

#define DCOY_OP_MUL  0x04
#define DCOY_OP_MLI  0x05
#define DCOY_OP_DIV  0x06
#define DCOY_OP_DVI  0x07
#define DCOY_OP_MOD  0x08
#define DCOY_OP_MDI  0x09

#define DCOY_OP_AND  0x0a
#define DCOY_OP_BOR  0x0b
#define DCOY_OP_XOR  0x0c
#define DCOY_OP_SHR  0x0d
#define DCOY_OP_ASR  0x0e
#define DCOY_OP_SHL  0x0f

#define DCOY_OP_IFB  0x10
#define DCOY_OP_IFC  0x11
#define DCOY_OP_IFE  0x12
#define DCOY_OP_IFN  0x13
#define DCOY_OP_IFG  0x14
#define DCOY_OP_IFA  0x15
#define DCOY_OP_IFL  0x16
#define DCOY_OP_IFU  0x17

#define DCOY_OP_ADX  0x1a
#define DCOY_OP_SBX  0x1b
#define DCOY_OP_STI  0x1e
#define DCOY_OP_STD  0x1f


/* Special opcodes */

#define DCOY_SOP_JSR 0x01

#define DCOY_SOP_INT 0x08
#define DCOY_SOP_IAG 0x09
#define DCOY_SOP_IAS 0x0a
#define DCOY_SOP_RFI 0x0b
#define DCOY_SOP_IAQ 0x0c

#define DCOY_SOP_HWN 0x10
#define DCOY_SOP_HWQ 0x11
#define DCOY_SOP_HWI 0x12


/* Argument */

#define DCOY_ARG_IS_LITERAL(arg)     ((arg) & 0x20)
#define DCOY_ARG_GET_LITERAL(arg)    (((arg) & 0x1f) - 1)

#define DCOY_ARG_GET_LEAD(arg)   ((arg) & 0x18)
#define DCOY_ARG_GET_REG(arg)    ((arg) & 0x7)
#define DCOY_ARG_LEAD_RVALUE     0x0     /* register */
#define DCOY_ARG_LEAD_RPOINTER   0x8     /* [register] */
#define DCOY_ARG_LEAD_ROFFSET    0x10    /* [register + next word] */

#define DCOY_ARG_PUSHPOP     0x18
#define DCOY_ARG_PEEK        0x19
#define DCOY_ARG_PICK        0x1a
#define DCOY_ARG_SP          0x1b
#define DCOY_ARG_PC          0x1c
#define DCOY_ARG_EX          0x1d
#define DCOY_ARG_NEXTVALUE   0x1e
#define DCOY_ARG_NEXTPOINTER 0x1f


/* Hardware */

#define DCOY_HARDWARE_LIMIT  65535

#endif
