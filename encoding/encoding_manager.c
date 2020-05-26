#include <config.h>
#include <encoding_manager.h>
#include <string.h>
#include <stdlib.h>

static PT_EncodingOpr g_ptEncodingOprHead;

// 将EncodingOpr添加到链表末尾
int RegisterEncodingOpr(PT_EncodingOpr ptEncodingOpr)
{
    PT_EncodingOpr ptTemp;

    if (g_ptEncodingOprHead == NULL)
    {
        g_ptEncodingOprHead = ptEncodingOpr;
        g_ptEncodingOprHead->ptNext = NULL;
    }
    else
    {
        ptTemp = g_ptEncodingOprHead;
        while (ptTemp->ptNext != NULL)
        {
            ptTemp = ptTemp->ptNext;
        }
        ptTemp->ptNext = ptEncodingOpr;
        ptTemp->ptNext->ptNext = NULL;
    }

    return 0;
}

// 依次显示链表中的注册结构体序号和名称
void ShowEncoingOpr(void)
{
    int i = 0;

    PT_EncodingOpr ptTemp = g_ptEncodingOprHead;

    while (ptTemp->ptNext != NULL)
    {
        printf("%02d %s\n", i++, ptTemp->name);
        ptTemp = ptTemp->ptNext;
    }
    printf("%02d %s\n", i++, ptTemp->name);
}

// 根据指定的文件头查询是否有支持的操作类并返回
PT_EncodingOpr SelectEncodingOprForFIle(unsigned char *pucFileBUfHead)
{
    PT_EncodingOpr ptTemp = g_ptEncodingOprHead;

    while (ptTemp != NULL)
    {
        if (ptTemp->isSupport(pucFileBUfHead))
        {
            DBG_PRINTF("encoding:%s\n",ptTemp->name);
            return ptTemp;
        }
        else
            ptTemp = ptTemp->ptNext;
    }

    return NULL;
}

int AddFontOprForEncoding(PT_EncodingOpr ptEncodingOpr, PT_FontOpr ptFontOpr)
{
    PT_FontOpr ptFontOprCpy;

    if ((ptEncodingOpr == NULL) || (ptFontOpr == NULL))
    {
        return -1;
    }
    else
    {
        ptFontOprCpy = (PT_FontOpr)malloc(sizeof(struct FontOpr));
        if (ptFontOprCpy == NULL)
        {
            return -1;
        }
        else
        {
            memcpy(ptFontOprCpy, ptFontOpr, sizeof(struct FontOpr));
            ptFontOprCpy->ptNext = ptEncodingOpr->ptFontOprSupportedHead;
            ptEncodingOpr->ptFontOprSupportedHead = ptFontOprCpy;
            return 0;
        }
    }
}

int DelFontOprFrmEncoding(PT_EncodingOpr ptEncodingOpr, PT_FontOpr ptFontOpr)
{
    PT_FontOpr ptTemp, ptPre;

    if ((ptEncodingOpr == NULL) || (ptFontOpr == NULL))
    {
        return -1;
    }
    else
    {
        ptTemp = ptEncodingOpr->ptFontOprSupportedHead;
        if (strcmp(ptTemp->name, ptFontOpr->name) == 0)
        {
            ptEncodingOpr->ptFontOprSupportedHead = ptEncodingOpr->ptFontOprSupportedHead->ptNext;
            free(ptTemp);
            return 0;
        }

        ptPre  = ptEncodingOpr->ptFontOprSupportedHead;
        ptTemp = ptEncodingOpr->ptFontOprSupportedHead->ptNext;
        while (ptTemp != NULL)
        {
            if (strcmp(ptTemp->name, ptFontOpr->name) == 0)
            {
                ptPre->ptNext = ptTemp->ptNext;
                free(ptTemp);
                return 0;
            }
            else
            {
                ptPre = ptTemp;
                ptTemp = ptTemp->ptNext;
            } 
        }
        return -1;
    }
}

int EncodingInit(void)
{
    int iError;

    iError = AsciiEncodingInit();
    if (iError)
    {
        DBG_PRINTF("AsciiEncodingInit error!\n");
        return -1;
    }

    iError = Utf16leEncodingInit();
    if (iError)
    {
        DBG_PRINTF("Utf16leEncodingInit error!\n");
        return -1;
    }

    iError = Utf16beEncodingInit();
    if (iError)
    {
        DBG_PRINTF("Utf16beEncodingInit error!\n");
        return -1;
    }

    iError = Utf8EncodingInit();
    if (iError)
    {
        DBG_PRINTF("Utf8EncodingInit error!\n");
        return -1;
    }

    return 0;
}