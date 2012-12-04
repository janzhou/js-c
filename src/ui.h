#ifndef _H_UI_
#define _H_UI_

struct disk_t{
	int status;
	float capacty;
	char model_number[32];
	char serial_number[32];
	char firmware_revision[32];
};

struct machine_t{
	int id;
	char ip[16];
	int port;
	int status;
	char machine[16];
	struct disk_t disks[16];
};

#define IOMETER 0
#define BIT 1
#define IOMeter_Result 2

struct result_t{
	int type;
	int machine;
	int disk;
	
	double progress;
	
	double iops;
	
	double maxio;
	double minio;
	double avgio;

	double readspeed;
	double writespeed;
	double readiops;
	double writeiops;
	double errcnt;
};

struct machine_t machines[8];

void do_ui(char * ip, int port, int sock_id, int event, void * buf, int len, void * callbackdata);

void uiinit(void);
void uiheartbeat(void);

void printmachine(char * buf, struct machine_t * machine);

#endif