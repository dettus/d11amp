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

#include "window_main.h"
#include "datastructures.h"
#include "decoder.h"
#include "debugging.h"
#include "visualizer.h"
#include <string.h>



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



// callbacks

int window_main_refresh(tHandleWindowMain* pThis)
{
	printf("refresh\n");
	if (pThis->pixbufScaled!=NULL)
	{
		g_object_unref(pThis->pixbufScaled);
		pThis->pixbufScaled=NULL;
	}
	pThis->pixbufScaled=gdk_pixbuf_scale_simple(pThis->pixbufMain,275*pThis->scaleFactor,116*pThis->scaleFactor,GDK_INTERP_NEAREST);
	gtk_image_set_from_pixbuf(GTK_IMAGE(pThis->widgetMainImage),pThis->pixbufScaled);
	gtk_widget_queue_draw(pThis->widgetMainImage);
	gtk_widget_queue_draw(pThis->widgetMainWindow);
	gtk_window_resize(GTK_WINDOW(pThis->widgetMainWindow),275*pThis->scaleFactor,116*pThis->scaleFactor);

	return RETVAL_OK;	
}

int window_main_redraw(tHandleWindowMain* pThis)
{
	int i;
	eElementID numbers[11]={NUMBERS_0,NUMBERS_1,NUMBERS_2,NUMBERS_3,NUMBERS_4,NUMBERS_5,NUMBERS_6,NUMBERS_7,NUMBERS_8,NUMBERS_9,NUMBERS_BLANK};
	int timedigitposx[4]={48,60,78,90};
	printf("redraw\n");
	pthread_mutex_lock(&pThis->mutex);


	// the titlebar
	

	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,0, 0,(pThis->statusTitleBar==ACTIVE)?TITLEBAR_NORMAL_TITLEBAR_ACTIVE:TITLEBAR_NORMAL_TITLEBAR_INACTIVE);
	// the main background
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,0,14,MAIN_MAIN_DISPLAY);
	// the clutterbar
	if (pThis->statusClutterBar==SHOWN)
	{
		switch(pThis->lastPressed)
		{
			case PRESSED_CLUTTERBAR_O:      pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,10,22,TITLEBAR_CLUTTERBAR_O_PRESSED);break;
			case PRESSED_CLUTTERBAR_A:      pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,10,22,TITLEBAR_CLUTTERBAR_A_PRESSED);break;
			case PRESSED_CLUTTERBAR_I:      pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,10,22,TITLEBAR_CLUTTERBAR_I_PRESSED);break;
			case PRESSED_CLUTTERBAR_D:      pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,10,22,TITLEBAR_CLUTTERBAR_D_PRESSED);break;
			case PRESSED_CLUTTERBAR_V:      pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,10,22,TITLEBAR_CLUTTERBAR_V_PRESSED);break;
			default:        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,10,22,TITLEBAR_CLUTTERBAR_SHOWN);break;
		}
	} else {
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,10,22,TITLEBAR_CLUTTERBAR_HIDDEN);
	}
	// the time display
	for (i=0;i<4;i++)
	{
		int digit;
		digit=pThis->statusTimeDigits[i];
		if (digit>=0 && digit<=10)
		{
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,timedigitposx[i],26,numbers[digit]);
		}
	}
	// playpause indicator
	switch(pThis->statusPlayPause)
	{
		case PLAYPAUSE_PLAY:    pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,24,18, PLAYPAUSE_PLAY_INDICATOR);break;
		case PLAYPAUSE_PAUSE:   pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,24,18, PLAYPAUSE_PAUSE_INDICATOR);break;
		case PLAYPAUSE_STOP:    pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,24,18, PLAYPAUSE_STOP_INDICATOR);break;
		case PLAYPAUSE_START:   pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,24,18, PLAYPAUSE_START_OF_SONG_INDICATOR);break;
		case PLAYPAUSE_END:     pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,24,18, PLAYPAUSE_END_OF_SONG_INDICATOR);break;
	}
	// mono or stereo?
	switch(pThis->statusMonoSter)
	{
		case MONOSTER_MONO:
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,212,41, MONOSTER_MONO_ACTIVE);
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,239,41, MONOSTER_STEREO_INACTIVE);
			break;
		case MONOSTER_STEREO:
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,212,41, MONOSTER_MONO_INACTIVE);
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,239,41, MONOSTER_STEREO_ACTIVE);
			break;
		default:
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,212,41, MONOSTER_MONO_INACTIVE);
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,239,41, MONOSTER_STEREO_INACTIVE);
			break;
	}
