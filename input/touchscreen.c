#include <input_manager.h>
#include <disp_manager.h>
#include <stdlib.h>
#include <config.h>
#include <tslib.h>

static int TouchScreenDeviceInit(void);
static int TouchScreenDeviceExit(void);
static int TouchScreenGetInputEvent(PT_InputEvent ptInputEvent);

static T_InputOpr tTouchScreenInputOpr = {
	.name = "touchscreen",
	.DeviceInit = TouchScreenDeviceInit,
	.DeviceExit = TouchScreenDeviceExit,
	.GetInputEvent = TouchScreenGetInputEvent,
};

static struct tsdev *g_ptTsdev;
static int g_iXres;
static int g_iYres;

static int TouchScreenDeviceInit(void)
{
	char *pcTSName = NULL;

	if ((pcTSName = getenv("TSLIB_TSDEVICE")) != NULL)
	{
		// 以非阻塞方式打开
		g_ptTsdev = ts_open(pcTSName, 0);
	}
	else
	{
		g_ptTsdev = ts_open("/dev/event0", 1);
	}

	if (g_ptTsdev == NULL)
	{
		DBG_PRINTF("%s error!\n", __FUNCTION__);
		return -1;
	}

	if (ts_config(g_ptTsdev) != 0)
	{
		DBG_PRINTF("ts_config error!\n");
		return -1;
	}

	GetDispResolution(&g_iXres, &g_iYres);

	return 0;
}

static int TouchScreenDeviceExit(void)
{
	int iError;

	iError = ts_close(g_ptTsdev);

	return iError;
}

static int TimeInterval(struct timeval *ptPretime, struct timeval *ptThisTime)
{
	int iPreMs;
	int iThisMs;

	iPreMs = ptPretime->tv_sec * 1000 + ptPretime->tv_usec / 1000;
	iThisMs = ptThisTime->tv_sec * 1000 + ptThisTime->tv_usec / 1000;

	return (iThisMs - iPreMs);
}

static int TouchScreenGetInputEvent(PT_InputEvent ptInputEvent)
{
	int iRet;
	struct ts_sample tSamp;

	static struct timeval tPreTime;

	// 如果没有数据就休眠
	iRet = ts_read(g_ptTsdev, &tSamp, 1);

	if (iRet < 0)
	{
		return -1;
	}

	if (TimeInterval(&tPreTime, &tSamp.tv) >= TOUCH_INPUT_INTERVAL_MS)
	{
		tPreTime = tSamp.tv;
		ptInputEvent->iType = INPUT_YPE_TOUCHSCREEN;
		ptInputEvent->tTime = tSamp.tv;

		if (tSamp.y < (g_iYres/3))
		{
			ptInputEvent->iVal = INPUT_VALUE_UP;
		}
		else if (tSamp.y > (g_iYres/3*2))
		{
			ptInputEvent->iVal = INPUT_VALUE_DOWN;
		}
		else
		{
			ptInputEvent->iVal = INPUT_VALUE_UNKNOWN;
		}

		return 0;
	}
	else 
	{
		return -1;
	}
}

int TouchScreenInit(void)
{
	return RegisterInputOpr(&tTouchScreenInputOpr);
}