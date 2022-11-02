.PHONY: build

LIB_DIR=lib/
LIB_CGLM=$(LIB_DIR)cglm-0.7.9/include
LIB_INCLUDE=-I $(LIB_CGLM)

CFLAGS= -o app -Wall $(LIB_INCLUDE) -lSDL2 -lm
CC= gcc
CFILES= main.c gal.c

build:
	echo "> Building app (debug)"
	$(CC) $(CFILES) $(CFLAGS) -gdwarf-2 -O0
	echo "> App built (debug)"

.PHONY: optbuild
optbuild:
	echo "> Building app (speed optimized)"
	$(CC) $(CFILES) $(CFLAGS) -O3
	echo "> App built (speed optimized)"

.PHONY: run
run: build
	./app

.PHONY: optrun
optrun: optbuild
	./app

.PHONY: clean
clean:
	rm -f app
