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
#include "window_equalizer.h"
#include <string.h>
#include <stdio.h>

// function headers for the gtk events. the implementation is at the end of this file
static void window_equalizer_event_pressed(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window);
static void window_equalizer_event_released(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window);
static void window_equalizer_event_drag_begin(GtkGestureDrag *gesture, double x, double y, GtkWidget *window);
static void window_equalizer_event_drag_update(GtkGestureDrag *gesture, double x, double y, GtkWidget *window);
static void window_equalizer_event_drag_end(GtkGestureDrag *gesture, double x, double y, GtkWidget *window);
static gboolean window_equalizer_close(GtkWidget *widget,gpointer data);


int window_equalizer_init(tHandleWindowEqualizer* pThis,void* pControllerContext,tHandleThemeManager *pHandleThemeManager,GtkApplication* app)
{
	int retval;
	retval=RETVAL_OK;
		
	memset(pThis,0,sizeof(tHandleWindowEqualizer));
	pThis->app=app;
	pThis->pControllerContext=pControllerContext;
	pThis->pHandleThemeManager=pHandleThemeManager;
	
	pThis->pixbuf_handle=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HANDLE_HEIGHT);
	pThis->pixbuf_main=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT-WINDOW_EQUALIZER_HANDLE_HEIGHT);
	pThis->pixbufBackground=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT);


	pThis->window=gtk_application_window_new(pThis->app);
	gtk_window_set_default_size(GTK_WINDOW(pThis->window),WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT);

	pThis->box=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
	gtk_box_set_homogeneous(GTK_BOX(pThis->box),FALSE);
	gtk_widget_show(pThis->box);
	gtk_window_set_child(GTK_WINDOW(pThis->window),pThis->box);
	gtk_window_set_title(GTK_WINDOW(pThis->window),"d11amp equalizer");
	gtk_window_set_resizable(GTK_WINDOW(pThis->window),FALSE);
#ifdef	D11AMP_NODECORATED
	gtk_window_set_decorated(GTK_WINDOW(pThis->window),FALSE);
#endif

	
	pThis->picture_handle=gtk_picture_new();
	pThis->picture_main=gtk_picture_new();
	gtk_widget_show(pThis->picture_handle);
	gtk_widget_show(pThis->picture_main);
	
	pThis->handle=gtk_window_handle_new();
	gtk_window_handle_set_child(GTK_WINDOW_HANDLE(pThis->handle),pThis->picture_handle);
	gtk_widget_show(pThis->handle);

	gtk_box_append(GTK_BOX(pThis->box),pThis->handle);
	gtk_box_append(GTK_BOX(pThis->box),pThis->picture_main);


	pThis->pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT);
	pThis->lastPressed=ePRESSED_NONE;
	pThis->gesture_click=gtk_gesture_click_new();
	g_object_set_data(G_OBJECT(pThis->gesture_click),"pThis",pThis);	// add a pointer to the handle to the widget. this way it is available in the gesture callbacks
	g_signal_connect(pThis->gesture_click,"pressed", G_CALLBACK(window_equalizer_event_pressed) ,(pThis->window));
	g_signal_connect(pThis->gesture_click,"released",G_CALLBACK(window_equalizer_event_released),(pThis->window));
	gtk_widget_add_controller(pThis->window,GTK_EVENT_CONTROLLER(pThis->gesture_click));

	pThis->gesture_drag=gtk_gesture_drag_new();
	g_object_set_data(G_OBJECT(pThis->gesture_drag),"pThis",pThis);	// add a pointer to the handle to the widget. this way it is available in the gesture callbacks
	g_signal_connect (pThis->gesture_drag,"drag-begin", G_CALLBACK (window_equalizer_event_drag_begin), (pThis->window));
	g_signal_connect (pThis->gesture_drag,"drag-update",G_CALLBACK (window_equalizer_event_drag_update),(pThis->window));
	g_signal_connect (pThis->gesture_drag,"drag-end",   G_CALLBACK (window_equalizer_event_drag_end),   (pThis->window));

	gtk_widget_add_controller(pThis->window,GTK_EVENT_CONTROLLER(pThis->gesture_drag));
