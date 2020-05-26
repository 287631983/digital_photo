#include <input_manager.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

static int StdinDeviceInit(void);
static int StdinDeviceExit(void);
static int StdinInputEvent(PT_InputEvent ptInputEvent);

static T_InputOpr g_tInputOpr = {
	.name = "stdin",
	.DeviceInit = StdinDeviceInit,
	.DeviceExit = StdinDeviceExit,
	.GetInputEvent = StdinInputEvent,
};

static int StdinDeviceInit(void)
{
	struct termios tTTYState;

	// 获得标准输入的属性
	tcgetattr(STDIN_FILENO, &tTTYState);

	// 关闭标准模式
	tTTYState.c_lflag &= ~ICANON;

	// 有一个数据就立刻返回
	tTTYState.c_cc[VMIN] = 1;

	// TCSANOW 立即生效
	tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);

	return 0;
}

static int StdinDeviceExit(void)
{
	struct termios tTTYState;

	tcgetattr(STDIN_FILENO, &tTTYState);

	tTTYState.c_lflag |= ICANON;

	tcsetattr(STDIN_FILENO, TCSANOW, &tTTYState);

	return 0;
}

static int StdinInputEvent(PT_InputEvent ptInputEvent)
{
	// 如果有数据就读取,否则立刻返回
	char c;
	ptInputEvent->iType = INPUT_TYPE_STDIN;

	c = fgetc(stdin);
	gettimeofday(&ptInputEvent->tTime, NULL);

	if (c == 'w')
	{
		ptInputEvent->iType = INPUT_VALUE_UP;
	}
	else if (c == 's')
	{
		ptInputEvent->iType = INPUT_VALUE_DOWN;
	}
	else if (c == 'q')
	{
		ptInputEvent->iType = INPUT_VALUE_EXIT;
	}
	else 
	{
		ptInputEvent->iType = INPUT_VALUE_UNKNOWN;
	}

	return 0;
}

int StdinInit(void)
{
	return RegisterInputOpr(&g_tInputOpr);
}