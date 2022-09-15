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
#include "controller.h"
#include "gui_helpers.h"
#include "visualizer.h"
#include "window_main.h"
#include "window_license.h"
#define	WINDOW_MAIN_WIDTH	275
#define	WINDOW_MAIN_HEIGHT	116


// function headers for the gtk events. the implementation is at the end of this file
static void window_main_event_pressed(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window);
static void window_main_event_released(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window);
static void window_main_event_drag_begin(GtkGestureDrag *gesture, double x, double y, GtkWidget *window);
static void window_main_event_drag_update(GtkGestureDrag *gesture, double x, double y, GtkWidget *window);
static void window_main_event_drag_end(GtkGestureDrag *gesture, double x, double y, GtkWidget *window);

static gboolean window_main_heartbeat(gpointer user_data);
static gboolean window_main_close(GtkWidget *widget,gpointer user_data);


static void window_main_filechooser_response(GtkNativeDialog *native,int response);


int window_main_init(tHandleWindowMain* pThis,void* pControllerContext,tHandleThemeManager *pHandleThemeManager,GtkApplication* app)
{
	int retval;
	memset(pThis,0,sizeof(tHandleWindowMain));

	retval=RETVAL_OK;
	pThis->app=app;
	pThis->pControllerContext=pControllerContext;
	pThis->pHandleThemeManager=pHandleThemeManager;


	pThis->pixbufBackground=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);
	pThis->pixbufSongtitle=NULL;
	pThis->pixbufKhz=NULL;
	pThis->pixbufKbps=NULL;


	pThis->status.clutterbar=eONOFF_ON;
	pThis->status.equalizer=eONOFF_OFF;
	pThis->status.playlist=eONOFF_OFF;
	pThis->status.shuffle=eONOFF_OFF;
	pThis->status.repeat=eONOFF_OFF;
	pThis->status.countdown=eONOFF_OFF;

	pThis->status.volume= 50;
	pThis->status.balance=-100;



	pThis->pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);
	pThis->picture=gtk_picture_new_for_pixbuf(pThis->pixbuf);
	pThis->window=gtk_application_window_new(pThis->app);
	gtk_window_set_default_size(GTK_WINDOW(pThis->window),WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);

	gtk_window_set_child(GTK_WINDOW(pThis->window),pThis->picture);
	gtk_window_set_title(GTK_WINDOW(pThis->window),"d11amp main");

	pThis->lastPressed=ePRESSED_NONE;
	pThis->gesture_click=gtk_gesture_click_new();
	g_object_set_data(G_OBJECT(pThis->gesture_click),"pThis",pThis);	// add a pointer to the handle to the widget. this way it is available in the gesture callbacks
	g_signal_connect(pThis->gesture_click,"pressed", G_CALLBACK(window_main_event_pressed) ,(pThis->window));
	g_signal_connect(pThis->gesture_click,"released",G_CALLBACK(window_main_event_released),(pThis->window));
	gtk_widget_add_controller(pThis->window,GTK_EVENT_CONTROLLER(pThis->gesture_click));

	pThis->gesture_drag=gtk_gesture_drag_new();
	g_object_set_data(G_OBJECT(pThis->gesture_drag),"pThis",pThis);	// add a pointer to the handle to the widget. this way it is available in the gesture callbacks
	g_signal_connect (pThis->gesture_drag,"drag-begin", G_CALLBACK (window_main_event_drag_begin), (pThis->window));
	g_signal_connect (pThis->gesture_drag,"drag-update",G_CALLBACK (window_main_event_drag_update),(pThis->window));
	g_signal_connect (pThis->gesture_drag,"drag-end",   G_CALLBACK (window_main_event_drag_end),   (pThis->window));



	gtk_widget_add_controller(pThis->window,GTK_EVENT_CONTROLLER(pThis->gesture_drag));


