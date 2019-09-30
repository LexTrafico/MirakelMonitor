#pragma once

#include <Windows.h>
#include "Windowsx.h"
#include <CommCtrl.h>
#include "CCOLDefines.h"
#include "DataGridView.h"

typedef enum MirakelColorsEnum
{
	BRUSH_RED,
	BRUSH_DARKRED,
	BRUSH_GREEN,
	BRUSH_DARKGREEN,
	BRUSH_BLUE,
	BRUSH_DARKBLUE,
	BRUSH_YELLOW,
	BRUSH_ORANGE,
	BRUSH_MAGENTA,
	BRUSH_TEAL,
	BRUSH_WHITE,
	BRUSH_BLACK,
	BRUSH_GRAY,
	BRUSH_LIGHTGRAY,
	BRUSH_LIGHTBLUE,
	BRUSH_DARKGRAY,
	BRUSH_NULL,
	BRUSH_MAX
};

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

typedef struct tracerlayoutstruct
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
} TRACERLAYOUT;

#define TRACERBUFFERSIZE	3600
#define	TRACES_MAX			25
#define tracerzoom_SEC 10
#define tracerzoom_TSEC 1
#define tracerzoom_HSEC 5

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

extern HBRUSH hBrushMirakel[BRUSH_MAX];
extern HBRUSH hBrushCG[_CG_MAX + 1];
extern HPEN hPenMirakel[BRUSH_MAX];
extern HPEN hPenCG[_CG_MAX + 1];
extern HWND hMainTab;
extern HINSTANCE hMainInstance;

extern int iCharWidth, iCharHeight;

extern COLORREF greenprtext;
extern COLORREF greenartext;
extern COLORREF redtext;
extern COLORREF whitetext;
extern COLORREF blacktext;

#define SZBUFFERSIZE 128

extern char lpszTemp1[SZBUFFERSIZE];

void Initialize(HWND hWnd);
void CreateBrushes();
HWND CreateTabDisplayWindow(HWND hwndTab, HINSTANCE hInstance, char * pcClassName, WNDPROC wProc);
ATOM RegisterSomeClass(HINSTANCE hInstance, char * className, WNDPROC proc);
HTREEITEM AddItemToTree(HWND hwndTV, LPTSTR lpszItem, HTREEITEM hParent, int num);

#define MIRMSG_TABCHANGE WM_USER + 0x13

/* Parser plus controls */
#define PPEDIT 10090
#define PPLOG  10091

/* Tracer buttons */
#define BUTTONDELTRACE	 10101
#define BUTTONTRACEUP	 10102
#define BUTTONTRACEDOWN	 10103
#define BUTTONLOGZOOMIN  10104
#define BUTTONLOGZOOMSW  10105
#define BUTTONLOGZOOMUIT 10106
#define DEBUGCOMBOTYPE	 10107
#define DEBUGCOMBOELEM	 10108