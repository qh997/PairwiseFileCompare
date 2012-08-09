#include "filehash.h"

#define INIT_DATA_h0 0x67452301U
#define INIT_DATA_h1 0xEFCDAB89U
#define INIT_DATA_h2 0x98BADCFEU
#define INIT_DATA_h3 0x10325476U
#define INIT_DATA_h4 0xC3D2E1F0U

#define SHA1CircularShift(bits, word) (((word) << (bits)) | ((word) >> (32 - (bits))))

typedef struct SHAstate_st
{
    unsigned long h[SHA1_SIZE_BYTE / 4]; /* 存放摘要结果(32*5=160 bits)*/
    unsigned long Nl;
    unsigned long Nh; /*存放信息总位数，Nh：高32位，Nl：低32位*/
    unsigned long data[16]; /*数据从第0个的高8位开始依次放置*/
    int FlagInWord; /*标识一个data元素中占用的字节数（从高->低），取值0,1,2,3*/
    int msgIndex; /*当前已填充满的data数组元素数。*/
    int isTooMang; /*正常为0，当处理的信息超过2^64 bits时为1；*/
} SHA1_Context;

typedef unsigned long(*SHA1_pFun)();

const unsigned long SHA1_Kt[] = {0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6};

static Status SHA1_Init(SHA1_Context *c);
static Status SHA1_One(SHA1_Context *c);
static Status SHA1_Clear_data(SHA1_Context *c);
static Status SHA1_Update(SHA1_Context *c, const unsigned char *chBuff, unsigned int n);
static Status SHA1_Final(SHA1_Context *c, unsigned char * md);
static Status SHA1_PadMessage(SHA1_Context *c);
static int SHA1_GetMsgBits(SHA1_Context *c);

static unsigned long SHA1_ft00_19(unsigned long b, unsigned long c, unsigned long d)
{
    return((b & c) | ((~b) & d));
}

static unsigned long SHA1_ft20_39(unsigned long b, unsigned long c, unsigned long d)
{
    return(b ^ c ^ d);
}

static unsigned long SHA1_ft40_59(unsigned long b, unsigned long c, unsigned long d)
{
    return((b & c) | (b & d) | (c & d));
}

static unsigned long SHA1_ft60_79(unsigned long b, unsigned long c, unsigned long d)
{
    return(b ^ c ^ d);
}

SHA1_pFun ft[] = {SHA1_ft00_19, SHA1_ft20_39, SHA1_ft40_59, SHA1_ft60_79};

static Status SHA1_Init(SHA1_Context *c)
{
    if (NULL == c)
        return ERROR;

    c->h[0] = INIT_DATA_h0;
    c->h[1] = INIT_DATA_h1;
    c->h[2] = INIT_DATA_h2;
    c->h[3] = INIT_DATA_h3;
    c->h[4] = INIT_DATA_h4;
    c->Nl = 0;
    c->Nh = 0;
    c->FlagInWord = 0;
    c->msgIndex = 0;
    c->isTooMang = 0;
    memset(c->data, 0, 64);

    return OK;
}

static Status SHA1_One(SHA1_Context *c)
{
    unsigned long AE[5];
    unsigned long w[80];
    unsigned long temp = 0;
    int t = 0;

    if ((NULL == c) || (0 != c->isTooMang))
    {
        return ERROR;
    }

    for (t = 0; t < 16; ++t)
    {
        w[t] = c->data[t];
    }

    for (t = 16; t < 80; ++t)
    {
        w[t] = SHA1CircularShift(1, w[t-3] ^ w[t-8] ^ w[t-14] ^ w[t-16]);
    }

    for (t = 0; t < 5; ++t)
    {
        AE[t] = c->h[t];
    }

    for (t = 0; t <= 79; ++t)
    {
        temp = SHA1CircularShift(5, AE[0]) + (*ft[t/20])(AE[1], AE[2], AE[3]) + AE[4] + w[t] + SHA1_Kt[t/20];
        AE[4] = AE[3];
        AE[3] = AE[2];
        AE[2] = SHA1CircularShift(30, AE[1]);
        AE[1] = AE[0];
        AE[0] = temp;
    }

    for (t = 0; t < 5; ++t)
    {
        c->h[t] += AE[t];
    }

    SHA1_Clear_data(c);

    return OK;
}

static Status SHA1_Clear_data(SHA1_Context *c)
{
    if (NULL == c)
        return ERROR;

    memset(c->data, 0, 64);
    c->msgIndex = 0;
    c->FlagInWord = 0;

    return OK;
}

