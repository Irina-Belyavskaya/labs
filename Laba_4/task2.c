#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <pthread.h>

#define TOTAL_SIGNALS 101
#define NUM_THREADS 8

int daytime [4];
int getSig1;
int numSigUSR1;
int numSigUSR2;

pid_t node0 = 0,node1 = 0,node2 = 0,node3 = 0,node4 = 0,node5 = 0,node6 = 0,node7 = 0,node8 = 0;
pid_t pidgroup1 = 0,pidgroup2 = 0;

char *sem_name1 = "SemGroup1";
char *sem_name2 = "SemGroup2";

sem_t *mutex1;
sem_t *mutex2;
FILE *f;

struct sigaction sa;
struct sigaction saterm;

pthread_barrier_t barrier; 

void Node1Handler(int);
void Node2Handler(int);
void Node3Handler(int);
void Node4Handler(int);
void Node5Handler(int);
void Node6Handler(int);
void Node7Handler(int);
void Node8Handler(int);

void Node1TermHandler(int);
void Node2TermHandler(int);
void Node3TermHandler(int);
void Node4TermHandler(int);
void Node5TermHandler(int);
void Node6TermHandler(int);
void Node7TermHandler(int);
void Node8TermHandler(int);

void BuildTree ();
void SetSignals();
void ShowInfo(char *, int, int);
void MutexGroup1(pid_t);
void MutexGroup2(pid_t);

void main () 
{
    sem_unlink (sem_name1);
    sem_unlink (sem_name2);
    mutex1 = sem_open(sem_name1,O_CREAT,0777,1);
    if ( mutex1 == NULL)
    {
        printf("Can`t open MUTEX 1.\n");
    }
    mutex2 = sem_open(sem_name2,O_CREAT,0777,1);
    if ( mutex2 == NULL)
    {
        printf("Can`t open MUTEX 2.\n");
    }
    
    // pthread_barrierattr_t attr;
    // int ret; 
    // ret = pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    // if (ret !=  0)
    // {
    //     printf("Cannot set attribute barrier\n");
    // }
    // ret = pthread_barrier_init(&barrier, &attr, NUM_THREADS + 1);
    // if (ret !=  0)
    // {
    //     printf("Cannot init barrier\n");
    // }
    BuildTree ();
    sleep(1);
    SetSignals();
    sem_unlink (sem_name1);
    sem_unlink (sem_name2);
    exit(0);
}

void BuildTree ()
{
    node0 = getpid();
    node1 = fork();
    if (node1 == 0)
    {
        node1 = getpid();
        printf("Node 1 was created PID: %d MY PARENT: %d\n",getpid(), getppid());

        node2 = fork();
        if (node2 == 0)
        {
            node2 = getpid();
            printf("Node 2 was created by Node 1 PID: %d MY PARENT: %d\n",getpid(), getppid());

            node5 = fork();
            if (node5 == 0)
            {
                node5 = getpid();
                printf("Node 5 was created by Node 2 PID: %d MY PARENT: %d\n",getpid(), getppid());

            } else if (node5 > 0)
            {
                node6 = fork();
                if (node6 == 0)
                {
                    node6 = getpid();
                    printf("Node 6 was created by Node 2 PID: %d MY PARENT: %d\n",getpid(), getppid());

                    node7 = fork();
                    if (node7 == 0)
                    {
                        node7 = getpid();
                        printf("Node 7 was created by Node 6 PID: %d MY PARENT: %d\n",getpid(), getppid());

                        node8 = fork();
                        if (node8 == 0)
                        {
                            node8 = getpid();
                            printf("Node 8 was created by Node 7 PID: %d MY PARENT: %d\n",getpid(), getppid());

                        } else if (node8 == -1)
                        {
                            perror("fork() error"); 
                            exit(-1);
                        }
                    } else if (node7 == -1)
                    {
                        perror("fork() error"); 
                        exit(-1);
                    }
                } else if (node6 == -1)
                {
                    perror("fork() error"); 
                    exit(-1);
                }
            } else 
            {
                perror("fork() error"); 
                exit(-1);
            }
        }else if (node2 > 0)
        {
            node3 = fork();
            if (node3 == 0) 
            {
                node3 = getpid();
                printf("Node 3 was created by Node 1 PID: %d MY PARENT: %d\n",getpid(), getppid());
                  
            } else if (node3 > 0)
            {
                node4 = fork();
                if (node4 == 0)
                {
                    node4 = getpid();
                    printf("Node 4 was created by Node 1 PID: %d MY PARENT: %d\n",getpid(), getppid());

                }else if (node4 == -1)
                {
                    perror("fork() error"); 
                    exit(-1);
                }

            } else if (node3 == -1)
            {
                perror("fork() error"); 
                exit(-1);
            }
        } else
        {
            perror("fork() error"); 
            exit(-1);
        }
    } else if (node1 == -1)
    {
        perror("fork() error"); 
        exit(-1);
    }
}

