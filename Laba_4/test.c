#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>

char *module;
int s1Count = 0, s2Count = 0;
pid_t pid0 = 0, pid1 = 0, pid2 = 0, pid3 = 0, pid4 = 0, pid5 = 0, pid6 = 0, pid7 = 0, pid8 = 0;

void printErr(const char *module, const char *errmsg, const char *filename) {
    fprintf(stderr, "%d %s: %s %s\n", getpid(), module, errmsg, filename ? filename : "");
}

long getTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_usec) % 1000;
}

// Gettign current time
void getCurrTime() {
    struct timeval te;
    errno = 0;
    gettimeofday(&te, NULL);
    if (errno != 0) {
        perror("gettimeofday");
    }

    long msec = te.tv_usec / 1000;
    long sec = te.tv_sec % 60;
    long min = (te.tv_sec / 60) % 60;
    long hours = (te.tv_sec / 3600 + 3) % 24;

    printf("time is: %02ld:%02ld:%02ld:%03ld\n", hours, min, sec, msec);
}

void getInfo(int childNum, char *action, int sigNum) {
    printf("%d %5d %5d %4s SIGUSR%d ", childNum, getpid(), getppid(), action, sigNum);
    getCurrTime();
}

void terminated(int childNum) {
    printf("%d %4d %4d terminated after %3d SIGUSR1 and %3d SIGUSR2\n", childNum, getpid(), getppid(), s1Count, s2Count);
}

void handler1() {
    static int received = 0;
    received++;

    //printf("1 %d %d РїРѕР»СѓС‡РёР» USR1 %ld\n", getpid(), getppid(), getTime());
    getInfo(1, "get", 1);

    if (received == 101) {
        sleep(1);
        if (kill(-pid2, SIGTERM) == -1)
            //printErr(module, strerror(errno), NULL);
            perror("kill");
        int children = 4; // process 2, 3, 4, 5
        while (children--)
            wait(NULL);
        //printf("%d %d Р·Р°РІРµСЂС€РёР» СЂР°Р±РѕС‚Сѓ РїРѕСЃР»Рµ %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR1 Рё %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR2\n", getpid(), getppid(), s1Count, s2Count);
        terminated(1);
        exit(0);
    } else {
        if (kill(-pid2, SIGUSR2) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("kill");
        } else {
            //printf("1 %d %d РїРѕСЃР»Р°Р» USR2 %ld\n", getpid(), getppid(), getTime());
            getInfo(1, "sent", 2);
        }
        s2Count++;
    }
}

void handler2() {
    //printf("2 %d %d РїРѕР»СѓС‡РёР» USR2 %ld\n", getpid(), getppid(), getTime());
    getInfo(2, "get", 2);
    if (kill(pid6, SIGUSR1) == -1)
        //printErr(module, strerror(errno), NULL);
        perror("kill");
    else
        //printf("2 %d %d РїРѕСЃР»Р°Р» USR1 %ld\n", getpid(), getppid(), getTime());
        getInfo(2, "sent", 1);
    s1Count++;
}

void handler3() {
    //printf("3 %d %d РїРѕР»СѓС‡РёР» USR2 %ld\n", getpid(), getppid(), getTime());
    getInfo(3, "get", 2);
    if (kill(pid7, SIGUSR1) == -1)
        //printErr(module, strerror(errno), NULL);
        perror("kill");
    else
        //printf("2 %d %d РїРѕСЃР»Р°Р» USR1 %ld\n", getpid(), getppid(), getTime());
        getInfo(3, "sent", 1);
    s1Count++;
}

void handler4() {
    //printf("4 %d %d РїРѕР»СѓС‡РёР» USR2 %ld\n", getpid(), getppid(), getTime());
    getInfo(4, "get", 2);
    if (kill(pid8, SIGUSR1) == -1)
        //printErr(module, strerror(errno), NULL);
        perror("kill");
    else
        //printf("2 %d %d РїРѕСЃР»Р°Р» USR1 %ld\n", getpid(), getppid(), getTime());
        getInfo(4, "sent", 1);
    s1Count++;
}

void handler5() {
    //printf("5 %d %d РїРѕР»СѓС‡РёР» USR2 %ld\n", getpid(), getppid(), getTime());
    getInfo(5, "get", 2);
}

void handler6() {
    //printf("6 %d %d РїРѕР»СѓС‡РёР» USR1 %ld\n", getpid(), getppid(), getTime());
    getInfo(6, "get", 1);
}

