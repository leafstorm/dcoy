/**
 * dcoy/specs.h
 *
 * Constants and typedefs describing the DCPU-16
 *
 * (C) 2013, Matthew Frazier
 * Released under the MIT license - see LICENSE for details
 */

#ifndef _dcoy_specs_h
#define _dcoy_specs_h


#include <stdint.h>

/* System architecture */

typedef uint16_t    dcoy_word;
typedef int16_t     dcoy_sword;
typedef uint32_t    dcoy_dword;
typedef int32_t     dcoy_sdword;

#define DCOY_MEM_WORDS      0x10000


/* Registers */

#define DCOY_REG_COUNT      8

#define DCOY_REG_A  0
#define DCOY_REG_B  1
#define DCOY_REG_C  2
#define DCOY_REG_X  3
#define DCOY_REG_Y  4
#define DCOY_REG_Z  5
#define DCOY_REG_I  6
#define DCOY_REG_J  7


/* Interrupts */

#define DCOY_INT_QUEUE_SIZE 256


/* Hardware */

typedef uint32_t    dcoy_hardware_mfid_t;
typedef uint32_t    dcoy_hardware_id_t;
typedef uint16_t    dcoy_hardware_version_t;

#define DCOY_HARDWARE_LIMIT 65535

#endif
