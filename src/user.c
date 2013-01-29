// Distributed under the terms of the GNU General Public License v2

#include <stdio.h>
#include <malloc.h>

#include "user.h"
#include "ui.h"
#include "sem.h"

void * usersem;

struct buffer_t{
	char msg[1024];
	struct buffer_t * next;
	struct buffer_t * pre;
};

struct user_t{
	int flag;
	int id;
	struct user_t * next;
	struct user_t * pre;
	struct buffer_t * buffer;
};

static struct user_t * userhead = NULL;

void userinit(void)
{
	usersem = create_sem(1, 1);
	return;
}

void sendwebmsg(char * msg)
{
	struct user_t * user;
	struct buffer_t * buffer;

	p(usersem);

	user = userhead;
	while(user != NULL){
		buffer = (struct buffer_t *)malloc(sizeof(struct buffer_t));
		sprintf(buffer->msg, "{\"type\":\"msg\",\"msg\":%s}", msg);

		if(user->buffer == NULL){
			user->buffer = buffer;
			buffer->next = buffer->pre = buffer;
		}else{
			user->buffer->pre->next = buffer;
			buffer->pre = user->buffer->pre;
			user->buffer->pre = buffer;
			buffer->next = user->buffer;
			user->buffer = buffer->next;
		}
		if(user->next == userhead){
			break;
		}
		user = user->next;
	}
	v(usersem);
}

char * getwebmsg(int user_id){
	struct user_t * user;
	struct buffer_t * buffer;

	int i;
	char msg[1024];

	static int global = 1;

	p(usersem);
	user = userhead;
	while(user != NULL){
		if(user->id == user_id){
			break;
		}
		if(user->next == userhead){
			break;
		}
		user = user->next;
	}
	//user 不存在
	if(user == NULL || user->id != user_id){
		user = (struct user_t *)malloc(sizeof(struct user_t));
		user->id = global++;
		user->flag = 0;
		user->buffer = NULL;

		if(userhead == NULL){
			userhead = user;
			user->next = user->pre = user;
		}else {
			userhead->pre->next = user;
			user->pre = userhead->pre;
			userhead->pre = user;
			user->next = userhead;
			userhead = user->next;
		}

		for(i = 0; i < 8; i++){
			buffer = (struct buffer_t *)malloc(sizeof(struct buffer_t));
			printmachine(msg, machines + i);
			sprintf(buffer->msg, "{\"type\":\"msg\",\"msg\":%s}", msg);
			if(user->buffer == NULL){
				user->buffer = buffer;
				buffer->next = buffer->pre = buffer;
			}else{
				user->buffer->pre->next = buffer;
				buffer->pre = user->buffer->pre;
				user->buffer->pre = buffer;
				buffer->next = user->buffer;
				user->buffer = buffer->next;
			}
		}
		v(usersem);
		buffer = (struct buffer_t *)malloc(sizeof(struct buffer_t));
		sprintf(buffer->msg, "{\"type\":\"new\",\"id\":%d}", user->id);

		return buffer->msg;
	}

	//user 存在
	user->flag = 0;
	buffer = user->buffer;

	
	if(buffer == NULL){//无消息，返回none
		buffer = (struct buffer_t *)malloc(sizeof(struct buffer_t));
		sprintf(buffer->msg, "{\"type\":\"none\"}");
	}else {//有消息
		if(buffer->next == buffer){
			user->buffer = NULL;
		}else {
			buffer->pre->next = buffer->next;
			buffer->next->pre = buffer->pre;
			user->buffer = buffer->next;
		}
	}
	v(usersem);
	return buffer->msg;   

} 

void userheartbeat(void){

	struct user_t * user;
	struct user_t * pre;
	struct buffer_t * buffer;
	struct buffer_t * buffer_next;
	
	p(usersem);

	user = userhead;
	
	while(user != NULL){
		user->flag++;
		if(30 < user->flag){
			buffer = user->buffer;
			
			if(buffer != NULL){
				buffer->pre->next = NULL;
			}
			
			while(buffer != NULL){
				buffer_next = buffer->next;
				free(buffer);
				buffer = buffer_next;
			}

			if(user->next == user){
				userhead = NULL;
				free(user);
				user = NULL;
				continue;
			}else {
				user->pre->next = user->next;
				user->next->pre = user->pre;
				userhead = user->next;
				pre = user->next;
				free(user);
				user = pre;
				continue;
			}		
		}
		user = user->next;

		if(user == userhead) break;
	}

	v(usersem);
}
