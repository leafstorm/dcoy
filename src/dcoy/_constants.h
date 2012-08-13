/**
 * dcoy/_constants.h
 *
 * Constants describing various system features of the DCPU-16
 *
 * (C) 2012, Matthew Frazier
 * Released under the MIT license - see LICENSE for details
 */

#ifndef _dcoy__constants_h
#define _dcoy__constants_h

#include <stdint.h>

/* System architecture */

typedef uint16_t    dcoy_word;
typedef int16_t     dcoy_sword;
typedef uint32_t    dcoy_dword;
typedef int32_t     dcoy_sdword;

#define SIGN(word)  ((dcoy_sword) (word))

#define MEM_WORDS   0x10000


/* Registers */

#define REGISTERS   8
#define A   0
#define B   1
#define C   2
#define X   3
#define Y   4
#define Z   5
#define I   6
#define J   7


/* Instruction parsing */

#define INST_GET_OP(inst)   ((inst) & 0x1f)
#define INST_GET_A(inst)    (((inst) & 0xfc00) >> 10)
#define INST_GET_B(inst)    (((inst) & 0x3e0) >> 5)


/* Opcodes */

#define OP_IS_CONDITIONAL(op)   (((op) & 0x18) == 0x10)

#define OP_SPEC 0x00
#define OP_SET  0x01
#define OP_ADD  0x02
#define OP_SUB  0x03

#define OP_MUL  0x04
#define OP_MLI  0x05
#define OP_DIV  0x06
#define OP_DVI  0x07
#define OP_MOD  0x08
#define OP_MDI  0x09

#define OP_AND  0x0a
#define OP_BOR  0x0b
#define OP_XOR  0x0c
#define OP_SHR  0x0d
#define OP_ASR  0x0e
#define OP_SHL  0x0f

#define OP_IFB  0x10
#define OP_IFC  0x11
#define OP_IFE  0x12
#define OP_IFN  0x13
#define OP_IFG  0x14
#define OP_IFA  0x15
#define OP_IFL  0x16
#define OP_IFU  0x17

#define OP_ADX  0x1a
#define OP_SBX  0x1b
#define OP_STI  0x1e
#define OP_STD  0x1f


/* Special opcodes */

#define SOP_JSR 0x01

#define SOP_INT 0x08
#define SOP_IAG 0x09
#define SOP_IAS 0x0a
#define SOP_RFI 0x0b
#define SOP_IAQ 0x0c

#define SOP_HWN 0x10
#define SOP_HWQ 0x11
#define SOP_HWI 0x12


/* Argument */

#define ARG_IS_LITERAL(arg)     ((arg) & 0x20)
#define ARG_GET_LITERAL(arg)    (((arg) & 0x1f) - 1)

#define ARG_SKIP_WORDS(arg)    \
    (ARG_GET_LEAD(arg) == ARG_LEAD_ROFFSET || \
     arg == ARG_PICK || arg == ARG_NEXTPOINTER || arg == ARG_NEXTVALUE)

#define ARG_GET_LEAD(arg)   ((arg) & 0x18)
#define ARG_GET_REG(arg)    ((arg) & 0x7)
#define ARG_LEAD_RVALUE     0x0     /* register */
#define ARG_LEAD_RPOINTER   0x8     /* [register] */
#define ARG_LEAD_ROFFSET    0x10    /* [register + next word] */

#define ARG_PUSHPOP     0x18
#define ARG_PEEK        0x19
#define ARG_PICK        0x1a
#define ARG_SP          0x1b
#define ARG_PC          0x1c
#define ARG_EX          0x1d
#define ARG_NEXTPOINTER 0x1e
#define ARG_NEXTVALUE   0x1f


/* Interrupts */

#define INTERRUPT_MAX   256


/* Hardware */

typedef uint32_t dcoy_mfid_t;
typedef uint32_t dcoy_hwid_t;
typedef uint16_t dcoy_hwversion_t;

#define HARDWARE_LIMIT  65535

#endif
