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

#include "window_playlist.h"
#include "theme_manager.h"
#include "datastructures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	MIN_PLAYLIST_ROWS	2	// each row has a height of 29 pixels. titlebar has 20, bottom 38. 30+38+2*9=116 pixel (size of the other windows)
#define	MIN_PLAYLIST_COLS	11	// each column has a width of 25 pixel. 11*25=275 pixel. (width of the other windows)

#define	COL_WIDTH 		ELEMENT_WIDTH(PLEDIT_TOP_FILLERS_ACTIVE)
#define	COL_MARGIN_LEFT 	ELEMENT_WIDTH(PLEDIT_LEFT_SIDE_FILLERS)
#define	COL_MARGIN_RIGHT 	(ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_LEFT_BAR)+ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_SCROLL_GROOVE)+ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_RIGHT_BAR))
#define	ROW_HEIGHT 		ELEMENT_HEIGHT(PLEDIT_LEFT_SIDE_FILLERS)
#define	ROW_MARGIN_TOP 		ELEMENT_HEIGHT(PLEDIT_PLAYLIST_TITLEBAR_ACTIVE)
#define ROW_MARGIN_BOTTOM 	ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)



// function headers for the gtk events. the implementation is at the end of this file
static void window_playlist_event_pressed(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window);
static void window_playlist_event_released(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window);
static void window_playlist_event_drag_begin(GtkGestureDrag *gesture, double x, double y, GtkWidget *window);
static void window_playlist_event_drag_update(GtkGestureDrag *gesture, double x, double y, GtkWidget *window);
static void window_playlist_event_drag_end(GtkGestureDrag *gesture, double x, double y, GtkWidget *window);

int window_playlist_resize(tHandleWindowPlaylist* pThis,int rows,int columns)
{
	int retval;
	int winheight;
	int winwidth;
	GdkPixbuf *newPixbufBackground;
	GdkPixbuf *newPixbuf;
	GtkWidget *newPicture;
	
	GdkPixbuf *ptr1;
	GdkPixbuf *ptr2;
	GtkWidget *ptr3;

	retval=RETVAL_OK;

	if (rows<MIN_PLAYLIST_ROWS)
	{
		rows=MIN_PLAYLIST_ROWS;
	}
	if (columns<MIN_PLAYLIST_COLS)
	{
		columns=MIN_PLAYLIST_COLS;
	}

	pThis->window_height=winheight=ELEMENT_HEIGHT(PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE)+rows*ELEMENT_HEIGHT(PLEDIT_LEFT_SIDE_FILLERS)+ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR);
	pThis->window_width=winwidth=columns*ELEMENT_WIDTH(PLEDIT_TOP_FILLERS_ACTIVE);

	newPixbufBackground=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,winwidth,winheight);
	newPixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,winwidth,winheight);
	newPicture=gtk_picture_new_for_pixbuf(newPixbuf);

	ptr3=pThis->picture;
	pThis->picture=newPicture;
	gtk_window_set_child(GTK_WINDOW(pThis->window),pThis->picture);
	gtk_window_set_default_size(GTK_WINDOW(pThis->window),winwidth,winheight);


	ptr2=pThis->pixbuf;
	pThis->pixbuf=newPixbuf;
	ptr1=pThis->pixbufBackground;
	pThis->pixbufBackground=newPixbufBackground;

	// at this point, the new references have been set.
	// the old ones are still "dangling".
	if (ptr3!=NULL)
	{
		g_object_unref(ptr3);
	}
	if (ptr2!=NULL)
	{
		g_object_unref(ptr2);
	}
	if (ptr1!=NULL)
	{
		g_object_unref(ptr1);
	}
	// not anymore. all cleaned up!


	return retval;
}

int window_playlist_init(tHandleWindowPlaylist* pThis,void* pControllerContext,tHandleThemeManager *pHandleThemeManager,GtkApplication* app)
{
	int retval;
	retval=RETVAL_OK;
		
	memset(pThis,0,sizeof(tHandleWindowPlaylist));
	pThis->app=app;
	pThis->pControllerContext=pControllerContext;
	pThis->pHandleThemeManager=pHandleThemeManager;
	pThis->window=gtk_application_window_new(pThis->app);
	gtk_window_set_title(GTK_WINDOW(pThis->window),"d11amp playlist");

	pThis->pixbuf=NULL;
	pThis->picture=NULL;
	retval|=window_playlist_resize(pThis,MIN_PLAYLIST_ROWS,MIN_PLAYLIST_COLS);

	pThis->lastPressed=ePRESSED_NONE;
	pThis->gesture_click=gtk_gesture_click_new();
	g_object_set_data(G_OBJECT(pThis->gesture_click),"pThis",pThis);	// add a pointer to the handle to the widget. this way it is available in the gesture callbacks
	g_signal_connect(pThis->gesture_click,"pressed", G_CALLBACK(window_playlist_event_pressed) ,(pThis->window));
	g_signal_connect(pThis->gesture_click,"released",G_CALLBACK(window_playlist_event_released),(pThis->window));
	gtk_widget_add_controller(pThis->window,GTK_EVENT_CONTROLLER(pThis->gesture_click));

	pThis->gesture_drag=gtk_gesture_drag_new();
	g_object_set_data(G_OBJECT(pThis->gesture_drag),"pThis",pThis);	// add a pointer to the handle to the widget. this way it is available in the gesture callbacks
	g_signal_connect (pThis->gesture_drag,"drag-begin", G_CALLBACK (window_playlist_event_drag_begin), (pThis->window));
	g_signal_connect (pThis->gesture_drag,"drag-update",G_CALLBACK (window_playlist_event_drag_update),(pThis->window));
	g_signal_connect (pThis->gesture_drag,"drag-end",   G_CALLBACK (window_playlist_event_drag_end),   (pThis->window));

	gtk_widget_add_controller(pThis->window,GTK_EVENT_CONTROLLER(pThis->gesture_drag));

	return retval;
}

