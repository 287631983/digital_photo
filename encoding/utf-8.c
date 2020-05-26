#include <config.h>
#include <encoding_manager.h>
#include <string.h>

static int isUtf8Coding(unsigned char *pucBufHead);
static int Utf8GetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *puchBufEnd, unsigned int *pdwCode);

static T_EncodingOpr g_tUtf8EncodingOpr = {
    .name           = "utf-8",
    .iHeadLen       = 3,
    .isSupport      = isUtf8Coding,
    .GetCodeFrmBuf  = Utf8GetCodeFrmBuf,
};

static int isUtf8Coding(unsigned char *pucBufHead)
{
    const char aStrUtf8[] = {0xef, 0xbb, 0xbf, 0x00};
    if (strncmp((const char *)pucBufHead, aStrUtf8, 3) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// 求一个字节从高到低连续bit1的个数
static int GetPreOneBits(unsigned char ucVal)
{
    int i;
    int j = 0;

    for (i = 7; i >= 0; i --)
    {
        if (ucVal & (1<<i))
        {
            j ++;
        }
        else
        {
            return j;
        }
    }

    return j;
}

/* 
对于UTF-8编码中的任意字节B,如果B的第一位为0,则B为ASCII码,并且B独立表示一个字符;
如果B的第一位为1,第二位为0,则B是非ASCII字符中的一个字符,总共两个字节表示该字符;
如果B的前两位为1,第三位为0,则B是非ASCII字符中的第一个字符,总共两个字节表示该字符;
如果B的前三位为1,第四位为0,则B为非ASCII字符中的第一个字符,总共三个字节表示该字符;
如果B的前四位为1,第五位为0,则B为非ASCII字符中的第一个字节,总共四个字节表示该字符;
因此根据第一位可以判断是否为ASCII字符;
根据前两位可判断该字节是否为一个字符编码的第一个字节;
根据前四位可确定该字节为字符编码的第一个字节,平且可判断对应的字符有几个字节;
根据前五位,可判断编码中是否有错误或者数据传输过程中是否有错误;
 */
static int Utf8GetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *puchBufEnd, unsigned int *pdwCode)
{
    int i;
    int iNum;
    unsigned char ucVal;
    unsigned int dwSum = 0;

    if (pucBufStart >= puchBufEnd)
    {
        // 文件结束
        return 0;
    }

    ucVal = pucBufStart[0];
    iNum = GetPreOneBits(ucVal);

    if ((pucBufStart + iNum) > puchBufEnd)
    {
        // 文件结束
        return 0;
    }

    if (iNum == 0)
    {
        // ASCII
        *pdwCode = pucBufStart[0];
        return 1;
    }
    else
    {
        ucVal = ucVal << iNum;
        ucVal = ucVal >> iNum;
        dwSum += ucVal;
        for (i = 1; i < iNum; i ++)
        {
            ucVal = pucBufStart[i] & 0x3f;
            dwSum = dwSum << 6;
            dwSum += ucVal;
        }
        *pdwCode = dwSum;

        return iNum;
    }
}

int Utf8EncodingInit(void)
{
    AddFontOprForEncoding(&g_tUtf8EncodingOpr, GetFontOpr("freetype"));
    AddFontOprForEncoding(&g_tUtf8EncodingOpr, GetFontOpr("ascii"));
    return RegisterEncodingOpr(&g_tUtf8EncodingOpr);
}