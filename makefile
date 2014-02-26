CC = gcc
CFLAGS = -Wall 
DEPS = TCPlib.h UDPlib.h utils.h inetutils.h
OBJ = dd.o utils.o inetutils.o
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
