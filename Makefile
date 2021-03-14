# should be in config.mk
TERMCOLOR_CFLAGS  ?= -I../libtermcolor/src

# force usage of static lib
TERMCOLOR_LDFLAGS ?= ../libtermcolor/libtermcolor.a


CFLAGS   += -g
CFLAGS   += -Isrc -fPIC
WARNINGS += -Wall -Wextra

SRC      := $(shell find src -type f -name '*.c')
OBJ      := ${SRC:.c=.o}
PRG      := libprettyerr

ifeq ($(shell uname), Darwin)
AR = /usr/bin/libtool
AR_OPT = -static $^ -o $@
else
AR = ar
AR_OPT = rcs $@ $^
endif

CFLAGS += ${WARNINGS} ${TERMCOLOR_CFLAGS}
LDFLAGS += ${TERMCOLOR_LDFLAGS}

all: static dynamic demo

.PHONY:  static dynamic
static:  ${PRG}.a
dynamic: ${PRG}.so

${PRG}.a: ${OBJ}
	$(info building $@ ${OBJ})
	${AR} ${AR_OPT}

${PRG}.so: ${OBJ}
	$(info building $@)
	${CC} -shared $< -o $@ ${LDFLAGS}

demo: ${PRG}.a
	${CC} main.c ${CFLAGS} $< -o $@ ${LDFLAGS}

.c.o:
	${CC} ${CFLAGS} $< -c -o ${<:.c=.o}

clean:
	rm -rf ${PRG}.a ${PRG}.so ${OBJ}
