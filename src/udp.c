// Distributed under the terms of the GNU General Public License v2

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#ifdef __linux
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>/*close*/
#endif

#include <stdio.h>
#include <malloc.h>
#include <string.h> 

#include "udp.h"
#include "thread.h"

#ifdef __linux
int udp_send(char * ip, int port, void *buf ,int len) {
	int sock, ret;
	struct sockaddr_in toAddr;
	sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(sock < 0) {
		printf("udp socket error.\n");
		return 0;
	}
	memset(&toAddr,0,sizeof(toAddr));
	toAddr.sin_family=AF_INET;
	toAddr.sin_addr.s_addr=inet_addr(ip);
	toAddr.sin_port = htons(port);
	if(( ret = sendto(sock, buf, len, 0, (struct sockaddr*)&toAddr, sizeof(toAddr))) != len) {
		printf("udp send error.\n");
		return 0;
	}

	close(sock);
	return ret;
}

int udp_receive(int sock_id,void *buf, int len) {
	struct sockaddr_in fromAddr;
	unsigned int fromLen;
	fromLen = sizeof(fromAddr);
	if(recvfrom(sock_id, buf, len, 0, (struct sockaddr*)&fromAddr,&fromLen) < 0) {
		printf("udp receive error\n");
		return -1;
	}
	return 0;
}