// define the pressables
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[ 0],ePRESSED_WINDOW_EQUALIZER_ONOFF,	EQMAIN_EQUALIZER_OFF_UNPRESSED);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[ 1],ePRESSED_WINDOW_EQUALIZER_AUTO,	EQMAIN_AUTO_OFF_UNPRESSED);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[ 2],ePRESSED_WINDOW_EQUALIZER_PRESET,	EQMAIN_PRESET_BUTTON_UNPRESSED);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[ 3],ePRESSED_WINDOW_EQUALIZER_20DB_RESET,	EQMAIN_20DB_RESET);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[ 4],ePRESSED_WINDOW_EQUALIZER_0DB_RESET,	EQMAIN_0DB_RESET);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[ 5],ePRESSED_WINDOW_EQUALIZER_M20DB_RESET,	EQMAIN_M20DB_RESET);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[ 6],ePRESSED_WINDOW_EQUALIZER_PREAMP,	EQMAIN_PREAMP_BAR);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[ 7],ePRESSED_WINDOW_EQUALIZER_60HZ,	EQMAIN_60HZ_BAR);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[ 8],ePRESSED_WINDOW_EQUALIZER_170HZ,	EQMAIN_170HZ_BAR);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[ 9],ePRESSED_WINDOW_EQUALIZER_310HZ,	EQMAIN_310HZ_BAR);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[10],ePRESSED_WINDOW_EQUALIZER_600HZ,	EQMAIN_600HZ_BAR);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[11],ePRESSED_WINDOW_EQUALIZER_1KHZ,	EQMAIN_1KHZ_BAR);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[12],ePRESSED_WINDOW_EQUALIZER_3KHZ,	EQMAIN_3KHZ_BAR);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[13],ePRESSED_WINDOW_EQUALIZER_6KHZ,	EQMAIN_6KHZ_BAR);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[14],ePRESSED_WINDOW_EQUALIZER_12KHZ,	EQMAIN_12KHZ_BAR);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[15],ePRESSED_WINDOW_EQUALIZER_14KHZ,	EQMAIN_14KHZ_BAR);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[16],ePRESSED_WINDOW_EQUALIZER_16KHZ,	EQMAIN_16KHZ_BAR);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[17],ePRESSED_WINDOW_EQUALIZER_SHADE,	EQEX_MAXIMIZEBUTTON_PRESSED);
	gui_helpers_define_pressable_by_element(WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,&pThis->boundingBoxes[18],ePRESSED_WINDOW_EQUALIZER_CLOSE,	EQMAIN_CLOSE_BUTTON_PRESSED);
		
	g_signal_connect(G_OBJECT(pThis->window), "close_request", G_CALLBACK (window_equalizer_close), (void*)pThis);

	return retval;
}
int window_equalizer_start(tHandleWindowEqualizer* pThis)
{
	int i;
	char key[16];
	tPayload payload;
	
	config_init(&(pThis->handleConfig),pThis->pControllerContext,"equalizer.config");
	for (i=0;i<BAR_NUM;i++)
	{
		snprintf(key,16,"bar%02d",i);
		config_getint(&(pThis->handleConfig),key,&(payload.equalizer.value),0);
		payload.equalizer.bar=i;
		controller_event(pThis->pControllerContext,eEVENT_SET_EQUALIZER,&payload);
	}
	config_getonoff(&(pThis->handleConfig),"equalizer",&(pThis->status.equalizer),pThis->status.equalizer);
	payload.off0on1=(pThis->status.equalizer==eONOFF_ON);
	controller_event(pThis->pControllerContext,eEVENT_EQUALIZER_ONOFF,&payload);
	config_getonoff(&(pThis->handleConfig),"automatic",&(pThis->status.automatic),pThis->status.automatic);
	return RETVAL_OK;
}

