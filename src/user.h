#ifndef _H_USER_
#define _H_USER_

char * getwebmsg(int user_id);
void sendwebmsg(char * msg);

void userinit(void);
void userheartbeat(void);

#endif
