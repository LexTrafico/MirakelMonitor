#pragma once

#include <Windows.h>
#include "MirakelMonitorUtilities.h"

#define TRACERBUFFERSIZE	3600
#define	TRACES_MAX			25

/* Variabelen */
static int iTracerTop, iTracerBottom, iTracerLeft, iTracerRight;
static int trlogtempsec, trlogtime;
char trlogtimestamp[128];
struct tracerstruct {
	char	type;
	int		elem;
	char	log[TRACERBUFFERSIZE][2];
	int		place;
	HWND	delwin;
	HWND	upwin;
	HWND	downwin;
};
typedef struct tracerstruct TRACERSTRUCT;
static TRACERSTRUCT * tracer[TRACES_MAX];
static int tracer_id = 0;
static int tracer_count = 0;
static int trindex = 0;
static char tracer_bloklog[TRACERBUFFERSIZE];
static char tracerchanged = 0;

#define tracerzoom_SEC 10
#define tracerzoom_TSEC 1
#define tracerzoom_HSEC 5
static int tracerzoom;

/* Window handles */
static HWND hDebugCBtype;
static HWND hDebugCBelem;
static HWND hLogZoomin;
static HWND hLogZoomout;
static HWND hLogZoomswitch;

/* Variabelen voor tracer layout */
static HBRUSH tracer_brush[12];
static struct tracerlayoutstruct
{
	int timestep;		// pixels per stape in de tijd
	int timeline;		// per hoeveel eenheden tijd staat een lijn
	int fc_height;		// hoogte per fase/element
	int cg_height;		// hoogte voor weergave fase status
	int rs_height;		// hoogte voor weergave realisatie status
	int pad_left;		// padding left
	int pad_right;		// padding right
	int pad_top;		// padding top
	int pad_bottom;		// padding bottom
	int rv_line_top;	// hoogte vanaf top van deze fase tot bovenkant weergave RV[]
	int rv_line_bottom;	// hoogte tot onderkant weergave RV[] streep: bepaalt samen de dikte
	int text_top;		// plaatsing van de tijd label boven de balken
} tracerlayout;

/* Defines voor onderscheid in uitlezen Faselog array */
#define FASESTATUS 			0
#define REALISATIESTATUS 	1
#define LOGITEMS_MAX		2

/* Defines voor aanmaken Debug entries */
/* Volgorde niet wijzigen ivw followTab */
#define Fase		0
#define Detector	1
#define Counter		2
#define Timer		3
#define Ingang		4
#define Uitgang		5
#define Hulpelement	6
#define Schakelaar	7
#define Parameter	8
#define Memoryelem	9
#define Typenmax	10

LRESULT CALLBACK WindowProcTabTracerLog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void TabTracerLogUpdate();
void TabTracerLogReset();
void tabDebugTracer_key(HWND hWnd, DWORD wKey);
void tabDebugTracer_newTrace(HWND hWnd, int _type, int _elem);
void tabDebugTracer_newTrace_create(HWND hWnd, int _type, int _elem, int _place);
void tabDebugTracer_deleteTrace(int id);
void tabDebugTracer_paint_trace(HDC hDC, INT trace, INT left, INT top, INT right, INT bottom);
void SetItemWindowsPos(int i);
