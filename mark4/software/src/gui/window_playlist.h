/*
Copyright 2023, dettus@dettus.net

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

#ifndef WINDOW_PLAYLIST_H
#define	WINDOW_PLAYLIST_H

#include "config.h"
#include "datastructures.h"
#include "playlist.h"
#include "theme_manager.h"
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define	WINDOW_PLAYLIST_HANDLE_HEIGHT	20	// ELEMENT_HEIGHT(PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE)

typedef struct _tWindowPlaylistStatus
{
	int menu_add;
	int menu_remove;
	int menu_select;
	int menu_misc;
	int menu_list;
} tWindowPlaylistStatus;

typedef	struct _tHandleWindowPlaylist
{
	tHandleConfig handleConfig;
	int handleConfigValid;	// TODO: is this one necessary?
	// gtk related bureaucracy
	GtkApplication *app;
	GdkPixbuf *pixbuf;
	GdkPixbuf *pixbuf_handle;
	GdkPixbuf *pixbuf_list;
	GdkPixbuf *pixbuf_frame;
	GdkTexture *texture_handle;
	GdkTexture *texture_list;
	GdkTexture *texture_frame;
	
	GtkWidget *handle;
	GtkWidget *box;
	GtkWidget *overlay;
	GtkWidget *picture_handle;
	GtkWidget *picture_list;
	GtkWidget *picture_frame;
	GtkWidget *picture;
	GtkWidget *window;
	GtkGesture *gesture_click;
	GtkGesture *gesture_drag;
	GtkEventController *event_scroll;


	
	// pixbufs to draw on
	GdkPixbuf *pixbufBackground;
	tHandlePlayList *pHandlePlayList;
	tHandleThemeManager *pHandleThemeManager;
	void *pControllerContext;
	ePressable lastPressed;
	int pressedX;
	int pressedY;
	int resizeCols;
	int resizeRows;
	int resizeWidth;
	int resizeHeight;

	
#define	PLAYLIST_ROW_HEIGHT	29
#define	PLAYLIST_COL_WIDTH	25
	int rows;	// one row has 29 pixels. default=2  --> titlebar 20 pixel, bottom 38 pixel. 20+38+2*29=116 pixel
	int columns;	// one column has 25 pixels. default=11 --> 11*25=275 pixel

	int window_height;	// height in pixels
	int window_width;	// width in pixels

	int list_posx;
	int list_posy;
	int list_dimx;
	int list_dimy;

	int list_topIndex;	// which entry is the first one in the list?
	int list_entriesPerPage;// how many entries are per page?
	int list_numberOfEntries;
	
	int scrollbarY;
	int scaleFactor;

	#define PRESSABLE_PLAYLIST_NUM 33
	tPressableBoundingBox	boundingBoxes[PRESSABLE_PLAYLIST_NUM];	

	tWindowPlaylistStatus status;

#define	MAXMENUITEMS	16
	GtkWidget *popUpMenu;
	GMenu *menu;
	GMenuItem *menuitems[MAXMENUITEMS];
	GSimpleAction *action[MAXMENUITEMS];
	int menuItemCnt;


	pthread_mutex_t mutex;	


// preferences
	int show_full_path;
	GtkWidget* pref_check;

	GtkFileDialog *fileDialog_fileopen;
	GtkFileDialog *fileDialog_diropen;
	GtkFileDialog *fileDialog_loadlist;
	GtkFileDialog *fileDialog_savelist;
} tHandleWindowPlaylist;
int window_playlist_init(tHandleWindowPlaylist* pThis,void* pControllerContext,tHandleThemeManager *pHandleThemeManager,GtkApplication* app,tHandlePlayList *pHandlePlayList);
int window_playlist_start(tHandleWindowPlaylist* pThis);
int window_playlist_signal_scalefactor(tHandleWindowPlaylist* pThis,int scaleFactor);
int window_playlist_signal_new_theme(tHandleWindowPlaylist* pThis);
int window_playlist_signal_jump_to_entry(tHandleWindowPlaylist* pThis,int currentEntry);
int window_playlist_show(tHandleWindowPlaylist* pThis);
int window_playlist_hide(tHandleWindowPlaylist* pThis);

int window_playlist_get_preferences_widget(tHandleWindowPlaylist* pThis,GtkWidget** pWidget);
int window_playlist_populate_preferences(void* pContext);
int window_playlist_apply_preferences(void* pContext);



#endif

