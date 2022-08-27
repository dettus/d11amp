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
	pThis->app=app;
	pThis->pControllerContext=pControllerContext;
	pThis->pHandleThemeManager=pHandleThemeManager;


	pThis->pixbufBackground=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);
	pThis->pixbufSongtitle=NULL;
	pThis->pixbufKhz=NULL;
	pThis->pixbufKbps=NULL;
	pThis->pixbufVisualizer=NULL;


	pThis->status.clutterbar=eONOFF_ON;
	pThis->status.equalizer=eONOFF_OFF;
	pThis->status.playlist=eONOFF_OFF;
	pThis->status.shuffle=eONOFF_OFF;
	pThis->status.repeat=eONOFF_OFF;

	pThis->status.volume=80;
	pThis->status.balance=0;



	pThis->pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);
	pThis->picture=gtk_picture_new_for_pixbuf(pThis->pixbuf);
	pThis->window=gtk_application_window_new(pThis->app);
	gtk_window_set_default_size(GTK_WINDOW(pThis->window),WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);

	gtk_window_set_child(GTK_WINDOW(pThis->window),pThis->picture);
	gtk_window_set_title(GTK_WINDOW(pThis->window),"d11amp main");
	

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
		theme_manager_draw_text(pThis->pHandleThemeManager,&(pThis->pixbufSongtitle),TEXT_TITLE_DISPLAY_SPACE,text,ELEMENT_WIDTH(MAIN_SONG_INFO_DISPLAY));
	}

	if (songInfo.samplerate!=pThis->songInfo.samplerate)
	{
		char text[16];
		snprintf(text,16,"%2d",songInfo.samplerate);
		theme_manager_draw_text(pThis->pHandleThemeManager,&(pThis->pixbufKhz),TEXT_TIME_DISPLAY_BACKGROUND,text,ELEMENT_WIDTH(MAIN_KHZ_DISPLAY));
		pThis->songInfo.samplerate=songInfo.samplerate;
	}
	if (songInfo.bitrate!=pThis->songInfo.bitrate)
	{
		char text[16];
		snprintf(text,16,"%3d",songInfo.bitrate);
		theme_manager_draw_text(pThis->pHandleThemeManager,&(pThis->pixbufKbps),TEXT_KBPS_DISPLAY_SPACE,text,ELEMENT_WIDTH(MAIN_KBPS_DISPLAY));
		pThis->songInfo.bitrate=songInfo.bitrate;
	}
	return retval;
}

