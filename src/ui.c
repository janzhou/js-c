#include <stdio.h>
#include <string.h>

#include"ui.h"
#include"server.h"

void printmachine(char * buf, struct machine_t * machine){
	int i = 0;
	sprintf(buf, "{\"type\":\"machine\",\"id\":\"%d\",\"ip\":\"%s\",\"port\":\"%d\",\"status\":\"%d\",\"disks\":[", machine->id, machine->ip, machine->port, machine->status);
	sprintf(buf+strlen(buf), "{\"type\":\"disk\",\"status\":\"%d\"}", machine->disks[i].status);
	for(i = 1; i < 16; i++){
		sprintf(buf+strlen(buf), ",{\"type\":\"disk\",\"status\":\"%d\"}", machine->disks[i].status);
	}
	sprintf(buf+strlen(buf), "]}");
}

void printtextresult(char * buf, char * text){
	int i = 0;
	sprintf(buf, "{\"type\":\"textresult\",\"text\":\"");
	i = strlen(buf);
	while(*text){
		switch(*text){
			case '\r':
			case '\n':
				*(buf+i++) = '<';
				*(buf+i++) = '/';
				*(buf+i++) = 'b';
				*(buf+i++) = 'r';
				*(buf+i++) = '>';
				break;
			case '\"':
				*(buf+i++) = '&';
				*(buf+i++) = 'q';
				*(buf+i++) = 'u';
				*(buf+i++) = 'o';
				*(buf+i++) = 't';
				*(buf+i++) = ';';
				break;
			default:
				*(buf+i++) = *text;
				break;
		}
		text++;
	}
	sprintf(buf+i, "\"}");
}

void uiinit(void)
{
	int i;
	memset ( machines, 0, sizeof(machines) );

	for(i = 0; i < 8; i++){
		machines[i].id = i;
		sprintf(machines[i].ip, "255.255.255.255");
		machines[i].port = 9527;
	}

	return;
}

void do_ui(char * ip, int port, int sock_id, int event, void * buf, int len, void * callbackdata)
{
//	FILE * file1;
	int flag = -1, i=0,flag1,n,m;
	char b[2];
	char *file = (char* )buf;
	char msg[1024];

	switch(event){
		case TCP_CONNECT:
			break;
		case TCP_DATAREADY:
		case UDP_DATAREADY:

			printtextresult(msg, buf);
			sendmsg(msg);

			/*
			if((file=strstr(buf,"worker"))!=NULL){
				b[0]=file[7];
				flag = atoi(b);
			}
		 
			if((file=strstr(buf,"result=success"))!=NULL){
				machines[flag].status=1;
			}
			
			if((file=strstr(buf,"Disk Information"))!=NULL){
				for(; i < 16; i++){
					machines[flag].disks[i].status=1;
				}
			}
			
			if((file=strstr(buf,"port"))!=NULL){
				b[0]=file[6];
				flag1 = atoi(b);
				machines[flag].disks[flag1].port=flag1;
				
				if((file=strstr(buf,"dev/sdh"))!=NULL){
					n=9,m=0;
					while(file[n]!='\n'){
						machines[flag].disks[flag1].disk_store[m++]=file[n++];
					}
				}

				if((file=strstr(buf,"Units"))!=NULL){
					n=29,m=0;
					while(file[n]!='\n'){
						machines[flag].disks[flag1].Units[m++]=file[n++];
					}
				}

				if((file=strstr(buf,"Sector size"))!=NULL){
					n=44,m=0;
					while(file[n]!='\n'){
						machines[flag].disks[flag1].Sector_size[m++]=file[n++];
					}
				}

				if((file=strstr(buf,"I/O size"))!=NULL){
					n=40,m=0;
					while(file[n]!='\n'){
						machines[flag].disks[flag1].iosize[m++]=file[n++];}
					}

					if((file=strstr(buf,"Disk identifier"))!=NULL){
						n=17,m=0;
						while(file[n]!='\n'){
							machines[flag].disks[flag1].disk_identifer[m++]=file[n++];
						}
					}

					if((file=strstr(buf,"Model Number"))!=NULL){
						n=20,m=0;
						while(file[n]!='\n'){
							machines[flag].disks[flag1].model_number[m++]=file[n++];}
					}

					if((file=strstr(buf,"Serial Number"))!=NULL){
						n=20,m=0;
						while(file[n]!='\n'){
							machines[flag].disks[flag1].serial_number[m++]=file[n++];
						}
					}
					if((file=strstr(buf,"Firmware Revision"))!=NULL){
						n=20,m=0;
						while(file[n]!='\n'){
							machines[flag].disks[flag1].fie_rev[m++]=file[n++];
						}
					}
			}
			
			if(flag == -1) break;
			strcpy(machines[flag].ip, ip);

			printmachine(msg, machines+flag);
			sendmsg(msg);

			file1 =  fopen("recv.txt",  "a");
			fprintf(file1, "=======================================\n");
			fprintf(file1, "receive from %s:%d\n", ip, port);
			fprintf(file1, "---------------------------------------\n");
			fwrite(buf, 1, len, file1);
			fprintf(file, "\n=======================================\n\n\n");
			fclose(file1);
			*/
			printf("server receive\n");
			break;
		case TCP_DISCONNECT:
			break;
	}
}

void uiheartbeat(void){
}