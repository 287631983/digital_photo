#include <fonts_manager.h>
#include <config.h>

static PT_FontOpr g_ptFontOprHead = NULL;

// 将FontOpr结构体添加到链表末尾
int RegisterFontOpr(PT_FontOpr ptFontOpr)
{
    PT_FontOpr ptTemp;

    if (g_ptFontOprHead == NULL)
    {
        g_ptFontOprHead = ptFontOpr;
        g_ptFontOprHead->ptNext = NULL;
    }
    else
    {
        ptTemp = g_ptFontOprHead;
        while (ptTemp->ptNext != NULL)
        {
            ptTemp = ptTemp->ptNext;
        }
        ptTemp->ptNext = ptFontOpr;
        ptTemp->ptNext->ptNext = NULL;
    }
    
    return 0;
}

// 遍历所有注册FontOpr结构体并打印name属性
void ShowFontOpr(void)
{
    int i = 0;
    PT_FontOpr ptTemp = g_ptFontOprHead;

    while(ptTemp->ptNext != NULL)
    {
        printf("%02d %s\n", i++, ptTemp->name);
        ptTemp = ptTemp->ptNext;
    }
    printf("%02d %s\n", i++, ptTemp->name);
}

// 根据name属性查找FontOpr对象
PT_FontOpr GetFontOpr(char *pcName)
{
    PT_FontOpr ptTemp = g_ptFontOprHead;

    while (ptTemp != NULL)
    {
        if (strcmp(ptTemp->name, pcName) == 0)
        {
            return ptTemp;
        }
        ptTemp = ptTemp->ptNext;
    }

    return NULL;
}

int FontsInit(void)
{
    int iError;

    // iError = ASCIIInit();
    // if (iError)
    // {
    //     DBG_PRINTF("ASCIIInit error!\n");
    //     return -1;
    // }

    // iError = GBKInit();
    // if (iError)
    // {
    //     DBG_PRINTF("GBKInit error!\n");
    //     return -1;
    // }

    iError = FreeTypeInit();
    if (iError)
    {
        DBG_PRINTF("FreeTypeInit error!\n");
        return -1;
    }

    return 0;
}