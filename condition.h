#ifndef _CONDITION_H_
#define _CONDITION_H_

#include <pthread.h>

//封装互斥量和条件变量作为初始状态
typedef struct condition
{
	pthread_mutex_t pmutex;
	pthread_cond_t  pcond;
}condition_t;

//对状态操作函数
int condition_init(condition_t *cond);
int condition_lock(condition_t *cond);
int condition_unlock(condition_t *cond);
//条件等待
int condition_wait(condition_t *cond);
//计时等待
int condition_timedwait(condition_t *cond,const struct timespec *abstime);
//激活一个等待该条件的线程
int condition_signal(condition_t *cond);
//激活所有等待线程
int condition_broadcast(condition_t *cond);
int condition_destroy(condition_t *cond);

#endif


