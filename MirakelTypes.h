#pragma once

#include <Windows.h>
#include <Windowsx.h>
#include <commctrl.h>	/* gebruik van tooltips */

#define FCLOGMAX 3200

struct fasenlogentry {
	short state;
	short aanvr;
	short ar;
	unsigned long timestamp;
};

typedef struct fasenlogbuf {
	struct fasenlogentry buffer[FCLOGMAX];
	char fc_code[4];
	int id;
	char show_fc;
} FASENLOGSTRUCT;

struct mirakel_controller {
	HTREEITEM hFasen, hAuto, hFiets, hVtg, hOV, * hFC;
	LPCTSTR lpWindowName;
	struct fasenlogbuf * fasenlog;
	char fc_selchanged;
	unsigned long cttime, cttime_val;
};
typedef struct mirakel_controller CONTROLLERSTRUCT;

extern CONTROLLERSTRUCT * Controller;

#define TYPE_FC		0
#define TYPE_DP		1
#define TYPE_SCH	2
#define TYPE_PRM	3
#define TYPE_TM		4
#define TYPE_IS		5
#define TYPE_US		6
#define TYPE_ME		7
#define TYPE_HE		8
#define TYPE_CT		9

unsigned long CALLBACK MirGetCTTime(void);
struct fasenlogentry * CALLBACK MirGetCCOLFCState(short fc);
LRESULT CALLBACK WindowProcTabFasenlog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
const LPCSTR CALLBACK MirGiveCCOLString(short type, short elem);

