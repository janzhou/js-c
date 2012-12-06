#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "server.h"
#include "tcp.h"
#include "user.h"
#include "ui.h"

#include "convert.h"

#define JSON	0
#define PRINT	1

typedef struct cmd_line{
	char cmd[32];
	void (* function)(int argc, char *argv[], char * output, int format);
}cmd_line_t;

static void v(int argc, char *argv[], char * output, int format)
{
	int mainversion = 0;
	int subversion = 6;
	int develop = 0;

	switch(format)
	{
		case JSON:
			sprintf(output, "\"v\":%d.%d" , mainversion, subversion);
			break;
		case PRINT:
			sprintf(output, "version: %d.%d.%d\n" , mainversion, subversion, develop);
			break;
	}
}

//u ip port filename
static void udpsend(int argc, char *argv[], char * output, int format)
{
	FILE * file;
	char buf[1024];
	int size;

	file =  fopen(argv[3],  "r");
	if(file == NULL){
		printf("file error\n");
	}

	size = fread(buf, 1, 1024, file);

	server_send(argv[1], atoi(argv[2]), buf, size, UDP_SEND);

	fclose(file);

	switch(format)
	{
		case JSON:
			sprintf(output,"{\"type\":\"cmd\",\"success\":1,\"msg\":\"success\"}");
			break;
		case PRINT:
			break;
	}
}

//t ip port filename
static void tcpsend(int argc, char *argv[], char * output, int format)
{
	FILE * file;
	char buf[1024];
	int size;
	
	file =  fopen(argv[3],  "r");
	if(file == NULL){
		printf("file error\n");
	}

	size = fread(buf, 1, 1024, file);

	if(server_send(argv[1], atoi(argv[2]), buf, size, TCP_SEND)){
		printf("tcp send error\n");
	}

	fclose(file);

	switch(format)
	{
		case JSON:
			sprintf(output,"{\"type\":\"cmd\",\"success\":1,\"msg\":\"success\"}");
			break;
		case PRINT:
			break;
	}
}

//b port file
static void broadcast(int argc, char *argv[], char * output, int format)
{
	FILE * file;
	char buf[1024];
	int size;

	file =  fopen(argv[2],  "r");
	if(file == NULL){
		printf("file error\n");
	}
	size = fread(buf, 1, 1024, file);
	server_broadcast(atoi(argv[1]), buf, size);
	fclose(file);

	switch(format)
	{
		case JSON:
			sprintf(output,"{\"type\":\"cmd\",\"success\":1,\"msg\":\"success\"}");
			break;
		case PRINT:
			break;
	}
}

//wu ip port msg
static void webudpsend(int argc, char *argv[], char * output, int format)
{
	if(argc < 4){
		switch(format)
		{
			case JSON:
				sprintf(output,"{\"type\":\"cmd\",\"success\":0,\"msg\":\"no enough args\"}");
				break;
			case PRINT:
				sprintf(output, "no enough args\n");
				break;
		}
		return;
	}
	server_send(argv[1], atoi(argv[2]), argv[3], strlen(argv[3]), UDP_SEND);

	switch(format)
	{
		case JSON:
			sprintf(output,"{\"type\":\"cmd\",\"success\":1,\"msg\":\"success\"}");
			break;
		case PRINT:
			sprintf(output, "success\n");
			break;
	}
}

//wt ip port filename
static void webtcpsend(int argc, char *argv[], char * output, int format)
{
	if(argc < 4){
		switch(format)
		{
			case JSON:
				sprintf(output,"{\"type\":\"cmd\",\"success\":0,\"msg\":\"no enough args\"}");
				break;
			case PRINT:
				sprintf(output, "no enough args\n");
				break;
		}
		return;
	}
	server_send(argv[1], atoi(argv[2]), argv[3], strlen(argv[3]), TCP_SEND);

	switch(format)
	{
		case JSON:
			sprintf(output,"{\"type\":\"cmd\",\"success\":1,\"msg\":\"success\"}");
			break;
		case PRINT:
			sprintf(output, "success\n");
			break;
	}
}

//wb port file
static void webbroadcast(int argc, char *argv[], char * output, int format)
{
	if(argc < 3){
		switch(format)
		{
			case JSON:
				sprintf(output,"{\"type\":\"cmd\",\"success\":0,\"msg\":\"no enough args\"}");
				break;
			case PRINT:
				sprintf(output, "no enough args\n");
				break;
		}
		return;
	}
	
	server_broadcast(atoi(argv[1]), argv[2], strlen(argv[2]));

	switch(format)
	{
		case JSON:
			sprintf(output,"{\"type\":\"cmd\",\"success\":1,\"msg\":\"success\"}");
			break;
		case PRINT:
			sprintf(output, "success\n");
			break;
	}
}

//scriptsend msg
static void scriptsend(int argc, char *argv[], char * output, int format)
{
	char msg[1024];
	int i;

	if(argc < 2){
		switch(format)
		{
			case JSON:
				sprintf(output,"{\"type\":\"cmd\",\"success\":0,\"msg\":\"no enough args\"}");
				break;
			case PRINT:
				sprintf(output, "no enough args\n");
				break;
		}
		return;
	}

	for(i=0;i<8;i++){
		if(machines[i].status!=0) server_send(machines[i].ip,machines[i].port,argv[1],strlen(argv[1]),TCP_SEND);
	}

	switch(format)
	{
		case JSON:
			sprintf(output,"{\"type\":\"cmd\",\"success\":1,\"msg\":\"success\"}");
			break;
		case PRINT:
			sprintf(output, "success\n");
			break;
	}
	
}

