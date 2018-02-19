#main: tree.o mymain.o
#	arm-none-eabi-ld -T /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x -o mymain /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o mymain.o tree.o

main: p1test.o asl.h	
	arm-none-eabi-gcc -nostartfiles -T /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x -o p1test /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o tree.o list.o pcbFree.o semaphore.o p1test.o

asl.h: list.o tree.o semaphore.o pcbFree.o

p1test.o: asl.h 
	arm-none-eabi-gcc -c -mcpu=arm7tdmi -I /usr/include/uarm/ p1test.c

mymain.o: tree.o 
	arm-none-eabi-gcc -c -mcpu=arm7tdmi mymain.c

list.o: list.c list.h
	arm-none-eabi-gcc -c -mcpu=arm7tdmi -I /usr/include/uarm/ list.c

semaphore.o: list.o semaphore.c semaphore.h
	arm-none-eabi-gcc -c -mcpu=arm7tdmi -I /usr/include/uarm/ semaphore.c

tree.o: tree.c tree.h
	arm-none-eabi-gcc -c -mcpu=arm7tdmi -I /usr/include/uarm/ tree.c

pcbFree.o: pcbFree.h pcbFree.c
	arm-none-eabi-gcc -c -mcpu=arm7tdmi -I /usr/include/uarm/ pcbFree.c	

clean:
	rm *.o p1test