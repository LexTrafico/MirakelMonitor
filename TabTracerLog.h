#pragma once

#include <Windows.h>
#include "MirakelMonitorUtilities.h"


#define tracerzoom_SEC 10
#define tracerzoom_TSEC 1
#define tracerzoom_HSEC 5

/* Defines voor onderscheid in uitlezen Faselog array */
#define FASESTATUS 			0
#define REALISATIESTATUS 	1
#define LOGITEMS_MAX		2


LRESULT CALLBACK WindowProcTabTracerLog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void TabTracerLogUpdate();
void TabTracerLogReset();
void tabDebugTracer_key(HWND hWnd, DWORD wKey);
void tabDebugTracer_newTrace(HWND hWnd, int _type, int _elem);
void tabDebugTracer_newTrace_create(HWND hWnd, int _type, int _elem, int _place);
void tabDebugTracer_deleteTrace(int id);
void tabDebugTracer_paint_trace(HDC hDC, INT trace, INT left, INT top, INT right, INT bottom);
void SetItemWindowsPos(int i);