int udp_broadcast(int port, void *buf ,int len){
	int inet_sock, so_broadcast = 1;
	struct sockaddr_in adr_bc;
	if ((inet_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Broadcast UDP created socket error");
		return -1;
	}
	if (setsockopt(inet_sock, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof so_broadcast) < 0) {
		perror("Broadcast UDP set socket error");
		close(inet_sock);
		return -1;
	}

	adr_bc.sin_family = AF_INET;
	adr_bc.sin_port = htons(port);
	adr_bc.sin_addr.s_addr = inet_addr("255.255.255.255");
	if (sendto(inet_sock, buf, len, 0, (struct sockaddr *)&adr_bc, sizeof adr_bc)< 0) {
		perror("Broadcast send error");
		close(inet_sock);
		return -1;
	}
	close(inet_sock);
	return 0;
	//	WORD wVersionRequested;  
	//	WSADATA wsaData;  
	//	int err;  
	//	SOCKET connect_socket;
	//	SOCKADDR_IN sin; 
	//	char bOpt;
	//	int nAddrLen;
	//	int nSendSize;
	//
	//	// 启动socket api   
	//	wVersionRequested = MAKEWORD( 2, 2 );  
	//	err = WSAStartup( wVersionRequested, &wsaData );  
	//	if ( err != 0 )  
	//	{  
	//		return -1;  
	//	}  
	//
	//	if ( LOBYTE( wsaData.wVersion ) != 2 ||  
	//			HIBYTE( wsaData.wVersion ) != 2 )  
	//	{  
	//		WSACleanup( );  
	//		return -1;   
	//	}  
	//
	//	// 创建socket
	//	connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  
	//	if(INVALID_SOCKET == connect_socket)  
	//	{  
	//		err = WSAGetLastError();  
	//		printf("\"socket\" error! error code is %d\n", err);  
	//		return -1;  
	//	}
	//
	//	sin.sin_family = AF_INET;  
	//	sin.sin_port = htons(port);  
	//	sin.sin_addr.s_addr = INADDR_BROADCAST;  
	//
	//
	//	//设置该套接字为广播类型   
	//	bOpt = -1;
	//	setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));  
	//
	//	nAddrLen = sizeof(SOCKADDR);  
	//
	//	// 发送数据   
	//	nSendSize = sendto(connect_socket, (char*)buf, len, 0, (SOCKADDR*)&sin, nAddrLen);  
	//	if(SOCKET_ERROR == nSendSize)  
	//	{  
	//		err = WSAGetLastError();  
	//		printf("\"sendto\" error!, error code is %d\n", err);  
	//		return -1;  
	//	}  


	return 0;  
}

struct svr_callback{
	void (*callback)(char * ip, int port, int sock_id, int event, void * buf, int len, void * callbackdata);
	void *callbackdata;
};


struct server_arg{
	int clientfd;
	struct svr_callback cb;
	int socket;

	thread_t thread;
};

static thread_func_t server_th(thread_arg_t arg)
{
	struct server_arg * s_arg;
	struct sockaddr_in from;
	int fromlen;

	fromlen = sizeof(from);

	s_arg =(struct server_arg *) arg;
	if(s_arg->cb.callback){
		char receiveBuf[SOCK_BUF_LEN];
		int len;

		while( 1 ){
			if((len = recvfrom(s_arg->socket, receiveBuf, SOCK_BUF_LEN, 0, (struct sockaddr*)&from, (socklen_t*)&fromlen)) > 0){
				receiveBuf[len]=0;
				(*s_arg->cb.callback)(inet_ntoa(from.sin_addr), from.sin_port, s_arg->clientfd, UDP_DATAREADY, receiveBuf, len, s_arg->cb.callbackdata);
			} else {
				printf("udp receive error %d %d\n", s_arg->socket, len);
			}
		}

	}

	close(s_arg->clientfd);

	return 0;
}

int udp_server_bind(int port, int num, void (*callback)(char * ip, int port, int sock_id, int event, void * buf, int len, void * callbackdata),void *callbackdata) {
	struct server_arg * arg;
	struct sockaddr_in local;

	arg = (struct server_arg *)malloc(sizeof(struct server_arg));

	arg->cb.callback = callback;
	arg->cb.callbackdata= callbackdata;

	local.sin_family=AF_INET;
	local.sin_port=htons(port); ///监听端口
	local.sin_addr.s_addr=INADDR_ANY; ///本机

	arg->socket=socket(AF_INET,SOCK_DGRAM,0);
	if(-1 == bind(arg->socket, (struct sockaddr*)&local, sizeof(local))) {
		printf("udp bind error\n");
	}

	if( create_thread(&arg->thread, server_th, arg) ) {
		close(arg->socket);
		free(arg);
		return -1;
		printf("udp bind error\n");
	}

	return 0;
}
#endif

#ifdef _WIN32
static int udp_startup(){
	static int running = 0;
	WSADATA wsaData;

	if(running) return 0;

	if (WSAStartup(MAKEWORD(2,1),&wsaData)) //调用Windows Sockets DLL
	{ 
		printf("Winsock无法初始化!\n");
		WSACleanup();
		return -1;
	}
	running = 1;
	return 0;
}

int udp_send(char * ip, int port, void *buf ,int len) {
	SOCKET           SendingSocket;  
	SOCKADDR_IN      ReceiverAddr;

	udp_startup();

	//创建一个新的套接字来接收数据报   
	if((SendingSocket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) == INVALID_SOCKET)  {  
		printf("ERROR:SOCKET failed with %d/n",WSAGetLastError());  
		WSACleanup();  
		return 1;  
	}  
	//建立一个SOCKADDR_IN结构，来识别发送数据报的目的地  
	ReceiverAddr.sin_family = AF_INET;  
	ReceiverAddr.sin_port = htons(port);  
	ReceiverAddr.sin_addr.s_addr = inet_addr(ip);  
	//把一个数据报发送到接收者   
	return  sendto(SendingSocket,(char *)buf,len,0,(SOCKADDR *)&ReceiverAddr,sizeof(ReceiverAddr)) ;
	closesocket(SendingSocket);

}

int udp_receive(int sock_id,void *buf, int len)
{
	SOCKET sockSrv = socket(AF_INET,SOCK_DGRAM,0);
	SOCKADDR_IN addrClient;
	int iSize = sizeof(SOCKADDR);

	if (recvfrom(sock_id,(char *)buf,len,0,(SOCKADDR *)&addrClient,&iSize)!=SOCKET_ERROR){
		return 0;
	}

	return -1;
}

int udp_broadcast(int port, void *buf ,int len){
	WORD wVersionRequested;  
	WSADATA wsaData;  
	int err;  
	SOCKET connect_socket;
	SOCKADDR_IN sin; 
	char bOpt;
	int nAddrLen;
	int nSendSize;

	// 启动socket api   
	wVersionRequested = MAKEWORD( 2, 2 );  
	err = WSAStartup( wVersionRequested, &wsaData );  
	if ( err != 0 )  {  
		return -1;  
	}  

	if ( LOBYTE( wsaData.wVersion ) != 2 ||  
			HIBYTE( wsaData.wVersion ) != 2 )  
	{  
		WSACleanup( );  
		return -1;   
	}  

	// 创建socket
	connect_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  
	if(INVALID_SOCKET == connect_socket)  
	{  
		err = WSAGetLastError();  
		printf("\"socket\" error! error code is %d\n", err);  
		return -1;  
	}

	sin.sin_family = AF_INET;  
	sin.sin_port = htons(port);  
	sin.sin_addr.s_addr = INADDR_BROADCAST;  


	//设置该套接字为广播类型   
	bOpt = -1;
	setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));  

	nAddrLen = sizeof(SOCKADDR);  

	// 发送数据   
	nSendSize = sendto(connect_socket, (char*)buf, len, 0, (SOCKADDR*)&sin, nAddrLen);  
	if(SOCKET_ERROR == nSendSize)  
	{  
		err = WSAGetLastError();  
		printf("\"sendto\" error!, error code is %d\n", err);  
		return -1;  
	}  


	return 0;  
}

