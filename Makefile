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

CFLAGS += ${WARNINGS}

all: dynamic demo

.PHONY:  static dynamic
.PHONY:  static
static:  ${PRG}.a
dynamic: ${PRG}.so

${PRG}.a: ${OBJ}
	${AR} ${AR_OPT}

${PRG}.so: ${OBJ}
	echo ${SRC}
	${CC} -shared $< -o $@

demo: ${PRG}.a
	${CC} ${CFLAGS} main.c $< -o main

.c.o:
	${CC} ${CFLAGS} $< -c -o ${<:.c=.o}

clean:
	rm -rf ${PRG}.a ${PRG}.so ${OBJ}
