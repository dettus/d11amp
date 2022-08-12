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
#include "datastructures.h"
#include "theme_elements.h"
#include "window_main.h"
#include <stdio.h>

#define ACTIVE          1
#define INACTIVE        0
#define UNPRESSED       0
#define PRESSED         1
#define HIDDEN          0
#define SHOWN           1

#define PLAYPAUSE_PLAY  0
#define PLAYPAUSE_PAUSE 1
#define PLAYPAUSE_STOP  2
#define PLAYPAUSE_START 3
#define PLAYPAUSE_END   4

#define MONOSTER_UNKNOWN        0
#define MONOSTER_MONO           1
#define MONOSTER_STEREO         2

#define CBUTTON_NONE            0
#define CBUTTON_PREV            1
#define CBUTTON_PLAY            2
#define CBUTTON_PAUSE           3
#define CBUTTON_STOP            4
#define CBUTTON_NEXT            5
#define CBUTTON_OPEN            6

#define PRESSABLE_NUM           20


int window_main_draw(tHandleWindowMain* pThis,GdkPixbuf *pixbufDestination)
{
	int i;
	eElementID numbers[11]={NUMBERS_0,NUMBERS_1,NUMBERS_2,NUMBERS_3,NUMBERS_4,NUMBERS_5,NUMBERS_6,NUMBERS_7,NUMBERS_8,NUMBERS_9,NUMBERS_BLANK};

	int timedigitxpos[4]={48,60,78,90};	// TODO: magic numbers
	
	// the title bar
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,0,0,(pThis->statusTitleBar==ACTIVE)?TITLEBAR_NORMAL_TITLEBAR_ACTIVE:TITLEBAR_NORMAL_TITLEBAR_INACTIVE);
	// the main background
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,ELEMENT_POS(MAIN_MAIN_DISPLAY),MAIN_MAIN_DISPLAY);
	// the clutterbar
	if (pThis->statusClutterBar==SHOWN)
	{
		switch(pThis->lastPressed)
		{
			case PRESSED_CLUTTERBAR_O:      theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,10,22,TITLEBAR_CLUTTERBAR_O_PRESSED);break;
			case PRESSED_CLUTTERBAR_A:      theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,10,22,TITLEBAR_CLUTTERBAR_A_PRESSED);break;
			case PRESSED_CLUTTERBAR_I:      theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,10,22,TITLEBAR_CLUTTERBAR_I_PRESSED);break;
			case PRESSED_CLUTTERBAR_D:      theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,10,22,TITLEBAR_CLUTTERBAR_D_PRESSED);break;
			case PRESSED_CLUTTERBAR_V:      theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,10,22,TITLEBAR_CLUTTERBAR_V_PRESSED);break;
			default:        theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,10,22,TITLEBAR_CLUTTERBAR_SHOWN);break;
		}
	} else {
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,10,22,TITLEBAR_CLUTTERBAR_HIDDEN);
	}
	// the time display	
	for (i=0;i<4;i++)
	{
		int digit;
		digit=pThis->statusTimeDigits[i];
		if (digit>=0 && digit<=10)
		{
			theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,timedigitxpos[i],26,numbers[digit]);
		}
	}
	// playpause indicator
	switch(pThis->statusPlayPause)
	{
		case PLAYPAUSE_PLAY:    theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,24,18, PLAYPAUSE_PLAY_INDICATOR);break;
		case PLAYPAUSE_PAUSE:   theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,24,18, PLAYPAUSE_PAUSE_INDICATOR);break;
		case PLAYPAUSE_STOP:    theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,24,18, PLAYPAUSE_STOP_INDICATOR);break;
		case PLAYPAUSE_START:   theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,24,18, PLAYPAUSE_START_OF_SONG_INDICATOR);break;
		case PLAYPAUSE_END:     theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,24,18, PLAYPAUSE_END_OF_SONG_INDICATOR);break;
	}
	// mono or stereo
	switch(pThis->statusMonoSter)
	{
		case MONOSTER_MONO:
			theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,212,41, MONOSTER_MONO_ACTIVE);
			theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,239,41, MONOSTER_STEREO_INACTIVE);
			break;
		case MONOSTER_STEREO:
			theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,212,41, MONOSTER_MONO_INACTIVE);
			theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,239,41, MONOSTER_STEREO_ACTIVE);
			break;
		default:
			theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,212,41, MONOSTER_MONO_INACTIVE);
			theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,239,41, MONOSTER_STEREO_INACTIVE);
			break;
	}
	// kbps and khz labels
        theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,111,43, MAIN_KBPS_DISPLAY);
        theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,156,43, MAIN_KHZ_DISPLAY);
	// the song info
        {
                int x;
                int margin;

                margin=154;     // the width of the song info window
                x=pThis->scrollpos;
                if (x>pThis->scrolllen-margin)
                {
                        x=pThis->scrolllen-margin;
                }
                if (x<0) x=0;
                gdk_pixbuf_copy_area(pThis->pixbufSongInfo,x,0,154,6,pixbufDestination,111,27);
                gdk_pixbuf_copy_area(pThis->pixbufBitrate,0,0,15,6,pixbufDestination,111,43);
                gdk_pixbuf_copy_area(pThis->pixbufSamplerate,0,0,10,6,pixbufDestination,156,43);
        }