// define the pressable elements
	gui_helpers_define_pressable_by_dimensions(&pThis->boundingBoxes[ 0],ePRESSED_WINDOW_MAIN_CLUTTERBAR_O,ELEMENT_DESTX(TITLEBAR_CLUTTERBAR_SHOWN),ELEMENT_DESTY(TITLEBAR_CLUTTERBAR_SHOWN)+ 3,ELEMENT_WIDTH(TITLEBAR_CLUTTERBAR_SHOWN),6);	
	gui_helpers_define_pressable_by_dimensions(&pThis->boundingBoxes[ 1],ePRESSED_WINDOW_MAIN_CLUTTERBAR_A,ELEMENT_DESTX(TITLEBAR_CLUTTERBAR_SHOWN),ELEMENT_DESTY(TITLEBAR_CLUTTERBAR_SHOWN)+11,ELEMENT_WIDTH(TITLEBAR_CLUTTERBAR_SHOWN),6);	
	gui_helpers_define_pressable_by_dimensions(&pThis->boundingBoxes[ 2],ePRESSED_WINDOW_MAIN_CLUTTERBAR_I,ELEMENT_DESTX(TITLEBAR_CLUTTERBAR_SHOWN),ELEMENT_DESTY(TITLEBAR_CLUTTERBAR_SHOWN)+19,ELEMENT_WIDTH(TITLEBAR_CLUTTERBAR_SHOWN),6);	
	gui_helpers_define_pressable_by_dimensions(&pThis->boundingBoxes[ 3],ePRESSED_WINDOW_MAIN_CLUTTERBAR_D,ELEMENT_DESTX(TITLEBAR_CLUTTERBAR_SHOWN),ELEMENT_DESTY(TITLEBAR_CLUTTERBAR_SHOWN)+27,ELEMENT_WIDTH(TITLEBAR_CLUTTERBAR_SHOWN),6);	
	gui_helpers_define_pressable_by_dimensions(&pThis->boundingBoxes[ 4],ePRESSED_WINDOW_MAIN_CLUTTERBAR_V,ELEMENT_DESTX(TITLEBAR_CLUTTERBAR_SHOWN),ELEMENT_DESTY(TITLEBAR_CLUTTERBAR_SHOWN)+34,ELEMENT_WIDTH(TITLEBAR_CLUTTERBAR_SHOWN),6);	
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[ 5],ePRESSED_WINDOW_MAIN_CLUTTERBAR_V,MAIN_VISUALIZATION_WINDOW);
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[ 6],ePRESSED_WINDOW_MAIN_EQUALIZER,SHUFREP_EQUALIZER_PRESSED);
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[ 7],ePRESSED_WINDOW_MAIN_PLAYLIST,SHUFREP_PLAYLIST_PRESSED);
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[ 8],ePRESSED_WINDOW_MAIN_PREV,CBUTTONS_PREV_BUTTON_PRESSED);
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[ 9],ePRESSED_WINDOW_MAIN_PLAY,CBUTTONS_PLAY_BUTTON_PRESSED);
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[10],ePRESSED_WINDOW_MAIN_PAUSE,CBUTTONS_PAUSE_BUTTON_PRESSED);
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[11],ePRESSED_WINDOW_MAIN_STOP,CBUTTONS_STOP_BUTTON_PRESSED);
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[12],ePRESSED_WINDOW_MAIN_NEXT,CBUTTONS_NEXT_BUTTON_PRESSED);
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[13],ePRESSED_WINDOW_MAIN_OPEN,CBUTTONS_OPEN_BUTTON_PRESSED);
	
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[14],ePRESSED_WINDOW_MAIN_SHUFFLE,SHUFREP_SHUFFLE_PRESSED);
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[15],ePRESSED_WINDOW_MAIN_REPEAT,SHUFREP_REPEAT_PRESSED);
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[16],ePRESSED_WINDOW_MAIN_VOLUME,VOLUME_000_001);
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[17],ePRESSED_WINDOW_MAIN_BALANCE,BALANCE_CENTERED);
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[18],ePRESSED_WINDOW_MAIN_SONGPOS,POSBAR_SONG_PROGRESS_BAR);
	gui_helpers_define_pressable_by_dimensions(&pThis->boundingBoxes[19],ePRESSED_WINDOW_MAIN_NUMBERS,48,ELEMENT_DESTY(NUMBERS_BLANK),90-48,ELEMENT_HEIGHT(NUMBERS_BLANK));
	gui_helpers_define_pressable_by_element(WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,&pThis->boundingBoxes[20],ePRESSED_WINDOW_MAIN_CLUTTERBAR_I,MAIN_INFO);
	
	visualizer_init(&pThis->handleVisualizer,pThis->pHandleThemeManager);
	pthread_mutex_init(&pThis->mutex,NULL);
