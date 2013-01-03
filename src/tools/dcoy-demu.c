/**
 * tools/dcoy-demu.c
 *
 * A simple tool that runs a DCPU image and dumps the machine state
 * (demu = debugging emulator)
 *
 * (C) 2013, Matthew Frazier
 * Released under the MIT license - see LICENSE for details
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "dcoy/dcpu.h"
#include "dcoy/code.h"
#include "dcoy/constants.h"
#include "dcoy/specs.h"

int load_image (dcoy_dcpu16 *d, const char *filename) {
    FILE *fd = fopen(filename, "r");
    if (!fd) {
        return -1;
    }

    int image_size = fread(d->mem, 2, DCOY_MEM_WORDS, fd);
    if (ferror(fd)) {
        return -1;
    }
    fclose(fd);

    return image_size;
}


int main (int argc, char *argv[]) {
    if (argc < 2) {
        printf("usage: dcoy-demu IMAGE\n");
        return 1;
    }

    dcoy_dcpu16 *d = dcoy_dcpu_create();
    int image_size = load_image(d, argv[1]);

    if (image_size < 0) {
        printf("can't read image from %s: %s\n", argv[1], strerror(errno));
        return 2;
    }

    printf(
        "Clock   PC    A    B    C    X    Y    Z    I    J    EX    About to run\n"
        "------  ----  ---- ---- ---- ---- ---- ---- ---- ---- ----  ------------\n"
    );

    dcoy_inst current_inst;
    char disassembled[64] = "";

    while (dcoy_dcpu_running(d)) {
        dcoy_dcpu_read_pc(&current_inst, d);
        dcoy_inst_write(current_inst, disassembled);

        printf(
            "%-6d  %04x  %04x %04x %04x %04x %04x %04x %04x %04x %04x  %s\n",
            d->cycles, d->pc,
            d->reg[A], d->reg[B], d->reg[C], d->reg[X], d->reg[Y], d->reg[Z],
            d->reg[I], d->reg[J], d->ex,
            disassembled
        );
        (void)dcoy_dcpu_step(d);
    }

    printf("        Error: %s 0x%04x (%d) at 0x%04x\n",
           d->error_message, d->error_data, d->error_data, d->error_pc);
}
