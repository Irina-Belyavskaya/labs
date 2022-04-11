#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

void GetTime () {
	struct timeval tv;
	gettimeofday( &tv, NULL );
	int msecs = tv.tv_usec / 1000;
	int secs = tv.tv_sec % 60;
	int mins = (tv.tv_sec / 60) % 60;
	int hrs = (tv.tv_sec / 3600 + 3) % 24;
	printf("%02d:%02d:%02d:%03d\n",hrs, mins, secs, msecs );
}

int main() {
    pid_t pid1, pid2;
    int res1, res2;
    pid1 = fork();
    if (pid1 == 0)  {
        printf("\nCHILD №1:Это первый процесс-дочерний!\n");
        printf("CHILD №1: Мой PID = %d\n", getpid());
        printf("CHILD №1: PID моего родителя = %d\n",getppid());
        
        puts("Время CHILD №1 : \n");
        GetTime();
        
        sleep(6);
        
        printf("CHILD №1: Введите мой код возврата:");
        scanf(" %d",&res1);
        printf("CHILD №1: Выход!\n\n");
        
        exit(res1);
        
    } else if (pid1 == -1) {
    	perror("fork() error"); 
        exit(1); 
    } else {
    
    	    pid2 = fork();
	    if (pid2 == 0)   {
		printf("\nCHILD №2: Это второй процесс-дочерний!\n");
		printf("CHILD №2: Мой PID = %d\n", getpid());
		printf("CHILD №2: PID моего родителя = %d\n",getppid());
		
		puts("Время CHILD №2 : \n");
		GetTime();
		
		sleep(3);
		
		printf("CHILD №2: Введите мой код возврата:");
		scanf(" %d",&res2);
		printf("CHILD №2: Выход!\n\n");

		exit(res2);
		
	    } else if (pid2 == -1) {
	    	perror("fork() error"); 
		exit(1); 
	    } else {
	    	    printf("\nPARENT: Это процесс-родитель!\n");
		    printf("PARENT: Мой PID = %d\n", getpid());
		    printf("PARENT: PID моего первого потомка %d\n",pid1);
		    printf("PARENT: PID моего второго потомка %d\n\n",pid2);
		    
		    system ("ps -x");
		    puts("\n");
		    
		    waitpid(pid1,&res1,0);
	    	    printf("PARENT: Код возврата первого потомка:%d\n",WEXITSTATUS(res1));
		    
		    waitpid(pid2,&res2,0);
		    
		    printf("PARENT: Код возврата второго потомка:%d\n",WEXITSTATUS(res2));
		    printf("PARENT: Выход!\n");
	    }
     }

}

