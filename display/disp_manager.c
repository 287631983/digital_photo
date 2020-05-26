#include <config.h>
#include <disp_manager.h>

static PT_DispOpr g_ptDispOprHead;

// 将目标结构体放入注册链表末尾
int RegisterDispOpr(PT_DispOpr ptDispOpr)
{
    PT_DispOpr ptTemp;

    if (g_ptDispOprHead == NULL)
    {
        g_ptDispOprHead = ptDispOpr;
        g_ptDispOprHead->ptnext = NULL;
    }
    else
    {
        ptTemp = g_ptDispOprHead;
        while (ptTemp->ptnext != NULL)
        {
            ptTemp = ptTemp->ptnext;
        }
        ptTemp->ptnext = ptDispOpr;
        ptTemp->ptnext->ptnext = NULL;
    }

    return 0;
}

// 依次显示所有注册过的结构体的名称属性
void ShowDispOpr(void)
{
    int i = 0;
    PT_DispOpr ptTemp = g_ptDispOprHead;

    while (ptTemp->ptnext != NULL)
    {
        printf("%02d %s\n", i++, ptTemp->name);
        ptTemp = ptTemp->ptnext;
    }
    printf("%02d %s\n", i++, ptTemp->name);
}

// 根据提供的名称属性返回对应的注册结构体
PT_DispOpr GetDispOpr(char *pcName)
{
    PT_DispOpr ptTemp = g_ptDispOprHead;

    while (ptTemp != NULL)
    {
        if (strcmp(ptTemp->name,pcName) == 0)
        {
            return ptTemp;
        }
        ptTemp = ptTemp->ptnext;
    }
    
    return NULL;
}

void GetDispResolution(int *piX, int *piY)
{
    *piX = g_ptDispOprHead->iXres;
    *piX = g_ptDispOprHead->iYres;
}

// 将DispOpr结构体添加到链表末尾
int DisplayInit(void)
{
    int iError;

    iError = FBInit();

    return iError;
}