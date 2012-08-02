#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "sha1.h"

#define DEBUG 1

#if DEBUG
#define show_var(fmt, var) printf("%s = "fmt"\n", #var, var)
#define show_log(var) printf("%s\n", var);
#else
#define show_var(fmt, var)
#define show_log(var)
#endif

int parse_cmd_line(int argc, char **argv);
void get_all_filelist(char **paths);
void get_filelist(char *dir);
void remove_last_separator(char *dir);

#define OPT_HASH (1 << 0)

unsigned int OPTIONS = 0;
char **DIRPATHS = NULL;

int main(int argc, char **argv)
{
    parse_cmd_line(argc, argv);
    get_all_filelist(DIRPATHS);
}

int parse_cmd_line(int argc, char **argv)
{
    int option;
    const char *optstring = "h";
    struct option longopts[] =
    {
        {"hash", no_argument, NULL, 'h'},
        {0, 0, 0, 0}
    };

    while ((option = getopt_long(argc, argv, optstring, longopts, NULL)) != -1)
    {
        switch (option)
        {
            case 'h':
                OPTIONS |= OPT_HASH;
                break;
            case '?':
            default:
                return 1;
        }
    }

    if (optind < argc)
    {
        DIRPATHS = (char **)malloc(sizeof(char *) * (argc - optind + 1));
        char **paths = DIRPATHS;
        for (int i = optind; i < argc; i++)
        {
            *paths = (char *)malloc(sizeof(char) * (strlen(*(argv + i)) + 1));
            strcpy(*paths, *(argv + i));
            paths++;
        }
        *paths = (char *)malloc(sizeof(char) * 2);
        strcpy(*paths, "#");
    }
    else
    {
        char crt_dir[FILENAME_MAX];
        getcwd(crt_dir, FILENAME_MAX);
        DIRPATHS = (char **)malloc(sizeof(char *) * 2);
        *DIRPATHS = (char *)malloc(sizeof(char) * (strlen(crt_dir) + 1));
        strcpy(*DIRPATHS, crt_dir);

        *(DIRPATHS + 1) = (char *)malloc(sizeof(char) * 2);
        strcpy(*(DIRPATHS + 1), "#");
    }
}

void get_all_filelist(char **paths)
{
    if (NULL != DIRPATHS)
    {
        char **dir = DIRPATHS;
        while (0 != strcmp("#", *dir))
        {
            remove_last_separator(*dir);
            show_var("%s", *dir);
            get_filelist(*dir++);
        }
    }
}

void get_filelist(char *dir)
{
    DIR *dp;

    if (NULL != (dp = opendir(dir)))
    {
        struct dirent *entry;
        struct stat statbuf;

        chdir(dir);
        while ((entry = readdir(dp)) != NULL)
        {
            lstat(entry->d_name, &statbuf);
            if(S_ISDIR(statbuf.st_mode))
            {
                if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                    continue;

                char *next_dir = (char *)malloc(sizeof(char) * (strlen(entry->d_name) + strlen(dir) + 2));
                strcpy(next_dir, dir);
                strcat(next_dir, "/");
                strcat(next_dir, entry->d_name);
                get_filelist(next_dir);
            }
            else
            {
                printf("%s/%s\n", dir, entry->d_name);
            }
        }
        chdir("..");
        closedir(dp);
    }
    else
    {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
}

void remove_last_separator(char *dir)
{
    int dir_length = strlen(dir);
    show_var("%d", dir_length);
    if (0 < dir_length && '/' == *(dir + dir_length - 1))
    {
        *(dir + dir_length - 1) = '\0';
    }
}
