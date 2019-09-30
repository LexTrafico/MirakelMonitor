#pragma once

#include <Windows.h>
#include "MirakelMonitorUtilities.h"

HWND CreateTabTracerLogLoggerPanel(HWND hwnd, HINSTANCE hInstance, RECT * r, TRACERLAYOUT * tracerl, TRACERSTRUCT ** tr);
LRESULT CALLBACK WindowProcTabTracerLogLoggerPanel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void tabDebugTracer_paint_trace(HDC hDC, INT trace, INT left, INT top, INT right, INT bottom);
