#include "threadpool.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define  THREADPOOL_MAX_NUM 5
void* mytask(void *arg)
{
    printf("thread %d is working on task %d\n", (int)pthread_self(), *(int*)arg);
    sleep(1);
    free(arg);
    return NULL;
}

//测试代码
int main(int argc,char* argv[])
{
    threadpool_t pool;
    //初始化线程池，最多三个线程
    threadpool_init(&pool, THREADPOOL_MAX_NUM);
    int i;
    //创建十个任务
    for(i=0; i < 10; i++)
    {
        int *arg =(int *)malloc(sizeof(int));
        *arg = i;
        threadpool_add_task(&pool, mytask, arg);
        //printf("arg address:%p,arg:[%d],i:[%d]\n",arg,*arg,i);
    }
    threadpool_destroy(&pool);
    return 0;
}





