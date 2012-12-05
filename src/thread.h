#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <process.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

#ifdef __linux
#include <arpa/inet.h>
#include <pthread.h>
#endif

#ifdef _WIN32
#define thread_t HANDLE
#define thread_func_t DWORD WINAPI 
#define thread_arg_t LPVOID
int create_thread(thread_t * ptid, thread_func_t (* func)(thread_arg_t), thread_arg_t arg){
	*ptid = CreateThread(NULL, 0, func, arg, 0, NULL);
	return !(*ptid)
}
void thread_sleep(int time){
	Sleep(time);
}
#endif

#ifdef __linux
#define thread_t pthread_t
#define thread_func_t void *
#define thread_arg_t void *
int create_thread(thread_t * ptid, thread_func_t (* func)(thread_arg_t arg), thread_arg_t arg){
	return pthread_create(ptid, NULL, func, arg);
}
void thread_sleep(int time){
	sleep(time);
}
#endif
