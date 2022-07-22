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

void mainwindow_defaultvalues(tHandleMainWindow* pThis)
{
	pThis->titlebar_active=INACTIVE;
	pThis->clutterbar_shown=SHOWN;
	pThis->clutterbar_pressed=0;
	pThis->time_digit[0]=1;
	pThis->time_digit[1]=3;
	pThis->time_digit[2]=3;
	pThis->time_digit[3]=7;
	pThis->playpause_state=PLAYPAUSE_START;
	pThis->volume_setting=18;	// between 0 and 28
	pThis->volume_slider=PRESSED;	
	pThis->balance_setting=0;	// between -18..0..18
	pThis->balance_pressed=UNPRESSED;	

	pThis->equalizer_pressed=UNPRESSED;
	pThis->equalizer_active=INACTIVE;
	pThis->playlist_pressed=UNPRESSED;
	pThis->playlist_active=INACTIVE;

	pThis->songpos=13;	// between 0 and 72-29
	pThis->songpos_pressed=UNPRESSED;

	pThis->cbutton_pressed=CBUTTON_NONE;

	pThis->shuffle_pressed=UNPRESSED;
	pThis->shuffle_active=INACTIVE;
	pThis->repeat_pressed=UNPRESSED;
	pThis->repeat_active=INACTIVE;
}
void mainwindow_redraw(tHandleMainWindow* pThis,GtkWidget* widget)
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
		switch(pThis->clutterbar_pressed)
		{
			case 'O':	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,10,22,TITLEBAR_CLUTTERBAR_O_PRESSED);break;
			case 'A':	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,10,22,TITLEBAR_CLUTTERBAR_A_PRESSED);break;
			case 'I':	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,10,22,TITLEBAR_CLUTTERBAR_I_PRESSED);break;
			case 'D':	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,10,22,TITLEBAR_CLUTTERBAR_D_PRESSED);break;
			case 'V':	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,10,22,TITLEBAR_CLUTTERBAR_V_PRESSED);break;
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
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,107+pThis->volume_setting*2,57, (pThis->volume_slider==PRESSED)?VOLUME_SLIDER_PRESSED:VOLUME_SLIDER_UNPRESSED);
	}
// the balance slider
	{
		eElementID balances[19]={BALANCE_CENTERED, BALANCE_FILLER0, BALANCE_FILLER1, BALANCE_FILLER2, BALANCE_24LEFTORRIGHT, BALANCE_33LEFTORRIGHT, BALANCE_FILLER6, BALANCE_40LEFTORRIGHT, BALANCE_FILLER8, BALANCE_49LEFTORRIGHT, BALANCE_FILLER9, BALANCE_58LEFTORRIGHT, BALANCE_FILLER10, BALANCE_66LEFTORRIGHT, BALANCE_FILLER12, BALANCE_74LEFTORRIGHT, BALANCE_82LEFTORRIGHT, BALANCE_91LEFTORRIGHT, BALANCE_100LEFTORRIGHT};
		int b;

		b=pThis->balance_setting;
		if (b<0) b=-b;
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,177,57, balances[b]);
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,177+38/2-14/2+pThis->balance_setting,57,pThis->balance_pressed==PRESSED?BALANCE_SLIDER_PRESSED:BALANCE_SLIDER_UNPRESSED);
	}	
// equalizer toggle
	if (pThis->equalizer_active==ACTIVE)
	{
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,219,58, pThis->equalizer_pressed==PRESSED?SHUFREP_EQUALIZER_PRESSED:SHUFREP_EQUALIZER_UNPRESSED);
	} else {
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,219,58, pThis->equalizer_pressed==PRESSED?SHUFREP_NO_EQUALIZER_PRESSED:SHUFREP_NO_EQUALIZER_UNPRESSED);

	}
// playlist toggle
	if (pThis->playlist_active==ACTIVE)
	{
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,242,58, pThis->playlist_pressed==PRESSED?SHUFREP_PLAYLIST_PRESSED:SHUFREP_PLAYLIST_UNPRESSED);
	} else {
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,242,58, pThis->playlist_pressed==PRESSED?SHUFREP_NO_PLAYLIST_PRESSED:SHUFREP_NO_PLAYLIST_UNPRESSED);
	}
