#ifndef __INPUT_MANAGER_H
#define __INPUT_MANAGER_H
#include <pthread.h>
#include <sys/time.h>

#define TOUCH_INPUT_INTERVAL_MS		500

#define INPUT_TYPE_STDIN			0
#define INPUT_YPE_TOUCHSCREEN		1

#define INPUT_VALUE_UP				0
#define INPUT_VALUE_DOWN			1
#define INPUT_VALUE_EXIT			2
#define INPUT_VALUE_UNKNOWN			-1

typedef struct InputEvent{
	struct timeval tTime;
	int iType;
	int iVal;
}T_InputEvent, *PT_InputEvent;

typedef struct InputOpr{
	char *name;
	pthread_t tThreadID;
	int (*DeviceInit)(void);
	int (*DeviceExit)(void);
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	struct InputOpr *ptNext;
}T_InputOpr, *PT_InputOpr;

int RegisterInputOpr(PT_InputOpr ptInputOpr);
void ShowInputOpr(void);
int AllInputDeviceInit(void);
int GetInputEvent(PT_InputEvent ptInputEvent);
int StdinInit(void);
int TouchScreenInit(void);
int InputInit(void);

#endif