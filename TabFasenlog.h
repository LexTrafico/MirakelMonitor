#pragma once

#include "MirakelTypes.h"
#include "MirakelMonitorUtilities.h"

void fasenlog_update(struct mirakel_controller * ct);
void fasenlog_init(struct mirakel_controller * ct);
void fasenlog_paintfc(HDC hDC, struct mirakel_controller * ct, int fc, double ppte, int x, int y, int w, int h, int scrollp);
void fasenlog_painttimebars(HDC hDC, struct mirakel_controller * ct, float ppte, int x, int y, int w, int h);

#define FL_PEN_TIMELINES	0
#define FL_PEN_MAX			1

#define FL_FONT_TIMESTAMP	0
#define FL_FONT_LABEL		1
#define FL_FONT_MAX			2

