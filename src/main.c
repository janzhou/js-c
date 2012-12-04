#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#pragma comment(lib, "Ws2_32.lib")


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include"global.h"
#include"server.h"
#include"tcp.h"
#include"cmd.h"
#include"ui.h"
#include"user.h"

HANDLE heartbeat_tid;

static DWORD WINAPI heartbeat_th(LPVOID arg){
	while(1){
		Sleep(1000);
		uiheartbeat();
		userheartbeat();
	}
}

int main(int argc, char *argv[])
{
	char * msg;
	port = 6000;
	if(argc > 1) port = atoi(argv[1]);

	uiinit();
	userinit();

	server_bind(port, 20, do_ui, NULL);
	tcp_server_bind(2012, 20, do_cmd_tcp, NULL);

	heartbeat_tid=CreateThread(NULL, 0, heartbeat_th, NULL, 0, NULL);

	do_cmd_stdin();

	return 0;
}