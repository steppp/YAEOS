CC=arm-none-eabi-gcc 
LIBS=-I /usr/include/uarm/ -I include
OBJ=-c -mcpu=arm7tdmi 
BUILD=build
SRC=src
INCLUDE=include
ONAME=p1test

all: builddir main

builddir: 
	if [ ! -d "$(BUILD)" ]; then mkdir "$(BUILD)"; fi;

main: $(BUILD)/p1test.o $(INCLUDE)/asl.h	
	$(CC) -nostartfiles -T /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x -o $(ONAME) /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o $(BUILD)/tree.o $(BUILD)/list.o $(BUILD)/pcbFree.o $(BUILD)/semaphore.o $(BUILD)/p1test.o

$(INCLUDE)/asl.h: $(BUILD)/list.o $(BUILD)/tree.o $(BUILD)/semaphore.o $(BUILD)/pcbFree.o

$(BUILD)/p1test.o: $(SRC)/p1test.c $(INCLUDE)/asl.h 
	$(CC) $(OBJ) $(LIBS) -o $(BUILD)/p1test.o $(SRC)/p1test.c

$(BUILD)/list.o: $(SRC)/list.c $(INCLUDE)/list.h
	$(CC) $(OBJ) $(LIBS) -o $(BUILD)/list.o $(SRC)/list.c

$(BUILD)/semaphore.o: $(SRC)/semaphore.c $(INCLUDE)/semaphore.h
	$(CC) $(OBJ) $(LIBS) -o $(BUILD)/semaphore.o $(SRC)/semaphore.c

$(BUILD)/tree.o: $(SRC)/tree.c $(INCLUDE)/tree.h
	$(CC) $(OBJ) $(LIBS) -o $(BUILD)/tree.o $(SRC)/tree.c

$(BUILD)/pcbFree.o: $(INCLUDE)/pcbFree.h $(SRC)/pcbFree.c
	$(CC) $(OBJ) $(LIBS) -o $(BUILD)/pcbFree.o $(SRC)/pcbFree.c	

clean:
	if [ -d "$(BUILD)" ]; then rm -r "$(BUILD)"; fi;
	if [ -f $(ONAME) ]; then rm $(ONAME); fi;
