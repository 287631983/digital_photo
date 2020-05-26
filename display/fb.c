#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <disp_manager.h>
#include <config.h>

static int FBDeviceInit(void);
static int FBShowPixel(int iPenX, int iPenY, unsigned int dwColor);
static int FBCleanScreen(unsigned int dwBackColor);

static int g_iFBFd;
static struct fb_var_screeninfo g_tVar;
static struct fb_fix_screeninfo g_tFix;
static unsigned char *g_pucFbmem;
static int g_iScreenSize;
static int g_iLineWidth;
static int g_iPixelWidth;

/* 构造,设置,注册一个结构体 */
static T_DispOpr g_tFBDispOpr = {
    .name = "fb",
    .DeviceInit = FBDeviceInit,
    .ShowPixel = FBShowPixel,
    .CleanScreen = FBCleanScreen,
};

static int FBDeviceInit(void)
{
    g_iFBFd = open(FB_DEVICE_NAME, O_RDWR);
    if (g_iFBFd < 0)
    {
        DBG_PRINTF("can't open %s\n",FB_DEVICE_NAME);
        return -1;
    }

    if(ioctl(g_iFBFd, FBIOGET_VSCREENINFO, &g_tVar) != 0)
    {
        DBG_PRINTF("can't get val\n");
        return -1;
    }

    if(ioctl(g_iFBFd, FBIOGET_FSCREENINFO, &g_tFix) != 0)
    {
        DBG_PRINTF("can't get fix\n");
        return -1;
    }

    g_tFBDispOpr.iXres = g_tVar.xres;
    g_tFBDispOpr.iYres = g_tVar.yres;
    g_tFBDispOpr.iBpp  = g_tVar.bits_per_pixel;
    g_iScreenSize      = g_tVar.xres * g_tVar.yres * g_tVar.bits_per_pixel / 8;

    g_iLineWidth = g_tVar.xres * g_tVar.bits_per_pixel / 8;
    g_iPixelWidth = g_tVar.bits_per_pixel / 8;

    g_pucFbmem = (unsigned char *)mmap(NULL, g_iScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, g_iFBFd, 0);
    if(g_pucFbmem == (unsigned char *)-1)
    {
        DBG_PRINTF("can't mmap\n");
        return -1;
    }

    return 0;
}

/* dwColor:0xRRGGBB */
static int FBShowPixel(int iPenX, int iPenY, unsigned int dwColor)
{
    unsigned char *pucPen8 = g_pucFbmem + iPenY*g_iLineWidth + iPenX*g_iPixelWidth;
    unsigned short *puwPen16;
    unsigned int *pdwPen32;
    unsigned int iRed, iGreen, iBlue;
    puwPen16 = (unsigned short *)pucPen8;
    pdwPen32 = (unsigned int *)pucPen8;

    switch (g_tFBDispOpr.iBpp)
    {
        case 8:
        {
            *pucPen8 = dwColor;
            break;
        }
        case 16:
        {
            /* RGB565 */
            iRed = (dwColor >> 16) & 0xff;
            iGreen = (dwColor >> 8) & 0xff;
            iBlue = (dwColor >> 0) & 0xff;
            dwColor = ((iRed >> 3) << 11) | ((iGreen >> 2) << 5) | (iBlue >> 3);
            *puwPen16 = dwColor;
            break;
        }
        case 32:
        {
            *pdwPen32 = dwColor;
            break;
        }
        default:
        {
            DBG_PRINTF("can't support %dbpp for display pixel\n", g_tFBDispOpr.iBpp);
            return -1;
            break;
        }
    }
    return 0;
}

static int FBCleanScreen(unsigned int dwBackColor)
{
    int i;
    unsigned int iRed, iGreen, iBlue;
    unsigned char *pucPen8 = g_pucFbmem;
    unsigned short *puwPen16;
    unsigned int *pdwPen32;
    unsigned short wColor16bpp;
    puwPen16 = (unsigned short *)pucPen8;
    pdwPen32 = (unsigned int *)pucPen8;

    switch (g_tFBDispOpr.iBpp)
    {
        case 8:
        {
            memset(g_pucFbmem, dwBackColor, g_iScreenSize);
            break;
        }
        case 16:
        {
            /* RGB565 */
            iRed = (dwBackColor >> 16) & 0xff;
            iGreen = (dwBackColor >> 8) & 0xff;
            iBlue = (dwBackColor >> 0) & 0xff;
            dwBackColor = ((iRed >> 3) << 11) | ((iGreen >> 2) << 5) | (iBlue >> 3);
            for(i= 0; i < g_iScreenSize;i += 2)
            {
                *puwPen16 = dwBackColor;
                puwPen16 ++;
            }
            break;
        }
        case 32:
        {
            for(i = 0; i < g_iScreenSize; i += 4)
            {
                *pdwPen32 = dwBackColor;
                pdwPen32 ++;
            }
            break;
        }
        default:
        {
            DBG_PRINTF("can't support %dbpp for clean screen\n", g_tFBDispOpr.iBpp);
            return -1;
            break;
        }
    }
    return 0;
}

int FBInit(void)
{
    return RegisterDispOpr(&g_tFBDispOpr);
}