// volume adjuster
        {
                eElementID volumes[28]={ VOLUME_000_001, VOLUME_003_005, VOLUME_007_009, VOLUME_011_013, VOLUME_015_017, VOLUME_019_021, VOLUME_023_025, VOLUME_027_029, VOLUME_031, VOLUME_033_035, VOLUME_037_039, VOLUME_041_043, VOLUME_045_047, VOLUME_049_050, VOLUME_052_054, VOLUME_056_058, VOLUME_060_062, VOLUME_064, VOLUME_066_068, VOLUME_070_072, VOLUME_074_076, VOLUME_078_080, VOLUME_082_084, VOLUME_086_088, VOLUME_090_092, VOLUME_094_096, VOLUME_098, VOLUME_100 };
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,107,57, volumes[pThis->statusVolume]);
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,107+pThis->statusVolume*2,57, (pThis->lastPressed==PRESSED_VOLUME_SLIDER)?VOLUME_SLIDER_PRESSED:VOLUME_SLIDER_UNPRESSED);
        }
// the balance slider
        {
                eElementID balances[19]={BALANCE_CENTERED, BALANCE_FILLER0, BALANCE_FILLER1, BALANCE_FILLER2, BALANCE_24LEFTORRIGHT, BALANCE_33LEFTORRIGHT, BALANCE_FILLER6, BALANCE_40LEFTORRIGHT, BALANCE_FILLER8, BALANCE_49LEFTORRIGHT, BALANCE_FILLER9, BALANCE_58LEFTORRIGHT, BALANCE_FILLER10, BALANCE_66LEFTORRIGHT, BALANCE_FILLER12, BALANCE_74LEFTORRIGHT, BALANCE_82LEFTORRIGHT, BALANCE_91LEFTORRIGHT, BALANCE_100LEFTORRIGHT};
                int b;

                b=pThis->statusBalance;
                if (b<0) b=-b;
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,177,57, balances[b]);
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,177+38/2-14/2+pThis->statusBalance,57,pThis->lastPressed==PRESSED_BALANCE_SLIDER?BALANCE_SLIDER_PRESSED:BALANCE_SLIDER_UNPRESSED);
        }
// equalizer toggle
        if (pThis->statusEqualizer==ACTIVE)
        {
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,219,58, pThis->lastPressed==PRESSED_EQUALIZER?SHUFREP_EQUALIZER_PRESSED:SHUFREP_EQUALIZER_UNPRESSED);
        } else {
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,219,58, pThis->lastPressed==PRESSED_EQUALIZER?SHUFREP_NO_EQUALIZER_PRESSED:SHUFREP_NO_EQUALIZER_UNPRESSED);
        }
// playlist toggle
        if (pThis->statusPlaylist==ACTIVE)
        {
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,242,58, pThis->lastPressed==PRESSED_PLAYLIST?SHUFREP_PLAYLIST_PRESSED:SHUFREP_PLAYLIST_UNPRESSED);
        } else {
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,242,58, pThis->lastPressed==PRESSED_PLAYLIST?SHUFREP_NO_PLAYLIST_PRESSED:SHUFREP_NO_PLAYLIST_UNPRESSED);
        }
// song position
        theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,16,72, POSBAR_SONG_PROGRESS_BAR);
        theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,16+pThis->statusSongPos,72, (pThis->lastPressed==PRESSED_SONGPOS)?POSBAR_SONG_SLIDER_PRESSED:POSBAR_SONG_SLIDER_UNPRESSED);