//	pthread_create(&pThis->thread,NULL,&window_main_thread,(void*)pThis);
	
	g_timeout_add(19,window_main_heartbeat,pThis);	// every 19 ms call the window update


	retval|=window_license_init(&(pThis->handleWindowLicense),app);
	g_signal_connect(G_OBJECT(pThis->window), "close_request", G_CALLBACK (window_main_close), (void*)pThis);
	
	return retval;

}

int window_main_update_songinfo(tHandleWindowMain* pThis,tSongInfo* pSongInfo)
{

	int retval;

	retval=RETVAL_OK;
	if (strncmp(pSongInfo->filename,pThis->songInfo.filename,1024) || strncmp(pSongInfo->songinfo,pThis->songInfo.songinfo,256))
	{
		char *text;
		memcpy(pThis->songInfo.filename,pSongInfo->filename,1024);
		memcpy(pThis->songInfo.songinfo,pSongInfo->songinfo,256);
		text=pThis->songInfo.songinfo;
		if (text[0]==0)
		{
			text=pThis->songInfo.filename;
		}
		theme_manager_draw_text(pThis->pHandleThemeManager,&(pThis->pixbufSongtitle),TEXT_TITLE_DISPLAY_SPACE,text,ELEMENT_WIDTH(MAIN_SONG_INFO_DISPLAY));
	}

	if (pSongInfo->samplerate!=pThis->songInfo.samplerate)
	{
		char text[16];
		snprintf(text,16,"%2d",pSongInfo->samplerate);
		theme_manager_draw_text(pThis->pHandleThemeManager,&(pThis->pixbufKhz),TEXT_TIME_DISPLAY_BACKGROUND,text,ELEMENT_WIDTH(MAIN_KHZ_DISPLAY));
		pThis->songInfo.samplerate=pSongInfo->samplerate;
	}
	if (pSongInfo->bitrate!=pThis->songInfo.bitrate)
	{
		char text[16];
		snprintf(text,16,"%3d",pSongInfo->bitrate);
		theme_manager_draw_text(pThis->pHandleThemeManager,&(pThis->pixbufKbps),TEXT_KBPS_DISPLAY_SPACE,text,ELEMENT_WIDTH(MAIN_KBPS_DISPLAY));
		pThis->songInfo.bitrate=pSongInfo->bitrate;
	}

	if (pThis->lastPressed!=ePRESSED_WINDOW_MAIN_SONGPOS)	// when the song position slider is pressed, the actual song position will be ignored. instead the one from the drag update is being displayed.
	{
		pThis->songInfo.pos=pSongInfo->pos;
	}
	pThis->songInfo.len=pSongInfo->len;
	pThis->songInfo.channels=pSongInfo->channels;
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

		pThis->balancex=ELEMENT_DESTX(BALANCE_CENTERED)+(ELEMENT_WIDTH(BALANCE_CENTERED)-ELEMENT_WIDTH(BALANCE_SLIDER_UNPRESSED))/2;	// the balance slider 0 is in the center
		pThis->balancex+=(pThis->status.balance*(ELEMENT_WIDTH(BALANCE_100LEFTORRIGHT)-ELEMENT_WIDTH(BALANCE_SLIDER_UNPRESSED)-1))/200;	// -100 is left, 100 is right

		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,VOLUME_SLIDER_UNPRESSED,pThis->volumex,ELEMENT_DESTY(VOLUME_SLIDER_UNPRESSED));
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,BALANCE_SLIDER_UNPRESSED,pThis->balancex,ELEMENT_DESTY(BALANCE_SLIDER_UNPRESSED));
		
	}

	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.clutterbar==eONOFF_ON)?TITLEBAR_CLUTTERBAR_SHOWN:TITLEBAR_CLUTTERBAR_HIDDEN);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.equalizer==eONOFF_ON)?SHUFREP_EQUALIZER_UNPRESSED:SHUFREP_NO_EQUALIZER_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.playlist==eONOFF_ON)?SHUFREP_PLAYLIST_UNPRESSED:SHUFREP_NO_PLAYLIST_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.shuffle==eONOFF_ON)?SHUFREP_SHUFFLE_UNPRESSED:SHUFREP_NO_SHUFFLE_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.repeat==eONOFF_ON)?SHUFREP_REPEAT_UNPRESSED:SHUFREP_NO_REPEAT_UNPRESSED);
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

		if (pThis->status.countdown==eONOFF_ON)
		{
			minutes=(pThis->songInfo.len-pThis->songInfo.pos)/60;
			seconds=(pThis->songInfo.len-pThis->songInfo.pos)%60;
		} else {
			minutes=pThis->songInfo.pos/60;
			seconds=pThis->songInfo.pos%60;
		}
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


	pThis->songposx=ELEMENT_DESTX(POSBAR_SONG_PROGRESS_BAR);	
	if (pThis->songInfo.len)
	{
		pThis->songposx+=(pThis->songInfo.pos*(ELEMENT_WIDTH(POSBAR_SONG_PROGRESS_BAR)-ELEMENT_WIDTH(POSBAR_SONG_SLIDER_UNPRESSED)-1))/pThis->songInfo.len;
	} else {
		pThis->songposx+=0;
	}
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,POSBAR_SONG_SLIDER_UNPRESSED,pThis->songposx,ELEMENT_DESTY(POSBAR_SONG_SLIDER_UNPRESSED));

	switch (pThis->songInfo.channels)
	{
		case 1:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,MONOSTER_STEREO_INACTIVE);
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,MONOSTER_MONO_ACTIVE);
			break;
		case 2:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,MONOSTER_STEREO_ACTIVE);
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,MONOSTER_MONO_INACTIVE);
			break;
		default:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,MONOSTER_STEREO_INACTIVE);
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,MONOSTER_MONO_INACTIVE);
			break;
	}
		
	retval|=visualizer_render(&(pThis->handleVisualizer),destBuf,ELEMENT_DESTX(MAIN_VISUALIZATION_WINDOW),ELEMENT_DESTY(MAIN_VISUALIZATION_WINDOW));
	
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
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.repeat==eONOFF_ON)?SHUFREP_REPEAT_PRESSED:SHUFREP_NO_REPEAT_PRESSED);
			break;
		case ePRESSED_WINDOW_MAIN_VOLUME:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,VOLUME_SLIDER_PRESSED,pThis->volumex,ELEMENT_DESTY(VOLUME_SLIDER_PRESSED));
			break;
		case ePRESSED_WINDOW_MAIN_BALANCE:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,BALANCE_SLIDER_PRESSED,pThis->balancex,ELEMENT_DESTY(BALANCE_SLIDER_PRESSED));
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
	gdk_pixbuf_copy_area(pThis->pixbufBackground,0,0,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,destBuf,0,0);
	window_main_draw_status(pThis,destBuf);
	window_main_draw_dynamic(pThis,destBuf);
	window_main_draw_presses(pThis,destBuf);

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
// signal handlers

