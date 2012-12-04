#ifndef _H_UI_
#define _H_UI_

struct disk_t{
	int status;
	int port;
	char disk_store[32];
	char Units[16];
	char Sector_size[16];
	char iosize[16];
	char disk_identifer[12];
	char model_number[16];
	char serial_number[12];
	char fie_rev[10];
};

struct machine_t{
	int id;
	char ip[16];
	int port;
	int status;
	char machine[16];
	struct disk_t disks[16];
};

struct machine_t machines[8];

void do_ui(char * ip, int port, int sock_id, int event, void * buf, int len, void * callbackdata);

void uiinit(void);
void uiheartbeat(void);

void printmachine(char * buf, struct machine_t * machine);

#endif