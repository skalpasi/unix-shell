#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>

int print_error(char *error) {
    printf("Error: %s\n", error);
    return 1;
}

void display_help() {
    printf("Usage: dir [OPTION] [DIR_NAME]\n");
    printf("Create a directory and change the path to that directory.\n\n");
    printf("Options: -v     Print a message for each step in the running of this command.\n");
    printf("         -r     Remove the directory if it already exists and create a new one.\n");
    printf("         -h     Display this help message.\n\n");
    printf("Example: dir -v -r dir_name\n");
}

int rm_rf(const char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    if ((dir = opendir(path)) == NULL) return print_error("Could not open the directory for deletion.");

    while ((entry = readdir(dir)) != NULL) {
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        if (stat(fullpath, &statbuf) == -1) return print_error("stat error.");
        if (S_ISDIR(statbuf.st_mode)) if (rm_rf(fullpath) == -1) return -1;
        else if (remove(fullpath) == -1) return print_error("Error removing files. Aborting.");
    }

    closedir(dir);

    if (rmdir(path) == -1) return print_error("Could not remove directory.");

    return 0;
}

int main(int argc, char *argv[]) {
    bool remove_existing = false;
    bool verbose = false;
    bool help = false;
    char *dir_name = NULL;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-h")) help = true;
        else if (!strcmp(argv[i], "-r")) remove_existing = true;
        else if (strcmp(argv[i], "-v") == 0) verbose = true;
        else if (dir_name == NULL) dir_name = argv[i];
        else return print_error("Invalid syntax. Use '-h' for help");
    }

    if (help) {
        display_help();
        exit(0);
    }

    if (dir_name == NULL) return print_error("Directory name not provided. Use '-h' for help.");

    struct stat st;
    if (stat(dir_name, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            if (remove_existing) {
                if (verbose) printf("Removing existing directory '%s'.\n", dir_name);
                if (rm_rf(dir_name) != 0) return print_error("rmdir error.");
            } else return print_error("Directory already exists. Use '-r' to remove it or '-h' for help.");
        } else {
            printf("Error: '%s' is not a directory.\n", dir_name);
            exit(EXIT_FAILURE);
        }
    }

    if (verbose) printf("Creating directory '%s'.\n", dir_name);
    if (mkdir(dir_name, 0777) != 0) return print_error("Could not create directory.");
    if (chdir(dir_name) != 0) return print_error("Could not change directory.");
    if (verbose) printf("Changed working directory to '%s'.\n", dir_name);

    return 0;
}
