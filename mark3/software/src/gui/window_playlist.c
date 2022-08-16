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
#include "datastructures.h"
#include "window_playlist.h"

#define	INACTIVE	0
#define	ACTIVE		1


int window_playlist_draw(tHandleWindowPlaylist* pThis,GdkPixbuf *pixbufDestination)
{
	int i;
	int x,x0;
	int y,y0,y1;
	x=0;
	y=0;

	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,(pThis->statusActive==ACTIVE?PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE:PLEDIT_UPPER_LEFT_CORNERPIECE_INACTIVE));x+=ELEMENT_WIDTH(PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE);

	while (x<WINDOW_PLAYLIST_WIDTH-ELEMENT_WIDTH(PLEDIT_UPPER_RIGHT_CORNERPIECE_ACTIVE))
	{
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,(pThis->statusActive==ACTIVE?PLEDIT_TOP_FILLERS_ACTIVE:PLEDIT_TOP_FILLERS_INACTIVE));x+=ELEMENT_WIDTH(PLEDIT_TOP_FILLERS_ACTIVE);
	}
	// TODO: this is left aligned. center it.

	x=WINDOW_PLAYLIST_WIDTH/2-ELEMENT_WIDTH(PLEDIT_PLAYLIST_TITLEBAR_INACTIVE)/2;
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,(pThis->statusActive==ACTIVE?PLEDIT_PLAYLIST_TITLEBAR_ACTIVE:PLEDIT_PLAYLIST_TITLEBAR_INACTIVE));
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,WINDOW_PLAYLIST_WIDTH-ELEMENT_WIDTH(PLEDIT_UPPER_RIGHT_CORNERPIECE_ACTIVE),y,(pThis->statusActive==ACTIVE?PLEDIT_UPPER_RIGHT_CORNERPIECE_ACTIVE:PLEDIT_UPPER_RIGHT_CORNERPIECE_INACTIVE));
	

	y0=y=ELEMENT_HEIGHT(PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE);
	x0=WINDOW_PLAYLIST_WIDTH-ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_LEFT_BAR)-ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_SCROLL_GROOVE)-ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_RIGHT_BAR);
	while (y<WINDOW_PLAYLIST_HEIGHT-ELEMENT_HEIGHT(PLEDIT_BOTTOM_FILLERS))
	{
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,0,y,PLEDIT_LEFT_SIDE_FILLERS);
		x=x0;
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_RIGHT_SIDE_FILLERS_LEFT_BAR);
		x+=ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_LEFT_BAR);	
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_RIGHT_SIDE_FILLERS_SCROLL_GROOVE);
		x+=ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_SCROLL_GROOVE);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_RIGHT_SIDE_FILLERS_RIGHT_BAR);
		y+=ELEMENT_HEIGHT(PLEDIT_LEFT_SIDE_FILLERS);	
	}

	y1=y=WINDOW_PLAYLIST_HEIGHT-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR);
	if (pThis->scrollPos<0) 
	{
		pThis->scrollPos=0;
	}
	if (pThis->scrollPos>pThis->scrollLen)
	{
		pThis->scrollPos=pThis->scrollLen;
	}
	if (pThis->scrollLen)
	{
		y=((y0-y1)*pThis->scrollPos)/pThis->scrollLen;
	} else {
		y=y0;
	}
	if (y<0) y=0;
	if (y>y1-ELEMENT_HEIGHT(PLEDIT_SCROLL_BUTTON_PRESSED)) y=y1-ELEMENT_HEIGHT(PLEDIT_SCROLL_BUTTON_PRESSED);
	x=WINDOW_PLAYLIST_WIDTH-ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_RIGHT_BAR)-ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_SCROLL_GROOVE);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_SCROLL_BUTTON_PRESSED);
		

	y=y1;
	x=0;
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,0,y,PLEDIT_BOTTOM_LEFT_CONTROL_BAR);
	x+=ELEMENT_WIDTH(PLEDIT_BOTTOM_LEFT_CONTROL_BAR);
	while (x<WINDOW_PLAYLIST_WIDTH-ELEMENT_WIDTH(PLEDIT_BOTTOM_RIGHT_CONTROL_BAR))
	{
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_BOTTOM_FILLERS);
		x+=ELEMENT_WIDTH(PLEDIT_BOTTOM_FILLERS);
	}
	x=WINDOW_PLAYLIST_WIDTH-ELEMENT_WIDTH(PLEDIT_BOTTOM_RIGHT_CONTROL_BAR);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_BOTTOM_RIGHT_CONTROL_BAR);


	
	
	return RETVAL_OK;
}

int window_playlist_event_pressed(GtkWidget *widget, double x,double y,guint event_button, gpointer data)
{
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)data;
	int width,height;
        width=gtk_widget_get_width(pThis->windowPlaylist);
        height=gtk_widget_get_height(pThis->windowPlaylist);
	return TRUE;	
}

int window_playlist_event_released(GtkWidget *widget, double x,double y,guint event_button, gpointer data)
{
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)data;
	int width,height;
        width=gtk_widget_get_width(pThis->windowPlaylist);
        height=gtk_widget_get_height(pThis->windowPlaylist);
	return TRUE;	
}

int window_playlist_init(GtkApplication* app,tHandleWindowPlaylist* pThis,tHandleThemeManager *pHandleThemeManager)
{
	memset(pThis,0,sizeof(tHandleWindowPlaylist));
	pThis->pHandleThemeManager=pHandleThemeManager;

	pThis->scaleFactor=4;
	pThis->scrollPos=0;
	pThis->scrollLen=0;
	pThis->pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_PLAYLIST_WIDTH,WINDOW_PLAYLIST_HEIGHT);
	window_playlist_draw(pThis,pThis->pixbuf);
	pThis->picture=gtk_picture_new_for_pixbuf(pThis->pixbuf);
	pThis->windowPlaylist=gtk_application_window_new(app);
	gtk_window_set_default_size(GTK_WINDOW(pThis->windowPlaylist),WINDOW_PLAYLIST_WIDTH*pThis->scaleFactor,WINDOW_PLAYLIST_HEIGHT*pThis->scaleFactor);
	gtk_window_set_child(GTK_WINDOW(pThis->windowPlaylist),pThis->picture);
	gtk_window_set_title(GTK_WINDOW(pThis->windowPlaylist),"d11amp playlist");

	pThis->gesture=gtk_gesture_click_new();
	g_signal_connect(pThis->gesture,"pressed",G_CALLBACK(window_playlist_event_pressed),&(pThis->windowPlaylist));
	g_signal_connect(pThis->gesture,"released",G_CALLBACK(window_playlist_event_released),&(pThis->windowPlaylist));
	gtk_widget_add_controller(pThis->windowPlaylist,GTK_EVENT_CONTROLLER(pThis->gesture));


	return RETVAL_OK;
}
int window_playlist_show(tHandleWindowPlaylist* pThis)
{
	gtk_widget_show(pThis->windowPlaylist);
	return RETVAL_OK;
}
int window_playlist_hide(tHandleWindowPlaylist* pThis)
{
	gtk_widget_hide(pThis->windowPlaylist);
	return RETVAL_OK;
}

