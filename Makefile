# compiler
CC=arm-none-eabi-gcc
# where to look for .h files
HDIR=-I /usr/include/uarm/ -I include
# machine specific compilation options
CFLAGS=-c -mcpu=arm7tdmi
# object files folder
BUILD=build
# source code folder
SRC=src
# headers folder
INCLUDE=include
# name of the executable
ONAME=p1test
# modules of the project
MODULES=list tree semaphore pcbFree p1test scheduler interrupts syscall main
# adds a prefix with the name of the build dir to the names of the modules
OBJ=$(patsubst %,$(BUILD)/%.o,$(MODULES))
# global depencencies
_DEPS=const.h pcb.h main.h types.h
DEPS=$(patsubst %,$(INCLUDE)/%,$(_DEPS))

all: builddir executable

builddir:
	if [ ! -d "$(BUILD)" ]; then mkdir "$(BUILD)"; fi;

executable: $(OBJ)
	$(CC) -nostartfiles -T /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x -o $(ONAME) /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o $^

$(BUILD)/p1test.o: $(SRC)/p1test.c $(INCLUDE)/asl.h $(DEPS)
	$(CC) $(CFLAGS) $(HDIR) -o $@ $<

$(BUILD)/%.o: $(SRC)/%.c $(INCLUDE)/%.h $(DEPS)
	$(CC) $(CFLAGS) $(HDIR) -o $@ $<

.PHONY: clean all builddir executable

clean:
	if [ -d "$(BUILD)" ]; then rm -r "$(BUILD)"; fi;
	if [ -f $(ONAME) ]; then rm $(ONAME); fi;
