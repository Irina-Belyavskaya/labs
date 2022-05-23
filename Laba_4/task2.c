#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>

#define TREE_LENGTH 8
#define TOTAL_SIGNALS 101

int daytime [4];
int getSig1;
int numSigUSR1;
int numSigUSR2;

pid_t node0,node1,node2,node3,node4,node5,node6,node7,node8;
pid_t pidgroup1,pidgroup2;

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
void Node3458TermHandler(int);
void Node6TermHandler(int);
void Node7TermHandler(int);

void BuildTree ();
void SetSignals();
void ShowInfo(char *, int, int);

void main () 
{
    BuildTree ();
    sleep(1);
    SetSignals();
    sleep(1);
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
            setpgid(node2,pidgroup1);

            node5 = fork();
            if (node5 == 0)
            {
                node5 = getpid();
                printf("Node 5 was created by Node 2 PID: %d MY PARENT: %d\n",getpid(), getppid());
                setpgid(node5,pidgroup2);

            } else if (node5 > 0)
            {
                node6 = fork();
                if (node6 == 0)
                {
                    node6 = getpid();
                    printf("Node 6 was created by Node 2 PID: %d MY PARENT: %d\n",getpid(), getppid());
                    setpgid(node6,pidgroup2);

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
                setpgid(node3,pidgroup1);
            } else if (node3 > 0)
            {
                node4 = fork();
                if (node4 == 0)
                {
                    node4 = getpid();
                    printf("Node 4 was created by Node 1 PID: %d MY PARENT: %d\n",getpid(), getppid());
                    setpgid(node4,pidgroup1);
                }
            } else if (node4 == -1)
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

void SetSignals()
{
    if (getpid() == node1) 
    {
        struct sigaction sa;
        sa.sa_handler = Node1Handler;
        if (sigaction(SIGUSR2, &sa, NULL) == -1) 
        {
            perror("sigaction");
            exit(-1);
        }

        if (kill(-pidgroup1, SIGUSR1) == -1)
            perror("kill");
        else
        {
            ShowInfo("sent", 1, 1);
            numSigUSR1++;
        }
    }

    if (getpid() == node2) 
    {
        struct sigaction sa;
        sa.sa_handler = Node2Handler;
        if (sigaction(SIGUSR1, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        struct sigaction saterm;
        saterm.sa_handler = Node2TermHandler;
        if (sigaction(SIGTERM, &saterm, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        if (kill(-pidgroup2, SIGUSR2) == -1)
            perror("kill");
        else
        {
            ShowInfo("sent", 2, 2);
            numSigUSR2++;
        }
    }

    if (getpid() == node3) 
    {
        struct sigaction sa;
        sa.sa_handler = Node3Handler;
        if (sigaction(SIGUSR1, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        struct sigaction saterm;
        saterm.sa_handler = Node3458TermHandler;
        if (sigaction(SIGTERM, &saterm, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }
    }

    if (getpid() == node4) 
    {
        struct sigaction sa;
        sa.sa_handler = Node4Handler;
        if (sigaction(SIGUSR1, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        struct sigaction saterm;
        saterm.sa_handler = Node3458TermHandler;
        if (sigaction(SIGTERM, &saterm, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

    }

    if (getpid() == node5) 
    {
        struct sigaction sa;
        sa.sa_handler = Node5Handler;
        if (sigaction(SIGUSR2, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        struct sigaction saterm;
        saterm.sa_handler = Node3458TermHandler;
        if (sigaction(SIGTERM, &saterm, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }
    }

    if (getpid() == node6) 
    {
        struct sigaction sa;
        sa.sa_handler = Node6Handler;
        if (sigaction(SIGUSR2, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        struct sigaction saterm;
        saterm.sa_handler = Node6TermHandler;
        if (sigaction(SIGTERM, &saterm, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        if (kill(node7, SIGUSR1) == -1)
            perror("kill");
        else 
        {
            ShowInfo("sent", 1, 6);
            numSigUSR1++;
        }
    }

    if (getpid() == node7) 
    {
        struct sigaction sa;
        sa.sa_handler = Node7Handler;
        if (sigaction(SIGUSR1, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        struct sigaction saterm;
        saterm.sa_handler = Node7TermHandler;
        if ( sigaction(SIGTERM, &saterm, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        if (kill(node8, SIGUSR1) == -1)
            perror("kill");
        else 
        {
            ShowInfo("sent", 1, 7);
            numSigUSR1++;
        }
    }

    if (getpid() == node8) 
    {
        struct sigaction sa;
        sa.sa_handler = Node8Handler;
        if (sigaction(SIGUSR1, &sa, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        struct sigaction saterm;
        saterm.sa_handler = Node3458TermHandler;
        if (sigaction(SIGTERM, &saterm, NULL) == -1)
        {
            perror("sigaction");
            exit(-1);
        }

        if (kill(node1, SIGUSR2) == -1)
            perror("kill");
        else
        {
            ShowInfo("sent", 2, 8);
            numSigUSR2++;
        }
    }

    if (getpid() == node0)
    {
        while (1);
    }
}

void getTime () {
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

void ShowInfo (char *action, int signal, int numNode) 
{
    getTime();
    printf("№%d PID: %d Parent: %d %s SIGUSR%d Time: %02d:%02d:%02d:%03d\n",numNode,getpid(),getppid(),action,signal,daytime[0],daytime[1],daytime[2],daytime[3]);
}

void Node1Handler(int sig)
{
    ++getSig1;
    ShowInfo ("got", 2, 1);
    if (getSig1 == TOTAL_SIGNALS)
    {
        sleep(1);
        if (kill(-pidgroup1, SIGTERM) == -1 )
        {
            perror("KILL SIGTERM");
        };

        int children = 3; 
        while (children--)
            wait(NULL);

        printf("PID: %d Parent: %d finished work with amount of signals = %d\n", getpid(), getppid(), getSig1);
        // if (kill(getpid(), SIGKILL) == -1)
        // {
        //     perror("KILL SIGKILL");
    
        // };
        exit(0);
    }
    else
    {
        if (kill(-pidgroup1, SIGUSR1) == -1)
        {
            perror("KILL");
    
        }
        ShowInfo ("sent", 1, 1);
    }
}

void Node2Handler(int sig)
{
    ShowInfo ("got", 1, 2);
    ++numSigUSR1;
    if (kill(-pidgroup2, SIGUSR2) == -1)
    {
        perror("KILL");
    }
    ShowInfo ("sent", 2, 2);
}

void Node3Handler(int sig)
{
    ShowInfo ("got", 1, 3);
    ++numSigUSR1;
}

void Node4Handler(int sig)
{
    ShowInfo ("got", 1, 4);
    ++numSigUSR1;
}

void Node5Handler(int sig)
{
    ShowInfo ("got", 2, 5);
    ++numSigUSR2;
}

void Node6Handler(int sig)
{
    ShowInfo ("got", 2, 6);
    ++numSigUSR2;
    if (kill(node7, SIGUSR1) == -1)
    {
        perror("KILL");
    }
    ShowInfo ("sent", 1, 6);
}

void Node7Handler(int sig)
{
    ShowInfo ("got", 1, 7);
    ++numSigUSR1;
    if (kill(node8, SIGUSR1) == -1)
    {
        perror("KILL");
    }
    ShowInfo ("sent", 1, 7);
}

void Node8Handler(int sig)
{
    ShowInfo ("got", 1, 8);
    ++numSigUSR1;
    if (kill(node1, SIGUSR1) == -1)
    {
        perror("KILL");
    }
    ShowInfo ("sent", 1, 8);
}

void Node2TermHandler(int sig)
{
    if (kill(-pidgroup2, SIGTERM) == -1)
    {
        perror("KILL SIGTERM");

    };
    if (waitpid(node5, NULL, 0) == -1 || waitpid(node6, NULL, 0) == -1)
    {
        perror("WAIT");

    };
    printf ("PID: %d  Parent: %d finished work with amount of signals SIGUSR1 = %d and SIGUSR2 = %d\n",getpid(),getppid(),numSigUSR1,numSigUSR2);
    // if (kill(getpid(), SIGKILL) == -1)
    // {
    //     perror("KILL SIGKILL");

    // };
    exit(0);
}

void Node3458TermHandler(int sig)
{
    printf ("PID: %d  Parent: %d finished work with amount of signals SIGUSR1 = %d and SIGUSR2 = %d\n",getpid(),getppid(),numSigUSR1,numSigUSR2);
    // if (kill(getpid(), SIGKILL) == -1)
    // {
    //     perror("KILL SIGKILL");

    // };
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
    printf ("PID: %d  Parent: %d finished work with amount of signals SIGUSR1 = %d and SIGUSR2 = %d\n",getpid(),getppid(),numSigUSR1,numSigUSR2);
    // if (kill(getpid(), SIGKILL) == -1)
    // {
    //     perror("KILL SIGKILL");

    // };
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
    printf ("PID: %d  Parent: %d finished work with amount of signals SIGUSR1 = %d and SIGUSR2 = %d\n",getpid(),getppid(),numSigUSR1,numSigUSR2);
    // if (kill(getpid(), SIGKILL) == -1)
    // {
    //     perror("KILL SIGKILL");
    // };
    exit(0);
}