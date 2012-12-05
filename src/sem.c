#include <Windows.h>

#include "sem.h"

void * create_sem(int max, int init){
	return CreateSemaphore( 
        NULL,           // default security attributes - lpSemaphoreAttributes���ź����İ�ȫ����
        max,  // initial count - lInitialCount�ǳ�ʼ�����ź���
        init,  // maximum count - lMaximumCount�������ź������ӵ����ֵ
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
                sem,  // handle to semaphore - hSemaphore��Ҫ���ӵ��ź������
                1,            // increase count by one - lReleaseCount�����ӵļ���
                NULL) ;
}
