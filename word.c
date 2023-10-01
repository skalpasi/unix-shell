#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "word.h"
#include "csh.h"

void print_word_help() {
    printf("Usage: word [OPTION] [FILE]\n");
    printf("Counts the number of words in a file.\n\n");
    printf("Options: -n    Ignores new line character.\n");
    printf("         -d    Prints the word difference between 2 files.\n");
    printf("         -h    Display this help message.\n\n");
    printf("Example: word -n file\n");
    printf("         word -d file1 file2\n");
}

int invalid_args_word() {
    return print_error("Invalid number of arguments. Use '-h' for help.");
}

int word_count(char *filename, bool ignore_newline) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: File '%s' not found. Use '-h' for help.\n", filename);
        return -1;
    }

    int wc = 0;
    int in_word = 0;

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == ' ' || ch == '\t' || ch == '\n') {
            if (!ignore_newline || (ignore_newline && ch != '\n')) {
                if (in_word) {
                    in_word = false;
                    wc++;
                }
            }
        } else in_word = true;
    }

    if (in_word) wc++;

    fclose(file);
    return wc;
}

int word(char **args) {
    if (args[0] == 0) {
        print_word_help();
        return 0;
    }

    bool ignore_newline = false;
    bool diff_mode = false;
    bool help = false;

    char *file1 = malloc(255 * sizeof(char));
    char *file2 = malloc(255 * sizeof(char));
    int files = 0;

    int i = 1;
    while (args[i] != NULL) {
        if (i > 4) break;
        if (!strcmp(args[i], "-n")) ignore_newline = true; 
        else if (!strcmp(args[i], "-d")) diff_mode = true;
        else if (!strcmp(args[i], "-h")) help = true;
        else {
            int j = 0;
            while (args[i][j] != '\0'){
                if (args[i][j] == '-') return print_error("Invalid option passed. Use '-h' for help.");
                j++;
            }
            if (!files) {
                file1 = args[i];
                files++;
            } else file2 = args[i];
        }
        i++;
    }

    if (help) {
        if (args[0] != 1) return invalid_args_word();
        else print_word_help();
        return 0;
    } else if ((ignore_newline && diff_mode && args[0] != 4) ||
               (ignore_newline && !diff_mode && args[0] != 2) ||
               (diff_mode && !ignore_newline && args[0] != 3) ||
               (!diff_mode && !ignore_newline && args[0] != 1)) return invalid_args_word();
    
    int wc1 = word_count(file1, ignore_newline);
    if (wc1 == -1) return 1;
    if (diff_mode) {
        int wc2 = word_count(file2, ignore_newline);
        if (wc2 == -1) return 1;
        printf("Word difference in files %s and %s is %d\n", file1, file2, (wc2 - wc1));
        return 0;
    }
    printf("Words in %s: %d\n", file1, wc1);

    return 0;
}