// cbuttons
        theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination, 16,88,pThis->lastPressed==PRESSED_PREV?CBUTTONS_PREV_BUTTON_PRESSED:CBUTTONS_PREV_BUTTON_UNPRESSED);
        theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination, 39,88,pThis->lastPressed==PRESSED_PLAY?CBUTTONS_PLAY_BUTTON_PRESSED:CBUTTONS_PLAY_BUTTON_UNPRESSED);
        theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination, 62,88,pThis->lastPressed==PRESSED_PAUSE?CBUTTONS_PAUSE_BUTTON_PRESSED:CBUTTONS_PAUSE_BUTTON_UNPRESSED);
        theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination, 85,88,pThis->lastPressed==PRESSED_STOP?CBUTTONS_STOP_BUTTON_PRESSED:CBUTTONS_STOP_BUTTON_UNPRESSED);
        theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,108,88,pThis->lastPressed==PRESSED_NEXT?CBUTTONS_NEXT_BUTTON_PRESSED:CBUTTONS_NEXT_BUTTON_UNPRESSED);
        theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,136,88,pThis->lastPressed==PRESSED_OPEN?CBUTTONS_OPEN_BUTTON_PRESSED:CBUTTONS_OPEN_BUTTON_UNPRESSED);

// shuffle
        if (pThis->statusShuffle==ACTIVE)
        {
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,164,89, pThis->lastPressed==PRESSED_SHUFFLE?SHUFREP_SHUFFLE_PRESSED:SHUFREP_SHUFFLE_UNPRESSED);
        } else {
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,164,89, pThis->lastPressed==PRESSED_SHUFFLE?SHUFREP_NO_SHUFFLE_PRESSED:SHUFREP_NO_SHUFFLE_UNPRESSED);
        }

// repeat
        if (pThis->statusRepeat==ACTIVE)
        {
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,211,89, pThis->lastPressed==PRESSED_REPEAT?SHUFREP_REPEAT_PRESSED:SHUFREP_REPEAT_UNPRESSED);
        } else {
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,211,89, pThis->lastPressed==PRESSED_REPEAT?SHUFREP_NO_REPEAT_PRESSED:SHUFREP_NO_REPEAT_UNPRESSED);
        }
// info
        theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,253,91,MAIN_INFO);

	return RETVAL_OK;		
}
int window_main_refresh(tHandleWindowMain* pThis)
{
	GdkPixbuf *pixbuf;
	gtk_picture_set_pixbuf(GTK_PICTURE(pThis->picture),pThis->pixbuf);
	pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);
	window_main_draw(pThis,pixbuf);
	gdk_pixbuf_copy_area(pixbuf,0,0,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,pThis->pixbuf,0,0);
	g_object_unref(pixbuf);	

	return RETVAL_OK;
}


// 
eMainWindowPressed window_main_find_pressable(int x, int y,int scaleFactor)
{
	int i;
	
	eMainWindowPressed      pressed;
	typedef struct _tPressable
	{
		eMainWindowPressed      pressed;
		int posx;
		int posy;
		int dimx;
		int dimy;
	} tPressable;

	const tPressable cMainWindow_pressable[PRESSABLE_NUM]=
	{
		{.pressed=PRESSED_CLUTTERBAR_O,     .posx= 10,.posy=22,.dimx=  8,.dimy= 8},
		{.pressed=PRESSED_CLUTTERBAR_A,     .posx= 10,.posy=30,.dimx=  8,.dimy= 8},
		{.pressed=PRESSED_CLUTTERBAR_I,     .posx= 10,.posy=39,.dimx=  8,.dimy= 8},
		{.pressed=PRESSED_CLUTTERBAR_D,     .posx= 10,.posy=47,.dimx=  8,.dimy= 8},
		{.pressed=PRESSED_CLUTTERBAR_V,     .posx= 10,.posy=56,.dimx=  8,.dimy= 8},
		{.pressed=PRESSED_VOLUME_SLIDER,    .posx=107,.posy=57,.dimx= 68,.dimy=13},
		{.pressed=PRESSED_BALANCE_SLIDER,   .posx=177,.posy=57,.dimx= 38,.dimy=13},
		{.pressed=PRESSED_EQUALIZER,        .posx=219,.posy=58,.dimx= 23,.dimy=12},
		{.pressed=PRESSED_PLAYLIST,         .posx=242,.posy=58,.dimx= 23,.dimy=12},
		{.pressed=PRESSED_SONGPOS,          .posx= 16,.posy=72,.dimx=248,.dimy=10},
		{.pressed=PRESSED_PREV,             .posx= 16,.posy=89,.dimx= 23,.dimy=18},
		{.pressed=PRESSED_PLAY,             .posx= 39,.posy=89,.dimx= 23,.dimy=18},
		{.pressed=PRESSED_PAUSE,            .posx= 62,.posy=89,.dimx= 23,.dimy=18},
		{.pressed=PRESSED_STOP,             .posx= 85,.posy=89,.dimx= 23,.dimy=18},
		{.pressed=PRESSED_NEXT,             .posx=108,.posy=89,.dimx= 22,.dimy=18},
		{.pressed=PRESSED_OPEN,             .posx=136,.posy=89,.dimx= 22,.dimy=16},
		{.pressed=PRESSED_SHUFFLE,          .posx=164,.posy=89,.dimx= 47,.dimy=15},
		{.pressed=PRESSED_REPEAT,           .posx=211,.posy=89,.dimx= 27,.dimy=15},
		{.pressed=PRESSED_INFO,             .posx=253,.posy=91,.dimx= 13,.dimy=15}
	};
	pressed=PRESSED_NONE;
	for (i=0;i<PRESSABLE_NUM;i++)
	{
		int x1,x2;
		int y1,y2;

		x1=scaleFactor*cMainWindow_pressable[i].posx;
		x2=x1+cMainWindow_pressable[i].dimx*scaleFactor;

		y1=scaleFactor*cMainWindow_pressable[i].posy;
		y2=y1+cMainWindow_pressable[i].dimy*scaleFactor;

		if (x>=x1 && x<x2 && y>=y1 && y<y2)
		{
			pressed=cMainWindow_pressable[i].pressed;
		}
	}

	return pressed;
}



