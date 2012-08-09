#include "filehash.h"

void toHexString(SHA1Context sha1, char output[SHA1_HEX_LENGTH])
{
    strcpy(output, "");
    
    for(int i = 0; i < 5 ; i++)
    {
        char str[9];
        sprintf(str, "%08x", sha1.Message_Digest[i]);
        strcat(output, str);
    }
}

void GetStringSHA1_Hex(char *input, unsigned long length, char output[SHA1_HEX_LENGTH])
{
    if (NULL == input || NULL == output)
        return;

    SHA1Context sha1;

    SHA1Reset(&sha1);
    SHA1Input(&sha1, input, length);
    SHA1Result(&sha1);

    toHexString(sha1, output);
}

void GetFileSHA1_Hex(char *filepath, char output[SHA1_HEX_LENGTH])
{
    SHA1Context sha1;
    FILE *file  = NULL;
    int len = 0;
    unsigned char buffer[0x0400] = {0};

    file = fopen(filepath, "rb");

    if (NULL == (file = fopen(filepath, "rb")))
        return;

    SHA1Reset(&sha1);

    while (len = fread(buffer, 1, 1024, file))
    {
        SHA1Input(&sha1, buffer, len);
    }

    fclose(file);

    SHA1Result(&sha1);
    toHexString(sha1, output);

    return;
}