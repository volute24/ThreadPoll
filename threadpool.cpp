
#include "threadpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

using namespace std;

//�߳�ִ��
void *thread_routine(void *arg)
{
	struct timespec abstime;
	int timeout;
	printf("thread %d is starting\n",(int)pthread_self());
	threadpool_t *pool = (threadpool_t *)arg;

	while(1)
	{
		timeout = 0;
		//���ʳ�ǰ����
		condition_lock(&pool->ready);
		//����
		pool->idle++;
		//�ȴ���������|| �յ��̳߳�����֪ͨ
		while(pool->first == NULL && !pool->quit)
		{
			//�����߳������ȴ�
			printf("thread %d is waiting\n",(int)pthread_self());
			//��ȡ�ӵ�ǰʱ����ϵȴ�ʱ�䣬���ó�ʱ˯��ʱ��
			//clock_gettime �ڱ�������ʱ����� -lrt ,librt��ʵ����clock_gettime����
			clock_gettime(CLOCK_REALTIME,&abstime);  //CLOCK_REALTIME ϵͳʵʱʱ��
			abstime.tv_sec += 2;
			int status;
			status = condition_timedwait(&pool->ready,&abstime);
			if(status == ETIMEDOUT)
			{
				printf("thread %d wait timed out\n",(int)pthread_self());
				timeout = -1;
				break;
			}
		}

		pool->idle--;
		
		if(pool->first != NULL)
		{
		 	//ȡ���ȴ�������ǰ�����Ƴ����񣬲�ִ������
		 	task_t *t = pool->first;
			pool->first = t->next;
			 //��������ִ����Ҫ����ʱ�䣬�Ƚ����������̷߳����̳߳�
			condition_unlock(&pool->ready);
			//ִ������
			t->run(t->arg);
			//ִ���������ͷ��ڴ�
            		free(t);
           		//���¼���
            		condition_lock(&pool->ready);	
		}
		 //�˳��̳߳�
	        if(pool->quit && pool->first == NULL)
	        {
	            pool->counter--;//��ǰ�������߳���-1
	            //���̳߳���û���̣߳�֪ͨ�ȴ��̣߳����̣߳�ȫ�������Ѿ����
	            if(pool->counter == 0)
	            {
	                condition_signal(&pool->ready);
	            }
	            condition_unlock(&pool->ready);
	            break;
	        }
		 //��ʱ�����������߳�
	        if(timeout == 1)
	        {
	            pool->counter--;//��ǰ�������߳���-1
	            condition_unlock(&pool->ready);
	            break;
	        }
		condition_unlock(&pool->ready);
	}
	   printf("thread %d is exiting\n", (int)pthread_self());
    return NULL;
}

//�̳߳س�ʼ��
void threadpool_init(threadpool_t *pool, int threads)
{
    
    int nstatu = condition_init(&pool-ready>);
    printf("Init return values:%d\n",nstatu);
    pool->first = NULL;
    pool->last =NULL;
    pool->counter =0;
    pool->idle =0;
    pool->max_threads = threads;
    pool->quit =0;
    
}

//����һ�������̳߳�
void threadpool_add_task(threadpool_t *pool, void *(*run)(void *arg), void *arg)
{
    //����һ���µ�����
    task_t *newtask = (task_t *)malloc(sizeof(task_t));
    newtask->run = run;
    newtask->arg = arg;
    newtask->next=NULL;//�¼ӵ�������ڶ���β��
    
    //�̳߳ص�״̬������̹߳�������ǰ��Ҫ����
    condition_lock(&pool->ready);
    
    if(pool->first == NULL)//��һ���������
    {
        pool->first = newtask;
    }        
    else    
    {
        pool->last->next = newtask;
    }
    pool->last = newtask;  //����βָ���¼�����߳�
    
    //�̳߳������߳̿��У�����
    if(pool->idle > 0)
    {
        condition_signal(&pool->ready);
    }
    //��ǰ�̳߳����̸߳���û�дﵽ�趨�����ֵ������һ���µ�����
    else if(pool->counter < pool->max_threads)
    {
        pthread_t tid;
        pthread_create(&tid, NULL, thread_routine,pool);
        pool->counter++;
    }
    //����������
    condition_unlock(&pool->ready);
}



//�̳߳�����
void threadpool_destroy(threadpool_t *pool)
{
    //����Ѿ��������٣�ֱ�ӷ���
    if(pool->quit)
    {
    return;
    }
    //����
    condition_lock(&pool->ready);
    //�������ٱ��Ϊ1
    pool->quit = 1;
    //�̳߳����̸߳�������0
    if(pool->counter > 0)
    {
        //���ڵȴ����̣߳������źŻ���
        if(pool->idle > 0)
        {
            condition_broadcast(&pool->ready);
        }
        //����ִ��������̣߳��ȴ����ǽ�������
        while(pool->counter)
        {
            condition_wait(&pool->ready);
        }
    }
    condition_unlock(&pool->ready);
    condition_destroy(&pool->ready);
}













