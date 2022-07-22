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
#include "mainwindow.h"

#define	ACTIVE		1
#define	INACTIVE	0
#define	UNPRESSED	0
#define	PRESSED		1
#define	HIDDEN		0
#define	SHOWN		1

#define	PLAYPAUSE_PLAY	0
#define	PLAYPAUSE_PAUSE	1
#define	PLAYPAUSE_STOP	2
#define	PLAYPAUSE_START	3
#define	PLAYPAUSE_END	4

#define	MONOSTER_UNKNOWN	0
#define	MONOSTER_MONO		1
#define	MONOSTER_STEREO		2

#define	CBUTTON_NONE		0
#define	CBUTTON_PREV		1
#define	CBUTTON_PLAY		2
#define	CBUTTON_PAUSE		3
#define	CBUTTON_STOP		4
#define	CBUTTON_NEXT		5
#define	CBUTTON_OPEN		6

#define	PRESSABLE_NUM		19
typedef struct _tPressable
{
	eMainWindowPressed	pressed;
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
	{.pressed=PRESSED_REPEAT,           .posx=211,.posy=89,.dimx= 27,.dimy=15}
};

eMainWindowPressed mainwindow_findPressable(int x,int y,int scalefactor)
{
	int i;
	eMainWindowPressed retval=PRESSED_NONE;
	x/=scalefactor;
	y/=scalefactor;
	for (i=0;i<PRESSABLE_NUM;i++)
	{
		int posx;
		int posy;
		int dimx;
		int dimy;

		posx=cMainWindow_pressable[i].posx;
		posy=cMainWindow_pressable[i].posy;
		dimx=cMainWindow_pressable[i].dimx;
		dimy=cMainWindow_pressable[i].dimy;
		if (x>=posx && x<(posx+dimx) && y>=posy && y<(posy+dimy)) retval=cMainWindow_pressable[i].pressed;
	}
	return retval;
}

void mainwindow_defaultvalues(tHandleMainWindow* pThis)
{
	pThis->scalefactor=1;
	pThis->titlebar_active=INACTIVE;
	pThis->clutterbar_shown=SHOWN;
	pThis->time_digit[0]=10;
	pThis->time_digit[1]=0;
	pThis->time_digit[2]=0;
	pThis->time_digit[3]=0;
	pThis->playpause_state=PLAYPAUSE_START;
	pThis->volume_setting=18;	// between 0 and 28
	pThis->balance_setting=0;	// between -18..0..18

	pThis->equalizer_active=INACTIVE;
	pThis->playlist_active=INACTIVE;

	pThis->songpos=13;	// between 0 and 72-29


	pThis->shuffle_active=INACTIVE;
	pThis->repeat_active=INACTIVE;
	pThis->pressed=PRESSED_NONE;
}






void mainwindow_redraw(tHandleMainWindow* pThis)
{
	int i;
	eElementID numbers[11]={NUMBERS_0,NUMBERS_1,NUMBERS_2,NUMBERS_3,NUMBERS_4,NUMBERS_5,NUMBERS_6,NUMBERS_7,NUMBERS_8,NUMBERS_9,NUMBERS_BLANK};
	int timedigitposx[4]={48,60,78,90};
// the titlebar
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,0, 0,(pThis->titlebar_active==ACTIVE)?TITLEBAR_NORMAL_TITLEBAR_ACTIVE:TITLEBAR_NORMAL_TITLEBAR_INACTIVE);
// the main background
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,0,14,MAIN_MAIN_DISPLAY);
// the clutterbar
	if (pThis->clutterbar_shown==SHOWN)
	{
		switch(pThis->pressed)
		{
			case PRESSED_CLUTTERBAR_O:	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,10,22,TITLEBAR_CLUTTERBAR_O_PRESSED);break;
			case PRESSED_CLUTTERBAR_A:	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,10,22,TITLEBAR_CLUTTERBAR_A_PRESSED);break;
			case PRESSED_CLUTTERBAR_I:	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,10,22,TITLEBAR_CLUTTERBAR_I_PRESSED);break;
			case PRESSED_CLUTTERBAR_D:	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,10,22,TITLEBAR_CLUTTERBAR_D_PRESSED);break;
			case PRESSED_CLUTTERBAR_V:	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,10,22,TITLEBAR_CLUTTERBAR_V_PRESSED);break;
			default:	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,10,22,TITLEBAR_CLUTTERBAR_SHOWN);break;
		}
	} else {
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,10,22,TITLEBAR_CLUTTERBAR_HIDDEN);
	}