// background: the default picture
int window_equalizer_refresh_background(tHandleWindowEqualizer* pThis)
{
	int retval;

	retval=RETVAL_OK;
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_EQUALIZER_TITLEBAR_INACTIVE);	// FIXME: when the cursor leaves the window, the titlebar stays active
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_EQUALIZER_TITLEBAR_ACTIVE);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_EQUALIZER_MAIN_DISPLAY);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_EQUALIZER_MINIDISPLAY);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_EQUALIZER_OFF_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_AUTO_OFF_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_PRESET_BUTTON_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_20DB_RESET);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_0DB_RESET);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_M20DB_RESET);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_PREAMP_BAR);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_60HZ_BAR);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_170HZ_BAR);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_310HZ_BAR);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_600HZ_BAR);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_1KHZ_BAR);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_3KHZ_BAR);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_6KHZ_BAR);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_12KHZ_BAR);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_14KHZ_BAR);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_16KHZ_BAR);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,EQMAIN_CLOSE_BUTTON_UNPRESSED);
	
	
	return retval;	
}
	
// status: what changes due to user interaction
int window_equalizer_draw_status(tHandleWindowEqualizer* pThis,GdkPixbuf *destBuf)
{
	int i;
	int retval;
#define	VALUES_NUM 28	// 29
	eElementID backgroundIDs[BAR_NUM]={EQMAIN_PREAMP_BAR,EQMAIN_60HZ_BAR,EQMAIN_170HZ_BAR,EQMAIN_310HZ_BAR,EQMAIN_600HZ_BAR,EQMAIN_1KHZ_BAR ,EQMAIN_3KHZ_BAR,EQMAIN_6KHZ_BAR,EQMAIN_12KHZ_BAR,EQMAIN_14KHZ_BAR,EQMAIN_16KHZ_BAR};
	eElementID valueIDs[VALUES_NUM]={EQMAIN_M2000DB_BAR,EQMAIN_M1875DB_BAR,EQMAIN_M1714DB_BAR,EQMAIN_M1571DB_BAR,EQMAIN_M1429DB_BAR,EQMAIN_M1286DB_BAR,EQMAIN_M1143DB_BAR,EQMAIN_M1000DB_BAR,EQMAIN_M0857DB_BAR,EQMAIN_M0714DB_BAR,EQMAIN_M0571DB_BAR,EQMAIN_M0428DB_BAR,EQMAIN_M0285DB_BAR,EQMAIN_M0142DB_BAR,
// ELEMENT_NONE
	EQMAIN_P0142DB_BAR,EQMAIN_P0285DB_BAR,EQMAIN_P0428DB_BAR,EQMAIN_P0571DB_BAR,EQMAIN_P0714DB_BAR,EQMAIN_P0857DB_BAR,EQMAIN_P1000DB_BAR,EQMAIN_P1143DB_BAR,EQMAIN_P1286DB_BAR,EQMAIN_P1429DB_BAR,EQMAIN_P1571DB_BAR,EQMAIN_P1714DB_BAR,EQMAIN_P1857DB_BAR,EQMAIN_P2000DB_BAR};

	retval=RETVAL_OK;
	for (i=0;i<BAR_NUM;i++)
	{
		eElementID backgroundID;
		eElementID valueID;
		int value;
		backgroundID=backgroundIDs[i];

		value=((pThis->status.bar[i]+100)*(VALUES_NUM-1))/200;
		valueID=valueIDs[value];
		if (valueID==ELEMENT_NONE)
		{
			valueID=backgroundID;
		}
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,valueID,ELEMENT_DESTX(backgroundID),ELEMENT_DESTY(backgroundID));
		pThis->status.barY[i]=((-pThis->status.bar[i]+100)*(ELEMENT_HEIGHT(backgroundID)-(ELEMENT_HEIGHT(EQMAIN_EQUALIZER_SLIDER_UNPRESSED))))/200+ELEMENT_DESTY(backgroundID);

		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,EQMAIN_EQUALIZER_SLIDER_UNPRESSED,1+ELEMENT_DESTX(backgroundID),pThis->status.barY[i]);
	}
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.equalizer==eONOFF_ON)?EQMAIN_EQUALIZER_ON_UNPRESSED:EQMAIN_EQUALIZER_OFF_UNPRESSED);
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.automatic==eONOFF_ON)?EQMAIN_AUTO_ON_UNPRESSED:EQMAIN_AUTO_OFF_UNPRESSED);

	
	return retval;
}
int window_equalizer_draw_presses(tHandleWindowEqualizer* pThis,GdkPixbuf *destBuf)
{
	int retval;
	retval=RETVAL_OK;

	switch(pThis->lastPressed)
	{
		case ePRESSED_WINDOW_EQUALIZER_ONOFF:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.equalizer==eONOFF_ON)?EQMAIN_EQUALIZER_ON_PRESSED:EQMAIN_EQUALIZER_OFF_PRESSED);
			break;
		case ePRESSED_WINDOW_EQUALIZER_AUTO:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(pThis->status.automatic==eONOFF_ON)?EQMAIN_AUTO_ON_PRESSED:EQMAIN_AUTO_OFF_PRESSED);
			break;
		case ePRESSED_WINDOW_EQUALIZER_PRESET:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,EQMAIN_PRESET_BUTTON_PRESSED);
			break;
		case ePRESSED_WINDOW_EQUALIZER_20DB_RESET:
		case ePRESSED_WINDOW_EQUALIZER_0DB_RESET:
		case ePRESSED_WINDOW_EQUALIZER_M20DB_RESET:
			break;
		case ePRESSED_WINDOW_EQUALIZER_PREAMP:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,1+EQMAIN_EQUALIZER_SLIDER_PRESSED,ELEMENT_DESTX(EQMAIN_PREAMP_BAR),pThis->status.barY[ 0]);
			break;
		case ePRESSED_WINDOW_EQUALIZER_60HZ:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,1+EQMAIN_EQUALIZER_SLIDER_PRESSED,ELEMENT_DESTX(EQMAIN_60HZ_BAR),pThis->status.barY[ 1]);
			break;
		case ePRESSED_WINDOW_EQUALIZER_170HZ:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,1+EQMAIN_EQUALIZER_SLIDER_PRESSED,ELEMENT_DESTX(EQMAIN_170HZ_BAR),pThis->status.barY[ 2]);
			break;
		case ePRESSED_WINDOW_EQUALIZER_310HZ:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,1+EQMAIN_EQUALIZER_SLIDER_PRESSED,ELEMENT_DESTX(EQMAIN_310HZ_BAR),pThis->status.barY[ 3]);
			break;
		case ePRESSED_WINDOW_EQUALIZER_600HZ:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,1+EQMAIN_EQUALIZER_SLIDER_PRESSED,ELEMENT_DESTX(EQMAIN_600HZ_BAR),pThis->status.barY[ 4]);
			break;
		case ePRESSED_WINDOW_EQUALIZER_1KHZ:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,1+EQMAIN_EQUALIZER_SLIDER_PRESSED,ELEMENT_DESTX(EQMAIN_1KHZ_BAR),pThis->status.barY[ 5]);
			break;
		case ePRESSED_WINDOW_EQUALIZER_3KHZ:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,1+EQMAIN_EQUALIZER_SLIDER_PRESSED,ELEMENT_DESTX(EQMAIN_3KHZ_BAR),pThis->status.barY[ 6]);
			break;
		case ePRESSED_WINDOW_EQUALIZER_6KHZ:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,1+EQMAIN_EQUALIZER_SLIDER_PRESSED,ELEMENT_DESTX(EQMAIN_6KHZ_BAR),pThis->status.barY[ 7]);
			break;
		case ePRESSED_WINDOW_EQUALIZER_12KHZ:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,1+EQMAIN_EQUALIZER_SLIDER_PRESSED,ELEMENT_DESTX(EQMAIN_12KHZ_BAR),pThis->status.barY[ 8]);
			break;
		case ePRESSED_WINDOW_EQUALIZER_14KHZ:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,1+EQMAIN_EQUALIZER_SLIDER_PRESSED,ELEMENT_DESTX(EQMAIN_14KHZ_BAR),pThis->status.barY[ 9]);
			break;
		case ePRESSED_WINDOW_EQUALIZER_16KHZ:
			retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,1+EQMAIN_EQUALIZER_SLIDER_PRESSED,ELEMENT_DESTX(EQMAIN_16KHZ_BAR),pThis->status.barY[10]);
			break;
		case ePRESSED_WINDOW_EQUALIZER_SHADE:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,EQEX_MAXIMIZEBUTTON_PRESSED);
			break;
		case ePRESSED_WINDOW_EQUALIZER_CLOSE:
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,EQMAIN_CLOSE_BUTTON_PRESSED);
			break;
		default: break;

	}
	return retval;
}
int window_equalizer_draw_dynamic(tHandleWindowEqualizer *pThis,GdkPixbuf *destBuf)
{
	int i;
	int splineY[113]={0};	// -9..0..9
	int preampY;
	int retval;

	retval=RETVAL_OK;
	if (gtk_window_is_active(GTK_WINDOW(pThis->window)))
	{
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,EQMAIN_EQUALIZER_TITLEBAR_ACTIVE);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,EQMAIN_CLOSE_BUTTON_UNPRESSED);
		
	}


	//preampY=ELEMENT_HEIGHT(EQMAIN_ACTUAL_EQUALIZER_MINIDISPLAY)-1-((pThis->status.bar[0]+100)*(ELEMENT_HEIGHT(EQMAIN_ACTUAL_EQUALIZER_MINIDISPLAY)-1))/200;
	preampY=((pThis->status.bar[0]+100)*(ELEMENT_HEIGHT(EQMAIN_ACTUAL_EQUALIZER_MINIDISPLAY)-1))/200;	// TODO: WHICH IS THE CORRECT ONE?
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,EQMAIN_ACTUAL_EQUALIZER_MINIDISPLAY);
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,EQMAIN_PREAMP_LINE,ELEMENT_DESTX(EQMAIN_ACTUAL_EQUALIZER_MINIDISPLAY),1+ELEMENT_DESTY(EQMAIN_ACTUAL_EQUALIZER_MINIDISPLAY)+preampY);

	// TODO: find a better spline interpolation implementation
