/**
 * dcoy/opcodes.h
 *
 * Aliases for all opcodes
 *
 * (C) 2012, Matthew Frazier
 * Released under the MIT license - see LICENSE for details
 */

#ifndef _dcoy_opcodes_h
#define _dcoy_opcodes_h

#define SPEC 0x00   /* SPECial, not an actual mnemonic */
#define SET  0x01   /* SET */
#define ADD  0x02   /* ADD */
#define SUB  0x03   /* SUBtract */
#define MUL  0x04   /* MULtiply */
#define MLI  0x05   /* MuLtiply sIgned */
#define DIV  0x06   /* DIVide */
#define DVI  0x07   /* DiVide sIgned */
#define MOD  0x08   /* MODulo */
#define MDI  0x09   /* MoDulo sIgned */
#define AND  0x0a   /* bitwise AND */
#define BOR  0x0b   /* Bitwise OR */
#define XOR  0x0c   /* bitwise XOR */
#define SHR  0x0d   /* SHift Right */
#define ASR  0x0e   /* Arithmetic (signed) Shift Right */
#define SHL  0x0f   /* SHift Left */
#define IFB  0x10   /* IF all Bits are set */
#define IFC  0x11   /* IF all bits are Clear */
#define IFE  0x12   /* IF Equal */
#define IFN  0x13   /* IF Not equal */
#define IFG  0x14   /* IF Greater */
#define IFA  0x15   /* IF greater signed ("Above")*/
#define IFL  0x16   /* IF Less */
#define IFU  0x17   /* IF less signed ("Under") */
#define ADX  0x1a   /* add with carry (EX) */
#define SBX  0x1b   /* subtract with borrow (EX) */
#define STI  0x1e   /* set then increment I, J */
#define STD  0x1f   /* set then decrement I, J */
#define JSR 0x01   /* jump and set return */
#define INT 0x08   /* trigger interrupt */
#define IAG 0x09   /* get interrupt address */
#define IAS 0x0a   /* set interrupt address */
#define RFI 0x0b   /* return from interrupt */
#define IAQ 0x0c   /* enable/disable interrupt queue */
#define HWN 0x10   /* hardware count ("number") */
#define HWQ 0x11   /* hardware query */
#define HWI 0x12   /* hardware interrupt */
#endif
