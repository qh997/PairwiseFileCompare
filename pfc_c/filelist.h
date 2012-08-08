#ifndef FILELIST_H
#define FILELIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "defs.h"

typedef struct FileNode
{
    char filename[FILENAME_MAX];
    struct FileNode *next;
} FileList;

typedef struct IdenticalFileNode
{
    FileList *filelist;
    struct IdenticalFileNode *next;
} IdentFiles;

Status FileList_Init(FileList *list);
Status FileList_Push(FileList *list, char *filename);
Status FileList_Insert(FileList *list, char *filename);
int FileList_Count(FileList *list);
Status GetFileListInPath(char *dir, FileList *filelist);
Status GetFileListInPaths(char **dir, FileList *filelist);
Status GetFileByIndex(FileList *list, int index, char *filename);
Status RemoveFileByName(FileList *list, char *filename);
Status RemoveFileByIndex(FileList *list, int index);
void PrintFileList(FileList *list);

#endif //FILELIST_H