#if 1
	{
		int j;
		int x1[124];
		int x2[124];

		for (i=0;i<124;i++)
		{
			x1[i]=0;
		}
		for (i=0;i<10;i++)
		{
			x1[i*12]=pThis->status.bar[i+1];
		}
		for (i=0;i<124;i++)
		{
			x2[i]=0;
			for (j=0;j<12;j++)
			{
				if ((i-j)>=0)
				{
					x2[i]+=(x1[(i-j)]);
				}
			}
			x2[i]/=12;
		}
		for (i=0;i<124;i++)
		{
			x1[i]=0;
			for (j=0;j<12;j++)
			{
				if ((i-j)>=0)
				{
					x1[i]+=(x2[(i-j)]);
				}
			}
			x1[i]/=12;
		}

		for (i=0;i<124;i++)
		{
			x2[i]=0;
			for (j=0;j<12;j++)
			{
				if ((i-j)>=0)
				{
					x2[i]+=(x1[(i-j)]);
				}
			}
			if (i>=11 && i<124) splineY[i-11]=x2[i];
		}
#else
	{
		{
			int slidingwin[12]={0};
			int sum;
			int b;
			int v;
			int p;
			p=10;
			b=1;
			v=pThis->status.bar[b++];
			for (i=0;i<113;i++)
			{
//				splineY[2+i*12]=pThis->status.bar[i+1];
				splineY[i]=v;
				p++;
				if (p==12)
				{
					v=pThis->status.bar[b++];
					p=0;
				}
			}
		}
#endif
		// scale into the window
		for (i=0;i<113;i++)
		{
			splineY[i]=((100-splineY[i])*(ELEMENT_HEIGHT(EQMAIN_SPLINE_LINE)-1))/200;
			if (splineY[i]<0) splineY[i]=0;
			if (splineY[i]>=ELEMENT_HEIGHT(EQMAIN_ACTUAL_EQUALIZER_MINIDISPLAY)) splineY[i]=ELEMENT_HEIGHT(EQMAIN_ACTUAL_EQUALIZER_MINIDISPLAY)-1;
		}
	}

	{
	// the spline line has different colors, depending on the Y-value
		GdkPixbuf *pixbuf;
		pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,1,19);
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pixbuf,EQMAIN_SPLINE_LINE,0,0);
		for (i=0;i<113;i++)
		{
			gdk_pixbuf_copy_area(pixbuf,0,splineY[i],1,1,destBuf,ELEMENT_DESTX(EQMAIN_ACTUAL_EQUALIZER_MINIDISPLAY)+i,1+ELEMENT_DESTY(EQMAIN_ACTUAL_EQUALIZER_MINIDISPLAY)+splineY[i]);
		}
		g_object_unref(pixbuf);
	}
	return retval;

}
int window_equalizer_draw(tHandleWindowEqualizer *pThis,GdkPixbuf *destBuf)
{
	int retval;
	retval=RETVAL_OK;
	gdk_pixbuf_copy_area(pThis->pixbufBackground,0,0,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,destBuf,0,0);
	retval|=window_equalizer_draw_status(pThis,destBuf);
	retval|=window_equalizer_draw_dynamic(pThis,destBuf);
	retval|=window_equalizer_draw_presses(pThis,destBuf);

	return retval;
}
int window_equalizer_refresh(tHandleWindowEqualizer* pThis)
{
	int retval;
	retval=RETVAL_OK;
	retval|=window_equalizer_draw(pThis,pThis->pixbuf);

	gdk_pixbuf_copy_area(pThis->pixbuf,0,0,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HANDLE_HEIGHT,pThis->pixbuf_handle,0,0);
	gdk_pixbuf_copy_area(pThis->pixbuf,0,WINDOW_EQUALIZER_HANDLE_HEIGHT,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT-WINDOW_EQUALIZER_HANDLE_HEIGHT,pThis->pixbuf_main,0,0);

	gtk_picture_set_pixbuf(GTK_PICTURE(pThis->picture_handle),pThis->pixbuf_handle);
	gtk_picture_set_pixbuf(GTK_PICTURE(pThis->picture_main),pThis->pixbuf_main);


	gtk_widget_queue_draw(pThis->window);

	return retval;	
}