void handler7() {
    //printf("7 %d %d РїРѕР»СѓС‡РёР» USR1 %ld\n", getpid(), getppid(), getTime());
    getInfo(7, "get", 1);
}

void handler8() {
    //printf("8 %d %d РїРѕР»СѓС‡РёР» USR1 %ld\n", getpid(), getppid(), getTime());
    getInfo(8, "get", 1);
    if (kill(pid1, SIGUSR1) == -1)
        //printErr(module, strerror(errno), NULL);
        perror("kill");
    else
        //printf("8 %d %d РїРѕСЃР»Р°Р» USR1 %ld\n", getpid(), getppid(), getTime());
        getInfo(8, "get", 1);
    s1Count++;
}

void handlerTerm2() {
    if (kill(pid6, SIGTERM) == -1) {
        perror("kill");
    }
    wait(NULL);

    //printf("2 %d %d Р·Р°РІРµСЂС€РёР» СЂР°Р±РѕС‚Сѓ РїРѕСЃР»Рµ %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR1 Рё %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR2\n", getpid(), getppid(), s1Count, s2Count);
    terminated(2);
    exit(0);
}

void handlerTerm3() {
    if (kill(pid7, SIGTERM) == -1) {
        perror("kill");
    }
    wait(NULL);

    //printf("3 %d %d Р·Р°РІРµСЂС€РёР» СЂР°Р±РѕС‚Сѓ РїРѕСЃР»Рµ %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR1 Рё %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR2\n", getpid(), getppid(), s1Count, s2Count);
    terminated(3);
    exit(0);
}

void handlerTerm4() {
    if (kill(pid8, SIGTERM) == -1) {
        perror("kill");
    }
    wait(NULL);

    //printf("4 %d %d Р·Р°РІРµСЂС€РёР» СЂР°Р±РѕС‚Сѓ РїРѕСЃР»Рµ %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR1 Рё %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR2\n", getpid(), getppid(), s1Count, s2Count);
    terminated(4);
    exit(0);
}

void handlerTerm5() {
    /*if (kill(-pid2, SIGTERM) == -1)
        printErr(module, strerror(errno), NULL);
    int children = 3;
    while (children--)
        wait(NULL);*/
    //printf("5 %d %d Р·Р°РІРµСЂС€РёР» СЂР°Р±РѕС‚Сѓ РїРѕСЃР»Рµ %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR1 Рё %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR2\n", getpid(), getppid(), s1Count, s2Count);
    terminated(5);
    exit(0);
}

void handlerTerm6() {
    //printf("6 %d %d Р·Р°РІРµСЂС€РёР» СЂР°Р±РѕС‚Сѓ РїРѕСЃР»Рµ %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR1 Рё %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR2\n", getpid(), getppid(), s1Count, s2Count);
    terminated(6);
    exit(0);
}

void handlerTerm7() {
    //printf("7 %d %d Р·Р°РІРµСЂС€РёР» СЂР°Р±РѕС‚Сѓ РїРѕСЃР»Рµ %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR1 Рё %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR2\n", getpid(), getppid(), s1Count, s2Count);
    terminated(7);
    exit(0);
}

void handlerTerm8() {
    /*if (kill(-pid5, SIGTERM) == -1)
        printErr(module, strerror(errno), NULL);
    int children = 2;
    while (children--)
        wait(NULL);*/
    //printf("8 %d %d Р·Р°РІРµСЂС€РёР» СЂР°Р±РѕС‚Сѓ РїРѕСЃР»Рµ %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR1 Рё %d-РіРѕ СЃРёРіРЅР°Р»Р° SIGUSR2\n", getpid(), getppid(), s1Count, s2Count);
    terminated(8);
    exit(0);
}

void createFile(char N) {
    char *filename = (char *) malloc(16);
    sprintf(filename, "/tmp/lab4/%c.pid", N);
    FILE *f = NULL;
    f = fopen(filename, "w");
    if (f == NULL) {
        //printErr(module, strerror(errno), filename);
        perror("fopen");
        return;
    }
    fprintf(f, "%d", getpid());
    errno = 0;
    fclose(f);
    if (errno != 0) {
        perror("fclose");
    }

    free(filename);
}

pid_t getPidFromFile(char N) {
    char *filename = (char *) malloc(16);
    sprintf(filename, "/tmp/lab4/%c.pid", N);
    pid_t result = 0;

    while (result == 0) {
        FILE *f = NULL;
        while ((f = fopen(filename, "r")) == NULL);
        fscanf(f, "%d", &result);
        fclose(f);
    }

    free(filename);
    return result;
}

