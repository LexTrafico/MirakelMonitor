#pragma once

#include <Windows.h>
#include "MirakelMonitorUtilities.h"

#define TFBMAXMEASURE 250

struct TFBMonitorStruct
{
	int iTFBTimer;
	int iTFBMax;
	int iTFBGemiddeld;
	int * iTFBMeasured;
	int iTFBCount;
	int iTFBCurrent;
	unsigned long ulTFBTotal;
	char bOverstaan;
	short iMKStartTime, iMKEndTime;
	unsigned long ulTotalTime;
};
typedef struct TFBMonitorStruct TFBMONSTRUCT;
TFBMONSTRUCT * pstrTFBMonitor;

static struct TFBMonitorContent
{
	int iShowFasestat;
	char lpszFasestat[128];
	HWND * hwndTip;
	TOOLINFO * ti;
} strTFBContent;

static int iTFBTop, iTFBBottom, iTFBLeft, iTFBRight;
static int iTFBHoogsteooit = 0;

LRESULT CALLBACK WindowProcTabWachttijden(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void TabWachttijdenUpdate();
void TabWachttijdenReset();
