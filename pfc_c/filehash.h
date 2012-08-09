#ifndef FILEHASH_H
#define FILEHASH_H

#include <stdio.h>
#include <string.h>
#include "sha1.h"

#define SHA1_HEX_LENGTH (40 + 1)

void GetStringSHA1_Hex(char *input, unsigned long length, char output[SHA1_HEX_LENGTH]);
void GetFileSHA1_Hex(char *filepath, char output[SHA1_HEX_LENGTH]);

#endif //FILEHASH_H