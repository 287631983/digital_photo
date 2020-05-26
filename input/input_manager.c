#include <input_manager.h>
#include <config.h>
#include <pthread.h>

static PT_InputOpr g_ptInputOprHead;
static T_InputEvent g_tInputEvent;

static pthread_mutex_t g_tMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_tConVar = PTHREAD_COND_INITIALIZER;

int RegisterInputOpr(PT_InputOpr ptInputOpr)
{
	PT_InputOpr ptTemp;

	if (ptInputOpr == NULL)
	{
		return -1;
	}

	if (g_ptInputOprHead != NULL)
	{
		ptTemp = g_ptInputOprHead;
		while (ptTemp->ptNext != NULL)
		{
			ptTemp = ptTemp->ptNext;
		}
		ptTemp->ptNext = ptInputOpr;
		ptTemp->ptNext->ptNext = NULL;
	}
	else
	{
		g_ptInputOprHead = ptInputOpr;
		g_ptInputOprHead->ptNext = NULL;
	}

	return 0;
}

void ShowInputOpr(void)
{
	int i = 0;
	PT_InputOpr ptTemp = g_ptInputOprHead;

	while (ptTemp->ptNext != NULL)
	{
		printf("%02d %s\n", i++, ptTemp->name);
		ptTemp = ptTemp->ptNext;
	}
	printf("%02d %s\n", i++, ptTemp->name);
}

static void *InputEventThreadFunction(void *pVoid)
{
	T_InputEvent tInputEvent;

	// 定义一个函数指针
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	GetInputEvent = (int (*)(PT_InputEvent))(pVoid);

	while (1)
	{
		DBG_PRINTF("%s\n",__FUNCTION__);
		if (GetInputEvent(&tInputEvent) == 0)
		{
			// 上锁,如果有别的线程试图上锁,会休眠直到该线程解锁
			pthread_mutex_lock(&g_tMutex);
			g_tInputEvent = tInputEvent;

			// 唤醒主线程
			pthread_cond_signal(&g_tConVar);

			pthread_mutex_unlock(&g_tMutex);
		}
	}

	return NULL;
}

int AllInputDeviceInit(void)
{
	PT_InputOpr ptTemp = g_ptInputOprHead;
	int iError = -1;

	while (ptTemp != NULL)
	{
		if (ptTemp->DeviceInit() == 0)
		{
			// 创建输入检测子线程
			pthread_create(&ptTemp->tThreadID, NULL, InputEventThreadFunction, ptTemp->GetInputEvent);
			iError = 0;
		}
		if (ptTemp->ptNext != NULL)
		{
			ptTemp = ptTemp->ptNext;
		}
		else 
			break;
	}

	return iError;
}

// 查找所有已注册按键类中是否有触发
int GetInputEvent(PT_InputEvent ptInputEvent)
{
	DBG_PRINTF("%s\n",__FUNCTION__);
	// 上锁,如果有别的线程试图上锁,会休眠直到该线程解锁
	pthread_mutex_lock(&g_tMutex);
	pthread_cond_wait(&g_tConVar, &g_tMutex);

	// 被唤醒后, 返回数据
	*ptInputEvent = g_tInputEvent;
	pthread_mutex_unlock(&g_tMutex);

	return 0;
}

int InputInit(void)
{
	int iError;
	iError = StdinInit();
	// iError += TouchScreenInit();

	AllInputDeviceInit();
	return iError;
}