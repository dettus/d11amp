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
#include "constants.h"
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

#define PRESSABLE_NUM           20


int window_main_draw(tHandleWindowMain* pThis,GdkPixbuf *pixbufDestination)
{
	int i;
	eElementID numbers[11]={NUMBERS_0,NUMBERS_1,NUMBERS_2,NUMBERS_3,NUMBERS_4,NUMBERS_5,NUMBERS_6,NUMBERS_7,NUMBERS_8,NUMBERS_9,NUMBERS_BLANK};

	int timedigitxpos[4]={48,60,78,90};	// TODO: magic numbers
	pthread_mutex_lock(&pThis->mutex);
	
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
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,107+pThis->statusVolume*2,57, (pThis->lastPressed==PRESSED_VOLUME_SLIDER)?VOLUME_SLIDER_PRESSED:VOLUME_SLIDER_UNPRESSED); // FIXME: this one is strange with some themes
        }
// the balance slider
        {
                eElementID balances[19]={BALANCE_CENTERED, BALANCE_FILLER0, BALANCE_FILLER1, BALANCE_FILLER2, BALANCE_24LEFTORRIGHT, BALANCE_33LEFTORRIGHT, BALANCE_FILLER6, BALANCE_40LEFTORRIGHT, BALANCE_FILLER8, BALANCE_49LEFTORRIGHT, BALANCE_FILLER9, BALANCE_58LEFTORRIGHT, BALANCE_FILLER10, BALANCE_66LEFTORRIGHT, BALANCE_FILLER12, BALANCE_74LEFTORRIGHT, BALANCE_82LEFTORRIGHT, BALANCE_91LEFTORRIGHT, BALANCE_100LEFTORRIGHT};
                int b;

                b=pThis->statusBalance;
                if (b<0) b=-b;
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,177,57, balances[b]);
                theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,177+38/2-14/2+pThis->statusBalance,57,pThis->lastPressed==PRESSED_BALANCE_SLIDER?BALANCE_SLIDER_PRESSED:BALANCE_SLIDER_UNPRESSED);	// FIXME: this one is strange with some themes
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


// visualizer
	visualizer_render(&(pThis->handleVisualizer),pixbufDestination,24,44);
	pthread_mutex_unlock(&pThis->mutex);

	return RETVAL_OK;		
}
int window_main_refresh(tHandleWindowMain* pThis)
{
	GdkPixbuf *pixbuf;
	pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);
	window_main_draw(pThis,pixbuf);
	gdk_pixbuf_copy_area(pixbuf,0,0,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,pThis->pixbuf,0,0);
	g_object_unref(pixbuf);	
	pthread_mutex_lock(&pThis->mutex);
	gtk_picture_set_pixbuf(GTK_PICTURE(pThis->picture),pThis->pixbuf);
	{
		
		gtk_widget_queue_draw(pThis->windowMain);
		
	}
	pthread_mutex_unlock(&pThis->mutex);

	return RETVAL_OK;
}
int window_main_render_text(tHandleWindowMain* pThis,char* text,int minwidth,GdkPixbuf *pixbuf,eElementID background_element,int *xpos)
{
	int i;
	int x;
	int len;
	int width;
	pthread_mutex_lock(&pThis->mutex);
	
	len=strlen(text);
	width=len*5;
	if (width<minwidth) width=minwidth;
	*xpos=width;

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
			theme_manager_textelement(pThis->pHandleThemeManager,text[i],&elementID,background_element);
		}
		theme_manager_addelement(pThis->pHandleThemeManager,pixbuf,x,0,elementID);
		x+=5;
		i++;
	}
	while (x<minwidth)
	{
		theme_manager_addelement(pThis->pHandleThemeManager,pixbuf,x,0,background_element);
		x+=5;
	}
	pthread_mutex_unlock(&pThis->mutex);
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
		if (strncmp(pThis->songInfo.songinfo,songInfo.songinfo,256))
		{
			window_main_render_text(pThis,songInfo.songinfo,154,pThis->pixbufSongInfo,TEXT_TITLE_DISPLAY_SPACE,&xpos);
			pThis->scrolllen=xpos;
			change=1;
		}
	} else {		// if not, display the filename
		if (strncmp(pThis->songInfo.filename,songInfo.filename,1024))
		{
			window_main_render_text(pThis,songInfo.filename,154,pThis->pixbufSongInfo,TEXT_TITLE_DISPLAY_SPACE,&xpos);
			pThis->scrolllen=xpos;
			change=1;
		}
	}
	if (pThis->songInfo.bitrate!=songInfo.bitrate)
	{
		snprintf(tmp,16,"%3d",songInfo.bitrate);
		window_main_render_text(pThis,tmp,15,pThis->pixbufBitrate,TEXT_KBPS_DISPLAY_SPACE,&xpos);
		change=1;
	}
	if (pThis->songInfo.samplerate!=songInfo.samplerate)
	{
		int kbps;
		kbps=songInfo.samplerate/1000;
		if (kbps<0) kbps=0;
		if (kbps>99) kbps=99;
		snprintf(tmp,16,"%2d",kbps);
		window_main_render_text(pThis,tmp,10,pThis->pixbufSamplerate,TEXT_KBPS_DISPLAY_SPACE,&xpos);
		change=1;
	}


