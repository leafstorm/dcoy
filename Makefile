# DCOY Makefile

src/dcoy/constants.h: src/dcoy/_constants.h
	perl -pe "s/_constants/constants/; s/#define ([A-Z][A-Z_]*)/#define DCOY_\1/" \
		src/dcoy/_constants.h > src/dcoy/constants.h
