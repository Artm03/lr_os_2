#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define MAX_BUF 65534

int main() {
    char* fname1;
    char* fname2;
    fname1 = malloc(MAX_BUF);
    if (read(0, fname1, MAX_BUF) < 0) {
        perror("ERROR\n");
        return 1;
    }
    int l1 = strlen(fname1);
    fname1[l1 - 1] = '\0';
    fflush(stdout);
    int f1 = open(fname1, O_CREAT|O_WRONLY|O_APPEND, S_IRWXU);
    if (f1 < 0){
		perror("Problems with file\n");
		return 1;
	}
    fname2 = malloc(MAX_BUF);
    if (read(0, fname2, MAX_BUF) < 0) {
        close(f1);
        perror("ERROR\n");
        return 1;
    }
    int l2 = strlen(fname2);
    fname2[l2 - 1] = '\0';
    fflush(stdout);
    if (strcmp(fname1, fname2) == 0) {
        close(f1);
        perror("The files have the same names\n");
		return 1;
    }    
    int f2 = open(fname2, O_CREAT|O_WRONLY|O_APPEND, S_IRWXU);
    if (f2 < 0){
        close(f1);
		perror("Problems with file\n");
		return 1;
	}
    int pipe1[2];
    int pipe2[2];
    pipe(pipe1);
    pipe(pipe2);
    int pid1 = fork();
    if (pid1 == -1) {
        close(f1);
        close(f2);
        perror("Fork error\n");
        return 1;
    }
    if (pid1 != 0) {
        int pid2 = fork();
        if (pid2 == -1) {
            close(f1);
            close(f2);
            perror("Fork error\n");
            return 1;
        }
        if (pid2 != 0) {
            char c = ' ';
            int cnt_str = 0;
            while (c != EOF) {
                char* str;
                str = malloc(MAX_BUF);
                int i = 0;
                c = ' ';
                bool is_eof = false;
                while (c != '\n') {
                    if (read(0, &c, sizeof(char)) < 1){
                        is_eof = true;
                        break;
                    }
                    str[i] = c;
                    i++;
                }
                cnt_str++;
                int l = strlen(str); 
                str[l] = '\0';
                if (cnt_str % 2 == 1) {
                    write(pipe1[1], str, sizeof(char) * strlen(str));
                }
                if (cnt_str % 2 == 0) {
                    write(pipe2[1], str, sizeof(char) * strlen(str));
                }
                if (is_eof) {
                    break;
                }
            }
        }   
        else {
            close(pipe2[1]);
            if (dup2(f2, 1) < 0) {
                close(f1);
                close(f2);
                perror("Can't dup child.out to stdout");
                return 2;
            }
            if (dup2(pipe2[0], 0) < 0) {
                close(f1);
                close(f2);
                perror("Can't dup pipe1[1] to stdin");
                return 2;
            }
            if (execl("child.out", "", NULL) == -1) {
                close(f1);
                close(f2);
                perror("Execl child problem");
                return 2;
            }
        }         
    }
    else {
        close(pipe1[1]);
        if (dup2(f1, 1) < 0) {
            close(f1);
            close(f2);
            perror("Can't dup child.out to stdout");
            return 2;
        }
        if (dup2(pipe1[0], 0) < 0) {
            close(f1);
            close(f2);
            perror("Can't dup pipe1[1] to stdin");
            return 2;
        }
        if (execl("child.out", "", NULL) == -1) {
            close(f1);
            close(f2);
            perror("Execl child problem");
            return 2;
        }
    }
    close(f1);
    close(f2);
    return 0;
}