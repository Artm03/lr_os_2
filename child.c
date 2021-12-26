#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define MAX_BUF 65534

int main() {
    char c = ' ';
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
        int l = strlen(str); 
        str[l] = '\0';
        for (int i = 0; i <= (l - 2) / 2; i++) {
            char k = str[l - 2 - i];
            str[l - 2 - i] = str[i];
            str[i] = k;
        }
        write(1, str, sizeof(char) * strlen(str));
        if (is_eof) {
            break;
        } 
    }
    return 0;
}