// TODO: done
void createProcessTree() {
    pid0 = getpid();

    pid1 = fork();

    // Creating 1->2
    if (!pid1) {
        pid1 = getpid();
        printf("Process 0 with pid %d creates process 1 with pid %d\n", pid0, pid1);
        pid2 = fork();
        if (!pid2) {
            pid2 = getpid();
            printf("Process 1 with pid %d creates process 2 with pid %d\n", pid1, pid2);
        }

        else if (pid2 == -1)
            perror("fork");
    }
    else if (pid1 == -1) {
        perror("fork");
    }

    if (pid2 == getpid()) {

        // create process 3
        pid3 = fork();
        if (!pid3) {
            pid3 = getpid();
            printf("Process 2 with pid %d creates process 3 with pid %d\n", pid2, pid3);
        }
        else if (pid3 == -1)
            perror("fork");
    }

    if (pid2 == getpid()) {

        // create process 4
        pid4 = fork();
        if (!pid4) {
            pid4 = getpid();
            printf("Process 2 with pid %d creates process 4 with pid %d\n", pid2, pid4);
        }
        else if (pid4 == -1)
            perror("fork");
    }

    if (pid2 == getpid()) {

        // create process 5
        pid5 = fork();
        if (!pid5) {
            pid5 = getpid();
            printf("Process 2 with pid %d creates process 5 with pid %d\n", pid2, pid5);
        }
        else if (pid5 == -1)
            perror("fork");
    }

    // if pid4 is created than creating 6
    if (pid4 == getpid()) {
        pid6 = fork();
        if (!pid6) {
            pid6 = getpid();
            printf("Process 4 with pid %d creates process 6 with pid %d\n", pid4, pid6);
        }
        else if (pid6 == -1)
            perror("fork");
    }

    // if pid3 is created than creating 7
    if (pid3 == getpid()) {
        pid7 = fork();
        if (!pid7) {
            pid7 = getpid();
            printf("Process 3 with pid %d creates process 7 with pid %d\n", pid3, pid7);
        }
        else if (pid7 == -1)
            perror("fork");
    }

    // if pid5 is created than creating 8
    if (pid5 == getpid()) {
        pid8 = fork();
        if (!pid8) {
            pid8 = getpid();
            printf("Process 5 with pid %d creates process 8 with pid %d\n", pid5, pid8);
        }
        else if (pid8 == -1)
            perror("fork");
    }
}

