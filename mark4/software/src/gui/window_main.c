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
#define	WINDOW_MAIN_WIDTH	275
#define	WINDOW_MAIN_HEIGHT	116


int window_main_init(tHandleWindowMain* pThis,void* pControllerContext,tHandleThemeManager *pHandleThemeManager,GtkApplication* app)
{
	memset(pThis,0,sizeof(tHandleWindowMain));
	pThis->pControllerContext=pControllerContext;
	pThis->pHandleThemeManager=pHandleThemeManager;

	pThis->pixbufBackground=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);
	pThis->pixbufSongtitle=NULL;
	pThis->pixbufKhz=NULL;
	pThis->pixbufKbps=NULL;

	return RETVAL_OK;

}

int window_main_update_songinfo(tHandleWindowMain* pThis,tSongInfo songInfo)
{

	int retval;

	retval=RETVAL_OK;
	if (strncmp(songInfo.filename,pThis->songInfo.filename,1024) || strncmp(songInfo.songinfo,pThis->songInfo.songinfo,256))
	{
		char *text;
		memcpy(pThis->songInfo.filename,songInfo.filename,1024);
		memcpy(pThis->songInfo.songinfo,songInfo.songinfo,256);
		text=pThis->songInfo.songinfo;
		if (text[0]==0)
		{
			text=pThis->songInfo.filename;
		}
		theme_manager_draw_text(pThis->pHandleThemeManager,&(pThis->pixbufSongtitle),TEXT_TITLE_DISPLAY_SPACE,text);
	}

	if (songInfo.samplerate!=pThis->songInfo.samplerate)
	{
		char text[16];
		snprintf(text,16,"%2d",songInfo.samplerate);
		theme_manager_draw_text(pThis->pHandleThemeManager,&(pThis->pixbufKhz),TEXT_TIME_DISPLAY_BACKGROUND,text);
		pThis->songInfo.samplerate=songInfo.samplerate;
	}
	if (songInfo.bitrate!=pThis->songInfo.bitrate)
	{
		char text[16];
		snprintf(text,16,"%3d",songInfo.bitrate);
		theme_manager_draw_text(pThis->pHandleThemeManager,&(pThis->pixbufKbps),TEXT_KBPS_DISPLAY_SPACE,text);
		pThis->songInfo.bitrate=songInfo.bitrate;
	}
	return retval;
}

int window_main_draw_background(tHandleWindowMain* pThis)
{
	int retval;

	retval=RETVAL_OK;
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,TITLEBAR_NORMAL_TITLEBAR_ACTIVE);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,MAIN_MAIN_DISPLAY);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,TITLEBAR_CLUTTERBAR_SHOWN);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,MONOSTER_MONO_INACTIVE);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,MONOSTER_STEREO_INACTIVE);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,MAIN_KBPS_DISPLAY);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,MAIN_KHZ_DISPLAY);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,VOLUME_100);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,BALANCE_CENTERED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,SHUFREP_NO_PLAYLIST_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,SHUFREP_NO_EQUALIZER_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,POSBAR_SONG_PROGRESS_BAR);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,CBUTTONS_PREV_BUTTON_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,CBUTTONS_PLAY_BUTTON_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,CBUTTONS_PAUSE_BUTTON_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,CBUTTONS_STOP_BUTTON_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,CBUTTONS_NEXT_BUTTON_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,CBUTTONS_OPEN_BUTTON_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,SHUFREP_NO_SHUFFLE_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,SHUFREP_NO_REPEAT_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,MAIN_INFO);

	return retval;
}


