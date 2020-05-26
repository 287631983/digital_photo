#include <config.h>
#include <fonts_manager.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

static int FreeTypeFontInit(char *pcFontFile, unsigned int dwFontSize);
static int FreeTypeGetFontBitMap(unsigned int dwCode, PT_FontBitMap pt_FontBitMap);

static T_FontOpr g_tFreeTypeOpr = {
    .name = "freetype",
    .FontInit = FreeTypeFontInit,
    .GetFontBitMap = FreeTypeGetFontBitMap,
};

static FT_Library g_tLibrary;
static FT_Face g_tFace;
static FT_GlyphSlot g_tSlot;

static int FreeTypeFontInit(char *pcFontFile, unsigned int dwFontSize)
{
    int iError;

    iError = FT_Init_FreeType(&g_tLibrary);
    if (iError)
    {
        DBG_PRINTF("FT_Init_FreeType error!\n");
        return -1;
    }

    iError = FT_New_Face(g_tLibrary, pcFontFile, 0, &g_tFace);
    if (iError)
    {
        DBG_PRINTF("FT_New_Face error!\n");
        return -1;
    }

    g_tSlot = g_tFace->glyph;

    iError = FT_Set_Pixel_Sizes(g_tFace, dwFontSize,0);
    if (iError)
    {
        DBG_PRINTF("FT_Set_Pixel_Sizes error!\n");
        return -1;
    }

    return 0;
}

static int FreeTypeGetFontBitMap(unsigned int dwCode, PT_FontBitMap pt_FontBitMap)
{
    int iError;
    int iPenX = pt_FontBitMap->iCurOriginX;
    int iPenY = pt_FontBitMap->iCurOriginY;

    iError = FT_Load_Char(g_tFace, dwCode, FT_LOAD_RENDER);
    if (iError)
    {
        DBG_PRINTF("FT_Load_Char error!\n");
        return -1;
    }

    pt_FontBitMap->iXLeft   = iPenX + g_tSlot->bitmap_left;
    pt_FontBitMap->iYTop    = iPenY - g_tSlot->bitmap_top;
    pt_FontBitMap->iXMax    = pt_FontBitMap->iXLeft + g_tSlot->bitmap.width;
    pt_FontBitMap->iYMax    = pt_FontBitMap->iYTop + g_tSlot->bitmap.rows;
    pt_FontBitMap->iBpp     = 8;
    pt_FontBitMap->iPitch   = g_tSlot->bitmap.pitch;
    pt_FontBitMap->pucBuffer= g_tSlot->bitmap.buffer;

    pt_FontBitMap->iNextOriginX = iPenX + g_tSlot->advance.x / 64;
    pt_FontBitMap->iNextOriginY = iPenY + g_tSlot->advance.y / 64;

    return 0;
}

int FreeTypeInit(void)
{
    return RegisterFontOpr(&g_tFreeTypeOpr);
}