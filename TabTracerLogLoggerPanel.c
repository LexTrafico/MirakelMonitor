HWND CreateTabTracerLogLoggerPanel(HWND hwnd, HINSTANCE hInstance);
LRESULT CALLBACK WindowProcTabTracerLogLoggerPanel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND CreateTabTracerLogLoggerPanel(HWND hwnd, HINSTANCE hInstance)
{
	RECT r;
	RegisterSomeClass(hInstance, "TabTracerLogLoggerPanelClass", (WNDPROC)WindowProcTabTracerLogLoggerPanel);
	int e = GetLastError();
	GetClientRect(hwnd, &r);
	HWND hWnd = CreateWindow(
		"TabTracerLogLoggerPanelClass", "",
		WS_CHILD,
		r.left, r.top, r.right - r.left - 200, r.bottom - r.top,
		hwnd, NULL, hInstance, NULL);
	e = GetLastError();
	return hWnd;
}


LRESULT CALLBACK WindowProcTabTracerLogLoggerPanel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
}