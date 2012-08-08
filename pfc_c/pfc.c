#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "filelist.h"
#include "identicalfile.h"
#include "defs.h"

int parse_cmd_line(int argc, char **argv);
void remove_last_separator(char *dir);
void get_all_filelist(char **paths, FileList *filelist);
bool FileIsIdentical(char *fileL, char *fileR);

#define OPT_HASH (1 << 0)

unsigned int OPTIONS = 0;
char **DIRPATHS = NULL;

int main(int argc, char **argv)
{
    parse_cmd_line(argc, argv);

    FileList *filelist = (FileList *)malloc(sizeof(FileList));
    FileList_Init(filelist);
    GetFileListInPaths(DIRPATHS, filelist);

    IdentFileList *ifiles;
    IdenticalFile_Init(&ifiles);
    GetIdenticalFile(filelist, ifiles);

    PrintIdenticalFile(ifiles);
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
            *paths = (char *)malloc(strlen(*(argv + i)) + 1);
            strcpy(*paths, *(argv + i));
            remove_last_separator(*paths);
            paths++;
        }
        *paths = (char *)malloc(2);
        strcpy(*paths, "#");
    }
    else
    {
        char crt_dir[FILENAME_MAX];
        getcwd(crt_dir, FILENAME_MAX);
        DIRPATHS = (char **)malloc(sizeof(char *) * 2);
        *DIRPATHS = (char *)malloc(strlen(crt_dir) + 1);
        strcpy(*DIRPATHS, crt_dir);
        remove_last_separator(*DIRPATHS);

        *(DIRPATHS + 1) = (char *)malloc(2);
        strcpy(*(DIRPATHS + 1), "#");
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