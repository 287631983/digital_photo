#include <draw.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <config.h>
#include <disp_manager.h>
#include <encoding_manager.h>

typedef struct PageDesc{
    int iPage;
    unsigned char *pucLcdFirstPosAtFile;
    unsigned char *pucLcdNextPageFirstPosAtFile;
    struct PageDesc *ptPrePage;
    struct PageDesc *ptNextPage;
}T_PageDesc, *PT_PageDesc;

static int g_iFdTextFile;
static unsigned char *g_pucTextFileMem;
static unsigned char *g_pucTextFimeMemEnd;
static PT_EncodingOpr g_ptEncodingOprForFile;

static unsigned char *g_pucLcdFirstPosAtFile;
static unsigned char *g_pucLcdNextPosAtFile;

static PT_DispOpr g_ptDispOpr;

static int g_dwFontSize;

static PT_PageDesc g_ptPages = NULL;
static PT_PageDesc g_ptCurPage = NULL;

int OpenTextFile(char *pcFileName)
{
    struct stat tstat;

    g_iFdTextFile = open(pcFileName, O_RDONLY);

    if (g_iFdTextFile < 0)
    {
        DBG_PRINTF("can't opennnn %s for %s\n",pcFileName,strerror(errno));
        return -1;
    }

    if (fstat(g_iFdTextFile,&tstat))
    {
        DBG_PRINTF("can't get fstat!\n");
        return -1;
    }
    g_pucTextFileMem = (unsigned char *)mmap(NULL, tstat.st_size, PROT_READ, MAP_SHARED, g_iFdTextFile, 0);
    if (g_pucTextFileMem == (unsigned char *)-1)
    {
        DBG_PRINTF("can't mmap for line:%d\n",__LINE__);
        return -1;
    }

    g_pucTextFimeMemEnd = g_pucTextFileMem + tstat.st_size;

    g_ptEncodingOprForFile = SelectEncodingOprForFIle(g_pucTextFileMem);

    if (g_ptEncodingOprForFile)
    {
        g_pucLcdFirstPosAtFile = g_pucTextFileMem + g_ptEncodingOprForFile->iHeadLen;
        return 0;
    }
    else
    {
        DBG_PRINTF("g_ptEncodingOprForFile is NULL!\n");
        return -1;
    }
}

int SetTexDetail(char *pucHZKFile, char *pcFileFreetype, unsigned int dwFontSize)
{
    int iError = 0;
    int iRet = -1;
    PT_FontOpr ptFontOpr;
    PT_FontOpr ptFontTemp;

    g_dwFontSize = dwFontSize;

    ptFontOpr = g_ptEncodingOprForFile->ptFontOprSupportedHead;
    while (ptFontOpr != NULL)
    {
        if (strcmp(ptFontOpr->name, "ascii") == 0)
        {
            iError = ptFontOpr->FontInit(NULL, dwFontSize);
        }
        else if (strcmp(ptFontOpr->name, "gbk") == 0)
        {
            iError = ptFontOpr->FontInit(pucHZKFile, dwFontSize);
        }
        else
        {
            iError = ptFontOpr->FontInit(pcFileFreetype, dwFontSize);
        }

        ptFontTemp = ptFontOpr->ptNext;

        if (iError == 0)
        {
            iRet = 0;
        }
        else
        {
            DelFontOprFrmEncoding(g_ptEncodingOprForFile, ptFontOpr);
        }

        ptFontOpr = ptFontTemp;
    }

    return iRet;
}

int SelectAnInitDisplay(char *pcName)
{
    int iError;

    if (pcName == NULL)
    {
        DBG_PRINTF("no param for %s\n",__FUNCTION__);
    }

    g_ptDispOpr = GetDispOpr(pcName);
    if (!g_ptDispOpr)
    {
        return -1;
    }

    iError = g_ptDispOpr->DeviceInit();

    return iError;
}

int IncLcdX(int iX)
{
    if (iX + 1 < g_ptDispOpr->iXres)
    {
        return (iX + 1);
    }
    else
    {
        return 0;
    }
}