void exchangeSignals(const char *dirname) {
    struct sigaction sg;
    sigemptyset(&sg.sa_mask);
    sg.sa_flags = SA_SIGINFO;

    struct sigaction sgTerm;
    sigemptyset(&sgTerm.sa_mask);
    sgTerm.sa_flags = SA_SIGINFO;

    if (getpid() == pid1) {
        createFile('1');

        const int required = 8;
        int curr = 0;
        // Wait while all child create pid file
        while (curr != required) {
            curr = 0;
            DIR *currdir;
            if (!(currdir = opendir(dirname))) {
                //printErr(module, strerror(errno), dirname);
                perror("opendir");
                exit(1);
            }

            struct dirent *cdirent;
            errno = 0;

            while ((cdirent = readdir(currdir))) {
                if (!strcmp(".", cdirent->d_name) || !strcmp("..", cdirent->d_name)) {
                    continue;
                }
                curr++;
            }
            if (errno)
                //printErr(module, strerror(errno), dirname);
                perror("");
            if (closedir(currdir) == -1)
                //printErr(module, strerror(errno), dirname);
                perror("closedir");

        }
        pid2 = getPidFromFile('2');
        pid3 = getPidFromFile('3');
        pid4 = getPidFromFile('4');
        pid5 = getPidFromFile('5');

        sg.sa_sigaction = handler1;
        if (sigaction(SIGUSR1, &sg, NULL) == -1) {
            perror("sigaction");
            exit(1);
        }
        // Print process tree
        char buffer[256];
        sprintf(buffer, "pstree -p %d", getppid());
        system(buffer);

        // Sent first signal to process group (2, 3, 4, 5)
        if (kill(-pid2, SIGUSR2) == -1)
            //printErr(module, strerror(errno), NULL);
            perror("kill");
        else
            //printf("1 %d %d РїРѕСЃР»Р°Р» USR1 %ld\n", getpid(), getppid(), getTime());
            getInfo(1, "sent", 1);
        s1Count++;
        //pid2 = getPidFromFile('2');
    }

//    debug();
    if (getpid() == pid2) {
        sg.sa_sigaction = handler2;
        if (sigaction(SIGUSR2, &sg, NULL) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        sgTerm.sa_sigaction = handlerTerm2;
        if (sigaction(SIGTERM, &sgTerm, NULL) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        pid6 = getPidFromFile('6');
        if (setpgid(pid2, pid2) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        createFile('2');
    }

    if (getpid() == pid3) {
        sg.sa_sigaction = handler3;
        if (sigaction(SIGUSR2, &sg, NULL) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(1);
        }
        sgTerm.sa_sigaction = handlerTerm3;
        if (sigaction(SIGTERM, &sgTerm, NULL) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        // Getting this pids from file to send signals
        pid2 = getPidFromFile('2');
        pid7 = getPidFromFile('7');
        if (setpgid(pid3, pid2) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        createFile('3');
    }

    if (getpid() == pid4) {
        sg.sa_sigaction = handler4;
        if (sigaction(SIGUSR2, &sg, NULL) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        sgTerm.sa_sigaction = handlerTerm4;
        if (sigaction(SIGTERM, &sgTerm, NULL) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        pid2 = getPidFromFile('2');
        pid8 = getPidFromFile('8');
        if (setpgid(pid4, pid2) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }

        createFile('4');
    }

    if (getpid() == pid5) {
        sg.sa_sigaction = handler5;
        if (sigaction(SIGUSR2, &sg, NULL) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        sgTerm.sa_sigaction = handlerTerm5;
        if (sigaction(SIGTERM, &sgTerm, NULL) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        pid2 = getPidFromFile('2');
        if (setpgid(pid5, pid2) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        createFile('5');
    }


    if (getpid() == pid6) {
        sg.sa_sigaction = handler6;
        if (sigaction(SIGUSR1, &sg, NULL) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        sgTerm.sa_sigaction = handlerTerm6;
        if (sigaction(SIGTERM, &sgTerm, NULL) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }

        //pid5 = getPidFromFile('5');
        if (setpgid(pid6, pid6) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }

        createFile('6');
    }

    if (getpid() == pid7) {
        sg.sa_sigaction = handler7;
        if (sigaction(SIGUSR1, &sg, NULL) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        sgTerm.sa_sigaction = handlerTerm7;
        if (sigaction(SIGTERM, &sgTerm, NULL) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        if (setpgid(pid7, pid7) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        createFile('7');
    }

    if (getpid() == pid8) {
        sg.sa_sigaction = handler8;
        if (sigaction(SIGUSR1, &sg, NULL) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        sgTerm.sa_sigaction = handlerTerm8;
        if (sigaction(SIGTERM, &sgTerm, NULL) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        pid1 = getPidFromFile('1');
        if (setpgid(pid8, pid8) == -1) {
            //printErr(module, strerror(errno), NULL);
            perror("sigaction");
            exit(-1);
        }
        //pid5 = getPidFromFile('5');
        createFile('8');
    }

    if (getpid() == pid0) {
        wait(NULL);

        DIR *currdir;
        if (!(currdir = opendir(dirname))) {
            //printErr(module, strerror(errno), dirname);
            perror("opendir");
            exit(-1);
        }

        struct dirent *cdirent;
        errno = 0;

        while ((cdirent = readdir(currdir))) {
            if (!strcmp(".", cdirent->d_name) || !strcmp("..", cdirent->d_name)) {
                continue;
            }
            char *filename = (char *) malloc(strlen(dirname) + 7);
            sprintf(filename, "%s/%s", dirname, cdirent->d_name);
            if (remove(filename) == -1) perror("remove");
            free(filename);
        }
        if (errno)
            //printErr(module, strerror(errno), dirname);
            perror("");
        if (closedir(currdir) == -1)
            //printErr(module, strerror(errno), dirname);
            perror("closedir");

        if (rmdir(dirname) == -1) {
            //printErr(module, strerror(errno), dirname);
            perror("rmdir");
            exit(-1);
        }
    } else {
        while (1) pause();
    }
}


int main(int argc, char *argv[]) {
    module = basename(argv[0]);
    char *dirname = "/tmp/lab4";
    if (mkdir(dirname, 0777) == -1) {
        //printErr(module, strerror(errno), dirname);
        perror("mkdir");
        exit(-1);
    }
    createProcessTree();
    exchangeSignals(dirname);
    exit(0);
}