// time is being displayed here
	if (pThis->songInfo.pos!=songInfo.pos || pThis->songInfo.len!=songInfo.len)
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
	if (pThis->songInfo.channels!=songInfo.channels)
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
		pThis->songInfo=songInfo;
//		window_main_redraw(pThis);
	}
	return RETVAL_OK;
	
}



// 
eMainWindowPressed window_main_find_pressable(int x, int y,int width,int height)
{
	int i;
	double scaleFactorX;
	double scaleFactorY;
	
	eMainWindowPressed      pressed;
	typedef struct _tPressable
	{
		eMainWindowPressed      pressed;
		int posx;
		int posy;
		int dimx;
		int dimy;
	} tPressable;

	const tPressable cWindowMain_pressable[PRESSABLE_NUM]=
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
	scaleFactorX=width/WINDOW_MAIN_WIDTH;
	scaleFactorY=height/WINDOW_MAIN_HEIGHT;
	pressed=PRESSED_NONE;
	for (i=0;i<PRESSABLE_NUM;i++)
	{
		int x1,x2;
		int y1,y2;

		x1=scaleFactorX*cWindowMain_pressable[i].posx;
		x2=x1+cWindowMain_pressable[i].dimx*scaleFactorX;

		y1=scaleFactorY*cWindowMain_pressable[i].posy;
		y2=y1+cWindowMain_pressable[i].dimy*scaleFactorY;

		if (x>=x1 && x<x2 && y>=y1 && y<y2)
		{
			pressed=cWindowMain_pressable[i].pressed;
		}
	}

	return pressed;
}
// threads
void* window_main_thread(void* user_data)
{
	tHandleWindowMain* pThis=(tHandleWindowMain*)user_data;
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
		if (memcmp(&songInfo,&pThis->songInfo,sizeof(tSongInfo)))
		{
			window_main_refresh_songinfo(pThis,songInfo);
		}
		{
			signed short pcm[512];
			audiooutput_getLastSamples(pThis->pHandleAudioOutput,pcm,512);
			visualizer_newPcm(&(pThis->handleVisualizer),pcm,512);
				
		}
		window_main_refresh(pThis);

		{	
			GdkFrameClock *gdkFrameClock=gtk_widget_get_frame_clock(pThis->windowMain);
			gdk_frame_clock_end_updating(gdkFrameClock);
			usleep(40000);	// 40 ms --> 25 fps
			gdk_frame_clock_begin_updating(gdkFrameClock);
		}
//		usleep(10000);	// 10 ms --> 100 fps
	}
}

