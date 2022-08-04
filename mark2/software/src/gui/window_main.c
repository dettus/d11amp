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

#define PRESSABLE_NUM           19
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
	{.pressed=PRESSED_REPEAT,           .posx=211,.posy=89,.dimx= 27,.dimy=15}
};




// callbacks

int window_main_refresh(tHandleWindowMain* pThis)
{
	if (pThis->pixbufScaled!=NULL)
	{
		g_object_unref(pThis->pixbufScaled);
	}
	pThis->pixbufScaled=gdk_pixbuf_scale_simpe(pThis->pixbufMain,275*pThis->scaleFactor,116*pThis->scaleFactor,GDK_INTERP_NEAREST);
	gtk_image_set_from_pixbuf(GTK_IMAGE(pThis->widgetMainImage),pThis->pixbufScaled);
	gtk_widget_queue_draw(pThis->widgetMainImage);
	gtk_widget_queue_draw(pThis->widgetMainWindow);

	return RETVAL_OK;	
}

int window_main_redraw(tHandleWindowMain* pThis)
{
	int i;
	eElementID numbers[11]={NUMBERS_0,NUMBERS_1,NUMBERS_2,NUMBERS_3,NUMBERS_4,NUMBERS_5,NUMBERS_6,NUMBERS_7,NUMBERS_8,NUMBERS_9,NUMBERS_BLANK};
	int timedigitposx[4]={48,60,78,90};
	// the titlebar
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,0, 0,(pThis->titlebar_active==ACTIVE)?TITLEBAR_NORMAL_TITLEBAR_ACTIVE:TITLEBAR_NORMAL_TITLEBAR_INACTIVE);
	// the main background
	pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,0,14,MAIN_MAIN_DISPLAY);
	// the clutterbar
	if (pThis->clutterbar_shown==SHOWN)
	{
		switch(pThis->pressed)
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
		pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,timedigitposx[i],26,numbers[pThis->time_digit[i]]);
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
                x=pThis->songInfo_scrollpos;
                if (x>pThis->songInfo_scrolllen-margin)
                {
                        x=pThis->songInfo_scrolllen-margin;
                }
                if (x<0) x=0;
                gdk_pixbuf_copy_area(pThis->songInfoPixbuf,x,0,154,6,pThis->pixbufMain,111,27);

                gdk_pixbuf_copy_area(pThis->bitratePixbuf,0,0,15,6,pThis->pixbufMain,111,43);
                gdk_pixbuf_copy_area(pThis->khzPixbuf,0,0,10,6,pThis->pixbufMain,156,43);
        }
// volume adjuster
        {
                eElementID volumes[29]={ VOLUME_000_001, VOLUME_003_005, VOLUME_007_009, VOLUME_011_013, VOLUME_015_017, VOLUME_019_021, VOLUME_023_025, VOLUME_027_029, VOLUME_031, VOLUME_033_035, VOLUME_037_039, VOLUME_041_043, VOLUME_045_047, VOLUME_049_050, VOLUME_052_054, VOLUME_056_058, VOLUME_060_062, VOLUME_064, VOLUME_066_068, VOLUME_070_072, VOLUME_074_076, VOLUME_078_080, VOLUME_082_084, VOLUME_086_088, VOLUME_090_092, VOLUME_094_096, VOLUME_098, VOLUME_100 };
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,107,57, volumes[pThis->volume_setting]);
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,107+pThis->volume_setting*2,57, (pThis->pressed==PRESSED_VOLUME_SLIDER)?VOLUME_SLIDER_PRESSED:VOLUME_SLIDER_UNPRESSED);
        }
// the balance slider
        {
                eElementID balances[19]={BALANCE_CENTERED, BALANCE_FILLER0, BALANCE_FILLER1, BALANCE_FILLER2, BALANCE_24LEFTORRIGHT, BALANCE_33LEFTORRIGHT, BALANCE_FILLER6, BALANCE_40LEFTORRIGHT, BALANCE_FILLER8, BALANCE_49LEFTORRIGHT, BALANCE_FILLER9, BALANCE_58LEFTORRIGHT, BALANCE_FILLER10, BALANCE_66LEFTORRIGHT, BALANCE_FILLER12, BALANCE_74LEFTORRIGHT, BALANCE_82LEFTORRIGHT, BALANCE_91LEFTORRIGHT, BALANCE_100LEFTORRIGHT};
                int b;

                b=pThis->balance_setting;
                if (b<0) b=-b;
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,177,57, balances[b]);
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,177+38/2-14/2+pThis->balance_setting,57,pThis->pressed==PRESSED_BALANCE_SLIDER?BALANCE_SLIDER_PRESSED:BALANCE_SLIDER_UNPRESSED);
        }
