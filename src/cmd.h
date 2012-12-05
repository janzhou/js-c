#ifndef _H_CMD_
#define _H_CMD_

void do_cmd_stdin(void);
void do_cmd_tcp(char * ip, int port, int sock_id, int event, void * cmd, int len, void * callbackdata);

#endif