// kbps and khz labels
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,111,43, MAIN_KBPS_DISPLAY);
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,156,43, MAIN_KHZ_DISPLAY);
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
                gdk_pixbuf_copy_area(pThis->pixbufSongInfo,x,0,154,6,pThis->pixbufMain,111,27);

                gdk_pixbuf_copy_area(pThis->pixbufBitrate,0,0,15,6,pThis->pixbufMain,111,43);
                gdk_pixbuf_copy_area(pThis->pixbufSamplerate,0,0,10,6,pThis->pixbufMain,156,43);
        }
// volume adjuster
        {
                eElementID volumes[28]={ VOLUME_000_001, VOLUME_003_005, VOLUME_007_009, VOLUME_011_013, VOLUME_015_017, VOLUME_019_021, VOLUME_023_025, VOLUME_027_029, VOLUME_031, VOLUME_033_035, VOLUME_037_039, VOLUME_041_043, VOLUME_045_047, VOLUME_049_050, VOLUME_052_054, VOLUME_056_058, VOLUME_060_062, VOLUME_064, VOLUME_066_068, VOLUME_070_072, VOLUME_074_076, VOLUME_078_080, VOLUME_082_084, VOLUME_086_088, VOLUME_090_092, VOLUME_094_096, VOLUME_098, VOLUME_100 };
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,107,57, volumes[pThis->statusVolume]);
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,107+pThis->statusVolume*2,57, (pThis->lastPressed==PRESSED_VOLUME_SLIDER)?VOLUME_SLIDER_PRESSED:VOLUME_SLIDER_UNPRESSED);
        }
// the balance slider
        {
                eElementID balances[19]={BALANCE_CENTERED, BALANCE_FILLER0, BALANCE_FILLER1, BALANCE_FILLER2, BALANCE_24LEFTORRIGHT, BALANCE_33LEFTORRIGHT, BALANCE_FILLER6, BALANCE_40LEFTORRIGHT, BALANCE_FILLER8, BALANCE_49LEFTORRIGHT, BALANCE_FILLER9, BALANCE_58LEFTORRIGHT, BALANCE_FILLER10, BALANCE_66LEFTORRIGHT, BALANCE_FILLER12, BALANCE_74LEFTORRIGHT, BALANCE_82LEFTORRIGHT, BALANCE_91LEFTORRIGHT, BALANCE_100LEFTORRIGHT};
                int b;

                b=pThis->statusBalance;
                if (b<0) b=-b;
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,177,57, balances[b]);
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,177+38/2-14/2+pThis->statusBalance,57,pThis->lastPressed==PRESSED_BALANCE_SLIDER?BALANCE_SLIDER_PRESSED:BALANCE_SLIDER_UNPRESSED);
        }
// equalizer toggle
        if (pThis->statusEqualizer==ACTIVE)
        {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,219,58, pThis->lastPressed==PRESSED_EQUALIZER?SHUFREP_EQUALIZER_PRESSED:SHUFREP_EQUALIZER_UNPRESSED);
        } else {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,219,58, pThis->lastPressed==PRESSED_EQUALIZER?SHUFREP_NO_EQUALIZER_PRESSED:SHUFREP_NO_EQUALIZER_UNPRESSED);
        }