// background: the default picture
int window_main_refresh_background(tHandleWindowMain* pThis)
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
// status: what changes due to user interaction
int window_main_draw_status(tHandleWindowMain* pThis,GdkPixbuf *destBuf)
{
	int retval;

	retval=RETVAL_OK;
	{
		int idx;
#define	VOLUME_ELEMENTS_NUM   28
#define BALANCE_ELEMENTS_NUM	28
		eElementID volumeIDs[VOLUME_ELEMENTS_NUM]={
			VOLUME_000_001, VOLUME_003_005, VOLUME_007_009, VOLUME_011_013, VOLUME_015_017,
			VOLUME_019_021, VOLUME_023_025, VOLUME_027_029, VOLUME_031, VOLUME_033_035,
			VOLUME_037_039, VOLUME_041_043, VOLUME_045_047, VOLUME_049_050, VOLUME_052_054,
			VOLUME_056_058, VOLUME_060_062, VOLUME_064, VOLUME_066_068, VOLUME_070_072,
			VOLUME_074_076, VOLUME_078_080, VOLUME_082_084, VOLUME_086_088, VOLUME_090_092,
			VOLUME_094_096, VOLUME_098, VOLUME_100,
		};
		eElementID balanceIDs[BALANCE_ELEMENTS_NUM]={
			BALANCE_CENTERED, BALANCE_FILLER0, BALANCE_FILLER1, BALANCE_FILLER2, BALANCE_FILLER3, 
			BALANCE_FILLER4, BALANCE_24LEFTORRIGHT, BALANCE_FILLER5, BALANCE_33LEFTORRIGHT, BALANCE_FILLER6, 
			BALANCE_FILLER7, BALANCE_40LEFTORRIGHT, BALANCE_FILLER8, BALANCE_49LEFTORRIGHT, BALANCE_FILLER9, 
			BALANCE_58LEFTORRIGHT, BALANCE_FILLER10, BALANCE_66LEFTORRIGHT, BALANCE_FILLER11, BALANCE_FILLER12, 
			BALANCE_74LEFTORRIGHT, BALANCE_FILLER13, BALANCE_82LEFTORRIGHT, BALANCE_FILLER14, BALANCE_91LEFTORRIGHT, 
			BALANCE_FILLER15, BALANCE_FILLER16, BALANCE_100LEFTORRIGHT
		};
		// decide on the background element for the Volume bar

		idx=(pThis->status.volume*(VOLUME_ELEMENTS_NUM-1))/100;
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,volumeIDs[idx]);

		// decide on the background element for the balance bar
		idx=pThis->status.balance;
		if (idx<0)
		{
			idx=-idx;
		}
		idx=(idx*(BALANCE_ELEMENTS_NUM-1))/100;
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,balanceIDs[idx]);

		

		// draw the sliders
		pThis->volumex=ELEMENT_DESTX(VOLUME_000_001);		// the volume slider 0 position is on the left
		pThis->volumex+=(pThis->status.volume*(ELEMENT_WIDTH(VOLUME_100)-ELEMENT_WIDTH(VOLUME_SLIDER_UNPRESSED)-1))/100;	// 100 is on the right

		pThis->balancex=ELEMENT_DESTX(BALANCE_CENTERED)+ELEMENT_WIDTH(BALANCE_CENTERED)/2;	// the balance slider 0 is in the center
		pThis->balancex+=(pThis->status.balance*(ELEMENT_WIDTH(BALANCE_100LEFTORRIGHT)-ELEMENT_WIDTH(BALANCE_SLIDER_UNPRESSED)-1))/200;	// -100 is left, 100 is right

		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,VOLUME_SLIDER_UNPRESSED,pThis->volumex,ELEMENT_DESTY(VOLUME_SLIDER_UNPRESSED));
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,BALANCE_SLIDER_UNPRESSED,pThis->balancex,ELEMENT_DESTY(BALANCE_SLIDER_UNPRESSED));
		
	}

	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.clutterbar==eONOFF_ON)?TITLEBAR_CLUTTERBAR_SHOWN:TITLEBAR_CLUTTERBAR_HIDDEN);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.equalizer==eONOFF_ON)?SHUFREP_EQUALIZER_UNPRESSED:SHUFREP_NO_EQUALIZER_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.playlist==eONOFF_ON)?SHUFREP_PLAYLIST_UNPRESSED:SHUFREP_NO_PLAYLIST_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.shuffle==eONOFF_ON)?SHUFREP_SHUFFLE_UNPRESSED:SHUFREP_NO_SHUFFLE_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.shuffle==eONOFF_ON)?SHUFREP_REPEAT_UNPRESSED:SHUFREP_NO_REPEAT_UNPRESSED);
	switch (pThis->status.indicator)
	{
		case eINDICATOR_PLAY:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLAYPAUSE_PLAY_INDICATOR);
			break;
		case eINDICATOR_PAUSE:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLAYPAUSE_PAUSE_INDICATOR);
			break;
		case eINDICATOR_STOP:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLAYPAUSE_STOP_INDICATOR);
			break;
		case eINDICATOR_START_OF_SONG:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLAYPAUSE_START_OF_SONG_INDICATOR);
			break;
		case eINDICATOR_END_OF_SONG:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLAYPAUSE_END_OF_SONG_INDICATOR);
			break;
		default:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLAYPAUSE_FILLER);
			break;
	}

	return retval;
}
// dynamic: what changes over time
int window_main_draw_dynamic(tHandleWindowMain* pThis,GdkPixbuf *destBuf)
{
	int retval;
	int width;

	retval=RETVAL_OK;
	{
		int minutes;
		int seconds;
		int i;
		eElementID numbers[10]={NUMBERS_0, NUMBERS_1, NUMBERS_2, NUMBERS_3, NUMBERS_4,
			NUMBERS_5, NUMBERS_6, NUMBERS_7, NUMBERS_8, NUMBERS_9};
		eElementID digits[4];
		int posx[4]={48,60, 78,90};	// where to draw the digits

		minutes=pThis->songInfo.pos/60;
		seconds=pThis->songInfo.pos%60;
		if (minutes>99)
		{
			minutes=99;
		}

		digits[0]=numbers[minutes/10];
		if (digits[0]==NUMBERS_0)
		{
			digits[0]=NUMBERS_BLANK;	// do not draw a leading zero
		}
		digits[1]=numbers[minutes%10];
		digits[2]=numbers[seconds/10];
		digits[3]=numbers[seconds%10];

		for (i=0;i<4;i++)
		{
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,digits[i],posx[i],ELEMENT_DESTY(digits[i]));
		}
	}

	if (pThis->pixbufSongtitle!=NULL)
	{
		int pos;
		width=gdk_pixbuf_get_width(pThis->pixbufSongtitle);
		pos=pThis->songinfo_scrollpos;

		if (pos<0) 
		{
			pos=0;
		}
		if (pos>=width-ELEMENT_WIDTH(MAIN_SONG_INFO_DISPLAY))
		{
			pos=width-ELEMENT_WIDTH(MAIN_SONG_INFO_DISPLAY)-1;
		}

		gdk_pixbuf_copy_area(pThis->pixbufSongtitle,pos,0,ELEMENT_WIDTH(MAIN_SONG_INFO_DISPLAY),ELEMENT_HEIGHT(MAIN_SONG_INFO_DISPLAY),destBuf,ELEMENT_DESTX(MAIN_SONG_INFO_DISPLAY),ELEMENT_DESTY(MAIN_SONG_INFO_DISPLAY));

		pThis->songinfo_scrollpos++;
		if (pThis->songinfo_scrollpos>=(width-ELEMENT_WIDTH(MAIN_SONG_INFO_DISPLAY)/2))
		{
			pThis->songinfo_scrollpos=-ELEMENT_WIDTH(MAIN_SONG_INFO_DISPLAY)/2;
		}
	}

	if (pThis->pixbufKhz!=NULL)
	{
		gdk_pixbuf_copy_area(pThis->pixbufKhz,0,0,ELEMENT_WIDTH(MAIN_KHZ_DISPLAY),ELEMENT_HEIGHT(MAIN_KHZ_DISPLAY),destBuf,ELEMENT_DESTX(MAIN_KHZ_DISPLAY),ELEMENT_DESTY(MAIN_KHZ_DISPLAY));	
	}	
	if (pThis->pixbufKbps!=NULL)
	{
		gdk_pixbuf_copy_area(pThis->pixbufKbps,0,0,ELEMENT_WIDTH(MAIN_KBPS_DISPLAY),ELEMENT_HEIGHT(MAIN_KBPS_DISPLAY),destBuf,ELEMENT_DESTX(MAIN_KBPS_DISPLAY),ELEMENT_DESTY(MAIN_KBPS_DISPLAY));	
	}
	if (pThis->pixbufVisualizer!=NULL)
	{
		gdk_pixbuf_copy_area(pThis->pixbufVisualizer,0,0,ELEMENT_WIDTH(MAIN_VISUALIZATION_WINDOW),ELEMENT_HEIGHT(MAIN_VISUALIZATION_WINDOW),destBuf,ELEMENT_DESTX(MAIN_VISUALIZATION_WINDOW),ELEMENT_DESTY(MAIN_VISUALIZATION_WINDOW));	
	}
	

	pThis->songposx=ELEMENT_DESTX(POSBAR_SONG_PROGRESS_BAR);	
	if (pThis->songInfo.len)
	{
		pThis->songposx+=(pThis->songInfo.pos*(ELEMENT_WIDTH(POSBAR_SONG_PROGRESS_BAR)-ELEMENT_WIDTH(POSBAR_SONG_SLIDER_UNPRESSED)-1))/pThis->songInfo.len;
	} else {
		pThis->songposx+=0;
	}
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,POSBAR_SONG_SLIDER_PRESSED,pThis->songposx,ELEMENT_DESTY(POSBAR_SONG_SLIDER_PRESSED));				
			
	return retval;
}
// presses: happens when the user has pressed a button
int window_main_draw_presses(tHandleWindowMain* pThis,GdkPixbuf *destBuf)
{
	int retval;
	retval=RETVAL_OK;
	switch (pThis->lastPressed)
	{
		case ePRESSED_WINDOW_MAIN_CLUTTERBAR_O:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,TITLEBAR_CLUTTERBAR_O_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_CLUTTERBAR_A:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,TITLEBAR_CLUTTERBAR_A_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_CLUTTERBAR_I:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,TITLEBAR_CLUTTERBAR_I_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_CLUTTERBAR_D:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,TITLEBAR_CLUTTERBAR_D_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_CLUTTERBAR_V:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,TITLEBAR_CLUTTERBAR_V_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_EQUALIZER:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.equalizer==eONOFF_ON)?SHUFREP_EQUALIZER_PRESSED:SHUFREP_NO_EQUALIZER_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_PLAYLIST:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.playlist==eONOFF_ON)?SHUFREP_PLAYLIST_PRESSED:SHUFREP_NO_PLAYLIST_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_PREV:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,CBUTTONS_PREV_BUTTON_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_PLAY:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,CBUTTONS_PLAY_BUTTON_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_PAUSE:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,CBUTTONS_PAUSE_BUTTON_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_STOP:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,CBUTTONS_STOP_BUTTON_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_NEXT:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,CBUTTONS_NEXT_BUTTON_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_OPEN:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,CBUTTONS_OPEN_BUTTON_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_SHUFFLE:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.shuffle==eONOFF_ON)?SHUFREP_SHUFFLE_PRESSED:SHUFREP_NO_SHUFFLE_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_REPEAT:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.shuffle==eONOFF_ON)?SHUFREP_REPEAT_PRESSED:SHUFREP_NO_REPEAT_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_VOLUME:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,VOLUME_SLIDER_UNPRESSED,pThis->volumex,ELEMENT_DESTY(VOLUME_SLIDER_UNPRESSED));
			break;
		case ePRESSED_WINDOW_MAIN_BALANCE:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,BALANCE_SLIDER_UNPRESSED,pThis->balancex,ELEMENT_DESTY(BALANCE_SLIDER_UNPRESSED));
			break;
		case ePRESSED_WINDOW_MAIN_SONGPOS:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,POSBAR_SONG_SLIDER_PRESSED,pThis->songposx,ELEMENT_DESTY(POSBAR_SONG_SLIDER_PRESSED));				
			break;
		default:
			break;
	}
	return retval;
}