// the time display
	for (i=0;i<4;i++)
	{
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,timedigitposx[i],26,numbers[pThis->time_digit[i]]);
	}
// playpause indicator
	switch(pThis->playpause_state)
	{
		case PLAYPAUSE_PLAY:	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,24,18, PLAYPAUSE_PLAY_INDICATOR);break;
		case PLAYPAUSE_PAUSE:	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,24,18, PLAYPAUSE_PAUSE_INDICATOR);break;
		case PLAYPAUSE_STOP:	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,24,18, PLAYPAUSE_STOP_INDICATOR);break;
		case PLAYPAUSE_START:	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,24,18, PLAYPAUSE_START_OF_SONG_INDICATOR);break;
		case PLAYPAUSE_END:	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,24,18, PLAYPAUSE_END_OF_SONG_INDICATOR);break;
	}
// mono or stereo?
	switch(pThis->mono_stereo)
	{
		case MONOSTER_MONO:
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,212,41, MONOSTER_MONO_ACTIVE);
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,239,41, MONOSTER_STEREO_INACTIVE);
			break;
		case MONOSTER_STEREO:
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,212,41, MONOSTER_MONO_INACTIVE);
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,239,41, MONOSTER_STEREO_ACTIVE);
			break;
		default:
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,212,41, MONOSTER_MONO_INACTIVE);
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,239,41, MONOSTER_STEREO_INACTIVE);
			break;
	}
// kbps and khz labels
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,111,43, MAIN_KBPS_DISPLAY);
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,156,43, MAIN_KHZ_DISPLAY);
// volume adjuster
	{
		eElementID volumes[29]={ VOLUME_000_001, VOLUME_003_005, VOLUME_007_009, VOLUME_011_013, VOLUME_015_017, VOLUME_019_021, VOLUME_023_025, VOLUME_027_029, VOLUME_031, VOLUME_033_035, VOLUME_037_039, VOLUME_041_043, VOLUME_045_047, VOLUME_049_050, VOLUME_052_054, VOLUME_056_058, VOLUME_060_062, VOLUME_064, VOLUME_066_068, VOLUME_070_072, VOLUME_074_076, VOLUME_078_080, VOLUME_082_084, VOLUME_086_088, VOLUME_090_092, VOLUME_094_096, VOLUME_098, VOLUME_100 };
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,107,57, volumes[pThis->volume_setting]);
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,107+pThis->volume_setting*2,57, (pThis->pressed==PRESSED_VOLUME_SLIDER)?VOLUME_SLIDER_PRESSED:VOLUME_SLIDER_UNPRESSED);
	}
// the balance slider
	{
		eElementID balances[19]={BALANCE_CENTERED, BALANCE_FILLER0, BALANCE_FILLER1, BALANCE_FILLER2, BALANCE_24LEFTORRIGHT, BALANCE_33LEFTORRIGHT, BALANCE_FILLER6, BALANCE_40LEFTORRIGHT, BALANCE_FILLER8, BALANCE_49LEFTORRIGHT, BALANCE_FILLER9, BALANCE_58LEFTORRIGHT, BALANCE_FILLER10, BALANCE_66LEFTORRIGHT, BALANCE_FILLER12, BALANCE_74LEFTORRIGHT, BALANCE_82LEFTORRIGHT, BALANCE_91LEFTORRIGHT, BALANCE_100LEFTORRIGHT};
		int b;

		b=pThis->balance_setting;
		if (b<0) b=-b;
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,177,57, balances[b]);
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,177+38/2-14/2+pThis->balance_setting,57,pThis->pressed==PRESSED_BALANCE_SLIDER?BALANCE_SLIDER_PRESSED:BALANCE_SLIDER_UNPRESSED);
	}	
