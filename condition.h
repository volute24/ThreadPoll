#ifndef _CONDITION_H_
#define _CONDITION_H_

#include <pthread.h>

//��װ������������������Ϊ��ʼ״̬
typedef struct condition
{
	pthread_mutex_t pmutex;
	pthread_cond_t  pcond;
}condition_t;

//��״̬��������
int condition_init(condition_t *cond);
int condition_lock(condition_t *cond);
int condition_unlock(condition_t *cond);
//�����ȴ�
int condition_wait(condition_t *cond);
//��ʱ�ȴ�
int condition_timedwait(condition_t *cond,const struct timespec *abstime);
//����һ���ȴ����������߳�
int condition_signal(condition_t *cond);
//�������еȴ��߳�
int condition_broadcast(condition_t *cond);
int condition_destroy(condition_t *cond);

#endif


