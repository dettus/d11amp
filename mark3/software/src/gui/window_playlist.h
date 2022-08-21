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

#include <gtk/gtk.h>
#include <pthread.h>
#include "playlist.h"
#include "theme_manager.h"
#ifndef WINDOW_PLAYLIST_H
#define	WINDOW_PLAYLIST_H
//#define	WINDOW_PLAYLIST_WIDTH	275
#define	WINDOW_PLAYLIST_WIDTH	275
#define	WINDOW_PLAYLIST_HEIGHT	116

#define	PLAYLIST_PRESSABLE_NUM	23
typedef enum
{
	PLAYLIST_PRESSED_NONE=0,
	PLAYLIST_PRESSED_ADD_BUTTON,
	PLAYLIST_PRESSED_REMOVE_BUTTON,
	PLAYLIST_PRESSED_SELECTION_BUTTON,
	PLAYLIST_PRESSED_MISCELLANEOUS_BUTTON,
	PLAYLIST_PRESSED_LIST_BUTTON,


	PLAYLIST_PRESSED_ADD_FILE,
	PLAYLIST_PRESSED_ADD_DIR,
	PLAYLIST_PRESSED_ADD_URL,

	PLAYLIST_PRESSED_REMOVE_MISC,
	PLAYLIST_PRESSED_REMOVE_FILE,
	PLAYLIST_PRESSED_REMOVE_CROP,
	PLAYLIST_PRESSED_REMOVE_ALL,

	PLAYLIST_PRESSED_SELECT_ALL,
	PLAYLIST_PRESSED_SELECT_NONE,
	PLAYLIST_PRESSED_SELECT_INVERT,

	PLAYLIST_PRESSED_MISC_OPTIONS,
	PLAYLIST_PRESSED_MISC_INFO,
	PLAYLIST_PRESSED_MISC_SORT,

	PLAYLIST_PRESSED_LIST_LOAD,
	PLAYLIST_PRESSED_LIST_SAVE,
	PLAYLIST_PRESSED_LIST_NEW,

	PLAYLIST_PRESSED_RESIZE_CONTROL


} ePlaylistPressed;

typedef struct _tPressablePlaylist
{
	ePlaylistPressed pressed;
	int posx;
	int posy;
	int dimx;
	int dimy;	
} tPressablePlaylist;

typedef struct _tHandleWindowPlaylist
{
	GtkWidget *windowPlaylist;
	GtkWidget *picture;
	GtkGesture *gesture;
	GdkPixbuf *pixbuf;
	int scaleFactor;

	tHandlePlaylist handlePlaylist;
	tHandleThemeManager *pHandleThemeManager;


	int statusActive;
	int scrollPos;
	int scrollLen;
	int statusMenuAdd;
	int statusMenuRem;
	int statusMenuSel;
	int statusMenuMisc;
	int statusMenuList;

// since this window is dynamic, the pressable lookup table is being calculated on the fly
	int window_width;
	int window_height;
	ePlaylistPressed lastPressed;
	tPressablePlaylist windowPlaylist_pressable[PLAYLIST_PRESSABLE_NUM];

	pthread_mutex_t mutex;
} tHandleWindowPlaylist;
int window_playlist_init(GtkApplication* app,tHandleWindowPlaylist* pThis,tHandleThemeManager *pHandleThemeManager);
int window_playlist_load(tHandleWindowPlaylist* pThis,char* filename);
int window_playlist_show(tHandleWindowPlaylist* pThis);
int window_playlist_hide(tHandleWindowPlaylist* pThis);
int window_playlist_parse_commandline(tHandleWindowPlaylist* pThis,char* argument);


int window_playlist_append(tHandleWindowPlaylist *pThis,char* filename);
int window_playlist_shuffle(tHandleWindowPlaylist *pThis,char* pFilename,int size);
int window_playlist_next(tHandleWindowPlaylist *pThis,char* pFilename,int size);
int window_playlist_prev(tHandleWindowPlaylist *pThis,char* pFilename,int size);
#endif