int window_main_signal_indicator(tHandleWindowMain *pThis,eWindowMainIndicator indicator)
{
	pThis->status.indicator=indicator;
	return RETVAL_OK;
}
int window_main_signal_scalefactor(tHandleWindowMain* pThis,int scale)
{
	int retval;
	retval=RETVAL_OK;
	gtk_window_set_default_size(GTK_WINDOW(pThis->window),scale*WINDOW_MAIN_WIDTH,scale*WINDOW_MAIN_HEIGHT);
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
int window_main_signal_volume(tHandleWindowMain *pThis,int volume)
{
	int retval;
	retval=RETVAL_OK;
	if (volume<0) volume=0;
	if (volume>100) volume=100;
	pThis->status.volume=volume;
	retval|=window_main_refresh(pThis);
	return retval;
}

int window_main_signal_balance(tHandleWindowMain *pThis,int balance)
{
	int retval;
	retval=RETVAL_OK;
	if (balance<-100) balance=-100;
	if (balance>100) balance=100;
	pThis->status.balance=balance;
	retval|=window_main_refresh(pThis);
	return retval;
}


int window_main_signal_equalizer(tHandleWindowMain *pThis,int hide0show1)
{
	pThis->status.equalizer=hide0show1?eONOFF_ON:eONOFF_OFF;
	return RETVAL_OK;
}

int window_main_signal_playlist(tHandleWindowMain *pThis,int hide0show1)
{
	pThis->status.playlist=hide0show1?eONOFF_ON:eONOFF_OFF;
	return RETVAL_OK;
}

int window_main_pull_shuffle_repeat(tHandleWindowMain *pThis,int* pShuffle,int* pRepeat)
{
	*pShuffle=(pThis->status.shuffle==eONOFF_ON);
	*pRepeat=(pThis->status.repeat==eONOFF_ON);
	return RETVAL_OK;
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



////////////////////////////////////////////////////////
// implementation of the user interaction events follow
////////////////////////////////////////////////////////



static void window_main_event_pressed(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window)
{
	ePressable pressed;
	tHandleWindowMain* pThis=(tHandleWindowMain*)g_object_get_data(G_OBJECT(gesture),"pThis");
	pressed=gui_helpers_find_pressable(pThis->boundingBoxes,PRESSABLE_MAIN_NUM,x,y,window,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);
	pThis->lastPressed=pressed;
	pThis->pressedX=x;
	pThis->pressedY=y;


	window_main_refresh(pThis);	
}
static void window_main_event_released(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window)
{
	ePressable released;
	tPayload payload;
	tHandleWindowMain* pThis=(tHandleWindowMain*)g_object_get_data(G_OBJECT(gesture),"pThis");
	released=gui_helpers_find_pressable(pThis->boundingBoxes,PRESSABLE_MAIN_NUM,x,y,window,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);


	if ((released!=ePRESSED_NONE && released==pThis->lastPressed) || (pThis->lastPressed==ePRESSED_WINDOW_MAIN_SONGPOS))
	{
		switch(pThis->lastPressed)
		{
			case ePRESSED_WINDOW_MAIN_PREV:
				controller_event(pThis->pControllerContext,eEVENT_PLAY_PREV_FILE,NULL);
				break;
			case ePRESSED_WINDOW_MAIN_PLAY:
				controller_event(pThis->pControllerContext,eEVENT_PLAY,NULL);
				break;
			case ePRESSED_WINDOW_MAIN_PAUSE:
				controller_event(pThis->pControllerContext,eEVENT_PAUSE,NULL);
				break;
			case ePRESSED_WINDOW_MAIN_STOP:
				controller_event(pThis->pControllerContext,eEVENT_STOP,NULL);
				break;
			case ePRESSED_WINDOW_MAIN_NEXT:
				controller_event(pThis->pControllerContext,eEVENT_PLAY_NEXT_FILE,NULL);
				break;
			case ePRESSED_WINDOW_MAIN_OPEN:
				{
					GtkFileChooserNative *fileChooser;
					fileChooser=gtk_file_chooser_native_new("Open File",
							GTK_WINDOW(pThis->window),
							GTK_FILE_CHOOSER_ACTION_OPEN,
							"_Open",
							"_Cancel");

					g_object_set_data(G_OBJECT(fileChooser),"pThis",pThis);
					g_signal_connect(fileChooser,"response",G_CALLBACK(window_main_filechooser_response),NULL);
					gtk_native_dialog_show(GTK_NATIVE_DIALOG(fileChooser));
				}
				break;
			case ePRESSED_WINDOW_MAIN_SONGPOS:
				payload.newSongPos=pThis->songInfo.pos;
				controller_event(pThis->pControllerContext,eEVENT_JUMP,&payload);
				break;
			case ePRESSED_WINDOW_MAIN_CLUTTERBAR_I:
				window_license_show(&(pThis->handleWindowLicense));
				break;
			case ePRESSED_WINDOW_MAIN_CLUTTERBAR_D:
				controller_event(pThis->pControllerContext,eEVENT_SCALE,NULL);
				break;

			case ePRESSED_WINDOW_MAIN_CLUTTERBAR_V:
				visualizer_cycle(&(pThis->handleVisualizer));	
				break;

			case ePRESSED_WINDOW_MAIN_EQUALIZER:
				payload.hide0show1=(pThis->status.equalizer==eONOFF_OFF);	// this one will toggle
				controller_event(pThis->pControllerContext,eEVENT_WINDOW_EQUALIZER,&payload);
				break;
			case ePRESSED_WINDOW_MAIN_PLAYLIST:
				payload.hide0show1=(pThis->status.playlist==eONOFF_OFF);	// this one will toggle
				controller_event(pThis->pControllerContext,eEVENT_WINDOW_PLAYLIST,&payload);
				break;

			case ePRESSED_WINDOW_MAIN_SHUFFLE:
				pThis->status.shuffle=(pThis->status.shuffle==eONOFF_ON)?eONOFF_OFF:eONOFF_ON;
				break;
			case ePRESSED_WINDOW_MAIN_REPEAT:
				pThis->status.repeat=(pThis->status.repeat==eONOFF_ON)?eONOFF_OFF:eONOFF_ON;
				break;
			case ePRESSED_WINDOW_MAIN_NUMBERS:
				pThis->status.countdown=(pThis->status.countdown==eONOFF_ON)?eONOFF_OFF:eONOFF_ON;
				break;
			default:
				break;
		}	
	}


	pThis->lastPressed=ePRESSED_NONE;
	window_main_refresh(pThis);	
}
static void window_main_event_drag_begin(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
	tHandleWindowMain* pThis=(tHandleWindowMain*)g_object_get_data(G_OBJECT(gesture),"pThis");
}
static void window_main_event_drag_update(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
	int pos;
	tPayload payload;
	tHandleWindowMain* pThis=(tHandleWindowMain*)g_object_get_data(G_OBJECT(gesture),"pThis");
	switch (pThis->lastPressed)
	{
		case ePRESSED_WINDOW_MAIN_VOLUME:
			payload.volume=gui_helpers_relative_value(0,100,ELEMENT_DESTX(VOLUME_000_001),ELEMENT_DESTX2(VOLUME_000_001),0,pThis->pressedX+x,pThis->pressedY+y,window,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);
			controller_event(pThis->pControllerContext,eEVENT_SET_VOLUME,&payload);
			break;
		case ePRESSED_WINDOW_MAIN_BALANCE:
			payload.balance=gui_helpers_relative_value(-100,100,ELEMENT_DESTX(BALANCE_CENTERED),ELEMENT_DESTX2(BALANCE_CENTERED),0,pThis->pressedX+x,pThis->pressedY+y,window,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);
			controller_event(pThis->pControllerContext,eEVENT_SET_BALANCE,&payload);
			break;
		case ePRESSED_WINDOW_MAIN_SONGPOS:
			pThis->songInfo.pos=gui_helpers_relative_value(0,pThis->songInfo.len,ELEMENT_DESTX(POSBAR_SONG_PROGRESS_BAR),ELEMENT_DESTX2(POSBAR_SONG_PROGRESS_BAR),0,pThis->pressedX+x,pThis->pressedY+y,window,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);		
			break;
		default:
		break;
	}
}
static void window_main_event_drag_end(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
	tHandleWindowMain* pThis=(tHandleWindowMain*)g_object_get_data(G_OBJECT(gesture),"pThis");
}
static void window_main_filechooser_response(GtkNativeDialog *native,int response)
{
	if (response==GTK_RESPONSE_ACCEPT)
	{
		tPayload payload;
		GtkFileChooser *fileChooser=GTK_FILE_CHOOSER(native);
		GFile *chosen=gtk_file_chooser_get_file(fileChooser);
		tHandleWindowMain* pThis=(tHandleWindowMain*)g_object_get_data(G_OBJECT(native),"pThis");
		payload.filename=(char*)g_file_get_parse_name(chosen);
		controller_event(pThis->pControllerContext,eEVENT_OPEN_FILE,&payload);
	}
	g_object_unref(native);
}

static gboolean window_main_heartbeat(gpointer user_data)
{
	signed short pcm[512];
	tHandleWindowMain* pThis=(tHandleWindowMain*)user_data;
	tSongInfo songInfo;
	pthread_mutex_lock(&pThis->mutex);
	controller_pull_songInfo(pThis->pControllerContext,&songInfo);
	controller_pull_pcm(pThis->pControllerContext,pcm,512);
	visualizer_newPcm(&(pThis->handleVisualizer),pcm,512);
	
	window_main_update_songinfo(pThis,&songInfo);
	pthread_mutex_unlock(&pThis->mutex);
	pThis->songinfo_scrollpos++;
	window_main_refresh(pThis);

        return G_SOURCE_CONTINUE;
}
static gboolean window_main_close(GtkWidget *widget,gpointer user_data)
{
	exit(0);	
	return TRUE;
}
	

