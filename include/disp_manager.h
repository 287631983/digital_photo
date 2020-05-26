#ifndef __DISP_MANAGER_H
#define __DISP_MANAGER_H

typedef struct DispOpr{
    char *name;
    int iXres;
    int iYres;
    int iBpp;
    int (*DeviceInit)(void);
    int (*ShowPixel)(int iPenX, int iPenY, unsigned int dwColor);
    int (*CleanScreen)(unsigned int dwBackColor);
    struct DispOpr *ptnext;
}T_DispOpr,*PT_DispOpr;

int FBInit(void);
int RegisterDispOpr(PT_DispOpr ptDispOpr);
PT_DispOpr GetDispOpr(char *pcName);
int DisplayInit(void);
void ShowDispOpr(void);
void GetDispResolution(int *piX, int *piY);

#endif