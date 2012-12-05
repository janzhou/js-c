//#ifdef _WIN32
//#define _CRT_SECURE_NO_WARNINGS
//#include <winsock2.h>
//#include <windows.h>
//#include <process.h>
//#pragma comment(lib, "Ws2_32.lib")
//#endif
//
//#ifdef __linux
//#include <sys/socket.h>
//#include <arpa/inet.h>
//#include <pthread.h>
//#include <unistd.h>/*close*/
//#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include"global.h"
#include"server.h"
#include"tcp.h"
#include"cmd.h"
#include"ui.h"
#include"user.h"
#include"thread.h"

thread_t heartbeat_tid;

thread_func_t heartbeat_th(thread_arg_t arg){
	arg = arg;
	while(1){
		thread_sleep(1000);
		uiheartbeat();
		userheartbeat();
	}
}

int main(int argc, char *argv[])
{
	port = 6000;
	if(argc > 1) port = atoi(argv[1]);

	uiinit();
	userinit();

	server_bind(port, 20, do_ui, NULL);
	tcp_server_bind(2012, 20, do_cmd_tcp, NULL);

	create_thread(&heartbeat_tid, heartbeat_th, NULL);

	do_cmd_stdin();

	return 0;
}
