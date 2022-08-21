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
#include "window_equalizer.h"

#define	INACTIVE	0
#define	ACTIVE		1

#define	PRESSABLE_NUM	18

int window_equalizer_draw(tHandleWindowEqualizer* pThis,GdkPixbuf *pixbufDestination)
{
	int i;
	int barX[BAR_NUM]={21,78,96,114,132,150,168,186,204,222,240};
	eEqualizerPressed barPressed[BAR_NUM]={EQUALIZER_PRESSED_PREAMP, EQUALIZER_PRESSED_60HZ, EQUALIZER_PRESSED_170HZ, EQUALIZER_PRESSED_310HZ, EQUALIZER_PRESSED_600HZ, EQUALIZER_PRESSED_1KHZ, EQUALIZER_PRESSED_3KHZ, EQUALIZER_PRESSED_6KHZ, EQUALIZER_PRESSED_12KHZ, EQUALIZER_PRESSED_14KHZ, EQUALIZER_PRESSED_16KHZ};
	
	eElementID barElements[]={EQMAIN_M2000DB_BAR,EQMAIN_M1875DB_BAR,EQMAIN_M1714DB_BAR,EQMAIN_M1571DB_BAR,EQMAIN_M1429DB_BAR,EQMAIN_M1286DB_BAR,EQMAIN_M1143DB_BAR,EQMAIN_M1000DB_BAR,EQMAIN_M0857DB_BAR,EQMAIN_M0714DB_BAR,EQMAIN_M0571DB_BAR,EQMAIN_M0428DB_BAR,EQMAIN_M0285DB_BAR,EQMAIN_M0142DB_BAR,
	ELEMENT_NONE,
	EQMAIN_P0142DB_BAR,EQMAIN_P0285DB_BAR,EQMAIN_P0428DB_BAR,EQMAIN_P0571DB_BAR,EQMAIN_P0714DB_BAR,EQMAIN_P0857DB_BAR,EQMAIN_P1000DB_BAR,EQMAIN_P1143DB_BAR,EQMAIN_P1286DB_BAR,EQMAIN_P1429DB_BAR,EQMAIN_P1571DB_BAR,EQMAIN_P1714DB_BAR,EQMAIN_P1857DB_BAR,EQMAIN_P2000DB_BAR};
	eElementID barElements0dB[BAR_NUM]={EQMAIN_PREAMP_BAR,EQMAIN_60HZ_BAR,EQMAIN_170HZ_BAR,EQMAIN_310HZ_BAR,EQMAIN_600HZ_BAR,EQMAIN_1KHZ_BAR,EQMAIN_3KHZ_BAR,EQMAIN_6KHZ_BAR,EQMAIN_12KHZ_BAR,EQMAIN_14KHZ_BAR,EQMAIN_16KHZ_BAR};
	
	
	pthread_mutex_lock(&pThis->mutex);

	// the title bar
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,  0,  0,(pThis->statusTitlebar==ACTIVE)?EQMAIN_EQUALIZER_TITLEBAR_ACTIVE:EQMAIN_EQUALIZER_TITLEBAR_INACTIVE);
	// main display
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,  0, 14,EQMAIN_EQUALIZER_MAIN_DISPLAY);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination, 86, 16,EQMAIN_EQUALIZER_MINIDISPLAY);
	// on/off
	if (pThis->lastPressed==EQUALIZER_PRESSED_ONOFF)
	{
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination, 14, 18,pThis->onOff?EQMAIN_EQUALIZER_ON_PRESSED:EQMAIN_EQUALIZER_OFF_PRESSED);
	} else {
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination, 14, 18,pThis->onOff?EQMAIN_EQUALIZER_ON_UNPRESSED:EQMAIN_EQUALIZER_OFF_UNPRESSED);	
	}
	if (pThis->lastPressed==EQUALIZER_PRESSED_AUTO)
	{
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination, 39, 18,pThis->automaticOnOff?EQMAIN_AUTO_ON_PRESSED:EQMAIN_AUTO_OFF_PRESSED);
	} else {
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination, 39, 18,pThis->automaticOnOff?EQMAIN_AUTO_ON_UNPRESSED:EQMAIN_AUTO_OFF_UNPRESSED);
	}
	
	// presets
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,217,18,pThis->lastPressed==EQUALIZER_PRESSED_PRESET?EQMAIN_PRESET_BUTTON_PRESSED:EQMAIN_PRESET_BUTTON_UNPRESSED);
	

	// bars
	for (i=0;i<BAR_NUM;i++)
	{
		int y;
		int bary;
		eElementID e=barElements0dB[i];
		y=pThis->valueBar[i];
		y=(y*28)/100;		// -100 .. 100 --> -28..28

		bary=32+38-7;
		if (y)
		{
			bary-=y;
			e=barElements[y/2+14];
		}
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,barX[i],38,e);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,1+barX[i],bary,pThis->lastPressed==barPressed[i]?EQMAIN_EQUALIZER_SLIDER_PRESSED:EQMAIN_EQUALIZER_SLIDER_UNPRESSED);
		
	}	
	// resets
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination, 42,33,EQMAIN_20DB_RESET);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination, 42,65,EQMAIN_0DB_RESET);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination, 42,92,EQMAIN_M20DB_RESET);

	// close button
	//theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,275-9-1,1,pThis->lastPressed=EQUALIZER_PRESSED_CLOSE?EQMAIN_CLOSE_BUTTON_PRESSED:EQMAIN_CLOSE_BUTTON_UNPRESSED);	// FIXME
	

	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,86,16,EQMAIN_ACTUAL_EQUALIZER_MINIDISPLAY);	

	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,86,16+10+(pThis->valueBar[0]*10)/100,EQMAIN_PREAMP_LINE);
	
	{
		GdkPixbuf *pixbuf;
		pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,1,19);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbuf,0,0,EQMAIN_SPLINE_LINE);
		for (i=0;i<113;i++)
		{
			gdk_pixbuf_copy_area(pixbuf,0,10-pThis->statusSpline[i],1,1,pixbufDestination,86+i,16+10-pThis->statusSpline[i]);
		}
		g_object_unref(pixbuf);
	}


	pthread_mutex_unlock(&pThis->mutex);
	return RETVAL_OK;
}
int window_equalizer_refresh(tHandleWindowEqualizer* pThis)
{
	GdkPixbuf *pixbuf;
	pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT);
	window_equalizer_draw(pThis,pixbuf);
	pthread_mutex_lock(&pThis->mutex);
	gdk_pixbuf_copy_area(pixbuf,0,0,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,pThis->pixbuf,0,0);
	g_object_unref(pixbuf);	
	gtk_picture_set_pixbuf(GTK_PICTURE(pThis->picture),pThis->pixbuf);
	{
		
		gtk_widget_queue_draw(pThis->windowEqualizer);
	}
	pthread_mutex_unlock(&pThis->mutex);

	return RETVAL_OK;
}
int window_equalizer_calculate_value_from_y(int y,int yup,int ydown,int margin, int height,  int valueup,int valuedown,int *value)
{
	int delta_value;
	int delta_y;
	int retval;
	double scaleFactor;


	yup-=margin;
	ydown+=margin;

	scaleFactor=height/WINDOW_EQUALIZER_HEIGHT;

	delta_value=valueup-valuedown;
	delta_y=(yup-ydown)*scaleFactor;



	retval=RETVAL_NOK;
	
	if (y<yup*scaleFactor)
	{
		*value=valueup;
		retval=RETVAL_OK;
	} 
	else if (y>ydown*scaleFactor)
	{
		*value=valuedown;
		retval=RETVAL_OK;
	}
	else if (delta_y)
	{
		*value=(((y-yup*scaleFactor)*delta_value)/delta_y)+valueup;
		retval=RETVAL_OK;
	}	
	return retval;
}

