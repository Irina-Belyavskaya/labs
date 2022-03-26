#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
int main (int argc,char *argv[]) {
	if (argc != 2) { 
		fputs("Invalid parameter\nOne parameter - name of the file\n",stderr);
		return -1;
	}  else {
		//strcat(argv[1],".txt");
		FILE *f;
		if ((f = fopen(argv[1],"w")) == NULL) 
		{
			fputs("Cannot open file\n",stderr);
			return -1;
		} else {
			char symbol = ~'/';
			
			while (symbol != '/') {
				if ((symbol = fgetc(stdin)) == EOF) {
					fputs("Cannot read symbol\n",stderr);
					return -1;
				} else if (symbol != '/') fputc(symbol,f);
			}  
			
			if (fclose(f)) {
				fputs("Cannot close file\n",stderr);
		        	return -1;
			} else return 0;
		}
	}
}
