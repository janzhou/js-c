#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ui.h"
#include "server.h"
#include "convert.h"

void printmachine(char * buf, struct machine_t * machine){
	int i = 0;
	sprintf(buf, "{\"type\":\"machine\",\"id\":%d,\"ip\":\"%s\",\"port\":%d,\"status\":%d,\"disks\":[", machine->id, machine->ip, machine->port, machine->status);
	sprintf(buf+strlen(buf), "{\"type\":\"disk\",\"status\":%d}", machine->disks[i].status);
	for(i = 1; i < 16; i++){
		sprintf(buf+strlen(buf), ",{\"type\":\"disk\",\"status\":%d, \"capacity\":%.2f}", machine->disks[i].status, machine->disks[i].capacty);
	}
	sprintf(buf+strlen(buf), "]}");
}

void printresult(char * buf, struct result_t * result){
	int i = 0;
	switch(result->type){
		case IOMETER:
			sprintf(buf, "{\"type\":\"result\",\"test\":\"iometer\",\"machine\":\"%d\",\"disk\":\"%d\",\"progress\":%f,\"iops\":%f}", result->machine, result->disk, result->progress, result->iops);
			break;
		case BIT:
			sprintf(buf, "{\"type\":\"result\",\"test\":\"bit\",\"machine\":\"%d\",\"disk\":\"%d\",\"progress\":%f,\"readspeed\":%f,\"writespeed\":%f,\"readiops\":%f,\"writeiops\":%f,\"errcnt\":%f}", result->machine, result->disk, result->progress, result->readspeed, result->writespeed, result->readiops, result->writeiops, result->errcnt);
			break;
		case IOMeter_Result:
			sprintf(buf, "{\"type\":\"result\",\"test\":\"iometerresult\",\"machine\":\"%d\",\"disk\":\"%d\",\"maxio\":%f,\"minio\":%f,\"avgio\":%f}", result->machine, result->disk, result->maxio, result->minio, result->avgio);
			break;
	}
}



void printtextresult(char * buf, char * text){
	int i = 0;
	char convertbuf[800];
	stringtohtml(text, convertbuf, sizeof(convertbuf));
	sprintf(buf, "{\"type\":\"textresult\",\"text\":\"%s\"}", convertbuf);
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

	char msg[1024];

	int machine, disk;
	struct result_t result;

	char * find;

	switch(event){
		case TCP_CONNECT:
			break;
		case TCP_DATAREADY:
		case UDP_DATAREADY:

			printtextresult(msg, buf);
			sendmsg(msg);

			//machine
			if((find = strstr(buf,"machine= worker "))!=NULL){
				machine = atoi(find + strlen("machine= worker "));
				//machine编号问题，返回
				if(machine > 7 || machine < 0) return;
				if(machine == 0 && *(find + strlen("machine= worker ")) != '0') return;
			}else return;//没有machine字段，直接返回
			
			buf = find;

			//disk
			if((find = strstr(buf, "port= "))!=NULL){
				buf = find;
				disk = atoi(find + strlen("port= ")) - 1;
				//disk编号问题，返回
				if(disk > 15 || disk < 0) return;
				if(disk == 0 && *(find + strlen("port= ")) != '0') return;

				//disk编号没有问题
				machines[machine].status = 1;
				strcpy(machines[machine].ip, ip);
				if((find = strstr(buf, "section=query"))!=NULL){//query
					machines[machine].disks[disk].status = 1;
					if((find = strstr(buf,"Disk /dev/"))!=NULL){
						machines[machine].disks[disk].capacty = atof(find + 15);
					}

					printmachine(msg, machines+machine);
					sendmsg(msg);
				}else if((find = strstr(buf,"Section=IOMeter"))!=NULL){//iometer
					result.type = IOMETER;
					result.progress = 0;
					result.iops = 0;

					if((find = strstr(buf,"Progress="))!=NULL){
						result.progress = atof(find + 9);
					}

					if((find = strstr(buf,"IOPS="))!=NULL){
						result.iops = strtod(find+5,NULL);
					}

					result.machine = machine;
					result.disk = disk;

					printresult(msg, &result);
					sendmsg(msg);

				}else if((find = strstr(buf,"section=bit"))!=NULL){//bit
					result.type = BIT;
					result.progress = 0;
					result.readspeed = 0;
					result.writespeed = 0;
					result.readiops = 0;
					result.writeiops = 0;
					result.errcnt = 0;

					if((find = strstr(buf,"progress="))!=NULL){
						result.progress = atof(find + strlen("progress="));
					}
					if((find = strstr(buf,"readspeed="))!=NULL){
						result.readspeed = atof(find + strlen("readspeed="));
					}
					if((find = strstr(buf,"writespeed="))!=NULL){
						result.writespeed = atof(find + strlen("writespeed="));
					}
					if((find = strstr(buf,"readiops="))!=NULL){
						result.readiops = atof(find + strlen("readiops="));
					}
					if((find = strstr(buf,"writeiops="))!=NULL){
						result.writeiops = atof(find + strlen("writeiops="));
					}
					if((find = strstr(buf,"errcnt="))!=NULL){
						result.errcnt = atof(find + strlen("errcnt="));
					}
					result.machine = machine;
					result.disk = disk;

					printresult(msg, &result);
					sendmsg(msg);
				}else if((find = strstr(buf,"Section=IOMeter_Result"))!=NULL){//bit
					result.type = IOMeter_Result;
					result.progress = 0;
					result.maxio = 0;
					result.minio = 0;
					result.avgio = 0;

					if((find = strstr(buf,"MaxIO= "))!=NULL){
						result.maxio = atof(find + 7);
					}

					if((find = strstr(buf,"MinIO= "))!=NULL){
						result.minio = atof(find + 7);
					}

					if((find = strstr(buf,"AvgIO= "))!=NULL){
						result.avgio = atof(find + 7);
					}

					result.machine = machine;
					result.disk = disk;

					printresult(msg, &result);
					sendmsg(msg);
				}

			}else{//没有port字段，为machine信息
				if((find = strstr(buf,"Result= Success"))!=NULL){//machine active
					strcpy(machines[machine].ip, ip);
					machines[machine].status = 1;
					printmachine(msg, machines+machine);
					sendmsg(msg);
				}else{//machine inactive 如何判断?
				//	machines[machine].status = 0;
				}
			}
			break;
		case TCP_DISCONNECT:
			break;
	}
}

void uiheartbeat(void){
}