int IncLcdY(int iY)
{
    if (iY + g_dwFontSize < g_ptDispOpr->iYres)
    {
        return (iY + g_dwFontSize);
    }
    else
    {
        return 0;
    }
}

int RelocateFontsPos(PT_FontBitMap ptFontBitMap)
{
    int iLcdY;
    
    // 横坐标的变动值
    int iDeltaX;
    // 纵坐标的变动值
    int iDeltaY;

    // 满页了
    if (ptFontBitMap->iYMax > g_ptDispOpr->iYres)
    {
        return -1;
    }

    //超过右边界
    if (ptFontBitMap->iXMax > g_ptDispOpr->iXres)
    {
        // 需要换行
        iLcdY = IncLcdY(ptFontBitMap->iCurOriginY);
        // 满页了
        if (iLcdY == 0)
        {
            return -1;
        }
        //未满页
        else
        {
            // 先求出横坐标和纵坐标的变动值,然后在所有的数值上面加上变动值就是更新后的值
            iDeltaX = 0 - ptFontBitMap->iCurOriginX;
            iDeltaY = iLcdY - ptFontBitMap->iCurOriginY;

            ptFontBitMap->iCurOriginX   += iDeltaX;
            ptFontBitMap->iCurOriginY   += iDeltaY;

            ptFontBitMap->iNextOriginX  += iDeltaX;
            ptFontBitMap->iNextOriginY  += iDeltaY;

            ptFontBitMap->iXMax         += iDeltaX;
            ptFontBitMap->iYMax         += iDeltaY;

            ptFontBitMap->iXLeft        += iDeltaX;
            ptFontBitMap->iYTop         += iDeltaY;

            return 0;
        }
    }

    return 0;
}

int ShowOneFont(PT_FontBitMap ptFontBitMap)
{
    int iX;
    int iY;
    unsigned char ucByte = 0;
    int i = 0;
    int iBit;

    // 每个像素1个bit
    if (ptFontBitMap->iBpp == 1)
    {
        for (iY = ptFontBitMap->iYTop; iY < ptFontBitMap->iYMax; iY++)
		{
			i = (iY - ptFontBitMap->iYTop) * ptFontBitMap->iPitch;
            printf("i%d\n",i);
			for (iX = ptFontBitMap->iXLeft, iBit = 7; iX < ptFontBitMap->iXMax; iX++)
			{
				if (iBit == 7)
				{
					ucByte = ptFontBitMap->pucBuffer[i++];
				}
				
				if (ucByte & (1<<iBit))
				{
					g_ptDispOpr->ShowPixel(iX, iY, COLOR_FOREGROUND);
				}
                
				iBit --;
				if (iBit == -1)
				{
					iBit = 7;
				}
			}
		}
    }
    // 每个像素1个byte
    else if (ptFontBitMap->iBpp == 8)
    {
        for (iY = ptFontBitMap->iYTop; iY < ptFontBitMap->iYMax; iY ++)
        {
            for (iX = ptFontBitMap->iXLeft; iX < ptFontBitMap->iXMax; iX ++)
            {
                if(ptFontBitMap->pucBuffer[i++])
                    g_ptDispOpr->ShowPixel(iX, iY, COLOR_FOREGROUND);
            }
        }
    }
    else
    {
        DBG_PRINTF("can't support %d Bpp!\n",ptFontBitMap->iBpp);
        return -1;
    }

    return 0;
}

