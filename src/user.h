#ifndef _H_USER_
#define _H_USER_

char * getmsg(int user_id);
void sendmsg(char * msg);

void userinit(void);
void userheartbeat(void);

#endif