//s msg
static void cmdsend(int argc, char *argv[], char * output, int format)
{
	char msg[1024];

	if(argc == 1) sprintf(msg, "\"void msg\"");
	else sprintf(msg, "\"%s\"" , argv[1]);
	
	sendwebmsg(msg);


	switch(format)
	{
		case JSON:
			if(argc == 1) sprintf(output, "\"void msg\"");
			else sprintf(output, "\"%s\"" , argv[1]);
			break;
		case PRINT:
			if(argc == 1) sprintf(output, "void msg");
			else sprintf(output, "%s\n" , argv[1]);
			break;
	}
	
}

//c msg
static void chat(int argc, char *argv[], char * output, int format)
{
	char msg[1024];
	char convertbuf[1024];

	if(argc == 1) sprintf(msg, "\"void msg\"");
	else {
		stringtohtml(argv[1], convertbuf, sizeof(convertbuf));
		sprintf(msg, "{\"type\":\"chat\",\"msg\":\"%s\"}" , convertbuf);
	}
	
	sendwebmsg(msg);

	switch(format)
	{
		case JSON:
			if(argc == 1) sprintf(output, "\"void msg\"");
			else sprintf(output, "\"%s\"" , convertbuf);
			break;
		case PRINT:
			if(argc == 1) sprintf(output, "void msg");
			else sprintf(output, "%s\n" , argv[1]);
			break;
	}
	
}

//g userid
static void cmdgetmsg(int argc, char *argv[], char * output, int format)
{
	int id;
	char * msg;

	if(argc == 1) id = 0;
	else id = atoi(argv[1]);

	msg = getwebmsg(id);
	sprintf(output, "%s" , msg);

	free(msg);
}

static struct cmd_line cmd_list[] = {
	{"g", cmdgetmsg},
	{"c", chat},
	{"scriptsend", scriptsend},
	{"s", cmdsend},
	{"t", tcpsend},
	{"u", udpsend},
	{"wb", webbroadcast},
	{"wt", webtcpsend},
	{"wu", webudpsend},
	{"b", broadcast},
	{"v", v}
};

//=================================================================================================
//=================================================================================================
//============                 命令入口，php和stdin入口封装完备，不用再看              ============
//=================================================================================================
//=================================================================================================

//	\n	\r
//	n	r
//	[	]	{	}	;	:	'	"	,	<		>	/	?
//	o	p	O	P	k	K	l	L	b	B		N	m	M
//	`~@#%^&=
//	qQ235678
static int devide(char * cmd, char *argv[], int len)
{
	int i = 0;

	char * pre;

	do{
		while(*cmd == 32 && *cmd != 0) cmd++;
		argv[i] = pre = cmd;
		while(*cmd != 32 && *cmd != 0) {
			if('!' == *cmd){
				cmd++;
				switch(*cmd){
					case 'n':
						*pre = '\n';
						break;
					case 'r':
						*pre = '\r';
						break;
					case 'o':
						*pre = '[';
						break;
					case 'p':
						*pre = ']';
						break;
					case 'O':
						*pre = '{';
						break;
					case 'P':
						*pre = '}';
						break;
					case 'k':
						*pre = ';';
						break;
					case 'K':
						*pre = ':';
						break;
					case 'l':
						*pre = '\'';
						break;
					case 'L':
						*pre = '"';
						break;
					case 'b':
						*pre = ',';
						break;
					case 'B':
						*pre = '<';
						break;
					case 'N':
						*pre = '>';
						break;
					case 'm':
						*pre = '/';
						break;
					case 'M':
						*pre = '?';
						break;
					case 'q':
						*pre = '`';
						break;
					case 'Q':
						*pre = '~';
						break;
					case '2':
						*pre = '@';
						break;
					case '3':
						*pre = '#';
						break;
					case '5':
						*pre = '%';
						break;
					case '6':
						*pre = '^';
						break;
					case '7':
						*pre = '&';
						break;
					case '8':
						*pre = '=';
						break;
					case '*':
						*pre = '+';
						break;
					default:
						*pre = *cmd;
				}
			}else *pre = *cmd;
			cmd++;
			pre++;
		}
		if(cmd > pre) *pre = 0;

		i++;
		if(*cmd) *(cmd++) = 0;
		else return i;
		if(i == len) return i;
	}while(cmd && *cmd);
	return i;
}

static void do_cmd(char *cmd, char *output, int format)
{
	int argc, i;
	char *argv[20];

	if(!(argc = devide(cmd, argv, 20))) return;

	for(i = sizeof(cmd_list)/sizeof(cmd_line_t) - 1; i >= 0; i--){
		if(!strcmp(cmd_list[i].cmd, argv[0])){
			(* (cmd_list[i].function))(argc, argv, output, format);
			break;
		}
	}
}

void do_cmd_tcp(char * ip, int port, int sock_id, int event, void * cmd, int len, void * callbackdata)
{
	char output[2048];
	
	switch(event){
		case TCP_CONNECT:
			break;
		case TCP_DATAREADY:
			*((char *)cmd + len) = 0;
			do_cmd(cmd, output, JSON);
			tcp_send(sock_id, output, strlen(output));
			break;
		case TCP_DISCONNECT:
			break;
	}
}

void do_cmd_stdin(void)
{
	char cmd[100];
	char output[1024];
	int i;
	while(1){
		i=0;
		while((cmd[i]=getchar())!='\n')	i++;
		cmd[i]='\0';

		*output = 0;
		do_cmd(cmd, output, PRINT);
		printf("%s", output);
	}
}
