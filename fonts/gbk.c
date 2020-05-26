#include <fonts_manager.h>
#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

static int GBKFontInit(char *pcFontFile, unsigned int dwFontSize);
static int GBKGetFontBitMap(unsigned int dwCode, PT_FontBitMap pt_FontBitMap);

static T_FontOpr g_tGBKFontOpr = {
    .name = "gbk",
    .FontInit = GBKFontInit,
    .GetFontBitMap = GBKGetFontBitMap,
};

static unsigned char *g_pucHZKMem;
static unsigned char *g_pucHZKMemEnd;

static int GBKFontInit(char *pcFontFile, unsigned int dwFontSize)
{
    int iFd;
    struct stat tStat;

    if (dwFontSize != 16)
    {
        DBG_PRINTF("GBK only support 16 font size\n");
        return -1;
    }

    iFd = open(pcFontFile, O_RDONLY);
    if (iFd < 0)
    {
        DBG_PRINTF("can't open %s !\n", pcFontFile);
        return -1;
    }

    if (fstat(iFd, &tStat) < 0)
    {
        DBG_PRINTF("can't get fstat!\n");
        return -1;
    }

    g_pucHZKMem = (unsigned char *)mmap(NULL, tStat.st_size, PROT_READ, MAP_SHARED, iFd, 0);
    if (g_pucHZKMem == (unsigned char *)-1)
    {
        DBG_PRINTF("can't mmap for hzk16\n");
        return -1;
    }

    g_pucHZKMemEnd = g_pucHZKMem + tStat.st_size;

    return 0;
}

static int GBKGetFontBitMap(unsigned int dwCode, PT_FontBitMap pt_FontBitMap)
{
    int iArea;
    int iWhere;
    int iPenX = pt_FontBitMap->iCurOriginX;
    int iPenY = pt_FontBitMap->iCurOriginY;

    if (dwCode > 0xffff)
    {
        DBG_PRINTF("GBK don't support this code:0x%x\n",dwCode);
        return -1;
    }

    iArea = (int)(dwCode & 0xff) - 0xa1;
    iWhere = (int)((dwCode >> 8) & 0xff) - 0xa1;

    if ((iArea < 0) || (iWhere < 0))
    {
        DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    pt_FontBitMap->iXLeft = iPenX;
    pt_FontBitMap->iYTop = iPenY - 16;
    pt_FontBitMap->iXMax = iPenX + 16;
    pt_FontBitMap->iYMax = iPenY;
    pt_FontBitMap->iBpp = 1;
    pt_FontBitMap->iPitch = 2;
    pt_FontBitMap->pucBuffer = g_pucHZKMem + (iArea * 0xa1 + iWhere) * 32;

    if(pt_FontBitMap->pucBuffer >= g_pucHZKMemEnd)
    {
        return -1;
    }

    pt_FontBitMap->iNextOriginX = iPenX + 16;
    pt_FontBitMap->iNextOriginY = iPenY;

    return 0;
}

int GBKInit(void)
{
    return RegisterFontOpr(&g_tGBKFontOpr);
}