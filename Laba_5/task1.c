#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
 
int daytime [4];

void getTime() 
{
	struct timeval tv;
	gettimeofday( &tv, NULL );
	int msecs = tv.tv_usec / 1000;
	int secs = tv.tv_sec % 60;
	int mins = (tv.tv_sec / 60) % 60;
	int hrs = (tv.tv_sec / 3600 + 3) % 24;
    daytime[0] = hrs;
    daytime[1] = mins;
    daytime[2] = secs;
    daytime[3] = msecs;
}

void *ShowInfo(void *arg) 
{
    int numThread = *(int *)arg;
    getTime();
    printf("Thread №%d ID: %ld PID: %d Time: %02d:%02d:%02d:%03d\n",numThread,pthread_self(),getppid(),daytime[0],daytime[1],daytime[2],daytime[3]);
}

void main()
{
    int id1, id2, result;
    pthread_t  thread1, thread2; 
    int numThread1 = 1;

    result = pthread_create(&thread1, NULL, ShowInfo,&numThread1);
    if (result != 0) 
    {
        perror("Creating the first thread");
    }
 
    int numThread2 = 2;
    result = pthread_create(&thread2, NULL, ShowInfo, &numThread2); 
    if (result != 0)
    {
        perror("Creating the second thread");
    }
 
    result = pthread_join(thread1, NULL);
    if (result != 0) {
        perror("Joining the first thread");
    }
    result = pthread_join(thread2, NULL); 
    
    if (result != 0) {
        perror("Joining the second thread");
    }

}