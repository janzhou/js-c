CFLAGS	= -Wall -W -O2 -g
LIBS		= -pthread

all: main

main: main.o cmd.o convert.c sem.c server.c tcp.c udp.c ui.c user.c
	cc $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $*.o $<

clean : 
	rm -f *.o
	rm -f main
