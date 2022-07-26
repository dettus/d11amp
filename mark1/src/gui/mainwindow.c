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
// the song info
	{
		int x;
		int margin;

		margin=154;	// the width of the song info window
		x=pThis->songInfo_scrollpos;
		if (x>pThis->songInfo_scrolllen-margin)
		{
			x=pThis->songInfo_scrolllen-margin;
		}
		if (x<0) x=0;
		gdk_pixbuf_copy_area(pThis->songInfoPixbuf,x,0,154,6,pThis->mainPixbuf,111,27);

		gdk_pixbuf_copy_area(pThis->bitratePixbuf,0,0,15,6,pThis->mainPixbuf,111,43);
		gdk_pixbuf_copy_area(pThis->khzPixbuf,0,0,10,6,pThis->mainPixbuf,156,43);
	}
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
	if (pThis->pressed==pressed && pressed!=PRESSED_NONE)
	{
		if (pressed==PRESSED_CLUTTERBAR_D) 
		{
			pThis->scalefactor*=2;
			if (pThis->scalefactor>=16) pThis->scalefactor=1;
			gtk_window_resize(GTK_WINDOW(pThis->mainWindow), 275*pThis->scalefactor, 116*pThis->scalefactor);
		}
		if (pressed==PRESSED_OPEN)
		{
     
			GtkWidget *dialog;

			dialog = gtk_file_chooser_dialog_new ("Open File",
					GTK_WINDOW(pThis->mainWindow),
					GTK_FILE_CHOOSER_ACTION_OPEN,
					"_Cancel", GTK_RESPONSE_CANCEL,
					"_Open", GTK_RESPONSE_ACCEPT,
					NULL);

			if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
			{
				char *filename;

				filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
				decode_fileopen(pThis->pHandleDecoderMain,filename);
				g_free (filename);
			}

			gtk_widget_destroy (dialog);
			printf("pressed open\n");
		}
		if (pressed==PRESSED_PLAY)
		{
			printf("pressed play\n");
			decode_fileplay(pThis->pHandleDecoderMain);	
		}
		if (pressed==PRESSED_PAUSE)
		{
			printf("pressed pause\n");
			decode_filepause(pThis->pHandleDecoderMain);	
		}
		if (pressed==PRESSED_STOP)
		{
			printf("pressed stop\n");
			decode_filestop(pThis->pHandleDecoderMain);	
		}
		if (pressed==PRESSED_SONGPOS)
		{
			int newpos;
			int posbarlen;

			if (pThis->file_len_seconds)
			{
				posx-=(29)*pThis->scalefactor;
				posbarlen=(248-17)*pThis->scalefactor;
				if (posx<0) posx=0;
				if (posx>=posbarlen) posx=posbarlen-1;
				newpos=(pThis->file_len_seconds*posx)/posbarlen;
				decode_filejump(pThis->pHandleDecoderMain,newpos);
			}
		}
	} else {

	}
	pThis->pressed=PRESSED_NONE;
	mainwindow_redraw(pThis);
	return TRUE;
}
int mainwindow_setsonginfo(tHandleMainWindow* pThis,char* songinfo,int bitrate,int rate,int channels)
{
	int i;
	int l;
	int x;
	char tmp[5];

	if (strncmp(songinfo,pThis->cur_songinfo,1024))
	{
		if (pThis->songInfoPixbuf!=NULL)
		{
			g_object_unref(pThis->songInfoPixbuf);
		}
		l=strlen(songinfo);
		if (l>1023) l=1023;
		memcpy(pThis->cur_songinfo,songinfo,l+1);
		pThis->cur_songinfo[1023]=0;

		x=l*5;
		if (x<155) x=155;
		pThis->songInfoPixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,x,6);
		pThis->songInfo_scrollpos=0;
		pThis->songInfo_scrolllen=x;	

		x=0;
		for (i=0;i<l;i++)
		{
			char c;
			eElementID elementID;

			c=songinfo[i];
			pixbufloader_textelement(pThis->pHandlePixbufLoader,c,&elementID,0);	
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->songInfoPixbuf,x,0,elementID);
			x+=5;
		}
		while (x<155)
		{
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->songInfoPixbuf,x,0,TEXT_TITLE_DISPLAY_SPACE);
			x+=5;
		}
		
	}


	if (pThis->cur_kbps!=bitrate)
	{
		if (pThis->bitratePixbuf!=NULL)
		{
			g_object_unref(pThis->bitratePixbuf);
		}
		pThis->bitratePixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,15,6);
		x=0;
		if (bitrate>999) bitrate=999;
		if (bitrate<0) bitrate=0;
		snprintf(tmp,4,"%3d",bitrate);
		for (i=0;i<3;i++)
		{
			char c;
			eElementID elementID;
			c=tmp[i];
			pixbufloader_textelement(pThis->pHandlePixbufLoader,c,&elementID,0);	
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->bitratePixbuf,x,0,elementID);
			x+=5;
		}
		pThis->cur_kbps=bitrate;
	}

	if (pThis->cur_rate!=rate)
	{
		pThis->cur_rate=rate;
		if (pThis->khzPixbuf!=NULL)
		{
			g_object_unref(pThis->khzPixbuf);
		}
		pThis->khzPixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,10,6);
		x=0;
		if (rate>99999) rate=99999;
		if (rate<0) rate=0;
		rate/=1000;
		snprintf(tmp,4,"%2d",rate);
		for (i=0;i<2;i++)
		{
			char c;
			eElementID elementID;
			c=tmp[i];
			pixbufloader_textelement(pThis->pHandlePixbufLoader,c,&elementID,0);	
			pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->khzPixbuf,x,0,elementID);
			x+=5;
		}
	}

	switch(channels)
	{
		case 1: pThis->mono_stereo=MONOSTER_MONO;break;
		case 2: pThis->mono_stereo=MONOSTER_STEREO;break;
		default:pThis->mono_stereo=MONOSTER_UNKNOWN;break;
	}
	
	return MAINWINDOW_OK;
}
int mainwindow_setnumbers(tHandleMainWindow* pThis,int minutes,int seconds)
{
	int digits[4];
	int i;
	int change;
	for (i=0;i<4;i++)
	{
		digits[i]=pThis->time_digit[i];
	}
	

	
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
	change=0;
	for (i=0;i<4;i++)
	{
		if (digits[i]!=pThis->time_digit[i])
		{
			change=1;
		}
	}
	if (change)
	{
		mainwindow_redraw(pThis);
	}

	return MAINWINDOW_OK;
}
int mainwindow_setpos(tHandleMainWindow* pThis,int file_len,int file_pos)
{
	int minutes;
	int seconds;

	if (file_len)
	{
		pThis->songpos=((248-29)*file_pos)/file_len;	// TODO: dimx of POSBAR_SONG_PROGRESS_BAR-POSBAR_SONG_SLIDER_PRESSED
	} else {
		pThis->songpos=0;
	}

	pThis->file_len_seconds=file_len;
	pThis->file_pos_seconds=file_pos;
	minutes=file_pos/60;
	seconds=file_pos%60;
	pThis->songInfo_scrollpos+=3;
	if (pThis->songInfo_scrollpos>=pThis->songInfo_scrolllen-77)
	{
		pThis->songInfo_scrollpos=-77;
	}
	mainwindow_redraw(pThis);


	return mainwindow_setnumbers(pThis,minutes,seconds);
}

int mainwindow_init(tHandleMainWindow* pThis,tHandlePixbufLoader *pPixbuf,tHandleDecoderMain* pHandleDecoderMain)
{


	mainwindow_defaultvalues(pThis);
	// step 1: create the pixbuf as the "drawing area"
	pThis->pHandlePixbufLoader=pPixbuf;
	pThis->pHandleDecoderMain=pHandleDecoderMain;

	pThis->scaledPixbuf=NULL;
	pThis->mainPixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,275,116);
	pThis->songInfoPixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,154,6);
	pThis->songInfo_scrollpos=0;
	pThis->songInfo_scrolllen=0;

	pThis->cur_rate=-1;
	pThis->cur_kbps=-1;
	pThis->cur_songinfo[0]=0;

	mainwindow_setnumbers(pThis,28,3);
	mainwindow_setsonginfo(pThis,"abcdefghijklmnopqrstuvwxyz01234567890!@#$%^&*()_-+[]\\/",128,44100,2);

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