int window_main_draw(tHandleWindowMain *pThis,GdkPixbuf *destBuf)
{
	int retval;

	retval=RETVAL_OK;
	// TODO: poll songinfo
	// TODO: poll visualizer
	{
		GError *err=NULL;
		gdk_pixbuf_save(destBuf,"debug0.png","png",&err,NULL);
	}
	gdk_pixbuf_copy_area(pThis->pixbufBackground,0,0,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,destBuf,0,0);
	{
		GError *err=NULL;
		gdk_pixbuf_save(destBuf,"debug1.png","png",&err,NULL);
	}
	window_main_draw_status(pThis,destBuf);
	{
		GError *err=NULL;
		gdk_pixbuf_save(destBuf,"debug2.png","png",&err,NULL);
	}
	window_main_draw_dynamic(pThis,destBuf);
	{
		GError *err=NULL;
		gdk_pixbuf_save(destBuf,"debug3.png","png",&err,NULL);
	}
	window_main_draw_presses(pThis,destBuf);
	{
		GError *err=NULL;
		gdk_pixbuf_save(destBuf,"debug4.png","png",&err,NULL);
	}

	return retval;
}
int window_main_refresh(tHandleWindowMain *pThis)
{
	int retval;
	retval=RETVAL_OK;
	retval|=window_main_draw(pThis,pThis->pixbuf);
        gtk_picture_set_pixbuf(GTK_PICTURE(pThis->picture),pThis->pixbuf);
	gtk_widget_queue_draw(pThis->window);

	return retval;
}
int window_main_signal_new_theme(tHandleWindowMain *pThis)
{
	int retval;

	retval=RETVAL_OK;
	retval|=window_main_refresh_background(pThis);
	retval|=window_main_refresh(pThis);
	

	return retval;
}
int window_main_show(tHandleWindowMain *pThis)
{
        gtk_widget_show(pThis->window);
	return RETVAL_OK;
}
int window_main_hide(tHandleWindowMain *pThis)
{
        gtk_widget_hide(pThis->window);
	return RETVAL_OK;
}