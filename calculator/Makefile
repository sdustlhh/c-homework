# File makefile

CFLAGS = -g -O
CC = gcc

tiny: main.o base.o cal.o
	$(CC) $(CFLAGS) main.o base.o cal.o -o $@  

main.o: main.c base.h cal.h
	$(CC) $(CFLAGS) -c main.c -o $@
	
base.o: base.c base.h
	$(CC) $(CFLAGS) -c base.c -o $@
	
cal.o: cal.c cal.h base.h
	$(CC) $(CFLAGS) -c cal.c  -o $@

clean:
	rm *.o