struct svr_callback{
	void (*callback)(char * ip, int port, int sock_id, int event, void * buf, int len, void * callbackdata);
	void *callbackdata;
};


struct server_arg{
	int clientfd;
	struct svr_callback cb;
	SOCKET socket;

	thread_t thread;
};

static thread_func_t server_th(thread_arg_t arg)
{
	struct server_arg * s_arg;
	struct sockaddr_in from;
	int fromlen;

	fromlen = sizeof(from);

	s_arg =(struct server_arg *) arg;

	if(s_arg->cb.callback){
		char receiveBuf[SOCK_BUF_LEN];
		int len;

		while( 1 ){
			if((len = recvfrom(s_arg->socket, receiveBuf, SOCK_BUF_LEN, 0, (struct sockaddr*)&from, &fromlen)) != SOCKET_ERROR){
				receiveBuf[len]=0;
				(*s_arg->cb.callback)(inet_ntoa(from.sin_addr), from.sin_port, s_arg->clientfd, UDP_DATAREADY, receiveBuf, len, s_arg->cb.callbackdata);
			}
		}

	}

	//	closesocket(s_arg->clientfd);

	return 0;
}

int udp_server_bind(int port, int num, void (*callback)(char * ip, int port, int sock_id, int event, void * buf, int len, void * callbackdata),void *callbackdata)
{
	struct server_arg * arg;
	struct sockaddr_in local;

	arg = (struct server_arg *)malloc(sizeof(struct server_arg));

	udp_startup();

	arg->cb.callback = callback;
	arg->cb.callbackdata= callbackdata;

	local.sin_family=AF_INET;
	local.sin_port=htons(port); ///监听端口
	local.sin_addr.s_addr=INADDR_ANY; ///本机

	arg->socket=socket(AF_INET,SOCK_DGRAM,0);
	bind(arg->socket, (struct sockaddr*)&local, sizeof(local));

	if( create_thread(&arg->thread, server_th, arg) ) {
		closesocket(arg->socket);
		WSACleanup();
		free(arg);
		return -1;
		printf("udp bind error\n");
	}

	return 0;
}
#endif