int window_equalizer_update_splines(tHandleWindowEqualizer *pThis)
{

	int i;
	int j;
	int x1[124];
	int x2[124];

	for (i=0;i<124;i++)
	{
		x1[i]=0;
	}
	for (i=1;i<11;i++)
	{
		x1[(i-1)*11]=pThis->valueBar[i];
	}
	for (i=0;i<124;i++)
	{
		x2[i]=0;
		for (j=0;j<11;j++)
		{
			if ((i-j)>=0)
			{
				x2[i]+=(x1[(i-j)]);
			}
		}
		x2[i]/=11;
	}

	for (i=0;i<124;i++)
	{
		x1[i]=0;
		for (j=0;j<11;j++)
		{
			if ((i-j)>=0)
			{
				x1[i]+=(x2[(i-j)]);
			}
		}
		x1[i]/=11;
	}

	for (i=0;i<124;i++)
	{
		if (i>=11)
		{
			pThis->statusSpline[i-11]=0;
			for (j=0;j<11;j++)
			{
				if ((i-j)>=0)
				{
					pThis->statusSpline[i-11]+=x1[(i-j)];
				}
			}
			pThis->statusSpline[i-11]/=11;
			if (pThis->statusSpline[i-11]>9) pThis->statusSpline[i-11]=9;
			if (pThis->statusSpline[i-11]<-8) pThis->statusSpline[i-11]=-8;
		}
	}


	return RETVAL_OK;
}
int window_equalizer_handle_press(tHandleWindowEqualizer *pThis,eEqualizerPressed pressed,int x,int y,int height)
{
	int bar;
	int value;
	switch(pressed)
	{
		case EQUALIZER_PRESSED_ONOFF:
// TODO since the equalizer does not have any functionality (yet), suggests that it is always OFF
//			pThis->onOff=1-pThis->onOff;
			break;
		case EQUALIZER_PRESSED_AUTO:
// TODO since the equalizer does not have any functionality (yet), suggests that it is always OFF
//			pThis->automaticOnOff=1-pThis->automaticOnOff;
			break;
		case EQUALIZER_PRESSED_PREAMP:
		case EQUALIZER_PRESSED_60HZ:
		case EQUALIZER_PRESSED_170HZ:
		case EQUALIZER_PRESSED_310HZ:
		case EQUALIZER_PRESSED_600HZ:
		case EQUALIZER_PRESSED_1KHZ:
		case EQUALIZER_PRESSED_3KHZ:
		case EQUALIZER_PRESSED_6KHZ:
		case EQUALIZER_PRESSED_12KHZ:
		case EQUALIZER_PRESSED_14KHZ:
		case EQUALIZER_PRESSED_16KHZ:
			bar=pressed-EQUALIZER_PRESSED_PREAMP;
//int window_equalizer_calculate_value_from_y(int y,int yup,int ydown,int margin, int height,  int valueup,int valuedown,int *value)
			window_equalizer_calculate_value_from_y(y,38,38+63,0,height,100,-100,&value);
			pThis->valueBar[bar]=value;
			window_equalizer_update_splines(pThis);
			break;
		case EQUALIZER_PRESSED_M20DB_RESET:
			for (bar=1;bar<BAR_NUM;bar++)
			{
				pThis->valueBar[bar]=-100;
			}
			window_equalizer_update_splines(pThis);
			break;
		case EQUALIZER_PRESSED_0DB_RESET:
			for (bar=1;bar<BAR_NUM;bar++)
			{
				pThis->valueBar[bar]=0;
			}
			window_equalizer_update_splines(pThis);
			break;
		case EQUALIZER_PRESSED_P20DB_RESET:
			for (bar=1;bar<BAR_NUM;bar++)
			{
				pThis->valueBar[bar]=100;
			}
			window_equalizer_update_splines(pThis);
			break;
		default:
			printf("todo; handle %d\n",pressed);
			break;
	}
	return RETVAL_OK;
}

