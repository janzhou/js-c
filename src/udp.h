// Distributed under the terms of the GNU General Public License v2

#ifndef _H_UDP_
#define _H_UDP_

#ifndef SOCK_BUF_LEN
#define SOCK_BUF_LEN 2048
#endif

#ifndef UDP_DATAREADY
#define UDP_DATAREADY 5
#endif

int udp_receive(int sock_id,void *buf, int len);

int udp_send(char * ip, int port, void *buf ,int len);

int udp_broadcast(int port, void *buf ,int len);

int udp_server_bind(int port, int num, void (*callback)(char * ip, int port, int sock_id, int event, void * buf, int len, void * callbackdata),void *callbackdata);

#endif