int window_equalizer_signal_scalefactor(tHandleWindowEqualizer* pThis,int scale)
{
	int retval;
	retval=RETVAL_OK;
	gtk_window_set_default_size(GTK_WINDOW(pThis->window),scale*WINDOW_EQUALIZER_WIDTH,scale*WINDOW_EQUALIZER_HEIGHT);
	return retval;
}

int window_equalizer_signal_new_theme(tHandleWindowEqualizer* pThis)
{
	int retval;

	retval=RETVAL_OK;
	retval|=window_equalizer_refresh_background(pThis);
	retval|=window_equalizer_refresh(pThis);

	return retval;

}

int window_equalizer_signal_bars(tHandleWindowEqualizer* pThis,int bar,int value)
{
	int retval;
	char key[16];
	snprintf(key,16,"bar%02d",bar);
	config_setint(&(pThis->handleConfig),key,value);

	retval=RETVAL_OK;
	pThis->status.bar[bar]=value;
	retval|=window_equalizer_refresh(pThis);

	return retval;
}
int window_equalizer_signal_onoff(tHandleWindowEqualizer* pThis,int off0on1)
{
	int retval;
	retval=RETVAL_OK;
	pThis->status.equalizer=(off0on1)?eONOFF_ON:eONOFF_OFF;
	config_setonoff(&(pThis->handleConfig),"equalizer",pThis->status.equalizer);
	retval|=window_equalizer_refresh(pThis);

	return retval;
}

