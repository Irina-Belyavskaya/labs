#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>

char buffer[32];
int n = 0, i = 0, j = 0;
pthread_mutex_t lock;

void calcTaylorRow(char *filepath) {
    FILE *f = fopen(filepath, "r+t");
    char buffer[32];
    fgets(buffer, 30, f);
    pthread_t id; 
    float item; 
    float sum = 0;
    while (fscanf(f, "%ld\t%f\n", &id, &item) != EOF)
        sum += item;
    fprintf(f, "Sum: %.15f", sum);
    fclose(f);
}

int fact(int x) {
    int res = 1;
    for (int i = 1; i <= x; i++)
        res *= i;
    return res;
}

float getTaylorItem(int n, int i, int k) {
    return pow(-1, k) * pow(2*3.14*i/n, 2*k+1) / fact(2*k+1);
}   

void *ThreadFunc()
{
    pthread_mutex_lock(&lock);
    FILE *f = fopen(buffer, "at");
    float TaylorItem = getTaylorItem(n, i, j);
    fprintf(f, "%ld\t%.15f\n", pthread_self(), TaylorItem);
    fclose(f);
    printf("Thread ID = %ld Taylor Item = %.15f\n",pthread_self(),TaylorItem);
    pthread_mutex_unlock(&lock);
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Error! Wrong number of params\n");
        return 1;
    }

    n = strtol(argv[1], NULL, 10); // from string to int
    if (n <= 0) {
        fprintf(stderr, "Error! Wrong 1st param\n");
        return 1;
    }
    
    int rowLen = strtol(argv[2], NULL, 10);
    if (rowLen <= 0) {
        fprintf(stderr, "Error! Wrong 2nd param\n");
        return 1;
    }

    if (pthread_mutex_init(&lock, NULL))
    {
        perror("Initialize mutex\n");
        return 1;
    }

    FILE *f;
    DIR* dir = opendir("results/");
    if (dir) {
        closedir(dir);
        system("rm -r results/");
    }
    system("mkdir results/");

    for (i = 0; i < n; i++) {

        const char *filepath = "results/res";
        char *num;
        asprintf(&num, "%d", i); // from int to string
        strcat(strcpy(buffer, filepath), num);
        strcat(buffer, ".txt");
        f = fopen(buffer, "w");
        fprintf(f, "Thread ID\t\tTaylor Item\n");
        fclose(f);

        pthread_t threads[rowLen];        

        for (j = 0; j < rowLen; j++) {

            int status = pthread_create(&threads[j], NULL, ThreadFunc,NULL);
            if (status != 0)
            {
                perror("Error creating thread");
            }
        }

        for (int k = 0; k < rowLen; k++)
        {
            int result = pthread_join(threads[k], NULL);
            if (result != 0) {
                perror("Eror joining");
            }
        }

        calcTaylorRow(buffer);
        
    }
    if (pthread_mutex_destroy(&lock))
    {
        perror("Destroy mutex\n");
        return 1;
    }

}