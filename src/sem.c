#include "sem.h"

#ifdef _WIN32
#include <Windows.h>

void * create_sem(int max, int init){
	return CreateSemaphore( 
        NULL,           // default security attributes - lpSemaphoreAttributes是信号量的安全属性
        init,  // initial count - lInitialCount是初始化的信号量
        max,  // maximum count - lMaximumCount是允许信号量增加到最大值
        NULL); 
}

void del_sem(void * sem){
	CloseHandle(sem);
}

void p(void * sem){
	WaitForSingleObject( 
            sem,   // handle to semaphore
            0L);
}

void v(void * sem){
	ReleaseSemaphore( 
                sem,  // handle to semaphore - hSemaphore是要增加的信号量句柄
                1,            // increase count by one - lReleaseCount是增加的计数
                NULL) ;
}

#endif

#ifdef __linux
#include <semaphore.h>
#include <malloc.h>

void * create_sem(int max, int init){
   sem_t *pmutex = (sem_t *)malloc(sizeof(sem_t)); 
   sem_init(pmutex, max=0, init);
   return pmutex;
}

void del_sem(void * sem){
	sem_destroy(sem);
}

void p(void * sem){
    sem_wait(sem);
}

void v(void * sem){
    sem_post(sem);
}

#endif


