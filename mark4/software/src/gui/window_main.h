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

#ifndef	WINDOW_MAIN_H
#define	WINDOW_MAIN_H

#include "config.h"
#include "datastructures.h"
#include "window_license.h"
#include "theme_manager.h"
#include "visualizer.h"
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define	WINDOW_MAIN_WIDTH	275
#define	WINDOW_MAIN_HANDLE_HEIGHT	14
#define	WINDOW_MAIN_HEIGHT	116
typedef enum
{
	eINDICATOR_NONE=0,
	eINDICATOR_PLAY,
	eINDICATOR_PAUSE,
	eINDICATOR_STOP,
	eINDICATOR_START_OF_SONG,
	eINDICATOR_END_OF_SONG
} eWindowMainIndicator;

typedef struct _tWindowMainStatus
{
	eOnOff clutterbar;
	eOnOff equalizer;
	eOnOff playlist;
	eOnOff shuffle;
	eOnOff repeat;
	eOnOff countdown;
	
	

	eWindowMainIndicator indicator;
	int volume;		// 0..100
	int balance;		// -100..0..100
} tWindowMainStatus;



typedef	struct _tHandleWindowMain
{
	tHandleConfig handleConfig;
	// gtk related bureaucracy
	GtkApplication *app;
	GdkPixbuf *pixbuf;
	GdkPixbuf *pixbuf_handle_shaded;
	GdkPixbuf *pixbuf_handle;
	GdkPixbuf *pixbuf_main;
	GdkTexture *texture_handle_shaded;
	GdkTexture *texture_handle;
	GdkTexture *texture_main;

	GtkWidget *stack;
	GtkWidget *handle;
	GtkWidget *handle_shaded;
	GtkWidget *box;
	GtkWidget *box_shaded;
	GtkWidget *picture_handle;
	GtkWidget *picture_handle_shaded;
	GtkWidget *picture_main;
	GtkWidget *window;
#define	MAXMENUITEMS	16
	GtkWidget *popUpMenu;
	GMenu *menu;
	GMenuItem *menuitems[MAXMENUITEMS];
	GSimpleAction *action[MAXMENUITEMS];
	int menuItemCnt;



	GtkGesture *gesture_click;
	GtkGesture *gesture_drag;
	GtkEventController *event_controller_key;

	// pixbufs to draw on
	GdkPixbuf *pixbufBackground;
	GdkPixbuf *pixbufSongtitle;
	GdkPixbuf *pixbufKhz;
	GdkPixbuf *pixbufKbps;



	tHandleThemeManager *pHandleThemeManager;
	tHandleVisualizer handleVisualizer;
	tHandleWindowLicense handleWindowLicense;
	void *pControllerContext;

	tSongInfo songInfo;
	ePressable lastPressed;
	int pressedX;
	int pressedY;
	tWindowMainStatus status;

	int volumex;
	int balancex;
	int songposx;

	int songinfo_scrollpos;
	int shaded;
	int scaleFactor;


	GtkFileChooserNative *fileChooser_open;
	GtkFileChooserNative *fileChooser_dir;
	GtkFileChooserNative *fileChooser_wsz;

#define	PRESSABLE_MAIN_NUM	25
	tPressableBoundingBox boundingBoxes[PRESSABLE_MAIN_NUM];
	pthread_mutex_t	mutex;
	pthread_t	thread;
} tHandleWindowMain;

int window_main_init(tHandleWindowMain* pThis,void* pControllerContext,tHandleThemeManager *pHandleThemeManager,GtkApplication* app);
int window_main_start(tHandleWindowMain* pThis);
int window_main_signal_indicator(tHandleWindowMain* pThis,eWindowMainIndicator indicator);
int window_main_signal_scalefactor(tHandleWindowMain* pThis,int scale);
int window_main_signal_new_theme(tHandleWindowMain* pThis);
int window_main_signal_volume(tHandleWindowMain *pThis,int volume);
int window_main_signal_balance(tHandleWindowMain *pThis,int balance);
int window_main_signal_equalizer(tHandleWindowMain *pThis,int hide0show1);
int window_main_signal_playlist(tHandleWindowMain *pThis,int hide0show1);
//int window_main_signal_shade(tHandleWindowMain *pThis,int shade0unshade1);

int window_main_show(tHandleWindowMain *pThis);
int window_main_hide(tHandleWindowMain *pThis);

int window_main_pull_shuffle_repeat(tHandleWindowMain *pThis,int* pShuffle,int* pRepeat);

#endif

