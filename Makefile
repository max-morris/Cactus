
# Temporary makefile for the CCTK

TOP              := $(shell pwd)

SHELL             = /bin/sh
MKDIR             = mkdir -p
CC                = egcs
CXX               = $(CC)
F90               = f90
F77               = f77
CPP               = cc -E -M
LD                = $(CXX)
AR                = /bin/ar rucs
RANLIB            = touch
AWK               = awk

INC_DIRS         += src/include

CFLAGS           += -g $(INC_DIRS:%=-I%)
CXXFLAGS         += -g $(INC_DIRS:%=-I%)


CSRCS := $(shell find src -name '*.c')
CCSRCS := $(shell find src -name '*.cc')
OBJS = $(CSRCS:%.c=%.o) $(CCSRCS:%.cc=%.o)
HEADERS := $(shell find src/include -name '*.h')


cctk: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $+ $(LOADLIBS)

$(OBJS): $(HEADERS)

clean:
	find src -name '*.o' -exec rm {} \;

tags:
	etags `find src -name '*.[ch]' -o -name '*.cc'`