// playlist toggle
        if (pThis->statusPlaylist==ACTIVE)
        {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,242,58, pThis->lastPressed==PRESSED_PLAYLIST?SHUFREP_PLAYLIST_PRESSED:SHUFREP_PLAYLIST_UNPRESSED);
        } else {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,242,58, pThis->lastPressed==PRESSED_PLAYLIST?SHUFREP_NO_PLAYLIST_PRESSED:SHUFREP_NO_PLAYLIST_UNPRESSED);
        }
// song position
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,16,72, POSBAR_SONG_PROGRESS_BAR);
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,16+pThis->statusSongPos,72, (pThis->lastPressed==PRESSED_SONGPOS)?POSBAR_SONG_SLIDER_PRESSED:POSBAR_SONG_SLIDER_UNPRESSED);

// cbuttons
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain, 16,88,pThis->lastPressed==PRESSED_PREV?CBUTTONS_PREV_BUTTON_PRESSED:CBUTTONS_PREV_BUTTON_UNPRESSED);
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain, 39,88,pThis->lastPressed==PRESSED_PLAY?CBUTTONS_PLAY_BUTTON_PRESSED:CBUTTONS_PLAY_BUTTON_UNPRESSED);
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain, 62,88,pThis->lastPressed==PRESSED_PAUSE?CBUTTONS_PAUSE_BUTTON_PRESSED:CBUTTONS_PAUSE_BUTTON_UNPRESSED);
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain, 85,88,pThis->lastPressed==PRESSED_STOP?CBUTTONS_STOP_BUTTON_PRESSED:CBUTTONS_STOP_BUTTON_UNPRESSED);
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,108,88,pThis->lastPressed==PRESSED_NEXT?CBUTTONS_NEXT_BUTTON_PRESSED:CBUTTONS_NEXT_BUTTON_UNPRESSED);
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,136,88,pThis->lastPressed==PRESSED_OPEN?CBUTTONS_OPEN_BUTTON_PRESSED:CBUTTONS_OPEN_BUTTON_UNPRESSED);

// shuffle
        if (pThis->statusShuffle==ACTIVE)
        {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,164,89, pThis->lastPressed==PRESSED_SHUFFLE?SHUFREP_SHUFFLE_PRESSED:SHUFREP_SHUFFLE_UNPRESSED);
        } else {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,164,89, pThis->lastPressed==PRESSED_SHUFFLE?SHUFREP_NO_SHUFFLE_PRESSED:SHUFREP_NO_SHUFFLE_UNPRESSED);
        }

// repeat
        if (pThis->statusRepeat==ACTIVE)
        {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,211,89, pThis->lastPressed==PRESSED_REPEAT?SHUFREP_REPEAT_PRESSED:SHUFREP_REPEAT_UNPRESSED);
        } else {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,211,89, pThis->lastPressed==PRESSED_REPEAT?SHUFREP_NO_REPEAT_PRESSED:SHUFREP_NO_REPEAT_UNPRESSED);
        }
// info
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,253,91,MAIN_INFO);
// the visualizer
	visualizer_render(&(pThis->handleVisualizer),&pThis->pixbufVisualizer);
	gdk_pixbuf_copy_area(pThis->pixbufVisualizer,0,0,76,15,pThis->pixbufMain,24,44);	
	pthread_mutex_unlock(&pThis->mutex);
	return window_main_refresh(pThis);

}

int window_main_calculate_value_from_x(int x,int xleft,int xright,int margin, int scaleFactor,  int valueleft,int valueright,int *value)
{
	int delta_value;
	int delta_x;
	int retval;

	xleft+=margin;
	xright-=margin;


	delta_value=valueright-valueleft;
	delta_x=(xright-xleft)*scaleFactor;



	retval=RETVAL_NOK;
	
	if (x<xleft*scaleFactor)
	{
		*value=valueleft;
		retval=RETVAL_OK;
	} 
	else if (x>xright*scaleFactor)
	{
		*value=valueright;
		retval=RETVAL_OK;
	}
	else if (delta_x)
	{
		*value=(((x-xleft*scaleFactor)*delta_value)/delta_x)+valueleft;
		retval=RETVAL_OK;
	}	
	return retval;
}

