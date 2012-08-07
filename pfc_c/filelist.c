#include "filelist.h"

Status FileList_Init(FileList *list)
{
    (list->filename)[0] = '\0';
    list->next = NULL;

    return OK;
}

Status FileList_Push(FileList *list, char *filename)
{
    while (NULL != list->next)
        list = list->next;

    if (NULL == (list->next = (FileList *)malloc(sizeof(FileList))))
        return OVERFLOW;

    list = list->next;
    list->next = NULL;
    strcpy(list->filename, filename);

    return OK;
}

int FileList_Count(FileList *list)
{
    int count = 0;
    while (NULL != list->next)
    {
        count++;
        list = list->next;
    }

    return count;
}

Status GetFileListInPath(char *dir, FileList *filelist)
{
    DIR *dp;

    if (NULL != (dp = opendir(dir)))
    {
        struct dirent *entry;

        chdir(dir);
        while ((entry = readdir(dp)) != NULL)
        {
            struct stat statbuf;
            lstat(entry->d_name, &statbuf);
            if(S_ISDIR(statbuf.st_mode))
            {
                if(strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                    continue;

                char next_dir[FILENAME_MAX] = {'\0'};
                strcpy(next_dir, dir);
                strcat(next_dir, "/");
                strcat(next_dir, entry->d_name);
                GetFileListInPath(next_dir, filelist);
            }
            else if(S_ISREG(statbuf.st_mode))
            {
                char file_path[FILENAME_MAX];
                sprintf(file_path, "%s/%s", dir, entry->d_name);

                FileList_Push(filelist, file_path);
            }
        }
        chdir("..");
        closedir(dp);
    }
    else
    {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return INFEASIBLE;
    }

    return OK;
}

Status GetFileListInPaths(char **dir, FileList *filelist)
{
    if (NULL != dir)
    {
        while (0 != strcmp("#", *dir))
        {
            show_var("%s", *dir);
            GetFileListInPath(*dir++, filelist);
        }
    }
}

Status GetFileByIndex(FileList *list, int index, char *filename)
{
    int i = 0;
    while (NULL != list->next)
    {
        list = list->next;
        i++;

        if (i == index)
        {
            strcpy(filename, list->filename);
            return OK;
        }
    }

    return INFEASIBLE;
}

Status RemoveFileByName(FileList *list, char *filename)
{
    while (NULL != list->next)
    {
        FileList *next_file = list->next;
        if (0 == strcmp(next_file->filename, filename))
        {
            next_file = next_file->next;
            free(list->next);
            list->next = next_file;

            return OK;
        }

        list = list->next;
    }

    return INFEASIBLE;
}

Status RemoveFileByIndex(FileList *list, int index)
{
    int i = 1;
    while (NULL != list->next)
    {
        FileList *next_file = list->next;
        if (i == index)
        {
            next_file = next_file->next;
            free(list->next);
            list->next = next_file;
            show_var("%d", index);

            return OK;
        }

        list = list->next;
        i++;
    }

    return INFEASIBLE;
}

void PrintFileList(FileList *list)
{
    while (NULL != list->next)
    {
        list = list->next;
        printf("%s\n", list->filename);
    }
}