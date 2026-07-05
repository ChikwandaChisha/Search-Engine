# Makefile for Tiny Search Engine
#
# David Kotz - April 2016, 2017, 2021

L = libengine
.PHONY: all clean

############## default: make all libs and programs ##########
# If libengine contains set.c, we build a fresh libengine.a;
# otherwise we use the pre-built library provided by instructor.
all: 
	(cd $L && if [ -r set.c ]; then make $L.a; else cp $L-given.a $L.a; fi)
	make -C common
	make -C crawler
	make -C indexer
	make -C querier

############### TAGS for emacs users ##########
TAGS:  Makefile */Makefile */*.c */*.h */*.md */*.sh
	etags $^

############## clean  ##########
clean:
	rm -f *~
	rm -f TAGS
	make -C libengine clean
	make -C common clean
	make -C crawler clean
	make -C indexer clean
	make -C querier clean
