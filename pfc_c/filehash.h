#ifndef FILEHASH_H
#define FILEHASH_H

#include <stdio.h>
#include <memory.h>
#include "defs.h"

#define SHA1_SIZE_BYTE 20

Status GetFileSha1(const char* filepath, unsigned char *buff);

#endif //FILEHASH_H