int window_main_calculate_value_from_x(int x,int xleft,int xright,int margin, int width,  int valueleft,int valueright,int *value)
{
	int delta_value;
	int delta_x;
	int retval;
	double scaleFactor;


	xleft+=margin;
	xright-=margin;

	scaleFactor=width/WINDOW_MAIN_WIDTH;

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

static void window_main_filechooser_response(GtkNativeDialog *native,int response)
{
	printf("response\n");
	if (response==GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser *fileChooser=GTK_FILE_CHOOSER(native);
		GFile *chosen=gtk_file_chooser_get_file(fileChooser);
		tHandleWindowMain* pThis=(tHandleWindowMain*)g_object_get_data(G_OBJECT(native),"userdata");
		decoder_openfile(pThis->pHandleDecoder,g_file_get_parse_name(chosen));	
	}
	g_object_unref(native);
}
static void window_main_message_response(GtkWidget *widget,int response)
{
	if (widget!=NULL)
	{
		g_object_unref(widget);
	}
}

// interactive callbacks
int window_main_interaction(tHandleWindowMain* pThis,eMainWindowPressed pressed,int x,int y)
{
	int retval;
	int value;
	int width;

	width=gtk_widget_get_width(pThis->windowMain);

	retval=RETVAL_OK;
	switch(pressed)
	{
		case PRESSED_CLUTTERBAR_I:
		case PRESSED_INFO:
			{
				GtkWidget *message;
				message=gtk_message_dialog_new(GTK_WINDOW(pThis->windowMain),0,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"d11amp %d.%d%d\n\n%s\n",VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION,d11amp_license_text);
				g_signal_connect(message,"response",G_CALLBACK(window_main_message_response),NULL);
				g_signal_connect(message,"destroy",G_CALLBACK(window_main_message_response),NULL);
				gtk_widget_show(message);
			}
			break;
		case PRESSED_CLUTTERBAR_D:
			pThis->scaleFactor*=2;
			if (pThis->scaleFactor==32) pThis->scaleFactor=1;

			gtk_window_set_default_size(GTK_WINDOW(pThis->windowMain),WINDOW_MAIN_WIDTH*pThis->scaleFactor,WINDOW_MAIN_HEIGHT*pThis->scaleFactor);
			break;
		case PRESSED_CLUTTERBAR_V:
			visualizer_cycle(&(pThis->handleVisualizer));
			break;
		case PRESSED_OPEN:
			{
				GtkFileChooserNative *fileChooser;
				fileChooser=gtk_file_chooser_native_new("Open File",
					GTK_WINDOW(pThis->windowMain),
					GTK_FILE_CHOOSER_ACTION_OPEN,
					"_Open",
					"_Cancel");
				
				g_object_set_data(G_OBJECT(fileChooser),"userdata",pThis);
				g_signal_connect(fileChooser,"response",G_CALLBACK(window_main_filechooser_response),NULL);
				gtk_native_dialog_show(GTK_NATIVE_DIALOG(fileChooser));
			}
			break;
		case PRESSED_SONGPOS:
			{
				if (!window_main_calculate_value_from_x(x,16,16+248,30/2,width,  0,pThis->songInfo.len, &value))
				{
					decoder_set_songPos(pThis->pHandleDecoder,value);
				}
			}
			break;
		case PRESSED_VOLUME_SLIDER:
			{
				if (!window_main_calculate_value_from_x(x,107,107+68,14/2,width,0,100,&value))
				{
					int value2;
					audiooutput_setVolume(pThis->pHandleAudioOutput,value);
					value2=27*value/100;
					pThis->statusVolume=value2;
				}
			}
			break;
		case PRESSED_BALANCE_SLIDER:
			{
				if (!window_main_calculate_value_from_x(x,177,177+38,14/2,width,-100,100,&value))
				{
					int value2;
					audiooutput_setBalance(pThis->pHandleAudioOutput,value);
					value2=(12*value)/100;
					pThis->statusBalance=value2;
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
		case PRESSED_EQUALIZER:
			if (pThis->statusEqualizer==HIDDEN)
			{
				pThis->statusEqualizer=SHOWN;
				window_equalizer_show(pThis->pHandleWindowEqualizer);
			} else {
				pThis->statusEqualizer=HIDDEN;
				window_equalizer_hide(pThis->pHandleWindowEqualizer);
			}
			break;
		case PRESSED_PLAYLIST:
			if (pThis->statusPlaylist==HIDDEN)
			{
				pThis->statusPlaylist=SHOWN;
				window_playlist_show(pThis->pHandleWindowPlaylist);
			} else {
				pThis->statusPlaylist=HIDDEN;
				window_playlist_hide(pThis->pHandleWindowPlaylist);
			}
			break;
		default:
			printf("TODO: handle press on %d\n",pressed);
			retval=RETVAL_NOK;
			break;
	
	}
	return retval;
}

int window_main_event_pressed(GtkWidget *widget, double x,double y,guint event_button, gpointer data)
{
	eMainWindowPressed pressed;
	tHandleWindowMain* pThis=(tHandleWindowMain*)data;
	int width,height;
        width=gtk_widget_get_width(pThis->windowMain);
        height=gtk_widget_get_height(pThis->windowMain);
	pressed=window_main_find_pressable((int)x,(int)y,width,height);

	pThis->lastPressed=pressed;
	window_main_refresh(pThis);
	return TRUE;
}
int window_main_event_released(GtkWidget *widget, double x,double y,guint event_button, gpointer data)
{
	eMainWindowPressed pressed;
	tHandleWindowMain* pThis=(tHandleWindowMain*)data;
	int width,height;
        width=gtk_widget_get_width(pThis->windowMain);
        height=gtk_widget_get_height(pThis->windowMain);
	pressed=window_main_find_pressable((int)x,(int)y,width,height);

	if (pressed==pThis->lastPressed && pressed!=PRESSED_NONE)
	{
		window_main_interaction(pThis,pressed,x,y);
	}

	pThis->lastPressed=PRESSED_NONE;
	window_main_refresh(pThis);
	return TRUE;
}
void window_main_event_signal(GtkWidget *widget,GtkAllocation *allocation, gpointer user_data)
{
	printf("ALLOCATE\n");
}

// initial setup
int window_main_init(GtkApplication* app,tHandleWindowMain* pThis,tHandleThemeManager* pHandleThemeManager,tHandleAudioOutput* pHandleAudioOutput,tHandleDecoder *pHandleDecoder,tHandleWindowEqualizer *pHandleWindowEqualizer, tHandleWindowPlaylist *pHandleWindowPlaylist)
{
	memset(pThis,0,sizeof(tHandleWindowMain));
	pthread_mutex_init(&pThis->mutex,NULL);
	visualizer_init(&(pThis->handleVisualizer),pHandleThemeManager);
	
	pThis->pHandleAudioOutput=pHandleAudioOutput;
	pThis->pHandleDecoder=pHandleDecoder;
	pThis->pHandleThemeManager=pHandleThemeManager;

	pThis->pHandleWindowEqualizer=pHandleWindowEqualizer;
	pThis->pHandleWindowPlaylist=pHandleWindowPlaylist;

	pThis->scaleFactor=4;
	pThis->pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);
	pThis->pixbufSongInfo=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,5*1024,6);
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
	gtk_window_set_default_size(GTK_WINDOW(pThis->windowMain),WINDOW_MAIN_WIDTH*pThis->scaleFactor,WINDOW_MAIN_HEIGHT*pThis->scaleFactor);
	gtk_window_set_child(GTK_WINDOW(pThis->windowMain),pThis->picture);
	gtk_window_set_title(GTK_WINDOW(pThis->windowMain),"d11amp main");

	pThis->gesture=gtk_gesture_click_new();
	g_signal_connect(pThis->gesture,"pressed",G_CALLBACK(window_main_event_pressed),&(pThis->windowMain));
	g_signal_connect(pThis->gesture,"released",G_CALLBACK(window_main_event_released),&(pThis->windowMain));
	gtk_widget_add_controller(pThis->windowMain,GTK_EVENT_CONTROLLER(pThis->gesture));




	snprintf(pThis->songInfo.songinfo,256,"Hello World");
	pThis->songInfo.bitrate=0;
	pThis->songInfo.samplerate=0;

	pthread_create(&pThis->thread,NULL,window_main_thread,(void*)pThis);
	return RETVAL_OK;
}

int window_main_show(tHandleWindowMain* pThis)
{
	gtk_widget_show(pThis->windowMain);
	return RETVAL_OK;
}

int window_main_get_shuffle_repeat(tHandleWindowMain* pThis,int *pShuffle,int *pRepeat)
{
	*pShuffle=pThis->statusShuffle;
	*pRepeat=pThis->statusRepeat;
	return RETVAL_OK;
}