int window_main_click_interaction(tHandleWindowMain* pThis,eMainWindowPressed pressed,int x,int y)
{
	int retval;
	int value;

	retval=RETVAL_NOK;
	DEBUGGING_WRITE_VCD("pressed",(int)pressed);
	switch(pressed)
	{
		case PRESSED_CLUTTERBAR_D:
			pThis->scaleFactor*=2;
			if (pThis->scaleFactor==32) pThis->scaleFactor=1;
			break;
		case PRESSED_CLUTTERBAR_V:
			visualizer_cycle(&(pThis->handleVisualizer));
			break;
		case PRESSED_OPEN:
			{
				GtkWidget *dialog;
				char *filename=NULL;
				dialog = gtk_file_chooser_dialog_new ("Open File",
					//	GTK_WINDOW(pThis->widgetMainWindow),
						NULL,
						GTK_FILE_CHOOSER_ACTION_OPEN,
						"_Cancel", GTK_RESPONSE_CANCEL,
						"_Open", GTK_RESPONSE_ACCEPT,
						NULL);
				if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
				{

					filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
				}
				gtk_widget_destroy (dialog);
				if (filename!=NULL)
				{
					decoder_openfile(pThis->pHandleDecoder,filename);
					g_free(filename);
				}
			}	
			break;
		case PRESSED_SONGPOS:
			{
				if (!window_main_calculate_value_from_x(x,16,16+248,30/2,pThis->scaleFactor,  0,pThis->songInfo_drawn.len, &value))
				{
					decoder_set_songPos(pThis->pHandleDecoder,value);
				}
			}
			break;
		case PRESSED_VOLUME_SLIDER:
			{
				if (!window_main_calculate_value_from_x(x,107,107+68,14/2,pThis->scaleFactor,0,100,&value))
				{
					int value2;
					audiooutput_setVolume(pThis->pHandleAudioOutput,value);
					value2=27*value/100;
					pThis->statusVolume=value2;
					// audiooutput_setvolume(value);
				}
			}
			break;
		case PRESSED_BALANCE_SLIDER:
			{
				if (!window_main_calculate_value_from_x(x,177,177+38,14/2,pThis->scaleFactor,-100,100,&value))
				{
					int value2;
					audiooutput_setBalance(pThis->pHandleAudioOutput,value);
					value2=(12*value)/100;
					pThis->statusBalance=value2;
					// audiooutput_setbalance(value);
				}
			}	
			break;
		case PRESSED_PLAY:
				decoder_set_state(pThis->pHandleDecoder,STATE_PLAY);
			break;
		case PRESSED_STOP:
				decoder_set_state(pThis->pHandleDecoder,STATE_STOP);
			break;
		case PRESSED_PAUSE:
				decoder_set_state(pThis->pHandleDecoder,STATE_PAUSE);
			break;
		case PRESSED_REPEAT:
				pThis->statusRepeat=(pThis->statusRepeat==ACTIVE)?INACTIVE:ACTIVE;
			break;
		case PRESSED_SHUFFLE:
				pThis->statusShuffle=(pThis->statusShuffle==ACTIVE)?INACTIVE:ACTIVE;
			break;
		default:
			printf("TODO: HANDLE PRESS %d\n",(int)pressed);
			retval=RETVAL_OK;
			break;	
	}

	return retval;
}
// gtk events



void window_main_event_allocate(GtkWidget *widget,GtkAllocation *allocation, gpointer user_data)
{
	tHandleWindowMain *pThis=(tHandleWindowMain*)user_data;
	gint x,y;
	gtk_window_get_position(GTK_WINDOW(widget),&x,&y);
	pThis->geometryX=x;
	pThis->geometryY=y;
	pThis->geometryWidth=allocation->width;
	pThis->geometryHeight=allocation->height;
	printf("allocate\n");
	pthread_mutex_lock(&pThis->mutex_click);
	window_main_redraw(pThis);
	pthread_mutex_unlock(&pThis->mutex_click);
}