// equalizer toggle
	if (pThis->equalizer_active==ACTIVE)
	{
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,219,58, pThis->pressed==PRESSED_EQUALIZER?SHUFREP_EQUALIZER_PRESSED:SHUFREP_EQUALIZER_UNPRESSED);
	} else {
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,219,58, pThis->pressed==PRESSED_EQUALIZER?SHUFREP_NO_EQUALIZER_PRESSED:SHUFREP_NO_EQUALIZER_UNPRESSED);
	}
// playlist toggle
	if (pThis->playlist_active==ACTIVE)
	{
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,242,58, pThis->pressed==PRESSED_PLAYLIST?SHUFREP_PLAYLIST_PRESSED:SHUFREP_PLAYLIST_UNPRESSED);
	} else {
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,242,58, pThis->pressed==PRESSED_PLAYLIST?SHUFREP_NO_PLAYLIST_PRESSED:SHUFREP_NO_PLAYLIST_UNPRESSED);
	}
// song position
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,16,72, POSBAR_SONG_PROGRESS_BAR);
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,16+pThis->songpos,72, (pThis->pressed==PRESSED_SONGPOS)?POSBAR_SONG_SLIDER_PRESSED:POSBAR_SONG_SLIDER_UNPRESSED);

// cbuttons
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf, 16,88,pThis->pressed==PRESSED_PREV?CBUTTONS_PREV_BUTTON_PRESSED:CBUTTONS_PREV_BUTTON_UNPRESSED);
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf, 39,88,pThis->pressed==PRESSED_PLAY?CBUTTONS_PLAY_BUTTON_PRESSED:CBUTTONS_PLAY_BUTTON_UNPRESSED);
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf, 62,88,pThis->pressed==PRESSED_PAUSE?CBUTTONS_PAUSE_BUTTON_PRESSED:CBUTTONS_PAUSE_BUTTON_UNPRESSED);
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf, 85,88,pThis->pressed==PRESSED_STOP?CBUTTONS_STOP_BUTTON_PRESSED:CBUTTONS_STOP_BUTTON_UNPRESSED);
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,108,88,pThis->pressed==PRESSED_NEXT?CBUTTONS_NEXT_BUTTON_PRESSED:CBUTTONS_NEXT_BUTTON_UNPRESSED);
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,136,88,pThis->pressed==PRESSED_OPEN?CBUTTONS_OPEN_BUTTON_PRESSED:CBUTTONS_OPEN_BUTTON_UNPRESSED);

// shuffle
	if (pThis->shuffle_active==ACTIVE)
	{
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,164,89, pThis->pressed==PRESSED_SHUFFLE?SHUFREP_SHUFFLE_PRESSED:SHUFREP_SHUFFLE_UNPRESSED);
	} else {
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,164,89, pThis->pressed==PRESSED_SHUFFLE?SHUFREP_NO_SHUFFLE_PRESSED:SHUFREP_NO_SHUFFLE_UNPRESSED);
	}

// repeat
	if (pThis->repeat_active==ACTIVE)
	{
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,211,89, pThis->pressed==PRESSED_REPEAT?SHUFREP_REPEAT_PRESSED:SHUFREP_REPEAT_UNPRESSED);
	} else {
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,211,89, pThis->pressed==PRESSED_REPEAT?SHUFREP_NO_REPEAT_PRESSED:SHUFREP_NO_REPEAT_UNPRESSED);
	}
// info
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,253,91,MAIN_INFO);

	if (pThis->scaledPixbuf!=NULL)
	{
		g_object_unref(pThis->scaledPixbuf);
	}
//	pThis->scaledPixbuf=gdk_pixbuf_scale_simple(pThis->mainPixbuf,275*pThis->scalefactor,116*pThis->scalefactor,GDK_INTERP_HYPER);	
	pThis->scaledPixbuf=gdk_pixbuf_scale_simple(pThis->mainPixbuf,275*pThis->scalefactor,116*pThis->scalefactor,GDK_INTERP_NEAREST);
	gtk_image_set_from_pixbuf(GTK_IMAGE(pThis->mainImage),pThis->scaledPixbuf);
	gtk_widget_queue_draw(pThis->mainImage);
	gtk_widget_queue_draw(pThis->mainWindow);
}

