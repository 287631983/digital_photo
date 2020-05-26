#include <config.h>
#include <encoding_manager.h>
#include <string.h>

static int isUtf16beCoding(unsigned char *pucBufHead);
static int Utf16beGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *puchBufEnd, unsigned int *pdwCode);
    

static T_EncodingOpr g_tUtf16beEncoingOpr = {
    .name           = "utf16-be",
    .iHeadLen       = 2,
    .isSupport      = isUtf16beCoding,
    .GetCodeFrmBuf  = Utf16beGetCodeFrmBuf,
};

static int isUtf16beCoding(unsigned char *pucBufHead)
{
    const char aStrUtf16be[] = {0xfe, 0xff, 0x00};

    if (strncmp((const char *)pucBufHead, aStrUtf16be, 2) == 0)
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}

static int Utf16beGetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *puchBufEnd, unsigned int *pdwCode)
{
    if (pucBufStart + 1 < puchBufEnd)
    {
        *pdwCode = (((unsigned int)pucBufStart[0] << 8) + pucBufStart[1]);
        return 2;
    }
    else
    {
        // 文件结束
        return 0;
    }
}

int Utf16beEncodingInit(void)
{
    AddFontOprForEncoding(&g_tUtf16beEncoingOpr, GetFontOpr("freetype"));
    AddFontOprForEncoding(&g_tUtf16beEncoingOpr, GetFontOpr("ascii"));
    return RegisterEncodingOpr(&g_tUtf16beEncoingOpr);
}