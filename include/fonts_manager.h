#ifndef __FONTS_MANAGER_H
#define __FONTS_MANAGER_H

typedef struct FontBitMap {
    int iXLeft;                 //字符距左边界的距离
    int iYTop;                  //字符距上边界的距离
    int iXMax;
    int iYMax;
    int iBpp;                   //每个像素的位数
    int iPitch;
    int iCurOriginX;
    int iCurOriginY;
    int iNextOriginX;
    int iNextOriginY;
    unsigned char *pucBuffer;
}T_FontBitMap, *PT_FontBitMap;

typedef struct FontOpr {
    char *name;
    int (*FontInit)(char *pcFontFile, unsigned int dwFontSize);
    int (*GetFontBitMap)(unsigned int dwCode, PT_FontBitMap pt_FontBitMap);
    struct FontOpr *ptNext;
}T_FontOpr, *PT_FontOpr;

int RegisterFontOpr(PT_FontOpr ptFontOpr);
void ShowFontOpr(void);
PT_FontOpr GetFontOpr(char *pcName);
int FontsInit(void);
int ASCIIInit(void);
int GBKInit(void);
int FreeTypeInit(void);

#endif