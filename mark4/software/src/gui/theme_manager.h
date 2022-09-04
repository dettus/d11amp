/*

Copyright 2022, dettus@dettus.net

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this 
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


*/
#ifndef	THEME_MANAGER_H
#define	THEME_MANAGER_H
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "datastructures.h"
#include "theme_elements.h"

#define	VISCOLOR_NUM	24
typedef struct _tVisColor
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} tVisColor;

typedef struct _tPlayListTheme
{
// the playlist
	tVisColor color_normal;
	tVisColor color_current;
	tVisColor color_normalBG;
	tVisColor color_selectedBG;
	char fontname[1024];
} tPlayListTheme;

typedef struct _tHandleThemeManager
{
// for the windows
	GdkPixbuf* loaded_bmp[SOURCES_NUM];
// the visualizer colors
	tVisColor visColors[VISCOLOR_NUM];

	tPlayListTheme playListTheme;
} tHandleThemeManager;

int theme_manager_init(tHandleThemeManager* pThis);
int theme_manager_load_from_directory(tHandleThemeManager* pThis,char* directory);
int theme_manager_draw_element(tHandleThemeManager* pThis,GdkPixbuf* destbuf,eElementID elementID);
int theme_manager_draw_element_at(tHandleThemeManager* pThis,GdkPixbuf* destbuf,eElementID elementID,int x,int y);
int theme_manager_draw_text(tHandleThemeManager* pThis,GdkPixbuf** pDestbuf,eElementID backGroundElement,char* text,int minwidth);
int theme_manager_get_viscolors(tHandleThemeManager* pThis,tVisColor** pVisColors);
int theme_manager_get_playListTheme(tHandleThemeManager* pThis,tPlayListTheme** playListTheme);

#endif
