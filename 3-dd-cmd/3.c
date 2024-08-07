#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <errno.h>

#define BUFFSIZE 64

void readFile(char f_in[], char content[], int size);
void dd(char content[], int fd[2]);
void writeFile(char f_out[], char content[]);


int main(int argc, char **argv){

    char f_in[32];
    char f_out[32];

    strcpy(f_in,  argv[1]);
    strcpy(f_out, argv[2]);
    
    //char s[32]     = argv[3];

    char content[BUFFSIZE];
    readFile(f_in, content, BUFFSIZE);

	int fd[2];
	int res = pipe(fd);
	if (res<0){
		printf("Pipe not created.\n");
		exit(100);
	}

	__pid_t pid = fork();
	if (pid < 0){
        exit(100);
		printf("Fork failed.\n");
	}else if (pid == 0){
		//printf("I am a child process\n");
    	dd(content, fd);
	}


	close(fd[1]);
	char result[BUFFSIZE];
	read(fd[0], result, BUFFSIZE-1);
	//printf("I got the result from child process: %s\n", result);
	wait(NULL);
	close(fd[0]);

    
    writeFile(f_out, result);

    
    return 0;
}


void readFile(char f_in[], char content[], int size){
    FILE *f_read;
    f_read  = fopen(f_in, "r");
    if (f_read==NULL){
        printf("Unable to read file %s becasue of %s\n", f_in, strerror(errno));
        exit(20);
    }
    
    char read_char;
    int index = 0;

    while (1){
        for (int i=0; i<4; i++){
            read_char = fgetc(f_read);
            if (read_char==EOF || read_char=='\0'){
                content[index] = '\0';
                return;
            }
            if (i==1 || i==2){
                if ((65<=read_char && read_char<=70) || (97<=read_char && read_char<=102) || (48<=read_char && read_char<=57)){
                    content[index] = read_char;
                    index++;
                }else{
                    fprintf(stderr, "Wrong format for input binary file %s\n", f_in);
                    exit(30);
                }
            }
        }
    }


}


void dd(char content[], int fd[2]){
    
    close(fd[0]);
    dup2(fd[1], STDOUT_FILENO);

    char command[64];
    sprintf(command, "echo %s | dd status=none", content);
    execl("/bin/sh", "sh", "-c", command, (char *)NULL);
    
    close(fd[1]);	
    exit(0);
    
    return;
}


void writeFile(char f_out[], char old_content[]){
    FILE *f_write;
    f_write = fopen(f_out, "w");
    if (f_write==NULL){
        printf("Unable to read file %s becasue of %s\n", f_out, strerror(errno));
        exit(20);
    }

    char new_content[BUFFSIZE];
    int  old_index = 0;
    int  new_index = 0;

    while(1){
        new_content[new_index] = 'F';
        new_index++;
        for (int i=0; i<2; i++){
            new_content[new_index] = old_content[old_index];
            new_index++;
            old_index++;
        }
        new_content[new_index] = 'F';
        new_index++;

        if (old_content[old_index] == '\0' || old_content[old_index] == '\n'){
            new_content[new_index] = old_content[old_index];
            break;
        }
    }

    fprintf(f_write, "%s", new_content);
    fclose(f_write);

    return;
}

