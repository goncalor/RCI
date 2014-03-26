CC = gcc
CFLAGS = -Wall -g 
DEPS = TCPlib.h UDPlib.h utils.h inetutils.h database.h item.h list.h interface.h incoming.h define.h okinfo.h
OBJ = dd.o utils.o inetutils.o TCPlib.o UDPlib.o list.o database.o interface.o incoming.o okinfo.o
EDITOR = gedit

dd: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@


c:
	clear	
	rm *.o

o:
	$(EDITOR) dd.c *.c *.h makefile &








# $@ reffers to named before :
# $^ reffers to all named after :
# $< reffers to first after :
