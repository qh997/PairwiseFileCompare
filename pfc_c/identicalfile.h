#ifndef IDENTICALFILE_H
#define IDENTICALFILE_H

#include <stdio.h>
#include <stdbool.h>
#include "filelist.h"
#include "defs.h"

typedef struct IdentFile
{
    FileList *filelist;
    struct IdentFile *next;
} IdentFileList;

Status IdenticalFile_Init(IdentFileList **ifiles);
Status GetIdenticalFile(FileList *filelist, IdentFileList *ifiles);
bool FileIsIdentical(char *fileL, char *fileR);
void PrintIdenticalFile(IdentFileList *ifiles);

#endif //IDENTICALFILE_H