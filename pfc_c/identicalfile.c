#include "identicalfile.h"

Status IdenticalFile_Init(IdentFileList **ifiles)
{
    *ifiles = (IdentFileList *)malloc(sizeof(IdentFileList));

    (*ifiles)->filelist = NULL;
    (*ifiles)->next = NULL;

    return OK;
}

Status IdenticalFile_Push(IdentFileList *ifiles, FileList *filelist)
{
    while (NULL != ifiles->next)
    {
        ifiles = ifiles->next;
    }

    IdenticalFile_Init(&(ifiles->next));
    ifiles = ifiles->next;
    ifiles->filelist = filelist;

    return OK;
}

Status GetIdenticalFile(FileList *filelist, IdentFileList *ifiles)
{
    FileList *i = filelist->next;
    while (NULL != i)
    {
        FileList *j = i->next;
        FileList *ifilelist = NULL;
        while (NULL != j)
        {
            if (FileIsIdentical(i->filename, j->filename))
            {
                if (NULL == ifilelist)
                {
                    ifilelist = (FileList *)malloc(sizeof(FileList));
                    FileList_Init(ifilelist);
                    FileList_Push(ifilelist, i->filename);
                }

                FileList_Push(ifilelist, j->filename);
                FileList *jnext = j->next;
                RemoveFileByName(filelist, j->filename);
                j = jnext;
            }
            else
            {
                j = j->next;
            }
        }

        if (NULL != ifilelist)
            IdenticalFile_Push(ifiles, ifilelist);

        i = i->next;
    }
}

bool FileIsIdentical(char *fileL, char *fileR)
{
    FILE *fL = NULL;
    FILE *fR = NULL;

    if (NULL == (fL = fopen(fileL, "rb")))
        return FALSE;

    if (NULL == (fR = fopen(fileR, "rb")))
    {
        fclose(fL);
        return FALSE;
    }

    unsigned char bufferL[0x0400] = {0};
    unsigned char bufferR[0x0400] = {0};
    int lenL = -1;
    int lenR = -1;

    while (0 != lenL || 0 != lenR)
    {
        lenL = fread(bufferL, 1, 1024, fL);
        lenR = fread(bufferR, 1, 1024, fR);

        if (lenL != lenR)
        {
            fclose(fL);
            fclose(fR);
            return FALSE;
        }

        for (int i = 0; i < lenL; i++)
        {
            if (bufferL[i] != bufferR[i])
            {
                fclose(fL);
                fclose(fR);
                return FALSE;
            }
        }
    }

    fclose(fL);
    fclose(fR);

    if (lenL != lenR)
        return FALSE;

    //printf("%s == %s\n", fileL, fileR);

    return TRUE;
}

void PrintIdenticalFile(IdentFileList *ifiles)
{
    while (NULL != ifiles->next)
    {
        ifiles = ifiles->next;

        PrintFileList(ifiles->filelist);
        printf("\n");
    }
}