void mainwindow_allocate(GtkWidget *widget,GtkAllocation *allocation, gpointer user_data)
{
	tHandleMainWindow* pThis=(tHandleMainWindow*)user_data;
	gint posx,posy;
	

	gtk_window_get_position(GTK_WINDOW(widget),&posx,&posy);
	pThis->width=allocation->width;
	pThis->height=allocation->height;
	pThis->posx=posx;
	pThis->posy=posy;


	mainwindow_redraw(pThis);
}
static gboolean mainwindow_mousepressed(GtkWidget *widget,GdkEventButton* event,gpointer user_data)
{
	tHandleMainWindow* pThis=(tHandleMainWindow*)user_data;
	int posx,posy;
	eMainWindowPressed pressed;
	posx=(int)event->x;
	posy=(int)event->y;

	pressed=mainwindow_findPressable(posx,posy,pThis->scalefactor);
	pThis->pressed=pressed;
	mainwindow_redraw(pThis);
	return TRUE;
}
static gboolean mainwindow_mousereleased(GtkWidget *widget,GdkEventButton* event,gpointer user_data)
{
	tHandleMainWindow* pThis=(tHandleMainWindow*)user_data;
	int posx,posy;
	eMainWindowPressed pressed;


	posx=(int)event->x;
	posy=(int)event->y;


	pressed=mainwindow_findPressable(posx,posy,pThis->scalefactor);
	// TODO: if pThis->pressed==pressed && pressed!=PRESSED_NONE
	if (pThis->pressed==pressed && pressed!=PRESSED_NONE)
	{
		if (pressed==PRESSED_CLUTTERBAR_D) 
		{
			pThis->scalefactor*=2;
			if (pThis->scalefactor>=16) pThis->scalefactor=1;
			gtk_window_resize(GTK_WINDOW(pThis->mainWindow), 275*pThis->scalefactor, 116*pThis->scalefactor);
		}
	} else {

	}
	pThis->pressed=PRESSED_NONE;
	mainwindow_redraw(pThis);
	return TRUE;
}

int mainwindow_setnumbers(tHandleMainWindow* pThis,int minutes,int seconds)
{
	if (minutes>100) minutes%=100;
	if (minutes<10)
	{
		pThis->time_digit[0]=10;
	} else {
		pThis->time_digit[0]=minutes/10;
	}
	pThis->time_digit[1]=minutes%10;

	pThis->time_digit[2]=seconds/10;
	pThis->time_digit[3]=seconds%10;

	return MAINWINDOW_OK;
}

int mainwindow_init(tHandleMainWindow* pThis,tHandlePixbufLoader *pPixbuf)
{


	mainwindow_defaultvalues(pThis);
	// step 1: create the pixbuf as the "drawing area"
	pThis->pHandlePixbufLoader=pPixbuf;
	pThis->scaledPixbuf=NULL;
	pThis->mainPixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,275,116);


	mainwindow_setnumbers(pThis,47,11);

	pThis->mainWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	pThis->layout=gtk_layout_new(NULL,NULL);
	gtk_container_add(GTK_CONTAINER(pThis->mainWindow),pThis->layout);
	gtk_window_set_default_size(GTK_WINDOW(pThis->mainWindow), 275*pThis->scalefactor, 116*pThis->scalefactor);
	gtk_widget_show(pThis->layout);
	pThis->mainImage=gtk_image_new();
	gtk_image_set_from_pixbuf(GTK_IMAGE(pThis->mainImage),pThis->mainPixbuf);
	gtk_widget_queue_draw(pThis->mainImage);
	gtk_layout_put(GTK_LAYOUT(pThis->layout),pThis->mainImage,0,0);
	gtk_widget_show(pThis->mainImage);
	gtk_widget_show(pThis->mainWindow);
	g_signal_connect(pThis->mainWindow, "size-allocate", G_CALLBACK(mainwindow_allocate), pThis);
	gtk_window_set_default_size(GTK_WINDOW(pThis->mainWindow), 275*pThis->scalefactor, 116*pThis->scalefactor);
        g_signal_connect(pThis->mainWindow, "destroy",G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(pThis->mainWindow, "button-press-event", G_CALLBACK(mainwindow_mousepressed), pThis);
	g_signal_connect(pThis->mainWindow, "button-release-event", G_CALLBACK(mainwindow_mousereleased), pThis);




	return MAINWINDOW_OK;
	
}


