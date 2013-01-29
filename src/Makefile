# Distributed under the terms of the GNU General Public License v2

CFLAGS	= -Wall -W -O2 -g
LIBS		= -pthread -L../mysql/linux/lib/ -lmysqlclient
all: main

main: main.o cmd.o convert.c sem.c server.c tcp.c udp.c ui.c user.c thread.c 
	cc $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $*.o $<

32bit:
	rm -f ../mysql/linux
	ln -s ../mysql/linux-x86-32bit ../mysql/linux
	make

64bit:
	rm -f ../mysql/linux
	ln -s ../mysql/linux-x86-64bit ../mysql/linux
	make

install_mysql:
	cp ../mysql/linux/lib/* /usr/lib/

uninstall_mysql:
	rm /usr/lib/libmysql*

clean : 
	rm -f *.o
	rm -f main