static gboolean window_main_event_mouse_pressed(GtkWidget *widget,GdkEventButton *event, gpointer user_data)
{
	tHandleWindowMain *pThis=(tHandleWindowMain*)user_data;
	int x,y;
	x=(int)event->x;
	y=(int)event->y;
	pthread_mutex_lock(&pThis->mutex_click);

	pThis->lastPressed=window_main_find_pressable(x,y,pThis->scaleFactor);

	window_main_redraw(pThis);	
	pthread_mutex_unlock(&pThis->mutex_click);

	return TRUE;
}

static gboolean window_main_event_mouse_released(GtkWidget *widget,GdkEventButton *event, gpointer user_data)
{
	tHandleWindowMain *pThis=(tHandleWindowMain*)user_data;
	int x,y;
	eMainWindowPressed      pressed;
	x=(int)event->x;
	y=(int)event->y;

	pressed=window_main_find_pressable(x,y,pThis->scaleFactor);
	if (pressed==pThis->lastPressed && pressed!=PRESSED_NONE)
	{
		window_main_click_interaction(pThis,pressed,x,y);
	}
	
	pThis->lastPressed=PRESSED_NONE;
	pthread_mutex_lock(&pThis->mutex_click);
	window_main_redraw(pThis);
	pthread_mutex_unlock(&pThis->mutex_click);

	return TRUE;
}
int window_main_render_text(tHandleWindowMain* pThis,char* text,int minwidth,GdkPixbuf **pPixbuf,eElementID background_element,int *xpos)
{
	int i;
	int x;
	int len;
	int width;
	printf("render text\n");
	pthread_mutex_lock(&pThis->mutex);
	
	if (*pPixbuf!=NULL)
	{
		g_object_unref(*pPixbuf);
		*pPixbuf=NULL;
	}
	len=strlen(text);
	width=len*5;
	if (width<minwidth) width=minwidth;
	*xpos=width;
	*pPixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,width,6);

	i=0;
	x=0;
	while (i<len)
	{
		eElementID elementID;
		elementID=ELEMENT_NONE;
		if (i<len-3)
		{
			if (text[i]=='.' && text[i+1]=='.' && text[i+2]=='.')
			{
				elementID=TEXT_ELLIPSIS;
				i+=2;
			}
		}
		if (elementID==ELEMENT_NONE)
		{
			pixbufloader_textelement(pThis->pHandlePixbufLoader,text[i],&elementID,background_element);
		}
		pixbufloader_addelement(pThis->pHandlePixbufLoader,*pPixbuf,x,0,elementID);
		x+=5;
		i++;
	}
	while (x<minwidth)
	{
		pixbufloader_addelement(pThis->pHandlePixbufLoader,*pPixbuf,x,0,background_element);
		x+=5;
	}
	pthread_mutex_unlock(&pThis->mutex);
	return RETVAL_OK;
}


// threads

void *window_main_animation_thread(void* handle)
{
	tHandleWindowMain* pThis=(tHandleWindowMain*)handle;
	int margin;
	tSongInfo songInfo;
	eDecoderState decState;
	while (1)
	{
// animation effects: scroll the text
		margin=154;
		pThis->scrollpos++;
		if (pThis->scrollpos>=pThis->scrolllen-margin/2)
		{
			pThis->scrollpos=-margin/2;
		}



// status
		decoder_get_state(pThis->pHandleDecoder,&decState);
		switch(decState)
		{
			case STATE_STOP:
				pThis->statusPlayPause=PLAYPAUSE_STOP;
				break;	
			case STATE_PLAY:
				pThis->statusPlayPause=PLAYPAUSE_PLAY;
				break;	
			case STATE_PAUSE:
				pThis->statusPlayPause=PLAYPAUSE_PAUSE;
				break;	
			case STATE_EOF:
				pThis->statusPlayPause=PLAYPAUSE_END;
				break;	
			default:
				pThis->statusPlayPause=PLAYPAUSE_STOP;
				break;
		}
// update the gui with information from the decoder
		decoder_get_songInfo(pThis->pHandleDecoder,&songInfo);
		if (memcmp(&songInfo,&pThis->songInfo_drawn,sizeof(tSongInfo)))
		{
			window_main_refresh_songinfo(pThis,songInfo);
		}
		{
			signed short pcm[512];
			audiooutput_getLastSamples(pThis->pHandleAudioOutput,pcm,512);
			visualizer_newPcm(&(pThis->handleVisualizer),pcm,512);
				
		}
		window_main_redraw(pThis);
		usleep(50000);	// sleep for 50 ms		 --> 20fps
		printf("animation step\n");
	}
}

