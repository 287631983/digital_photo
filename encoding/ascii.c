#include <config.h>
#include <encoding_manager.h>
#include <string.h>

static int isAsciiCoing(unsigned char *pucBufHead);
static int AsciiGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *puchBufEnd, unsigned int *pdwCode);


static T_EncodingOpr g_tAsciiEncodingOpr = {
    .name           = "ascii",
    .iHeadLen       = 0,
    .isSupport      = isAsciiCoing,
    .GetCodeFrmBuf  = AsciiGetCodeFrmBuf,
};

static int isAsciiCoing(unsigned char *pucBufHead)
{
    const char aStrutf8[]       = {0xef, 0xbb, 0xbf,0x00};
    const char aStrUtf16le[]    = {0xff, 0xfe, 0x00};
    const char aStrUtf16be[]    = {0xfe, 0xff, 0x00};

    if (strncmp((const char *)pucBufHead, aStrutf8, 3) == 0)
    {
        return 0;
    }
    else if(strcmp((const char *)pucBufHead, aStrUtf16le) == 0)
    {
        return 0;
    }
    else if(strcmp((const char *)pucBufHead, aStrUtf16be) == 0)
    {
        return 0;
    }

    return -1;
}

static int AsciiGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *puchBufEnd, unsigned int *pdwCode)
{
    unsigned char *pucBuf = pucBufStart;
    unsigned char c = *pucBuf;

    if ((pucBuf < puchBufEnd) && (c < (unsigned char)0x80))
    {
        // 返回ascii码
        *pdwCode = (unsigned int)c;
        return 1;
    }

    if (((pucBuf + 1) < puchBufEnd) && (c >= (unsigned char)0x80))
    {
        // 返回GBK码
        *pdwCode = pucBuf[0] + ((unsigned int)pucBuf[1] << 8);
        return 2;
    }

    if (pucBuf < puchBufEnd)
    {
        // 该文件可能损坏或者不识别,直接返回一个码
        *pdwCode = (unsigned int)c;
        return 1;
    }
    else
    {
        // 文件处理完毕
        return 0;
    }
}

int AsciiEncodingInit(void)
{
    AddFontOprForEncoding(&g_tAsciiEncodingOpr,GetFontOpr("freetype"));
    AddFontOprForEncoding(&g_tAsciiEncodingOpr,GetFontOpr("ascii"));
    AddFontOprForEncoding(&g_tAsciiEncodingOpr,GetFontOpr("gbk"));
    return RegisterEncodingOpr(&g_tAsciiEncodingOpr);
}