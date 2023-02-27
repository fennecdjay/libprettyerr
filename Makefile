CFLAGS   += -Isrc -fPIC
WARNINGS += -Wall -Wextra
PREFIX   ?= /usr/local

#SRC      := $(shell find src -type f -name '*.c')
SRC      := src/internal/error.c
OBJ      := ${SRC:.c=.o}
PRG      := libprettyerr

ifeq ($(shell uname), Darwin)
AR = /usr/bin/libtool
AR_OPT = -static $^ -o $@
else
AR = ar
AR_OPT = rcs $@ $^
endif

CFLAGS += ${WARNINGS}

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

install: ${PRG}.a ${PRG}.so
	cp ${PRG}.a ${PRG}.so ${PREFIX}/lib
	mkdir -p /usr/local/include/${PRG}
	cp src/prettyerr.h ${PREFIX}/include/${PRG}

uninstall:
	rm ${PREFIX}/lib/${PRG}.a ${PREFIX/lib/${PRG}.so
	rm ${PREFIX}/include/${PRG}/termcolor.h
	rmdir /usr/local/include/${PRG}
