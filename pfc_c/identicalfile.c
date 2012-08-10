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
        FILE *fileL = fopen(i->filename, "rb");
        while (NULL != j)
        {
            FILE *fileR = fopen(j->filename, "rb");
            if (FileIsIdentical(fileL, fileR))
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

            fclose(fileR);
            fseek(fileL, 0, SEEK_SET);
        }

        if (NULL != ifilelist)
            IdenticalFile_Push(ifiles, ifilelist);

        i = i->next;

        fclose(fileL);
    }
}

bool FileIsIdentical(FILE *fL, FILE *fR)
{
    if (NULL == fL || NULL == fR)
        return FALSE;

    unsigned char bufferL[1024] = {0};
    unsigned char bufferR[1024] = {0};
    int lenL = -1;
    int lenR = -1;

    while (0 != lenL || 0 != lenR)
    {
        lenL = fread(bufferL, 1, 1024, fL);
        lenR = fread(bufferR, 1, 1024, fR);

        if (lenL != lenR)
        {
            return FALSE;
        }

        for (int i = 0; i < lenL; i++)
        {
            if (bufferL[i] != bufferR[i])
            {
                return FALSE;
            }
        }
    }

    if (lenL != lenR)
        return FALSE;

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

void PrintIdenticalFileHash(IdentFileList *ifiles)
{
    while (NULL != ifiles->next)
    {
        ifiles = ifiles->next;

        char sha1_hex[SHA1_HEX_LENGTH];

        GetFileSHA1_Hex(ifiles->filelist->next->filename, sha1_hex);
        printf("%s\n", sha1_hex);

        PrintFileList(ifiles->filelist);
        printf("\n");
    }
}