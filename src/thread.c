#include "thread.h"

#ifdef _WIN32
int create_thread(thread_t * ptid, thread_func_t (* func)(thread_arg_t), thread_arg_t arg){
	*ptid = CreateThread(NULL, 0, func, arg, 0, NULL);
	return !(*ptid)
}
void thread_sleep(int time){
	Sleep(time);
}
#endif

#ifdef __linux
#include <unistd.h> //sleep
int create_thread(thread_t * ptid, thread_func_t (* func)(thread_arg_t arg), thread_arg_t arg){
	return pthread_create(ptid, NULL, func, arg);
}
void thread_sleep(int time){
	sleep(time);
}
#endif


