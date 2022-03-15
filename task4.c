#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

int main (int argc,char* argv[]) {
	if (argc != 3) { 
		fputs("Invalid parameters\nFirst parameter - name of the file\nSecond parameter - number of lines\n",stderr);
		return -1;
	}
	else {
		int N = atoi(argv[2]);
		if (N < 0) {
			fputs("Invalid parameter\nSecond parameter must be above zero\n",stderr);
			return -1;
		}
		else {
			//strcat(argv[1],".txt");
			FILE *f;
			if ((f = fopen(argv[1],"r")) == NULL) 
			{
				fputs("Cannot open file\n",stderr);
				return -1;
			} else {
				int i = 0;
				while (!feof(f)) {
					if (N == 0) {
						char symbol = getc(f);
						if (symbol != EOF) putc(symbol,stdout);
					} else {
						if (i == N) { 
							char key = getc(stdin);
							while (key != '\n') 
								key = getc(stdin);
							i = 0;
						}
						char symbol = getc(f);
						if (symbol != EOF) putc(symbol,stdout);
						if (symbol == '\n') i++;
					}
				}
				putc('\n',stdout);
			}
			if (fclose(f)) {
				fputs("Cannot close file\n",stderr);
		        	return -1;
			} else {
				return 0;
			}
		}
	}
}
