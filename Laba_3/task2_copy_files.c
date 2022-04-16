#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

#define BUF_SIZE 1024

int CopyFile(const char *source_file, const char *destination_file)
{
	int infd, outfd;
	ssize_t bytes;

	infd = open(source_file, O_RDONLY);
	if (infd == -1)
	{
		fprintf(stderr, "Cannot open source file "
						"(%s)\n",
				source_file);
		return -1;
	}

	outfd = open(destination_file, O_WRONLY | O_CREAT | O_TRUNC, 0640);
	if (outfd == -1)
	{
		fprintf(stderr, "Cannot open destination file "
						"(%s)\n",
				destination_file);
		return -1;
	}
	char buffer[BUF_SIZE];
	while ((bytes = read(infd, buffer, BUF_SIZE)) > 0)
		write(outfd, buffer, bytes);
	close(infd);
	close(outfd);
	return 0;
}

int numOpenStreams = 0;

int Synchronization(char *dirPathSrc, char *dirPathDst, int numStreams)
{

	struct stat buf;
	char SrcPath[PATH_MAX], DestPath[PATH_MAX];

	int isExistFile = 0, isExistFolder = 0;

	int j = 0;

	DIR *dirSrc = opendir(dirPathSrc);
	if (!dirSrc)
	{
		fprintf(stderr, "%s: %s\n", dirPathSrc, strerror(errno));
		return -1;
	}

	for (struct dirent *dirInfoSrc = readdir(dirSrc); dirInfoSrc != NULL; dirInfoSrc = readdir(dirSrc))
	{
		isExistFile = 0;
		isExistFolder = 0;
		if (strcmp(dirInfoSrc->d_name, ".") == 0 || strcmp(dirInfoSrc->d_name, "..") == 0)
			continue;

		DIR *dirDst = opendir(dirPathDst);
		if (!dirDst)
		{
			fprintf(stderr, "%s: %s\n", dirPathDst, strerror(errno));
			return -1;
		}

		for (struct dirent *dirInfoDst = readdir(dirDst); dirInfoDst != NULL; dirInfoDst = readdir(dirDst))
		{

			if (strcmp(dirInfoDst->d_name, ".") == 0 || strcmp(dirInfoDst->d_name, "..") == 0)
				continue;

			if (strcmp(dirInfoSrc->d_name, dirInfoDst->d_name) == 0 && dirInfoSrc->d_type == DT_REG && dirInfoDst->d_type == DT_REG)
			{
				isExistFile = 1;
				break;
			}
			else if (strcmp(dirInfoSrc->d_name, dirInfoDst->d_name) == 0 && dirInfoSrc->d_type == DT_DIR && dirInfoDst->d_type == DT_DIR)
			{
				isExistFolder = 1;
				break;
			}
		}
		closedir(dirDst);

		if (dirInfoSrc->d_type == DT_DIR)
		{

			char SrcPath[PATH_MAX], DestPath[PATH_MAX];

			strcpy(SrcPath, dirPathSrc);
			strcat(SrcPath, "/");
			strcat(SrcPath, dirInfoSrc->d_name);

			strcpy(DestPath, dirPathDst);
			strcat(DestPath, "/");
			strcat(DestPath, dirInfoSrc->d_name);

			if (isExistFolder == 1)
			{
				Synchronization(SrcPath, DestPath, numStreams);
			}
			else
			{
				if (stat(SrcPath, &buf) == -1)
				{
					fputs("Unable to fill stat structure\n", stderr);
					return -1;
				}
				if (mkdir(DestPath, buf.st_mode) != 0)
				{
					perror("mkdir");
				}
				else
				{
					Synchronization(SrcPath, DestPath, numStreams);
				}
			}
		}

		if (dirInfoSrc->d_type == DT_REG && isExistFile == 0)
		{

			if (numOpenStreams >= numStreams)
			{
				int status = 0;
				int child = wait(&status);
				if (child == -1)
				{
					perror("Error in wait1");
				}
				else
				{
					--numOpenStreams;
				}
			}

			int child = fork();
			switch (child)
			{
			case -1:
				perror("Error in fork");
				continue;
			case 0:
				strcpy(SrcPath, dirPathSrc);
				strcat(SrcPath, "/");
				strcat(SrcPath, dirInfoSrc->d_name);

				strcpy(DestPath, dirPathDst);
				strcat(DestPath, "/");
				strcat(DestPath, dirInfoSrc->d_name);

				if (CopyFile(SrcPath, DestPath) != 0)
				{
					puts("Error of function copy.");
					return -1;
				}

				int file_o = open(SrcPath, O_RDONLY);
				if (fstat(file_o, &buf))
				{
					fputs("Unable to fill stat structure\n", stderr);
					return -1;
				}
				close(file_o);
				printf("PID of stream: %d File name: %s File size: %ld bytes\n", getpid(), dirInfoSrc->d_name, buf.st_size);

				exit(EXIT_SUCCESS);
			default:
				++numOpenStreams;	
			}
		}
	}
	closedir(dirSrc);
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		puts("Not enougth parameters:\nFirst parameter - directory from which the files will be copied\nSecond parameter - directory where the files will be copied to");
		return -1;
	}

	char *dirPathSrc = argv[1];
	char *dirPathDst = argv[2];

	int numStreams = 0;
	do
	{
		printf("Enter the maximum number of threads: ");
		scanf("%d", &numStreams);
		if (numStreams <= 0)
		{
			puts("The number of threads must be bigger than 0\n");
		}
	} while (numStreams <= 0);

	if (Synchronization(dirPathSrc, dirPathDst, numStreams) == 0)
	{
		printf("%d\n", numOpenStreams);
		while (numOpenStreams > 0)
		{
			int status;
			int child = wait(&status);
			if (child == -1)
			{
				printf("Parent: %d Child: %d %d\n", getpid(),getppid(), numOpenStreams);
				perror("Error in wait2");
			}
			else
			{
				--numOpenStreams;
			}
		}
		puts("Folders Synchronized!");
	}
	else
		puts("Error happend in Synchronization.");

	return 0;
}