// interactive callbacks
int window_main_event_pressed(GtkWidget *widget, double x,double y,guint event_button, gpointer data)
{
	eMainWindowPressed pressed;
	tHandleWindowMain* pThis=(tHandleWindowMain*)data;
	pressed=window_main_find_pressable((int)x,(int)y,pThis->scaleFactor);	

	pThis->lastPressed=pressed;
	window_main_refresh(pThis);
	return TRUE;
}
int window_main_event_released(GtkWidget *widget, double x,double y,guint event_button, gpointer data)
{
	eMainWindowPressed pressed;
	tHandleWindowMain* pThis=(tHandleWindowMain*)data;
	pressed=window_main_find_pressable((int)x,(int)y,pThis->scaleFactor);	

	if (pressed==pThis->lastPressed && pressed!=PRESSED_NONE)
	{
		printf("pressed %d\n",(int)pressed);	
	}

	pThis->lastPressed=PRESSED_NONE;
	window_main_refresh(pThis);
	return TRUE;
}
// initial setup
int window_main_init(GtkApplication* app,tHandleWindowMain* pThis,tHandleThemeManager* pHandleThemeManager)
{
	memset(pThis,0,sizeof(tHandleWindowMain));

	pThis->pHandleThemeManager=pHandleThemeManager;

	pThis->scaleFactor=4;
	pThis->pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);
	pThis->pixbufSongInfo=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,154,6);
	pThis->pixbufBitrate=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,15,6);
	pThis->pixbufSamplerate=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,10,6);


	pThis->statusTimeDigits[0]=10;
	pThis->statusTimeDigits[1]= 0;
	pThis->statusTimeDigits[2]= 0;
	pThis->statusTimeDigits[3]= 0;
	pThis->statusClutterBar=SHOWN;
	pThis->statusMonoSter=MONOSTER_UNKNOWN;
	pThis->statusTitleBar=ACTIVE;
	pThis->statusVolume=27;
	pThis->statusBalance=0;
	pThis->statusEqualizer=INACTIVE;
	pThis->statusPlaylist=INACTIVE;
	pThis->statusShuffle=INACTIVE;
	pThis->statusRepeat=INACTIVE;



	window_main_draw(pThis,pThis->pixbuf);
	pThis->picture=gtk_picture_new_for_pixbuf(pThis->pixbuf);
	pThis->windowMain=gtk_application_window_new(app);
	gtk_window_set_child(GTK_WINDOW(pThis->windowMain),pThis->picture);
	gtk_window_set_title(GTK_WINDOW(pThis->windowMain),"d11amp main");
	gtk_window_set_default_size(GTK_WINDOW(pThis->windowMain),WINDOW_MAIN_WIDTH*pThis->scaleFactor,WINDOW_MAIN_HEIGHT*pThis->scaleFactor);

	pThis->gesture=gtk_gesture_click_new();
	g_signal_connect(pThis->gesture,"pressed",G_CALLBACK(window_main_event_pressed),&(pThis->windowMain));
	g_signal_connect(pThis->gesture,"released",G_CALLBACK(window_main_event_released),&(pThis->windowMain));
	gtk_widget_add_controller(pThis->windowMain,GTK_EVENT_CONTROLLER(pThis->gesture));
	return RETVAL_OK;
}

int window_main_show(tHandleWindowMain* pThis)
{
	gtk_widget_show(pThis->windowMain);
	return RETVAL_OK;
}