int ShowOnePage(unsigned char *pucTextFileMemCurPos)
{
    int iError;
    int iLen;
    int bHasCleanScreen = 0;                // 清屏标识
    unsigned int dwCode;
    unsigned char *pucBufStart;
    PT_FontOpr ptFontOpr;
    T_FontBitMap tFontBitMap;

    pucBufStart = pucTextFileMemCurPos;

    tFontBitMap.iCurOriginX = 0;
    tFontBitMap.iCurOriginY = g_dwFontSize;

    while (1)
    {
        iLen = g_ptEncodingOprForFile->GetCodeFrmBuf(pucBufStart, g_pucTextFimeMemEnd, &dwCode);
        // 文件处理完毕
        if (iLen == 0)
        {
            return 0;
        }

        pucBufStart += iLen;
        // windows下\n\r表示回车,linux下\n表示回车,所以可以忽略\r
        if (dwCode == '\n')
        {
            g_pucLcdNextPosAtFile = pucBufStart;

            // 回车换行
            tFontBitMap.iCurOriginX = 0;
            tFontBitMap.iCurOriginY = IncLcdY(tFontBitMap.iCurOriginY);
            // 已经显示到最后一行了
            if (tFontBitMap.iCurOriginY == 0)
            {
                return 0;
            }
        }
        else if (dwCode == '\r')
        {
            continue;
        }
        else if(dwCode == '\t')
        {
            // TAB用空格表示
            dwCode = ' ';
        }

        ptFontOpr = g_ptEncodingOprForFile->ptFontOprSupportedHead;
        while (ptFontOpr != NULL)
        {
            iError = ptFontOpr->GetFontBitMap(dwCode, &tFontBitMap);
            if (iError == 0)
            {
                if (RelocateFontsPos(&tFontBitMap) < 0)
                {
                    // 当前屏幕内容已满
                    return 0;
                }

                // 如果未清屏, 需要先清屏
                if (bHasCleanScreen == 0)
                {
                    g_ptDispOpr->CleanScreen(COLOR_BACKGROUND);
                    bHasCleanScreen = 1;
                }

                // 显示一个字符
                if (ShowOneFont(&tFontBitMap))
                {
                    DBG_PRINTF("show font error!\n");
                    return -1;
                }

                tFontBitMap.iCurOriginX = tFontBitMap.iNextOriginX;
                tFontBitMap.iCurOriginY = tFontBitMap.iNextOriginY;
                g_pucLcdNextPosAtFile = pucBufStart;

                // 显示完一个字符
                break;
            }
            ptFontOpr = ptFontOpr->ptNext;
        }
    }

    return 0;
}

/* 将页结构体加入到页链表的最后一项 */
static void RecordPage(PT_PageDesc ptPageNew)
{
    PT_PageDesc ptPage;

    if (g_ptPages == NULL)
    {
        g_ptPages = ptPageNew;
    }
    else
    {
        ptPage = g_ptPages;
        while (ptPage->ptNextPage != NULL)
        {
            ptPage = ptPage->ptNextPage;
        }
        ptPage->ptNextPage = ptPageNew;
        ptPageNew->ptPrePage = ptPage;
    }
}

int ShowNextPage(void)
{
    int iError;
    PT_PageDesc ptPage;
    unsigned char *pucTextFileMemCurPos;

    if (g_ptCurPage != NULL)
    {
        pucTextFileMemCurPos = g_ptCurPage->pucLcdNextPageFirstPosAtFile;
    }
    else
    {
        pucTextFileMemCurPos = g_pucLcdFirstPosAtFile;
    }
    
    iError = ShowOnePage(pucTextFileMemCurPos);
    if (iError == 0)
    {
        if (g_ptCurPage && g_ptCurPage->ptNextPage)
        {
            g_ptCurPage = g_ptCurPage->ptNextPage;
            return 0;
        }

        ptPage = (PT_PageDesc)malloc(sizeof(struct PageDesc));
        if (ptPage == NULL)
        {
            return -1;
        }
        else
        {
            ptPage->ptNextPage                      = NULL;
            ptPage->ptPrePage                       = NULL;
            ptPage->pucLcdFirstPosAtFile            = pucTextFileMemCurPos;
            ptPage->pucLcdNextPageFirstPosAtFile    = g_pucLcdNextPosAtFile;
            g_ptCurPage = ptPage;
            RecordPage(g_ptCurPage);
            return 0;
        }
    }

    return iError;
}

int ShowPrePage(void)
{
    int iError;

    DBG_PRINTF("%s\n",__FUNCTION__);

    if (!g_ptCurPage || !g_ptCurPage->ptPrePage)
    {
        return -1;
    }

    iError = ShowOnePage(g_ptCurPage->ptPrePage->pucLcdFirstPosAtFile);
    if (iError == 0)
    {
        g_ptCurPage = g_ptCurPage->ptPrePage;
    }

    return iError;
}