// song position
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,16,72, POSBAR_SONG_PROGRESS_BAR);
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,16+pThis->songpos,72, (pThis->songpos_pressed==PRESSED)?POSBAR_SONG_SLIDER_PRESSED:POSBAR_SONG_SLIDER_UNPRESSED);

// cbuttons
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf, 16,88,pThis->cbutton_pressed==CBUTTON_PREV?CBUTTONS_PREV_BUTTON_PRESSED:CBUTTONS_PREV_BUTTON_UNPRESSED);
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf, 39,88,pThis->cbutton_pressed==CBUTTON_PLAY?CBUTTONS_PLAY_BUTTON_PRESSED:CBUTTONS_PLAY_BUTTON_UNPRESSED);
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf, 62,88,pThis->cbutton_pressed==CBUTTON_PAUSE?CBUTTONS_PAUSE_BUTTON_PRESSED:CBUTTONS_PAUSE_BUTTON_UNPRESSED);
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf, 85,88,pThis->cbutton_pressed==CBUTTON_STOP?CBUTTONS_STOP_BUTTON_PRESSED:CBUTTONS_STOP_BUTTON_UNPRESSED);
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,108,88,pThis->cbutton_pressed==CBUTTON_NEXT?CBUTTONS_NEXT_BUTTON_PRESSED:CBUTTONS_NEXT_BUTTON_UNPRESSED);
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,136,88,pThis->cbutton_pressed==CBUTTON_OPEN?CBUTTONS_OPEN_BUTTON_PRESSED:CBUTTONS_OPEN_BUTTON_UNPRESSED);

// shuffle
	if (pThis->shuffle_active==ACTIVE)
	{
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,164,89, pThis->shuffle_pressed==PRESSED?SHUFREP_SHUFFLE_PRESSED:SHUFREP_SHUFFLE_UNPRESSED);
	} else {
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,164,89, pThis->shuffle_pressed==PRESSED?SHUFREP_NO_SHUFFLE_PRESSED:SHUFREP_NO_SHUFFLE_UNPRESSED);

	}

// repeat
	if (pThis->repeat_active==ACTIVE)
	{
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,211,89, pThis->repeat_pressed==PRESSED?SHUFREP_REPEAT_PRESSED:SHUFREP_REPEAT_UNPRESSED);
	} else {
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,211,89, pThis->repeat_pressed==PRESSED?SHUFREP_NO_REPEAT_PRESSED:SHUFREP_NO_REPEAT_UNPRESSED);
	}
// info
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,253,91,MAIN_INFO);

	
	gtk_image_set_from_pixbuf(GTK_IMAGE(pThis->mainImage),pThis->mainPixbuf);
	gtk_widget_queue_draw(pThis->mainImage);
	gtk_widget_queue_draw(widget);
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


	mainwindow_redraw(pThis,widget);
}

int mainwindow_init(tHandleMainWindow* pThis,tHandlePixbufLoader *pPixbuf)
{


	mainwindow_defaultvalues(pThis);
	// step 1: create the pixbuf as the "drawing area"
	pThis->pHandlePixbufLoader=pPixbuf;
	pThis->mainPixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,275,116);



	pThis->mainWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(pThis->mainWindow),275,116);
	pThis->layout=gtk_layout_new(NULL,NULL);
	gtk_container_add(GTK_CONTAINER(pThis->mainWindow),pThis->layout);
	gtk_widget_show(pThis->layout);
	pThis->mainImage=gtk_image_new();
	gtk_image_set_from_pixbuf(GTK_IMAGE(pThis->mainImage),pThis->mainPixbuf);
	gtk_widget_queue_draw(pThis->mainImage);
	gtk_layout_put(GTK_LAYOUT(pThis->layout),pThis->mainImage,0,0);
	gtk_widget_show(pThis->mainImage);
	gtk_widget_show(pThis->mainWindow);
	pThis->scalefactor=1;
	g_signal_connect(pThis->mainWindow, "size-allocate", G_CALLBACK(mainwindow_allocate), pThis);
	gtk_window_set_default_size(GTK_WINDOW(pThis->mainWindow), 275*pThis->scalefactor, 116*pThis->scalefactor);
        g_signal_connect(pThis->mainWindow, "destroy",G_CALLBACK(gtk_main_quit), NULL);





	return MAINWINDOW_OK;
	
}