static Status SHA1_Update(SHA1_Context *c, const unsigned char *chBuff, unsigned int n)
{
    unsigned int lastBytes = 0;
    unsigned int temp      = 0;
    unsigned int i         = 0;
    unsigned int tempBits  = 0;

    if (!c || !chBuff || !n || c->isTooMang)
        return ERROR;

    if (n > strlen((char *)chBuff))
        n = strlen((char *)chBuff);

    if (c->FlagInWord > 0)
    {
        temp = (unsigned int)(4 - c->FlagInWord) < n ? (unsigned int)(4 - c->FlagInWord) : n;

        for (i = temp; i > 0; --i)
            c->data[c->msgIndex] |= ((unsigned long)chBuff[temp-i]) << (3 - c->FlagInWord++) * 8;

        tempBits = c->Nl;
        c->Nl += 8 * temp;

        if (tempBits > c->Nl)
        {
            ++(c->Nh);
            if (c->Nh == 0)
                c->isTooMang = 1;
        }

        if ((c->FlagInWord) / 4 > 0)
            ++(c->msgIndex);

        c->FlagInWord = c->FlagInWord % 4;

        if (c->msgIndex == 16)
            SHA1_One(c);
    }

    chBuff += temp;
    n -= temp;

    if (n >= 4)
    {
        for (i = 0; i <= n - 4; i += 4)
        {
            c->data[c->msgIndex] |= ((unsigned long)chBuff[i]) << 24;
            c->data[c->msgIndex] |= ((unsigned long)chBuff[i+1]) << 16;
            c->data[c->msgIndex] |= ((unsigned long)chBuff[i+2]) << 8;
            c->data[c->msgIndex] |= ((unsigned long)chBuff[i+3]);
            ++(c->msgIndex);

            tempBits = c->Nl;
            c->Nl += 32;

            if (tempBits > c->Nl)
            {
                c->Nh++;

                if (c->Nh == 0)
                {
                    c->isTooMang = 1;
                }
            }

            if (c->msgIndex == 16)
            {
                SHA1_One(c);
            }
        }
    }

    if (n > 0 && n % 4 != 0)
    {
        lastBytes = n - i;

        switch (lastBytes)
        {

            case 3:
                c->data[c->msgIndex] |= ((unsigned long)chBuff[i+2]) << 8;

            case 2:
                c->data[c->msgIndex] |= ((unsigned long)chBuff[i+1]) << 16;

            case 1:
                c->data[c->msgIndex] |= ((unsigned long)chBuff[i]) << 24;
        }

        c->FlagInWord = lastBytes;

        tempBits = c->Nl;
        c->Nl += 8 * lastBytes;

        if (tempBits > c->Nl)
        {
            ++(c->Nh);

            if (0 == c->Nh)
            {
                c->isTooMang = 1;
            }
        }

        if (16 == c->msgIndex)
        {
            SHA1_One(c);
        }
    }

    return OK;
}

static Status SHA1_Final(SHA1_Context *c, unsigned char * md)
{
    int i = 0;

    if ((NULL == md) || (NULL == c) || (c->isTooMang))
    {
        return ERROR;
    }

    SHA1_PadMessage(c);

    SHA1_One(c);

    for (i = 0; i < 5; ++i)
    {
        md[4 * i] = (unsigned char)((c->h[i] & 0xff000000) >> 24);
        md[4 * i + 1] = (unsigned char)((c->h[i] & 0x00ff0000) >> 16);
        md[4 * i + 2] = (unsigned char)((c->h[i] & 0x0000ff00) >> 8);
        md[4 * i + 3] = (unsigned char)(c->h[i] & 0x000000ff);
    }

    return OK;
}

static Status SHA1_PadMessage(SHA1_Context *c)
{
    int msgBits = -1;

    if ((NULL == c) || (0 != c->isTooMang))
    {
        return ERROR;
    }

    msgBits = SHA1_GetMsgBits(c);

    if (440 < msgBits)
    {
        c->data[c->msgIndex++] |= (1 << (8 * (4 - c->FlagInWord) - 1));
        c->FlagInWord = 0;

        while (c->msgIndex < 16)
        {
            c->data[c->msgIndex++] = 0;
        }

        SHA1_One(c);

        while (c->msgIndex < 14)
        {
            c->data[c->msgIndex++] = 0;
        }

    }
    else
    {
        c->data[c->msgIndex++] |= (1 << (8 * (4 - c->FlagInWord) - 1));
        c->FlagInWord = 0;

        while (c->msgIndex < 14)
        {
            c->data[c->msgIndex++] = 0;
        }
    }

    while (c->msgIndex < 14)
    {
        c->data[c->msgIndex++] = 0;
    }

    c->data[c->msgIndex++] = c->Nh;
    c->data[c->msgIndex++] = c->Nl;

    return OK;
}

static int SHA1_GetMsgBits(SHA1_Context *c)
{
    int a = 0;

    if ((NULL == c) || (0 != c->isTooMang))
        return -1;

    a = sizeof(unsigned long) * 8 * c->msgIndex + 8 * c->FlagInWord;

    return a;
}

Status GetFileSha1(const char* filepath, unsigned char *buff)
{
    FILE *file  = NULL;
    SHA1_Context context;
    int len = 0;
    unsigned char buffer[0x0400] = {0};

    file = fopen(filepath, "rb");

    if (NULL == (file = fopen(filepath, "rb")))
        return ERROR;

    if (OK != SHA1_Init(&context))
    {
        fclose(file);
        return ERROR;
    }

    while (len = fread(buffer, 1, 1024, file))
    {
        if (OK != SHA1_Update(&context, buffer, len))
        {
            fclose(file);
            return ERROR;
        }
    }

    if(OK != SHA1_Final(&context, buff))
    {
        fclose(file);
        return ERROR;
    }

    fclose(file);

    return OK;
}