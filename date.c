#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

int print_error(char *error) {
    printf("Error: %s\n", error);
    return 1;
}

void display_help() {
    printf("Usage: date [OPTION] [FILE]\n");
    printf("Display the last modified date and time of a file.\n\n");
    printf("Options: -d     Display time described by STRING.\n");
    printf("         -R     Display date and time in RFC 5322 format.\n");
    printf("         -h     Display this help message.\n\n");
}

int main(int argc, char *argv[]) {
    char *file_name = NULL;
    char *display_string = NULL;
    bool output_rfc5322 = false;
    bool help = false;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-h")) {
            help = true;
        } else if (!strcmp(argv[i], "-d")) {
            i++;
            if (i < argc) display_string = argv[i];
            else return print_error("Missing argument for -d option.");
        } else if (!strcmp(argv[i], "-R")) output_rfc5322 = true;
        else if (argv[i][0] != '-') {
            if (file_name == NULL) file_name = argv[i];
            else return print_error("Invalid syntax.");
        } else return print_error("Invalid syntax.");
    }

    if (file_name == NULL || help) {
        display_help();
        exit(0);
    }

    struct stat file_info;
    if (stat(file_name, &file_info) == -1) {
        perror("stat");
        exit(1);
    }

    if (output_rfc5322) {
        struct tm *time_info = gmtime(&file_info.st_mtime);
        char buf[100];
        strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %z", time_info);
        // day, date month year hour:minute:seconds timezone
        printf("Last modified: %s\n", buf);
    } else if (display_string != NULL) {
        struct tm *timeinfo;
        timeinfo = localtime(&(file_info.st_mtime));
        char buf[100];
        if (strftime(buf, sizeof(buf), display_string, timeinfo) == 0) return print_error("Cannot parse date format.");
        printf("Last modified: %s\n", buf);
    } else printf("Last modified: %s", ctime(&file_info.st_mtime));

    return 0;
}