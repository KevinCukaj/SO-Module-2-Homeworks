#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#define BUFFSIZE 64

void readFile(char f_in[], char content[], int size);
void dd(int fd_child_to_father[2], int fd_father_to_child[2], char s[]);
void writeFile(char f_out[], char content[]);


int main(int argc, char **argv){

    if (argc != 4){
        fprintf(stderr, "Usage: %s file_in file_out dd_args\n", argv[0]);
        exit(10);
    }

    char f_in[32];
    char f_out[32];
    char s[32];

    strcpy(f_in,  argv[1]);
    strcpy(f_out, argv[2]);
    strcpy(s, argv[3]);

    char content[BUFFSIZE];
    readFile(f_in, content, BUFFSIZE);

	int fd_father_to_child[2];  // the two file descriptors needed for the pipe
	int fd_child_to_father[2];  // the two file descriptors needed for the pipe
    
	int res1 = pipe(fd_father_to_child); // creating pipe
	int res2 = pipe(fd_child_to_father); // creating pipe

    fcntl(fd_father_to_child[0], F_SETFL, O_NONBLOCK);  // VERY IMPORTANT (sets fd non-blocking)

	if (res1<0 || res2<0){
		printf("Pipe not created.\n");
		exit(100);
	}

	__pid_t pid = fork();
	if (pid < 0){
		printf("Fork failed.\n");
        exit(100);
	}else if (pid == 0){
		//printf("I am a child process\n");
    	dd(fd_child_to_father, fd_father_to_child, s);
	}

    close(fd_father_to_child[0]);
    close(fd_child_to_father[1]);
    

    write(fd_father_to_child[1], content, BUFFSIZE-1);
	char result[BUFFSIZE];
    read(fd_child_to_father[0], result, BUFFSIZE-1); // father is blocked until pipe is not empty 
	wait(NULL);  // get the exit status of the child process
    
    close(fd_father_to_child[1]);
	close(fd_child_to_father[0]);

    
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
                content[index] = '\n';
                content[index+1] = '\0';
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


void dd(int fd_child_to_father[2], int fd_father_to_child[2], char s[]){

    char result[BUFFSIZE];

    close(fd_father_to_child[1]);
    close(fd_child_to_father[0]);

    dup2(fd_child_to_father[1], STDOUT_FILENO);
    dup2(fd_father_to_child[0], STDIN_FILENO);

    int null_fd = open("/dev/null", O_WRONLY);
    dup2(null_fd, STDERR_FILENO);

    close(fd_father_to_child[0]);
    close(fd_child_to_father[1]);
    
    char *args[] = {"/bin/dd", s, "status=none", NULL};
    execvp(args[0], args);

    exit(0);

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

