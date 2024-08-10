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
#include <getopt.h>


// color value constants 
#define DEFAULT_COLOR  "\x1B[0m"
#define GREEN  "\x1B[32m"
#define BLUE  "\x1B[34m"

#define PATTERNLEN 256


void tree(char current_dir[], int depth, bool print_dash[], char pattern[]);
void printLine(int depth, bool last_element, bool print_dash[], int i);


int main(int argc, char **argv){

    char opts[] = "apP:";       // options string to be used by getopt()
    
    bool a_opt = 0;             // -a option
    bool p_opt = 0;             // -p option
    bool P_opt = 0;             // -P option
    char pattern[PATTERNLEN];   // pattern

    int dir_index = 0;

    int x;
    while ((x = getopt(argc, argv, opts)) != -1) {
        switch (x) {
            case 'a':
                a_opt = 1;
                dir_index = optind;
                break;
            case 'p':
                p_opt = 1;
                dir_index = optind;
                break;
            case 'P':
                P_opt = 1;
                strcpy(pattern, optarg);
                dir_index = optind;
                break;
            default:
                fprintf(stderr, "Usage: %s [-a] [-p] [-P pattern] [directories]\n", argv[0]);
                return 1;
        }
    }

    bool print_dash[256];
    for (int i=0; i<256; i++){
        print_dash[i] = 0;
    }

    if (dir_index == 0){  // no arguments were given
        if (argc == 1){
            printf(".\n");
            tree(".", 1, print_dash, pattern);
            printf("\n\n");
        } else{
            dir_index = 1;
        }
    }else if (dir_index >= argc){
        printf(".\n");
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