void DownloadFromFile1()
{
    if (sem_wait(mutex1) != 0)
        perror("sem_wait 1");
    FILE *file = fopen("group1.txt", "r");
    if (file != NULL)
    {
        pid_t pid;
        fscanf(file, "%d", &pid);
        pidgroup1 = pid;
    } else 
        perror("file group1");
    fclose(file);
    if (sem_post(mutex1) != 0)
        perror("sem_post 1");
}

void DownloadFromFile2()
{
    if (sem_wait(mutex2) != 0)
        perror("sem_wait 2");
    FILE *file = fopen("group2.txt", "r");
    if (file != NULL)
    {
        pid_t pid;
        fscanf(file, "%d", &pid);
        pidgroup2 = pid;
    } else 
        perror("file group2");
    fclose(file);
    if (sem_post(mutex2) != 0)
        perror("sem_post 2");
}

void MutexGroup1(pid_t node) 
{
    if (sem_wait(mutex1) != 0)
        perror("sem_wait 1");
    f = fopen("group1.txt", "r");
    if (f == NULL)
    {
        f = fopen("group1.txt", "w");
        if (f == NULL)
            printf("Can`t create file 'group1'\n");
        else
        {
            if (setpgid(node,pidgroup1) == -1)
            {
                perror("setpgid group1(1)");
                exit(-1);
            }
            pidgroup1 = node;
            fprintf(f, "%d", pidgroup1);
        }
    } else {
        pid_t pid;
        fscanf(f, "%d", &pid);
        pidgroup1 = pid;
        if (setpgid(node,pidgroup1) == -1)
        {
            perror("setpgid group1(2)");
            exit(-1);
        }
    }
    fclose(f);
    printf("pidgroup1 = %d group pid = %d process pid = %d\n",pidgroup1, getpgid(node), getpid());
    if (sem_post(mutex1) != 0)
        perror("sem_post 1");
}

void MutexGroup2(pid_t node) 
{
    if (sem_wait(mutex2) != 0 )
        perror("sem_wait 2");
    f = fopen("group2.txt", "r");
    if (f == NULL)
    {
        f = fopen("group2.txt", "w");
        if (f == NULL)
            printf("Can`t create file 'group1'\n");
        else
        {
            if (setpgid(node,pidgroup2) == -1)
            {
                perror("setpgid group2(1)");
                exit(-1);
            }
            pidgroup2 = node;
            fprintf(f, "%d", pidgroup2);
        }
    } else {
        pid_t pid;
        fscanf(f, "%d", &pid);
            pidgroup2 = pid;
        if (setpgid(node,pidgroup2) == -1)
        {
            perror("setpgid group2(2)");
            exit(-1);
        }
    }
    fclose(f);
    printf("pidgroup2 = %d group pid = %d process pid = %d\n",pidgroup2, getpgid(node), getpid());
    if (sem_post(mutex2) != 0)
        perror("sem_post 2");
}

void Hadler (int sig)
{
    printf ("node = %d SIGNAL = %d\n",getpid(),sig);
}

