#pragma once

#include <Windows.h>
#include "MirakelMonitorUtilities.h"

void TabTracerLogLabelPanel_drawLabels(HDC hDC, int type, int elem, int fc_height);
HWND CreateTabTracerLogLabelPanel(HWND hwnd, HINSTANCE hInstance, RECT * r, TRACERLAYOUT * tracerl, TRACERSTRUCT ** tr);
LRESULT CALLBACK WindowProcTabTracerLogLabelPanel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
