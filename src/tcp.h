#ifndef _H_TCP_
#define _H_TCP_

#ifndef SOCK_BUF_LEN
#define SOCK_BUF_LEN 2048
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

int tcp_connect(char * addr, int port, void (* callback)(int sock_id,int event,void * buf, int len, void * callbackdata), void * callbackdata);
int tcp_receive_socket(int sock_id,void *buf, int len);
int tcp_receive_wait(int sock_id,void *buf, int len);
int tcp_send(int sock_id,void *buf ,int len);
int tcp_disconnect(int sock_id);

int tcp_server_bind(int port, int num, void (*callback)(char * ip, int port, int sock_id, int event, void * buf, int len, void * callbackdata),void *callbackdata);

#endif