void SetSignals()
{
    struct sigaction sa;
    struct sigaction saterm;

    sigset_t set, mask;
    sigfillset(&mask);
    if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
    {
        perror("sigprocmask 0");
    }
    sigaddset(&set,SIGTERM);

    if (getpid() == node1) 
    {
        sigaddset(&set,SIGUSR2);
        if (sigprocmask(SIG_UNBLOCK,&set,&mask) == -1)
        {
            perror("sigprocmask 1");
        }
        sa.sa_mask = set;

        // sa.sa_handler = Node1Handler;
        // for (int sig = 1; sig <= SIGRTMAX; ++sig) {
        //     sigaction(sig, &sa, NULL);
        // }

        sa.sa_handler = Node1Handler;
        if (sigaction(SIGUSR2, &sa, NULL) == -1) 
        {
            perror("sigaction");
            exit(-1);
        }

        
        sleep(10);
        DownloadFromFile1();

        if (kill(-pidgroup1, SIGUSR1) == -1)
            perror("KILL GROUP1");
        else
        {
            ShowInfo("sent", 1, 1);
            numSigUSR1++;
        }
    }

    if (getpid() == node2) 
    {
        MutexGroup1(node2);

        sigaddset(&set,SIGUSR1);
        if (sigprocmask(SIG_UNBLOCK,&set,&mask) == -1)
        {
            perror("sigprocmask 2");
        }
        sa.sa_mask = set;
        
        sa.sa_handler = Node2Handler;
        if (sigaction(SIGUSR1, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        saterm.sa_handler = Node2TermHandler;
        if (sigaction(SIGTERM, &saterm, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        // sa.sa_handler = Node2Handler;
        // for (int sig = 1; sig <= SIGRTMAX; ++sig) {
        //     sigaction(sig, &sa, NULL);
        // }
    }

    if (getpid() == node3) 
    {
        MutexGroup1(node3);

        sigaddset(&set,SIGUSR1);
        if (sigprocmask(SIG_UNBLOCK,&set,&mask) == -1)
        {
            perror("sigprocmask 3");
        }
        sa.sa_mask = set;

        sa.sa_handler = Node3Handler;
        if (sigaction(SIGUSR1, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        saterm.sa_handler = Node3TermHandler;
        if (sigaction(SIGTERM, &saterm, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        // sa.sa_handler = Node3Handler;
        // for (int sig = 1; sig <= SIGRTMAX; ++sig) {
        //     sigaction(sig, &sa, NULL);
        // }
    }

    if (getpid() == node4) 
    {
        MutexGroup1(node4);

        sigaddset(&set,SIGUSR1);
        if (sigprocmask(SIG_UNBLOCK,&set,&mask) == -1)
        {
            perror("sigprocmask 4");
        }
        sa.sa_mask = set;

        sa.sa_handler = Node4Handler;
        if (sigaction(SIGUSR1, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        saterm.sa_handler = Node4TermHandler;
        if (sigaction(SIGTERM, &saterm, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        // sa.sa_handler = Node4Handler;
        // for (int sig = 1; sig <= SIGRTMAX; ++sig) {
        //     sigaction(sig, &sa, NULL);
        // }

    }

    if (getpid() == node5) 
    {
        MutexGroup2(node5);

        sigaddset(&set,SIGUSR2);
        if (sigprocmask(SIG_UNBLOCK,&set,&mask) == -1)
        {
            perror("sigprocmask 5");
        }
        sa.sa_mask = set;

        sa.sa_handler = Node5Handler;
        if (sigaction(SIGUSR2, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        saterm.sa_handler = Node5TermHandler;
        if (sigaction(SIGTERM, &saterm, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        // sa.sa_handler = Node5Handler;
        // for (int sig = 1; sig <= SIGRTMAX; ++sig) {
        //     sigaction(sig, &sa, NULL);
        // }
    }

    if (getpid() == node6) 
    {
        MutexGroup2(node6);

        sigaddset(&set,SIGUSR2);
        if (sigprocmask(SIG_UNBLOCK,&set,&mask) == -1)
        {
            perror("sigprocmask 6");
        }
        sa.sa_mask = set;

        sa.sa_handler = Node6Handler;
        if (sigaction(SIGUSR2, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        saterm.sa_handler = Node6TermHandler;
        if (sigaction(SIGTERM, &saterm, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        // sa.sa_handler = Node6Handler;
        // for (int sig = 1; sig <= SIGRTMAX; ++sig) {
        //     sigaction(sig, &sa, NULL);
        // }
    }

    if (getpid() == node7) 
    {
        sigaddset(&set,SIGUSR1);
        if (sigprocmask(SIG_UNBLOCK,&set,&mask) == -1)
        {
            perror("sigprocmask 7");
        }
        sa.sa_mask = set;

        sa.sa_handler = Node7Handler;
        if (sigaction(SIGUSR1, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        saterm.sa_handler = Node7TermHandler;
        if ( sigaction(SIGTERM, &saterm, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        // sa.sa_handler = Node7Handler;
        // for (int sig = 1; sig <= SIGRTMAX; ++sig) {
        //     sigaction(sig, &sa, NULL);
        // }
    }

    if (getpid() == node8) 
    {
        sigaddset(&set,SIGUSR1);
        if (sigprocmask(SIG_UNBLOCK,&set,&mask) == -1)
        {
            perror("sigprocmask 8");
        }
        sa.sa_mask = set;

        sa.sa_handler = Node8Handler;
        if (sigaction(SIGUSR1, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        saterm.sa_handler = Node8TermHandler;
        if (sigaction(SIGTERM, &saterm, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        // sa.sa_handler = Node8Handler;
        // for (int sig = 1; sig <= SIGRTMAX; ++sig) {
        //     sigaction(sig, &sa, NULL);
        // }
    }
    
    // if (pthread_barrier_wait(&barrier) == PTHREAD_BARRIER_SERIAL_THREAD)
    // {
    //     pthread_barrier_destroy(&barrier);
    // } else 
    // {
    //     printf("error wait barrier in thread\n");
    // }

    //sleep(3);

    // if (getpid() == node1)
    // {
    //     printf("go\n");
    //     DownloadFromFile1();

    //     if (kill(-pidgroup1, SIGUSR1) == -1)
    //         perror("KILL GROUP1");
    //     else
    //     {
    //         ShowInfo("sent", 1, 1);
    //         numSigUSR1++;
    //     }
    // } //else 
    // //printf("Node = %d\n", getpid());

    if (getpid() != node0)
    {
        while (1);
    }
    else 
        wait(NULL);
        
}

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

void ShowInfo(char *action, int signal, int numNode) 
{
    getTime();
    printf("№%d PID: %d Parent: %d %s SIGUSR%d Time: %02d:%02d:%02d:%03d GROUP:%d\n",numNode,getpid(),getppid(),action,signal,daytime[0],daytime[1],daytime[2],daytime[3],getpgid(getpid()));
}

void Node1Handler(int sig)
{
    ++getSig1;
    ShowInfo ("got", sig, 1);

    if (getSig1 == TOTAL_SIGNALS)
    {
        sleep(1);
        DownloadFromFile1();
        if (kill(-pidgroup1, SIGTERM) == -1 )
        {
            perror("KILL SIGTERM");
        };

        int children = 3; 
        while (children--)
        {
            wait(NULL);
        }
        printf("№1 PID: %d Parent: %d finished work with amount of recieved signals = %d SIGUSR2 and sent = %d SIGUSR1\n", getpid(), getppid(), getSig1,numSigUSR1);
        exit(0);
    }
    else
    {
        DownloadFromFile1();
        if (kill(-pidgroup1, SIGUSR1) == -1)
        {
            perror("KILL group1 handler1");
        } else 
        {
            ShowInfo ("sent", sig, 1);
            ++numSigUSR1;
        }
    }
}

void Node2Handler(int sig)
{
    ShowInfo ("got", 1, 2);
    DownloadFromFile2();

    if (kill(-pidgroup2, SIGUSR2) == -1)
    {
        perror("KILL group2 handler2");
    } else 
    {
        ShowInfo ("sent", 2, 2);
        ++numSigUSR2;
    }
}

void Node3Handler(int sig)
{
    ShowInfo ("got", 1, 3);
}

void Node4Handler(int sig)
{
    ShowInfo ("got", 1, 4);
}

void Node5Handler(int sig)
{
    ShowInfo ("got", 2, 5);
}

void Node6Handler(int sig)
{
    ShowInfo ("got", 2, 6);

    if (kill(node7, SIGUSR1) == -1)
    {
        perror("KILL handler6");
    } else 
    {
        ShowInfo ("sent", 1, 6);
        ++numSigUSR1;
    }
}

void Node7Handler(int sig)
{
    ShowInfo ("got", 1, 7);

    if (kill(node8, SIGUSR1) == -1)
    {
        perror("KILL handler7");
    } else 
    {
        ShowInfo ("sent", 1, 7);
        ++numSigUSR1;
    }
}

void Node8Handler(int sig)
{
    ShowInfo ("got", 1, 8);

    if (kill(node1, SIGUSR2) == -1)
    {
        perror("KILL handler8");
    } else 
    {
        ShowInfo ("sent", 2, 8);
        ++numSigUSR1;
    }
}

void Node2TermHandler(int sig)
{
    DownloadFromFile2();
    if (kill(-pidgroup2, SIGTERM) == -1)
    {
        perror("KILL SIGTERM");

    };
    int children = 2; 
    while (children--)
    {
        wait(NULL);
    }
    printf ("№2 PID: %d  Parent: %d finished work with amount of signals SIGUSR1 = %d and SIGUSR2 = %d\n",getpid(),getppid(),numSigUSR1,numSigUSR2);
    exit(0);
}

void Node3TermHandler(int sig)
{
    printf ("№3 PID: %d  Parent: %d finished work with amount of signals SIGUSR1 = %d and SIGUSR2 = %d\n",getpid(),getppid(),numSigUSR1,numSigUSR2);
    exit(0);
}

void Node4TermHandler(int sig)
{
    printf ("№4 PID: %d  Parent: %d finished work with amount of signals SIGUSR1 = %d and SIGUSR2 = %d\n",getpid(),getppid(),numSigUSR1,numSigUSR2);
    exit(0);
}

void Node5TermHandler(int sig)
{
    printf ("№5 PID: %d  Parent: %d finished work with amount of signals SIGUSR1 = %d and SIGUSR2 = %d\n",getpid(),getppid(),numSigUSR1,numSigUSR2);
    exit(0);
}

void Node6TermHandler(int sig)
{
    if (kill(node7, SIGTERM) == -1)
    {
        perror("KILL SIGTERM");

    };
    if (waitpid(node7, NULL, 0) == -1)
    {
        perror("WAIT");

    };
    printf ("№6 PID: %d  Parent: %d finished work with amount of signals SIGUSR1 = %d and SIGUSR2 = %d\n",getpid(),getppid(),numSigUSR1,numSigUSR2);
    exit(0);
}

void Node7TermHandler(int sig)
{
    if (kill(node8, SIGTERM) == -1)
    {
        perror("KILL SIGTERM");
    };
    if (waitpid(node8, NULL, 0) == -1)
    {
        perror("WAIT");
    };
    printf ("№7 PID: %d  Parent: %d finished work with amount of signals SIGUSR1 = %d and SIGUSR2 = %d\n",getpid(),getppid(),numSigUSR1,numSigUSR2);
    exit(0);
}

void Node8TermHandler(int sig)
{
    printf ("№8 PID: %d  Parent: %d finished work with amount of signals SIGUSR1 = %d and SIGUSR2 = %d\n",getpid(),getppid(),numSigUSR1,numSigUSR2);
    exit(0);
}