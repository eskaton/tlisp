CFLAGS  = -Wall -O3 -funroll-loops -I. -I/usr/include
LDFLAGS = -L/usr/lib -lc -lm 
CC      = gcc
LD      = gcc

SRC = hex_dump.c debug.c reader.c tlisp.c istream.c mstring.c htab.c list.c olist.c cons.c env.c stack.c binding.c builtin.c print.c eval.c utils.c symbol.c alloc.c collect.c number.c function.c object.c ctlstack.c
OBJ = ${SRC:.c=.o}

all: options tlisp

options:
	@echo tlisp build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"
	@echo "LD       = ${LD}"

.c.o:
	@echo CC $<
	${CC} -c $< ${CFLAGS} 

${OBJ}: hex_dump.h debug.h reader.h istream.h mstring.h htab.h list.h olist.h cons.h env.h stack.h binding.h builtin.h print.h eval.h utils.h symbol.h alloc.h collect.h number.h function.h

tlisp: ${OBJ}
	@echo LD $@
	@${LD} -o $@ ${OBJ} ${LDFLAGS} 

alloc: alloc.c list.c olist.c htab.c collect.c utils.c mstring.c number.c symbol.c
	gcc -Wall -ggdb -o alloc alloc.c list.c olist.c htab.c collect.c utils.c mstring.c number.c symbol.c cons.c alloc_test.c

clean:
	@rm -f ${OBJ} tlisp

tar:
	find . \( -name "*.c" -o -name "*.h" -o -name "*.tlp" -o -name "Makefile" \) -print | xargs tar -cf tlisp.tar
