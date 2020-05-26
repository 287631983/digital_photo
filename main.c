#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <draw.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <string.h>

/* ./show_file [-s Size] [-f freetype_font_file] [-h HZK] <text_file> */
int main(int argc, char *argv[])
{
    int iError;
    int bList = 0;
    char cInput;
    unsigned int dwFontSize = 16;
    char acHZKFile[128];
    char acFreeTypeFile[128];
    char acTextFile[128];

    char acDisplay[128];

    T_InputEvent tInputEvent;

    acHZKFile[0] = '\0';
    acFreeTypeFile[0] = '\0';
    acTextFile[0] = '\0';

    strcpy(acDisplay, "fb");

    // 字符后面不跟:表示该字符是选项,后面可以不加参数 字符后面跟:表示选项后必须加一个参数
    while ((iError = getopt(argc, argv, "ls:f:h:d:")) != -1)
    {
        switch (iError)
        {
            case 'l':
            {
                bList = 1;
                break;
            }
            case 's':
            {
                dwFontSize = strtoul(optarg, NULL, 0);
                break;
            }
            case 'f':
            {
                strncpy(acFreeTypeFile, optarg, 128);
                acFreeTypeFile[127] = '\0';
                break;
            }
            case 'h':
            {
                strncpy(acHZKFile, optarg, 128);
                acHZKFile[127] = '\0';
                break;
            }
            case 'd':
            {
                strncpy(acDisplay, optarg, 128);
                acDisplay[127] = '\0';
                break;
            }
            default:
            {
                printf("Usage: %s [-s size] [-d display] [-f font_file] [-h HZK] <file>\n", argv[0]);
                printf("Usage: %s -l\n", argv[0]);
                return -1;
            }
        }
    }

    if ((bList == 0) && (optind >= argc))
    {
        printf("Usage: %s [-s Size] [-d display] [-f font_file] [-h HZK] <text_file>\n", argv[0]);
		printf("Usage: %s -l\n", argv[0]);
		return -1;
    }
#if 0
    iError = DisplayInit();
    if (iError < 0)
    {
        printf("DisplayInit error!\n");
        return -1;
    }
#endif
    iError = FontsInit();
    if (iError < 0)
    {
        printf("FontsInit error!\n");
        return -1;
    }

    iError = EncodingInit();
    if (iError < 0)
    {
        printf("EncodingInit error!\n");
        return -1;
    }

    iError = InputInit();
    if (iError < 0)
    {
        printf("InputInit error!\n");
        return -1;
    }

    if (bList)
    {
        printf("Supported display:\n");
        ShowDispOpr();

        printf("Supported fonts:\n");
        ShowFontOpr();

        printf("Support encoding:\n");
        ShowEncoingOpr();

        printf("Supported input:\n");
        ShowInputOpr();

        return 0;
    }

    strncpy(acTextFile, argv[optind], 128);
    acTextFile[127] = '\0';
    iError = OpenTextFile(acTextFile);
    if (iError < 0)
    {
        printf("can't open %s!\n",acTextFile);
        return -1;
    }
#if 0
    iError = SetTexDetail(acHZKFile, acFreeTypeFile, dwFontSize);
    if (iError < 0)
    {
        printf("SetTextDetail error!\n");
		return -1;
    }

    iError = SelectAnInitDisplay(acDisplay);
    if (iError < 0)
	{
		printf("SelectAndInitDisplay error!\n");
		return -1;
	}

    iError = ShowNextPage();
    if (iError)
	{
		printf("Error to show first page\n");
		return -1;
	}
  #endif
    while(1)
    {
        if (GetInputEvent(&tInputEvent) == 0)
        {
            if (tInputEvent.iType == INPUT_VALUE_UP)
            {
                // ShowPrePage();
                DBG_PRINTF("show previous page\n");
            }
            else if (tInputEvent.iType == INPUT_VALUE_DOWN)
            {
                // ShowNextPage();
                DBG_PRINTF("show next page\n");
            }
            else if (tInputEvent.iType == INPUT_VALUE_EXIT)
            {
                return 0;
            }
        }
    }

    return 0;
}