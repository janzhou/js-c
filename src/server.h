// Distributed under the terms of the GNU General Public License v2

#ifndef _H_SERVER_
#define _H_SERVER_

#define TCP_SEND	3
#define UDP_SEND	4

#ifndef UDP_DATAREADY
#define UDP_DATAREADY 5
#endif

#ifndef TCP_CONNECT
#define TCP_CONNECT 1
#endif

#ifndef TCP_DATAREADY
#define TCP_DATAREADY 2
#endif

#ifndef TCP_DISCONNECT
#define TCP_DISCONNECT 3
#endif

int server_bind(int port, int num, void (*callback)(char * ip, int port, int sock_id, int event, void * buf, int len, void * callbackdata),void *callbackdata);

int server_send(char * ip, int port, void *buf ,int len, int type);

int server_broadcast(int port, void *buf ,int len);

#endif
