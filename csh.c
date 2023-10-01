#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <ctype.h>
#include "word.h"
#include "csh.h"

#define BUFF_SIZE 1024
#define TOTAL_COMMANDS 4

int print_error(char *error) {
    printf("Error: %s\n", error);
    return 1;
}

void print_prompt() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("[user@csh][%s] ", cwd);
}

char *get_command(char *input) {
    int BUFF = 128, spaces, i;
    int size = strlen(input);
    char *command = malloc(size * sizeof(char));
    bool flag = false;

    for (spaces=0; spaces<strlen(input); spaces++) if (input[spaces] != ' ') break;

    for (i=spaces; i<strlen(input); i++) {
        if ((char)input[i] == ' ' || (char)input[i] == '\n') break;
        command[i-spaces] = input[i];
    }
    command[i-spaces] = '\0';
    
    return command;
}

char **get_args(char *input) {
    char *delim = " ";
    int BUFF = 128;
    char **args = malloc(BUFF * sizeof(char));
    char *tok = strtok(input, delim);

    int idx = 1;
    while (tok != NULL) {
        if (idx == BUFF) {
            BUFF += BUFF;
            args = realloc(args, BUFF * sizeof(char));
        }
        tok = strtok(NULL, " \n\t");
        args[idx] = tok;
        idx++;
    }
    args[--idx] = NULL; 
    args[0] = (idx-1);

    return args;
}

bool is_valid_command(char *command) {
    char *valid_commands[TOTAL_COMMANDS] = {"word", "dir", "date", ""};
    bool found = false;

    for (int i=0; i<TOTAL_COMMANDS; i++) if (!strcmp(command, valid_commands[i])) found = true;

    return found;
}

int execute(char *command, char **args) {
    if (!strcmp(command, "word")) return word(args);
    else if (!strcmp(command, "")) return 0;
    else {
        int child = fork();
        int child_status = 0;

        if (child < 0) {
            print_error("Failed to create a child process.");
            return -1;
        } else if (child == 0) {
            args[0] = command;
            char path[256];
            snprintf(path, sizeof(path), "bin/%s", command);
            if (execv(path, args) == -1) return print_error("exec() error.");
        } else if (child > 0) {
            int child_pid = wait(&child_status);
        }

        return child_status;
    }
}

int main(int argc, char **argv) {
    while (1) {
        print_prompt();
        char input[BUFF_SIZE];
        fgets(input, BUFF_SIZE, stdin);
        char *command = get_command(input);
        char **args = get_args(input);
        if (!strcmp(command, "exit")) break;
        if (is_valid_command(command)) execute(command, args);
        else print_error("Not a valid command.");
    }
    return 0;
}