#ifndef __DRAW_H
#define __DRAW_H

int OpenTextFile(char *pcFileName);
int SetTexDetail(char *pucHZKFile, char *pcFileFreetype, unsigned int dwFontSize);
int SelectAnInitDisplay(char *pcName);
int ShowNextPage(void);
int ShowPrePage(void);

#endif