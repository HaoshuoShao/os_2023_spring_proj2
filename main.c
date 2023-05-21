#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 20

int pa, pb, pc;  
int buffer[BUFFER_SIZE];  //缓冲区
int buffer_count = 0;  //缓冲区未被消费的数据个数
int flag = 0; //生产者进程结束标志

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  //互斥锁
pthread_cond_t full = PTHREAD_COND_INITIALIZER;  //缓冲区满
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;  //缓冲区空

void *threadA(void *arg) {
    int i;

    for (int n = 0; n < BUFFER_SIZE; n++){  //初始化缓冲区，0表示空
        buffer[n] = 0;
    }

    for (i = 1; i <= pa; i++)
    {
        usleep(15000);
        pthread_mutex_lock(&mutex);

        while (buffer_count == BUFFER_SIZE) {
            pthread_cond_wait(&full, &mutex);  // 缓冲区已满，等待消费者消费
        }

        for (int in = 0; in < BUFFER_SIZE; in++)  // 在缓冲区的空位插入数据
        {
            if (!buffer[in])
            {
                buffer[in] = i;
                buffer_count++;
                break;
            } 
        }
        pthread_cond_signal(&empty);  // 通知消费者缓冲区非空

        pthread_mutex_unlock(&mutex);
    }    
    flag = 1;  //生产者进程结束
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void *threadB(void *arg) {
    int count = 0;  //统计所有能被pb整除的数字个数
    int sum = 0;  //统计所有能被pb整除的数字之和

    for(int n = 1; n<=pa; n++) {
        usleep(20000);
        pthread_mutex_lock(&mutex);

        while (buffer_count == 0 && !flag) {
            pthread_cond_wait(&empty, &mutex);  // 缓冲区为空，等待生产者生产
        }

        for (int out = 0; out < BUFFER_SIZE; out++)  // 取走缓冲区内可以被pb整除的数字
        {
            if (buffer[out] % pb == 0 && buffer[out])
            {   
                count++;
                sum += buffer[out];
                buffer[out] = 0;
                buffer_count--;
                break;
            } 
        }

        pthread_cond_signal(&full);  // 通知生产者缓冲区非满

        pthread_mutex_unlock(&mutex);
    }

    printf("Thread B, %d, %d\n", count, sum);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&empty);  // 通知消费者缓冲区非空
    pthread_exit(NULL);
}

void *threadC(void *arg) {
    int count = 0;  //统计所有能被pc整除的数字个数
    int sum = 0;  //统计所有能被pc整除的数字之和

    for(int n = 1; n<=pa; n++)  {
        usleep(15000);
        pthread_mutex_lock(&mutex);
        while (buffer_count == 0 && !flag) {
            pthread_cond_wait(&empty, &mutex);  // 缓冲区为空，等待生产者生产
        }

        for (int out = 0; out < BUFFER_SIZE; out++)  // 取走缓冲区内可以被pc整除的数字
        {
            if ((buffer[out] % pc == 0)&& (buffer[out]))
            {   
                count++;
                sum += buffer[out];
                buffer[out] = 0;
                buffer_count--;
                break;
            } 
        }

        pthread_cond_signal(&full);  // 通知生产者缓冲区非满

        pthread_mutex_unlock(&mutex);
    }

    printf("Thread C, %d, %d\n", count, sum);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&empty);  // 通知消费者缓冲区非空
    pthread_exit(NULL);
}

void *threadD(void *arg) {
    int count = 0;  //统计所有能被pd整除的数字个数
    int sum = 0;  //统计所有能被pd整除的数字之和

    for(int n = 1; n<=pa; n++) {
        usleep(10000);
        pthread_mutex_lock(&mutex);

        while (buffer_count == 0  && !flag) {
            pthread_cond_wait(&empty, &mutex);  // 缓冲区为空，等待生产者生产
        }

        for (int out = 0; out < BUFFER_SIZE; out++)  // 取走缓冲区内可以被pd整除的数字
        {
            if ((buffer[out] % pb)&&(buffer[out] % pc))
            {   
                count++;
                sum += buffer[out];
                buffer[out] = 0;
                buffer_count--;
            } 
        }

        pthread_cond_signal(&full);  // 通知生产者缓冲区非满

        pthread_mutex_unlock(&mutex);
    }

    printf("Thread D, %d, %d\n", count, sum);
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&empty);  // 通知消费者缓冲区非空
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {  //检查输入是否符合要求
        fprintf(stderr, "usage: main <pa> <pb> <pc>\n");
        return 1;
    }

    // 读入pa, pb, pc
    pa = atoi(argv[1]);
    pb = atoi(argv[2]);
    pc = atoi(argv[3]);

    //创建线程
    pthread_t thread[4];

    pthread_create(&thread[0], NULL, threadA, NULL);  
    pthread_create(&thread[1], NULL, threadB, NULL);
    pthread_create(&thread[2], NULL, threadC, NULL);
    pthread_create(&thread[3], NULL, threadD, NULL);

    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
    pthread_join(thread[2], NULL);
    pthread_join(thread[3], NULL);

    return 0;
}