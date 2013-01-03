# Dcoy Makefile

CFLAGS=-g -O2 -Wall -Wextra -Isrc $(MYCFLAGS)

### Table of Contents ###

DCOY_LIBRARY=lib/dcoy.a
DCOY_OBJECTS=src/dcoy/code.o src/dcoy/dcpu.o src/dcoy/dcpu/exec.o

DCOY_TOOLS=bin/dcoy-demu

DCOY_SOURCES=src/dcoy/opcodes.h

all: bin lib $(DCOY_SOURCES) $(DCOY_LIBRARY) $(DCOY_TOOLS)


### Core library ###

$(DCOY_LIBRARY): $(DCOY_OBJECTS)
	ar rcs $@ $(DCOY_OBJECTS)
	ranlib $@


### Source files ###

src/dcoy/opcodes.h: src/dcoy/opcodes.h.header src/dcoy/code.h
	@cp src/dcoy/opcodes.h.header $@
	@grep -E "^#define DCOY_(OP|SOP)_" src/dcoy/code.h | \
		sed -Ee "s/DCOY_(OP|SOP)_//" >> $@
	@echo "#endif" >> $@


### Tools ###

bin/dcoy-demu: src/tools/dcoy-demu.o lib/dcoy.a
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)


### Meta-targets ###

clean:
	rm $(DCOY_LIBRARY) $(DCOY_OBJECTS) $(DCOY_TOOLS)

bin:
	@mkdir -p bin

lib:
	@mkdir -p lib