// background: the default picture
int window_playlist_refresh_background(tHandleWindowPlaylist* pThis)
{
	int i;
	int retval;
	int winwidth;
	int winheight;
	int x;
	int y;

	winheight=ROW_MARGIN_TOP+pThis->rows*ROW_HEIGHT+ROW_MARGIN_BOTTOM;
	winwidth=pThis->columns*COL_WIDTH;

	


	retval=RETVAL_OK;

	// first: draw the top frame         ------------------------
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE,0,0);
	for (i=1;i<pThis->columns-1;i++)
	{
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_TOP_FILLERS_ACTIVE,i*COL_WIDTH,0);
	}
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_UPPER_RIGHT_CORNERPIECE_ACTIVE,winwidth-ELEMENT_WIDTH(PLEDIT_UPPER_RIGHT_CORNERPIECE_ACTIVE),0);
	// then: add the title in the center ---------[title]--------
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_PLAYLIST_TITLEBAR_ACTIVE,winwidth/2-ELEMENT_WIDTH(PLEDIT_PLAYLIST_TITLEBAR_ACTIVE)/2,0);

	// add the borders on the left and on the right side |              |||	
	for (i=0;i<pThis->rows;i++)
	{
		y=ROW_MARGIN_TOP+i*ROW_HEIGHT;
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_LEFT_SIDE_FILLERS,0,y);
	
		x=winwidth-COL_MARGIN_RIGHT;	
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_RIGHT_SIDE_FILLERS_LEFT_BAR,x,y);x+=ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_LEFT_BAR);
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_RIGHT_SIDE_FILLERS_SCROLL_GROOVE,x,y);x+=ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_SCROLL_GROOVE);
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_RIGHT_SIDE_FILLERS_RIGHT_BAR,x,y);x+=ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_RIGHT_BAR);
	}	
	// add the bottom menu
	x=0;
	y=winheight-ROW_MARGIN_BOTTOM;
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_BOTTOM_LEFT_CONTROL_BAR,x,y);x+=ELEMENT_WIDTH(PLEDIT_BOTTOM_LEFT_CONTROL_BAR);
	while (x<(winwidth-ELEMENT_WIDTH(PLEDIT_BOTTOM_RIGHT_CONTROL_BAR)))	// when the window is at its minimum width, the bottom left controlbar + bottom right controlbar are as wide as the window. otherwise, it might need some filling
	{
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_BOTTOM_FILLERS,x,y);x+=ELEMENT_WIDTH(PLEDIT_BOTTOM_FILLERS);
	}
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_BOTTOM_RIGHT_CONTROL_BAR,x,y);

	return retval;
}

int window_playlist_draw(tHandleWindowPlaylist *pThis,GdkPixbuf *destBuf)
{
	int retval;
	retval=RETVAL_OK;
	gdk_pixbuf_copy_area(pThis->pixbufBackground,0,0,pThis->window_width,pThis->window_height,destBuf,0,0);
//	retval|=window_playlist_draw_status(pThis,destBuf);

	return retval;
}


int window_playlist_refresh(tHandleWindowPlaylist* pThis)
{
	int retval;
	retval=RETVAL_OK;
	retval|=window_playlist_draw(pThis,pThis->pixbuf);
        gtk_picture_set_pixbuf(GTK_PICTURE(pThis->picture),pThis->pixbuf);
	gtk_widget_queue_draw(pThis->window);

	return retval;	
}
int window_playlist_signal_new_theme(tHandleWindowPlaylist* pThis)
{
	int retval;

	retval=RETVAL_OK;
	retval|=window_playlist_refresh_background(pThis);
	retval|=window_playlist_refresh(pThis);

	return retval;
}
int window_playlist_show(tHandleWindowPlaylist* pThis)
{
	gtk_widget_show(pThis->window);
	return RETVAL_OK;
}
int window_playlist_hide(tHandleWindowPlaylist* pThis)
{
	gtk_widget_hide(pThis->window);
	return RETVAL_OK;
}






/// event handler implementation
static void window_playlist_event_pressed(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window)
{
}
static void window_playlist_event_released(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window)
{
}
static void window_playlist_event_drag_begin(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
}
static void window_playlist_event_drag_update(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
}
static void window_playlist_event_drag_end(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
}


