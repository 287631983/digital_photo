#include <config.h>
#include <encoding_manager.h>
#include <string.h>

static int isUtf16leCoding(unsigned char *pucBufHead);
static int Utf16leGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *puchBufEnd, unsigned int *pdwCode);

static T_EncodingOpr g_tUtf16leEncodingOpr = {
    .name           = "utf16-le",
    .iHeadLen       = 2,
    .isSupport      = isUtf16leCoding,
    .GetCodeFrmBuf  = Utf16leGetCodeFrmBuf,
};

static int isUtf16leCoding(unsigned char *pucBufHead)
{
    const char aStrUtf16le[] = {0xff, 0xfe, 0x00};

    if (strncmp((const char *)pucBufHead, aStrUtf16le, 2) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int Utf16leGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *puchBufEnd, unsigned int *pdwCode)
{
    if (pucBufStart + 1 < puchBufEnd)
    {
        *pdwCode = ((unsigned int)pucBufStart[1] << 8) + pucBufStart[0];
        return 2;
    }
    else
    {
        // 文件结束
        return 0;
    }
}

int Utf16leEncodingInit(void)
{
    AddFontOprForEncoding(&g_tUtf16leEncodingOpr, GetFontOpr("freetype"));
    AddFontOprForEncoding(&g_tUtf16leEncodingOpr, GetFontOpr("ascii"));
    return RegisterEncodingOpr(&g_tUtf16leEncodingOpr);
}