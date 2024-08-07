/*
A program implementing a simplified `tree` (`LC_ALL=C tree -n --charset=ascii`).  
*/


#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include <regex.h>


// color value constants 
#define DEFAULT_COLOR  "\x1B[0m"
#define GREEN  "\x1B[32m"
#define BLUE  "\x1B[34m"


void tree(char current_dir[], int depth, bool print_dash[], char pattern[]);
void printLine(int depth, bool last_element, bool print_dash[], int i);


int main(int argc, char **argv){
    
    /*
    for (int i=0; i<argc; i++){
        printf("Argument %d: %s\n", i, argv[i]);
    }
    */

    bool P_opt = 0;     // -P option
    char pattern[10];   // pattern
    
    bool p_opt = 0;     // -p option
    bool a_opt = 0;     // -a option
    
    int dir_index = 0;  // argv[dir_index] continene la prima directory

    int index = 1;
    while (argc>1){
        if (argv[index][0] == '-'){
            if (strcmp(argv[index],"-P")==0){
                P_opt = 1;
                sprintf(pattern, "%s", argv[index+1]);
            }else if (strcmp(argv[index],"-a")==0){
                a_opt = 1;
            }else if (strcmp(argv[index],"-p")==0){
                p_opt = 1;
            }else{
                exit(20); // a not allowed option was given 
            }
        }else if (strcmp(argv[index-1],"-P")!=0) {
            if (dir_index==0){
                dir_index = index;
            }
        }
        index++;
        if (index==argc){
            break;
        }
    }

    bool print_dash[100];
    for (int i=0; i<100; i++){
        print_dash[i] = 0;
    }

    if (dir_index == 0){  // no arguments were given
        tree(".", 1, print_dash, pattern);
        printf("\n\n");
    }else{
        DIR *d_check;
        for (int i=dir_index; i<argc; i++){
            d_check = opendir(argv[i]);
            if (d_check==NULL){
                printf("\n%s [error opening dir because of being not a dir]\n\n", argv[i]);
                exit(10);  // comment this line if you want to consider the other directories instead
            }else{
                printf("%s\n", argv[i]);
                tree(argv[i], 1, print_dash, pattern);
                printf("\n\n");
            }
        }
    }

    printf("Options: -P (%d), -p (%d), -a (%d)\n", P_opt, p_opt, a_opt);

    return 0;
}


void tree(char current_dir[], int depth, bool print_dash[], char pattern[]){

    regex_t regex;
    int value = regcomp( &regex, pattern, 0);

    bool last_element = false;
    
    DIR *d = opendir(current_dir); // open the path
    if (d == NULL){
        return;
    }else{
        
        struct dirent **namelist;
        int n;
        n = scandir(current_dir, &namelist, NULL, alphasort);

        struct dirent *dir;                // for the directory entries
        
        int i=0;
        while (i<n){
            if (i>=n-3){
                last_element = true;
            }else if (i==0){
                print_dash[depth-1] = 1;
            }

            dir = readdir(d);
            if (dir == NULL){
                break;
            }
        
            if (dir->d_type != DT_DIR) // if the type is not directory just print it with blue color
            {
                for (int i=0; i<depth; i++){
                    printLine(depth, last_element, print_dash, i);
                }

                value = regexec( &regex, dir->d_name, 0, NULL, 0);
                if (value != REG_NOMATCH && pattern[0]!='\0'){
                    printf("%s%s", GREEN, dir->d_name);
                    printf("%s\n", DEFAULT_COLOR, NULL);
                }else{
                    printf("%s%s\n", DEFAULT_COLOR, dir->d_name);
                }

                if (last_element==1){
                    print_dash[depth-1] = 0;
                }
            }
            else if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) // if it is a directory
            {
                for (int i=0; i<depth; i++){
                    printLine(depth, last_element, print_dash, i);
                }
                if (last_element==1){
                    print_dash[depth-1] = 0;
                }
                
                value = regexec( &regex, dir->d_name, 0, NULL, 0);
                if (value != REG_NOMATCH && pattern[0]!='\0'){
                    printf("%s%s", GREEN, dir->d_name);
                    printf("%s\n", DEFAULT_COLOR, NULL);
                }else{
                    printf("%s%s\n", DEFAULT_COLOR, dir->d_name);
                }


                char d_path[255];                     
                sprintf(d_path, "%s/%s", current_dir, dir->d_name);
                tree(d_path, depth+1, print_dash, pattern); // recall with the new path
            }else{
                continue;
            }
            i++;
        }
    }
    closedir(d); // finally close the directory
}


void printLine(int depth, bool last_element, bool print_dash[], int i){
    if (last_element == 1 && i == depth-1){
        printf("`-- ");
    }else if (print_dash[i] == 1){
        printf("|");
        if (i==depth-1){
            printf("-- ");
        }else{
            printf("   ");    
        } 
    }else{
        printf("    ");
    }
}