//
int window_main_init(tHandleWindowMain* pThis,tHandlePixbufLoader* pHandlePixbufLoader,tHandleDecoder* pHandleDecoder,tHandleAudioOutput* pHandleAudioOutput)
{
	memset(pThis,0,sizeof(tHandleWindowMain));
	pThis->scaleFactor=4;

	pThis->pHandlePixbufLoader=pHandlePixbufLoader;
	pThis->pHandleDecoder=pHandleDecoder;
	pThis->pHandleAudioOutput=pHandleAudioOutput;
	pThis->pixbufMain=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,275,116);
	pThis->pixbufScaled=NULL;
	pThis->pixbufSongInfo=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,154,6);
	pThis->pixbufBitrate=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,15,6);
	pThis->pixbufSamplerate=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,10,6);
	
	pThis->widgetMainWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	pThis->widgetMainLayout=gtk_layout_new(NULL,NULL);
	pThis->widgetMainImage=gtk_image_new();



	g_signal_connect(pThis->widgetMainWindow,"size-allocate",       G_CALLBACK(window_main_event_allocate),pThis);
	g_signal_connect(pThis->widgetMainWindow,"destroy",             G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(pThis->widgetMainWindow,"button-press-event",  G_CALLBACK(window_main_event_mouse_pressed),pThis);
	g_signal_connect(pThis->widgetMainWindow,"button-release-event",G_CALLBACK(window_main_event_mouse_released),pThis);
	

	gtk_window_set_default_size(GTK_WINDOW(pThis->widgetMainWindow),275*pThis->scaleFactor,116*pThis->scaleFactor);
	gtk_container_add(GTK_CONTAINER(pThis->widgetMainWindow),pThis->widgetMainLayout);
	gtk_layout_put(GTK_LAYOUT(pThis->widgetMainLayout),pThis->widgetMainImage,0,0);


	gtk_widget_show(pThis->widgetMainLayout);
	gtk_widget_show(pThis->widgetMainImage);

// initialize the GUI elements
	pThis->statusClutterBar=SHOWN;
	pThis->statusBalance=0; // center balance
	pThis->statusVolume=27;	// maximum volume

	snprintf(pThis->songInfo_drawn.songinfo,256,"Hello World");
	pThis->songInfo_drawn.bitrate=127;
	pThis->songInfo_drawn.samplerate=44;


	visualizer_init(&(pThis->handleVisualizer));


// initialize the interactive elements
	pThis->lastPressed=PRESSED_NONE;


	pthread_mutex_init(&pThis->mutex,NULL);
	pthread_mutex_init(&pThis->mutex_click,NULL);
	return RETVAL_OK;
}