int window_equalizer_show(tHandleWindowEqualizer *pThis)
{
	gtk_widget_show(pThis->window);
	return RETVAL_OK;
}

int window_equalizer_hide(tHandleWindowEqualizer *pThis)
{
	gtk_widget_hide(pThis->window);
	return RETVAL_OK;
}

////////////////////////////////////////////////////////
// implementation of the user interaction events follow
////////////////////////////////////////////////////////

static void window_equalizer_event_pressed(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window)
{

	ePressable pressed;
	tHandleWindowEqualizer* pThis=(tHandleWindowEqualizer*)g_object_get_data(G_OBJECT(gesture),"pThis");
	pressed=gui_helpers_find_pressable(pThis->boundingBoxes,PRESSABLE_EQUALIZER_NUM,x,y,window,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT);
	pThis->lastPressed=pressed;
	pThis->pressedX=x;
	pThis->pressedY=y;
	window_equalizer_refresh(pThis);     
}
static void window_equalizer_event_released(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window)
{
	tPayload payload;
	ePressable released;
	int value;
	int i;
	tHandleWindowEqualizer* pThis=(tHandleWindowEqualizer*)g_object_get_data(G_OBJECT(gesture),"pThis");
	released=gui_helpers_find_pressable(pThis->boundingBoxes,PRESSABLE_EQUALIZER_NUM,x,y,window,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT);

	value=-100;
	switch(released)
	{
		case ePRESSED_WINDOW_EQUALIZER_ONOFF:
			payload.off0on1=(pThis->status.equalizer==eONOFF_OFF);
			controller_event(pThis->pControllerContext,eEVENT_EQUALIZER_ONOFF,&payload);
			break;
		case ePRESSED_WINDOW_EQUALIZER_20DB_RESET:
			value=0;
		case ePRESSED_WINDOW_EQUALIZER_0DB_RESET:
			value+=100;
		case ePRESSED_WINDOW_EQUALIZER_M20DB_RESET:
			for (i=1;i<11;i++)
			{
				tPayload payload;
				payload.equalizer.bar=i;
				payload.equalizer.value=value;
				controller_event(pThis->pControllerContext,eEVENT_SET_EQUALIZER,&payload);
			}
			break;
		case ePRESSED_WINDOW_EQUALIZER_CLOSE:
			window_equalizer_close(pThis->window,(gpointer)pThis);
			break;
		default:
			break;
	}


	pThis->lastPressed=ePRESSED_NONE;
	window_equalizer_refresh(pThis);

}
static void window_equalizer_event_drag_begin(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
}
static void window_equalizer_event_drag_update(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
	int i;
	int foundbar;
	ePressable pressableIDs[BAR_NUM]={
		ePRESSED_WINDOW_EQUALIZER_PREAMP,
		ePRESSED_WINDOW_EQUALIZER_60HZ,
		ePRESSED_WINDOW_EQUALIZER_170HZ,
		ePRESSED_WINDOW_EQUALIZER_310HZ,
		ePRESSED_WINDOW_EQUALIZER_600HZ,
		ePRESSED_WINDOW_EQUALIZER_1KHZ,
		ePRESSED_WINDOW_EQUALIZER_3KHZ,
		ePRESSED_WINDOW_EQUALIZER_6KHZ,
		ePRESSED_WINDOW_EQUALIZER_12KHZ,
		ePRESSED_WINDOW_EQUALIZER_14KHZ,
		ePRESSED_WINDOW_EQUALIZER_16KHZ
	};

	tHandleWindowEqualizer* pThis=(tHandleWindowEqualizer*)g_object_get_data(G_OBJECT(gesture),"pThis");
	foundbar=-1;
	for (i=0;i<BAR_NUM && foundbar==-1;i++)
	{
		if (pThis->lastPressed==pressableIDs[i])
		{
			foundbar=i;
		}
	}
	if (foundbar!=-1)
	{
		int value;
		tPayload payload;
		value=-gui_helpers_relative_value(-100,100,ELEMENT_DESTY(EQMAIN_PREAMP_BAR),ELEMENT_DESTY2(EQMAIN_PREAMP_BAR),1,-1,pThis->pressedY+y,window,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT);
		
		payload.equalizer.bar=foundbar;
		payload.equalizer.value=value;
		controller_event(pThis->pControllerContext,eEVENT_SET_EQUALIZER,&payload);
	}
}
static void window_equalizer_event_drag_end(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
}

static gboolean window_equalizer_close(GtkWidget *widget,gpointer user_data)
{
	tHandleWindowEqualizer* pThis=(tHandleWindowEqualizer*)user_data;
	tPayload payload;
	payload.hide0show1=0;
	controller_event(pThis->pControllerContext,eEVENT_WINDOW_EQUALIZER,&payload);
	
	return TRUE;
}
	