eEqualizerPressed window_equalizer_find_pressable(int x,int y,int width,int height)
{
	int i;
	double scaleFactorX;
	double scaleFactorY;
	eEqualizerPressed pressed;
	typedef struct _tPressableEqualizer
	{
		eEqualizerPressed pressed;
		int posx;
		int posy;
		int dimx;
		int dimy;
	} tPressableEqualizer;

	const tPressableEqualizer cWindowEqualizer_pressable[PRESSABLE_NUM]=
	{
		{.pressed=EQUALIZER_PRESSED_ONOFF, .posx= 14,.posy= 18,.dimx= 25,.dimy= 12},
		{.pressed=EQUALIZER_PRESSED_AUTO,  .posx= 39,.posy= 18,.dimx= 33,.dimy= 12},
		{.pressed=EQUALIZER_PRESSED_PRESET,.posx=217,.posy= 18,.dimx= 44,.dimy= 12},
		{.pressed=EQUALIZER_PRESSED_PREAMP,.posx= 21,.posy= 38,.dimx= 14,.dimy= 63},
		{.pressed=EQUALIZER_PRESSED_60HZ,  .posx= 78,.posy= 38,.dimx= 14,.dimy= 63},
		{.pressed=EQUALIZER_PRESSED_170HZ, .posx= 96,.posy= 38,.dimx= 14,.dimy= 63},
		{.pressed=EQUALIZER_PRESSED_310HZ, .posx=114,.posy= 38,.dimx= 14,.dimy= 63},
		{.pressed=EQUALIZER_PRESSED_600HZ, .posx=132,.posy= 38,.dimx= 14,.dimy= 63},
		{.pressed=EQUALIZER_PRESSED_1KHZ,  .posx=150,.posy= 38,.dimx= 14,.dimy= 63},
		{.pressed=EQUALIZER_PRESSED_3KHZ,  .posx=168,.posy= 38,.dimx= 14,.dimy= 63},
		{.pressed=EQUALIZER_PRESSED_6KHZ,  .posx=186,.posy= 38,.dimx= 14,.dimy= 63},
		{.pressed=EQUALIZER_PRESSED_12KHZ, .posx=204,.posy= 38,.dimx= 14,.dimy= 63},
		{.pressed=EQUALIZER_PRESSED_14KHZ, .posx=222,.posy= 38,.dimx= 14,.dimy= 63},
		{.pressed=EQUALIZER_PRESSED_16KHZ, .posx=240,.posy= 38,.dimx= 14,.dimy= 63},
		{.pressed=EQUALIZER_PRESSED_CLOSE, .posx=266,.posy=  0,.dimx=  9,.dimy=  9},

		{.pressed=EQUALIZER_PRESSED_P20DB_RESET, .posx= 42,.posy= 33,.dimx= 26,.dimy= 10},
		{.pressed=EQUALIZER_PRESSED_0DB_RESET,   .posx= 42,.posy= 65,.dimx= 26,.dimy= 10},
		{.pressed=EQUALIZER_PRESSED_M20DB_RESET, .posx= 42,.posy= 92,.dimx= 26,.dimy= 10}
	};
	scaleFactorX=width/WINDOW_EQUALIZER_WIDTH;
	scaleFactorY=height/WINDOW_EQUALIZER_HEIGHT;
	pressed=EQUALIZER_PRESSED_NONE;
	for (i=0;i<PRESSABLE_NUM;i++)
	{
		int x1,x2;
		int y1,y2;

		x1=scaleFactorX*cWindowEqualizer_pressable[i].posx;
		x2=x1+cWindowEqualizer_pressable[i].dimx*scaleFactorX;

		y1=scaleFactorY*cWindowEqualizer_pressable[i].posy;
		y2=y1+cWindowEqualizer_pressable[i].dimy*scaleFactorY;

		if (x>=x1 && x<x2 && y>=y1 && y<y2)
		{
			pressed=cWindowEqualizer_pressable[i].pressed;
		}
	}

	return pressed;

}
int window_equalizer_event_pressed(GtkWidget *widget, double x,double y,guint event_button, gpointer data)
{
	eEqualizerPressed pressed;
	tHandleWindowEqualizer* pThis=(tHandleWindowEqualizer*)data;
	int width,height;
        width=gtk_widget_get_width(pThis->windowEqualizer);
        height=gtk_widget_get_height(pThis->windowEqualizer);
	pressed=window_equalizer_find_pressable((int)x,(int)y,width,height);
	printf("found:%d\n",pressed);

	pThis->lastPressed=pressed;
	window_equalizer_refresh(pThis);

	return TRUE;
}
int window_equalizer_event_released(GtkWidget *widget, double x,double y,guint event_button, gpointer data)
{
	eEqualizerPressed pressed;
	tHandleWindowEqualizer* pThis=(tHandleWindowEqualizer*)data;
	int width,height;
        width=gtk_widget_get_width(pThis->windowEqualizer);
        height=gtk_widget_get_height(pThis->windowEqualizer);
	pressed=window_equalizer_find_pressable((int)x,(int)y,width,height);
	if (pressed==pThis->lastPressed && EQUALIZER_PRESSED_NONE!=pressed)
	{
		window_equalizer_handle_press(pThis,pressed,x,y,height);	
	}

	pThis->lastPressed=EQUALIZER_PRESSED_NONE;
	window_equalizer_refresh(pThis);

	return TRUE;
}
int window_equalizer_init(GtkApplication* app,tHandleWindowEqualizer* pThis,tHandleThemeManager* pHandleThemeManager,tHandleDecoder *pHandleDecoder)
{
	memset(pThis,0,sizeof(tHandleWindowEqualizer));
	pthread_mutex_init(&pThis->mutex,NULL);	
	pThis->pHandleDecoder=pHandleDecoder;
	pThis->pHandleThemeManager=pHandleThemeManager;

	pThis->scaleFactor=SCALEFACTOR;
	pThis->pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT);

	pThis->statusTitlebar=ACTIVE;
	pThis->onOff=0;
	pThis->automaticOnOff=0;

	window_equalizer_draw(pThis,pThis->pixbuf);
	pThis->picture=gtk_picture_new_for_pixbuf(pThis->pixbuf);
	pThis->windowEqualizer=gtk_application_window_new(app);
	gtk_window_set_default_size(GTK_WINDOW(pThis->windowEqualizer),WINDOW_EQUALIZER_WIDTH*pThis->scaleFactor,WINDOW_EQUALIZER_HEIGHT*pThis->scaleFactor);
	gtk_window_set_child(GTK_WINDOW(pThis->windowEqualizer),pThis->picture);
	gtk_window_set_title(GTK_WINDOW(pThis->windowEqualizer),"d11amp equalizer");

	pThis->gesture=gtk_gesture_click_new();
	g_signal_connect(pThis->gesture,"pressed",G_CALLBACK(window_equalizer_event_pressed),&(pThis->windowEqualizer));
	g_signal_connect(pThis->gesture,"released",G_CALLBACK(window_equalizer_event_released),&(pThis->windowEqualizer));
	gtk_widget_add_controller(pThis->windowEqualizer,GTK_EVENT_CONTROLLER(pThis->gesture));


	return RETVAL_OK;
}
int window_equalizer_show(tHandleWindowEqualizer* pThis)
{
	gtk_widget_show(pThis->windowEqualizer);
	return RETVAL_OK;
}
int window_equalizer_hide(tHandleWindowEqualizer* pThis)
{
	gtk_widget_hide(pThis->windowEqualizer);
	return RETVAL_OK;

}

