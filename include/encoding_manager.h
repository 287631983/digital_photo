#ifndef __ENCODING_MANAGER_H
#define __ENCODING_MANAGER_H

#include <fonts_manager.h>

typedef struct EncodingOpr{
    char *name;
    int iHeadLen;                               //编码文件的数据头长度
    PT_FontOpr ptFontOprSupportedHead;
    int (*isSupport)(unsigned char *pucBufHead);
    int (*GetCodeFrmBuf)(unsigned char *pucBufStart, unsigned char *puchBufEnd, unsigned int *pdwCode);
    struct EncodingOpr *ptNext;
}T_EncodingOpr, *PT_EncodingOpr;

int RegisterEncodingOpr(PT_EncodingOpr ptEncodingOpr);
void ShowEncoingOpr(void);
PT_EncodingOpr SelectEncodingOprForFIle(unsigned char *pucFileBUfHead);
int AddFontOprForEncoding(PT_EncodingOpr ptEncodingOpr, PT_FontOpr ptFontOpr);
int DelFontOprFrmEncoding(PT_EncodingOpr ptEncodingOpr, PT_FontOpr ptFontOpr);
int EncodingInit(void);
int AsciiEncodingInit(void);
int Utf8EncodingInit(void);
int Utf16beEncodingInit(void);
int Utf16leEncodingInit(void);

#endif