// equalizer toggle
        if (pThis->equalizer_active==ACTIVE)
        {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,219,58, pThis->pressed==PRESSED_EQUALIZER?SHUFREP_EQUALIZER_PRESSED:SHUFREP_EQUALIZER_UNPRESSED);
        } else {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,219,58, pThis->pressed==PRESSED_EQUALIZER?SHUFREP_NO_EQUALIZER_PRESSED:SHUFREP_NO_EQUALIZER_UNPRESSED);
        }
// playlist toggle
        if (pThis->playlist_active==ACTIVE)
        {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,242,58, pThis->pressed==PRESSED_PLAYLIST?SHUFREP_PLAYLIST_PRESSED:SHUFREP_PLAYLIST_UNPRESSED);
        } else {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,242,58, pThis->pressed==PRESSED_PLAYLIST?SHUFREP_NO_PLAYLIST_PRESSED:SHUFREP_NO_PLAYLIST_UNPRESSED);
        }
// song position
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,16,72, POSBAR_SONG_PROGRESS_BAR);
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,16+pThis->songpos,72, (pThis->pressed==PRESSED_SONGPOS)?POSBAR_SONG_SLIDER_PRESSED:POSBAR_SONG_SLIDER_UNPRESSED);

// cbuttons
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain, 16,88,pThis->pressed==PRESSED_PREV?CBUTTONS_PREV_BUTTON_PRESSED:CBUTTONS_PREV_BUTTON_UNPRESSED);
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain, 39,88,pThis->pressed==PRESSED_PLAY?CBUTTONS_PLAY_BUTTON_PRESSED:CBUTTONS_PLAY_BUTTON_UNPRESSED);
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain, 62,88,pThis->pressed==PRESSED_PAUSE?CBUTTONS_PAUSE_BUTTON_PRESSED:CBUTTONS_PAUSE_BUTTON_UNPRESSED);
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain, 85,88,pThis->pressed==PRESSED_STOP?CBUTTONS_STOP_BUTTON_PRESSED:CBUTTONS_STOP_BUTTON_UNPRESSED);
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,108,88,pThis->pressed==PRESSED_NEXT?CBUTTONS_NEXT_BUTTON_PRESSED:CBUTTONS_NEXT_BUTTON_UNPRESSED);
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,136,88,pThis->pressed==PRESSED_OPEN?CBUTTONS_OPEN_BUTTON_PRESSED:CBUTTONS_OPEN_BUTTON_UNPRESSED);

// shuffle
        if (pThis->shuffle_active==ACTIVE)
        {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,164,89, pThis->pressed==PRESSED_SHUFFLE?SHUFREP_SHUFFLE_PRESSED:SHUFREP_SHUFFLE_UNPRESSED);
        } else {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,164,89, pThis->pressed==PRESSED_SHUFFLE?SHUFREP_NO_SHUFFLE_PRESSED:SHUFREP_NO_SHUFFLE_UNPRESSED);
        }

// repeat
        if (pThis->repeat_active==ACTIVE)
        {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,211,89, pThis->pressed==PRESSED_REPEAT?SHUFREP_REPEAT_PRESSED:SHUFREP_REPEAT_UNPRESSED);
        } else {
                pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,211,89, pThis->pressed==PRESSED_REPEAT?SHUFREP_NO_REPEAT_PRESSED:SHUFREP_NO_REPEAT_UNPRESSED);
        }
// info
        pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->pixbufMain,253,91,MAIN_INFO);
	return window_main_refresh(pThis);

}

// threads




int window_main_init(tHandleWindowMain* pThis,tHandlePixbufLoader* pHandlePixbufLoader)
{
	memset(pThis,0,sizeof(tHandleWindowMain));
	pThis->scaleFactor=1;

	pThis->pHandlePixbufLoader=pHandlePixbufLoader;
	pThis->pixbufMain=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,275,116);
	pThis->pixbufScaled=NULL;
	pThis->pixbufSongInfo=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,154,6);
	pThis->pixbufBitrate=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,15,6);
	pThis->pixbufSamplerate=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,10,6);
	
	pThis->widgetMainWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	pThis->widgetMainLayout=gtk_layout_new(NULL,NULL);
	pThis->widgetMainImage=gtk_image_new();
	

	gtk_window_set_default_size(GTK_WINDOW(pThis->widgetMainWindow),275*pThis->scaleFactor,116*pThis->scaleFactor);
	gtk_container_add(GTK_CONTAINER(pThis->widgetMainWindow),pThis->widgetMainLayout);
	gtk_layout_put(GTK_LAYOUT(pThis->widgetMainLayout),0,0);


	gtk_widget_show(pThis->widgetMainLayout);
	gtk_widget_show(pThis->widgetMainImage);
	gtk_widget_show(pThis->widgetMainWindow);


	pthread_mutex_init(&pThis->window_main_mutex,NULL);
}

int window_main_update_songinfo(tHandleWindowMain* pThis,tSongInfo *pSongInfo)
{

}

int window_main_update_pcmsamples(tHandleWindowMain* pThis,tPcmSink *pPcmSink)
{

}



