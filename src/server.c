#include <stdio.h>

#include"server.h"
#include"tcp.h"
#include"udp.h"

int server_bind(int port, int num, void (*callback)(char * ip, int port, int sock_id, int event, void * buf, int len, void * callbackdata),void *callbackdata)
{
	return tcp_server_bind(port, num, callback, callbackdata) || udp_server_bind(port, num, callback, callbackdata);
}

int server_send(char * ip, int port, void *buf ,int len, int type)
{
	int id;

	switch(type){
		case TCP_SEND:
			id = tcp_connect(ip, port, NULL, NULL);
			tcp_send(id, buf, len);
			return tcp_disconnect(id);
		case UDP_SEND:
			return udp_send(ip, port, buf , len);
	}

	return -1;
}

int server_broadcast(int port, void *buf ,int len){
	return udp_broadcast(port, buf , len);
}
