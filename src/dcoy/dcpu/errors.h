/**
 * dcoy/dcpu/errors.h
 *
 * Error codes and messages for the emulator
 *
 * (C) 2013, Matthew Frazier
 * Released under the MIT license - see LICENSE for details
 */

#ifndef _dcoy_dcpu_errors_h
#define _dcoy_dcpu_errors_h

#define DCOY_DCPU_ERROR_NONE                    0x00
#define DCOY_DCPU_ERROR_MSG_NONE                NULL

/* 0x1_: Assembly format errors */
#define DCOY_DCPU_ERROR_INVALID_OPCODE          0x10    /* data: opcode */
#define DCOY_DCPU_ERROR_MSG_INVALID_OPCODE      "Invalid binary opcode"

#define DCOY_DCPU_ERROR_INVALID_SPEC_OPCODE     0x11    /* data: opcode */
#define DCOY_DCPU_ERROR_MSG_INVALID_SPEC_OPCODE "Invalid special opcode"

#define DCOY_DCPU_ERROR_INVALID_ARG_TYPE        0x14    /* data: argument type */
#define DCOY_DCPU_ERROR_MSG_INVALID_ARG_TYPE    "Invalid argument type"

#endif
