// Distributed under the terms of the GNU General Public License v2

#ifndef _SEM_H_
#define _SEM_H_

void * create_sem(int max, int init);

void del_sem(void * sem);

void p(void * sem);

void v(void * sem);

#endif
