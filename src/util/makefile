# Makefile for test drivers in this directory
# $Header: /mnt/data2/cvs2svn/cvs-repositories/Cactus/src/util/makefile,v 1.2 2002-01-21 15:03:58 rideout Exp $

EXTRA_GCC_FLAGS := -pipe \
                   -ffast-math \
                   -Wall -W -Wno-unused -Wshadow -Winline -Wpointer-arith \
                   -Wbad-function-cast -Wcast-align -Wcast-qual \
                   -Wstrict-prototypes -Wmissing-prototypes \
                   -Wmissing-declarations

CONFIG_H_DIR := $(firstword $(wildcard ../../configs/*/config-data))
LIB_DIR      := $(dir $(CONFIG_H_DIR))lib

# build outside cactus
.PHONY	: test-table-standalone
test-table-standalone	:
	gcc -I. -I../include -I$(CONFIG_H_DIR) -L$(LIB_DIR) \
            -g $(EXTRA_GCC_FLAGS) $(CFLAGS) \
            -pedantic -ansi \
            -DUTIL_TABLE_TEST -DUTIL_TABLE_DEBUG2 \
            -o test_Table \
            Table.c \
            $(LIB_DIR)/../datestamp.o -lCactus -lCactusBindings -lm
