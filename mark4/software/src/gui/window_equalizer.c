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
#include "window_equalizer.h"
#include <string.h>

// function headers for the gtk events. the implementation is at the end of this file
static void window_equalizer_event_pressed(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window);
static void window_equalizer_event_released(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window);
static void window_equalizer_event_drag_begin(GtkGestureDrag *gesture, double x, double y, GtkWidget *window);
static void window_equalizer_event_drag_update(GtkGestureDrag *gesture, double x, double y, GtkWidget *window);
static void window_equalizer_event_drag_end(GtkGestureDrag *gesture, double x, double y, GtkWidget *window);


int window_equalizer_init(tHandleWindowEqualizer* pThis,void* pControllerContext,tHandleThemeManager *pHandleThemeManager,GtkApplication* app)
{
	int retval;
	retval=RETVAL_OK;
		
	memset(pThis,0,sizeof(tHandleWindowEqualizer));
	pThis->app=app;
	pThis->pControllerContext=pControllerContext;
	pThis->pHandleThemeManager=pHandleThemeManager;
	
	pThis->pixbufBackground=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT);
	pThis->pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT);
	pThis->picture=gtk_picture_new_for_pixbuf(pThis->pixbuf);
	pThis->window=gtk_application_window_new(pThis->app);
	gtk_window_set_default_size(GTK_WINDOW(pThis->window),WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT);

	gtk_window_set_child(GTK_WINDOW(pThis->window),pThis->picture);
	gtk_window_set_title(GTK_WINDOW(pThis->window),"d11amp equalizer");

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
	

	return retval;
}

// background: the default picture
int window_equalizer_refresh_background(tHandleWindowEqualizer* pThis)
{
	int retval;

	retval=RETVAL_OK;
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
	
	
	return retval;	
}
	
// status: what changes due to user interaction
int window_equalizer_draw_status(tHandleWindowEqualizer* pThis,GdkPixbuf *destBuf)
{
	return RETVAL_OK;
}
int window_equalizer_draw_presses(tHandleWindowEqualizer* pThis,GdkPixbuf *destBuf)
{
	return RETVAL_OK;
}
int window_equalizer_draw(tHandleWindowEqualizer *pThis,GdkPixbuf *destBuf)
{
	int retval;
	retval=RETVAL_OK;
	gdk_pixbuf_copy_area(pThis->pixbufBackground,0,0,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,destBuf,0,0);
	retval|=window_equalizer_draw_status(pThis,destBuf);
	retval|=window_equalizer_draw_presses(pThis,destBuf);

	return retval;
}
int window_equalizer_refresh(tHandleWindowEqualizer* pThis)
{
	int retval;
	retval=RETVAL_OK;
	retval|=window_equalizer_draw(pThis,pThis->pixbuf);
        gtk_picture_set_pixbuf(GTK_PICTURE(pThis->picture),pThis->pixbuf);
	gtk_widget_queue_draw(pThis->window);

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
}
static void window_equalizer_event_released(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window)
{
}
static void window_equalizer_event_drag_begin(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
}
static void window_equalizer_event_drag_update(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
}
static void window_equalizer_event_drag_end(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
}