int window_main_refresh_songinfo(tHandleWindowMain* pThis,tSongInfo songInfo)
{
	
	char tmp[16];
	int change;
	int xpos;

// textual elements	
	if (songInfo.songinfo[0])	// if there is the name of the song parsed by the decoder
	{
		if (strncmp(pThis->songInfo_drawn.songinfo,songInfo.songinfo,256))
		{
			window_main_render_text(pThis,songInfo.songinfo,154,&(pThis->pixbufSongInfo),TEXT_TITLE_DISPLAY_SPACE,&xpos);
			pThis->scrolllen=xpos;
			change=1;
		}
	} else {		// if not, display the filename
		if (strncmp(pThis->songInfo_drawn.filename,songInfo.filename,1024))
		{
			window_main_render_text(pThis,songInfo.filename,155,&(pThis->pixbufSongInfo),TEXT_TITLE_DISPLAY_SPACE,&xpos);
			pThis->scrolllen=xpos;
			change=1;
		}
	}
	if (pThis->songInfo_drawn.bitrate!=songInfo.bitrate)
	{
		snprintf(tmp,16,"%3d",songInfo.bitrate);
		window_main_render_text(pThis,tmp,15,&(pThis->pixbufBitrate),TEXT_KBPS_DISPLAY_SPACE,&xpos);
		change=1;
	}
	if (pThis->songInfo_drawn.samplerate!=songInfo.samplerate)
	{
		int kbps;
		kbps=songInfo.samplerate/1000;
		if (kbps<0) kbps=0;
		if (kbps>99) kbps=99;
		snprintf(tmp,16,"%2d",kbps);
		window_main_render_text(pThis,tmp,10,&(pThis->pixbufSamplerate),TEXT_KBPS_DISPLAY_SPACE,&xpos);
		change=1;
	}


// time is being displayed here
	if (pThis->songInfo_drawn.pos!=songInfo.pos || pThis->songInfo_drawn.len!=songInfo.len)
	{
// first: as the MM:SS 
		int minutes;
		int seconds;
		minutes=songInfo.pos/60;
		seconds=songInfo.pos%60;

		if (minutes>99) minutes=99;
		if (minutes<10) pThis->statusTimeDigits[0]=10;	// empty space
		else pThis->statusTimeDigits[0]=minutes/10;
		pThis->statusTimeDigits[1]=minutes%10;
		pThis->statusTimeDigits[2]=seconds/10;
		pThis->statusTimeDigits[3]=seconds%10;
		change=1;
// second: as the position bar slider
		if (songInfo.len)
		{
			pThis->statusSongPos=((248-29)*songInfo.pos)/songInfo.len;
		} else {
			pThis->statusSongPos=0;
		}
		change=1;
	}
	if (pThis->songInfo_drawn.channels!=songInfo.channels)
	{
		switch(songInfo.channels)
		{
			case 1:
				pThis->statusMonoSter=MONOSTER_MONO;
				break;
			case 2:
				pThis->statusMonoSter=MONOSTER_STEREO;
				break;
			default:
				pThis->statusMonoSter=MONOSTER_UNKNOWN;
				break;
		}
		change=1;
	}
	if (change)
	{
		pThis->songInfo_drawn=songInfo;
//		window_main_redraw(pThis);
	}
	return RETVAL_OK;
	
}
int window_main_update_songinfo(tHandleWindowMain* pThis,tSongInfo *pSongInfo)
{

	memcpy(&(pThis->songInfo_drawn),pSongInfo,sizeof(tSongInfo));
	window_main_refresh_songinfo(pThis,*pSongInfo);
	


	return RETVAL_OK;
}

int window_main_update_pcmsamples(tHandleWindowMain* pThis,tPcmSink *pPcmSink)
{
	return RETVAL_OK;

}

int window_main_run(tHandleWindowMain* pThis)
{
	gtk_widget_show(pThis->widgetMainWindow);
	pthread_create(&pThis->threadWindowMain,NULL,window_main_animation_thread,(void*)pThis);
	return RETVAL_OK;
}
int window_main_get_shuffle_repeat(tHandleWindowMain* pThis,int* pShuffle,int* pRepeat)
{
	pthread_mutex_lock(&(pThis->mutex_click));
	*pShuffle=(pThis->statusShuffle==ACTIVE);
	*pRepeat=(pThis->statusRepeat==ACTIVE);
	pthread_mutex_unlock(&(pThis->mutex_click));
	return RETVAL_OK;
}
