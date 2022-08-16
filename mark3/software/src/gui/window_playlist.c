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

#define	MENU_CLOSE	0
#define	MENU_OPEN	1

#define	DEFINE_PRESSABLE(epressable,eelementid,posX,posY)	\
	pThis->windowPlaylist_pressable[(epressable)].pressed=(epressable);	\
	pThis->windowPlaylist_pressable[(epressable)].posx=(posX);	\
	pThis->windowPlaylist_pressable[(epressable)].posy=(posY);	\
	pThis->windowPlaylist_pressable[(epressable)].dimx=ELEMENT_WIDTH(eelementid);	\
	pThis->windowPlaylist_pressable[(epressable)].dimy=ELEMENT_HEIGHT(eelementid);
	
#define	UNDEFINE_PRESSABLE(epressable)	\
	pThis->windowPlaylist_pressable[(epressable)].pressed=(epressable);	\
	pThis->windowPlaylist_pressable[(epressable)].posx=-1;	\
	pThis->windowPlaylist_pressable[(epressable)].posy=-1;	\
	pThis->windowPlaylist_pressable[(epressable)].dimx=0;	\
	pThis->windowPlaylist_pressable[(epressable)].dimy=0;	
	

int window_playlist_draw(tHandleWindowPlaylist* pThis,GdkPixbuf *pixbufDestination)
{
	int x,x0;
	int y,y0,y1;
	x=0;
	y=0;

	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,(pThis->statusActive==ACTIVE?PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE:PLEDIT_UPPER_LEFT_CORNERPIECE_INACTIVE));x+=ELEMENT_WIDTH(PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE);

	while (x<WINDOW_PLAYLIST_WIDTH-ELEMENT_WIDTH(PLEDIT_UPPER_RIGHT_CORNERPIECE_ACTIVE))
	{
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,(pThis->statusActive==ACTIVE?PLEDIT_TOP_FILLERS_ACTIVE:PLEDIT_TOP_FILLERS_INACTIVE));x+=ELEMENT_WIDTH(PLEDIT_TOP_FILLERS_ACTIVE);
	}

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

// the add menu
	y=WINDOW_PLAYLIST_HEIGHT-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+7;
	x=11;
	if (pThis->statusMenuAdd==MENU_OPEN)
	{
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y-ELEMENT_HEIGHT(PLEDIT_DECORATION_BAR_ADD),PLEDIT_DECORATION_BAR_ADD);
		y-=ELEMENT_HEIGHT(PLEDIT_ADD_BUTTON);
		x+=ELEMENT_WIDTH(PLEDIT_DECORATION_BAR_ADD);

		
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,(pThis->lastPressed==PLAYLIST_PRESSED_ADD_FILE)?PLEDIT_ADD_FILE_BUTTON_PRESSED:PLEDIT_ADD_FILE_BUTTON_UNPRESSED);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_ADD_FILE,PLEDIT_ADD_FILE_BUTTON_PRESSED,x,y);
		y-=ELEMENT_HEIGHT(PLEDIT_ADD_FILE_BUTTON_PRESSED);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,(pThis->lastPressed==PLAYLIST_PRESSED_ADD_DIR)?PLEDIT_ADD_DIR_BUTTON_PRESSED:PLEDIT_ADD_DIR_BUTTON_UNPRESSED);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_ADD_DIR,PLEDIT_ADD_DIR_BUTTON_PRESSED,x,y);
		y-=ELEMENT_HEIGHT(PLEDIT_ADD_DIR_BUTTON_PRESSED);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,(pThis->lastPressed==PLAYLIST_PRESSED_ADD_URL)?PLEDIT_ADD_URL_BUTTON_PRESSED:PLEDIT_ADD_URL_BUTTON_UNPRESSED);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_ADD_URL,PLEDIT_ADD_URL_BUTTON_PRESSED,x,y);
		y-=ELEMENT_HEIGHT(PLEDIT_ADD_URL_BUTTON_PRESSED);
	} else {
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_ADD_FILE);
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_ADD_DIR);
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_ADD_URL);
	}
	
	y=WINDOW_PLAYLIST_HEIGHT-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+8;
	x=14;
	DEFINE_PRESSABLE(PLAYLIST_PRESSED_ADD_BUTTON,PLEDIT_ADD_BUTTON,x,y);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_ADD_BUTTON);
	


	y=WINDOW_PLAYLIST_HEIGHT-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+7;
	x=40;
	if (pThis->statusMenuRem==MENU_OPEN)
	{
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y-ELEMENT_HEIGHT(PLEDIT_DECORATION_BAR_REMOVE),PLEDIT_DECORATION_BAR_REMOVE);
		y-=ELEMENT_HEIGHT(PLEDIT_REMOVE_BUTTON);
		x+=ELEMENT_WIDTH(PLEDIT_DECORATION_BAR_REMOVE);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_REMOVE_MISC,PLEDIT_MISC_REMOVE_BUTTON_PRESSED,x,y);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,pThis->lastPressed==PLAYLIST_PRESSED_REMOVE_MISC?PLEDIT_MISC_REMOVE_BUTTON_PRESSED:PLEDIT_MISC_REMOVE_BUTTON_UNPRESSED);
		y-=ELEMENT_HEIGHT(PLEDIT_MISC_REMOVE_BUTTON_PRESSED);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_REMOVE_FILE,PLEDIT_REMOVE_FILE_BUTTON_PRESSED,x,y);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,pThis->lastPressed==PLAYLIST_PRESSED_REMOVE_FILE?PLEDIT_REMOVE_FILE_BUTTON_PRESSED:PLEDIT_REMOVE_FILE_BUTTON_UNPRESSED);
		y-=ELEMENT_HEIGHT(PLEDIT_REMOVE_FILE_BUTTON_PRESSED);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_REMOVE_CROP,PLEDIT_CROP_BUTTON_PRESSED,x,y);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,pThis->lastPressed==PLAYLIST_PRESSED_REMOVE_CROP?PLEDIT_CROP_BUTTON_PRESSED:PLEDIT_CROP_BUTTON_UNPRESSED);
		y-=ELEMENT_HEIGHT(PLEDIT_CROP_BUTTON_PRESSED);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_REMOVE_ALL,PLEDIT_REMOVE_ALL_BUTTON_PRESSED,x,y);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,pThis->lastPressed==PLAYLIST_PRESSED_REMOVE_ALL?PLEDIT_REMOVE_ALL_BUTTON_PRESSED:PLEDIT_REMOVE_ALL_BUTTON_UNPRESSED);
		y-=ELEMENT_HEIGHT(PLEDIT_REMOVE_ALL_BUTTON_PRESSED);
	} else {
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_REMOVE_MISC);
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_REMOVE_FILE);
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_REMOVE_CROP);
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_REMOVE_ALL);
	}
	y=WINDOW_PLAYLIST_HEIGHT-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+8;
	x=43;
	DEFINE_PRESSABLE(PLAYLIST_PRESSED_REMOVE_BUTTON,PLEDIT_REMOVE_BUTTON,x,y);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_REMOVE_BUTTON);
	

	y=WINDOW_PLAYLIST_HEIGHT-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+7;
	x=69;
	if (pThis->statusMenuSel==MENU_OPEN)
	{
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y-ELEMENT_HEIGHT(PLEDIT_DECORATION_BAR_SELECTION),PLEDIT_DECORATION_BAR_SELECTION);
		y-=ELEMENT_HEIGHT(PLEDIT_SELECTION_BUTTON);
		x+=ELEMENT_WIDTH(PLEDIT_DECORATION_BAR_SELECTION);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_SELECT_ALL,PLEDIT_SELECT_ALL_BUTTON_PRESSED ,x,y);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,pThis->lastPressed==PLAYLIST_PRESSED_SELECT_ALL?PLEDIT_SELECT_ALL_BUTTON_PRESSED:PLEDIT_SELECT_ALL_BUTTON_UNPRESSED);
		y-=ELEMENT_HEIGHT(PLEDIT_SELECT_ALL_BUTTON_PRESSED);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_SELECT_NONE,PLEDIT_SELECT_NONE_BUTTON_PRESSED ,x,y);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,pThis->lastPressed==PLAYLIST_PRESSED_SELECT_NONE?PLEDIT_SELECT_NONE_BUTTON_PRESSED:PLEDIT_SELECT_NONE_BUTTON_UNPRESSED);
		y-=ELEMENT_HEIGHT(PLEDIT_SELECT_NONE_BUTTON_PRESSED);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_SELECT_INVERT,PLEDIT_INVERT_SELECTION_BUTTON_PRESSED ,x,y);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,pThis->lastPressed==PLAYLIST_PRESSED_SELECT_INVERT?PLEDIT_INVERT_SELECTION_BUTTON_PRESSED:PLEDIT_INVERT_SELECTION_BUTTON_UNPRESSED);
		y-=ELEMENT_HEIGHT(PLEDIT_INVERT_SELECTION_BUTTON_PRESSED);
	} else {
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_SELECT_ALL);
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_SELECT_NONE);
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_SELECT_INVERT);
		
	}
	y=WINDOW_PLAYLIST_HEIGHT-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+8;
	x=72;
	DEFINE_PRESSABLE(PLAYLIST_PRESSED_SELECTION_BUTTON,PLEDIT_SELECTION_BUTTON,x,y);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_SELECTION_BUTTON);
	



	y=WINDOW_PLAYLIST_HEIGHT-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+7;
	x=98;
	if (pThis->statusMenuMisc==MENU_OPEN)
	{
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y-ELEMENT_HEIGHT(PLEDIT_DECORATION_BAR_MISC),PLEDIT_DECORATION_BAR_MISC);
		y-=ELEMENT_HEIGHT(PLEDIT_MISCELLANEOUS_BUTTON);
		x+=ELEMENT_WIDTH(PLEDIT_DECORATION_BAR_MISC);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_MISC_OPTIONS,PLEDIT_MISC_OPTIONS_BUTTON_PRESSED,x,y);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,pThis->lastPressed==PLAYLIST_PRESSED_MISC_OPTIONS?PLEDIT_MISC_OPTIONS_BUTTON_PRESSED:PLEDIT_MISC_OPTIONS_BUTTON_UNPRESSED);
		y-=ELEMENT_HEIGHT(PLEDIT_MISC_OPTIONS_BUTTON_PRESSED);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_MISC_INFO,PLEDIT_FILE_INFO_BUTTON_PRESSED,x,y);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,pThis->lastPressed==PLAYLIST_PRESSED_MISC_INFO?PLEDIT_FILE_INFO_BUTTON_PRESSED:PLEDIT_FILE_INFO_BUTTON_UNPRESSED);
		y-=ELEMENT_HEIGHT(PLEDIT_FILE_INFO_BUTTON_PRESSED);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_MISC_SORT,PLEDIT_SORT_LIST_BUTTON_PRESSED,x,y);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,pThis->lastPressed==PLAYLIST_PRESSED_MISC_SORT?PLEDIT_SORT_LIST_BUTTON_PRESSED:PLEDIT_SORT_LIST_BUTTON_UNPRESSED);
		y-=ELEMENT_HEIGHT(PLEDIT_SORT_LIST_BUTTON_PRESSED);
	} else {
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_MISC_OPTIONS);
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_MISC_INFO);
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_MISC_SORT);

	}
	y=WINDOW_PLAYLIST_HEIGHT-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+8;
	x=101;
	DEFINE_PRESSABLE(PLAYLIST_PRESSED_MISCELLANEOUS_BUTTON,PLEDIT_MISCELLANEOUS_BUTTON,x,y);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_MISCELLANEOUS_BUTTON);


	y=WINDOW_PLAYLIST_HEIGHT-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+7;
	x=WINDOW_PLAYLIST_WIDTH-29-18;
	if (pThis->statusMenuList==MENU_OPEN)
	{
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y-ELEMENT_HEIGHT(PLEDIT_DECORATION_BAR_LIST),PLEDIT_DECORATION_BAR_LIST);
		y-=ELEMENT_HEIGHT(PLEDIT_LIST_BUTTON);
		x+=ELEMENT_WIDTH(PLEDIT_DECORATION_BAR_LIST);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_LIST_LOAD,PLEDIT_LOAD_LIST_BUTTON_PRESSED,x,y);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,pThis->lastPressed==PLAYLIST_PRESSED_LIST_LOAD?PLEDIT_LOAD_LIST_BUTTON_PRESSED:PLEDIT_LOAD_LIST_BUTTON_UNPRESSED);
		y-=ELEMENT_HEIGHT(PLEDIT_LOAD_LIST_BUTTON_PRESSED);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_LIST_SAVE,PLEDIT_SAVE_LIST_BUTTON_PRESSED,x,y);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,pThis->lastPressed==PLAYLIST_PRESSED_LIST_SAVE?PLEDIT_SAVE_LIST_BUTTON_PRESSED:PLEDIT_SAVE_LIST_BUTTON_UNPRESSED);
		y-=ELEMENT_HEIGHT(PLEDIT_SAVE_LIST_BUTTON_PRESSED);
		DEFINE_PRESSABLE(PLAYLIST_PRESSED_LIST_NEW,PLEDIT_NEW_LIST_BUTTON_PRESSED,x,y);
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,pThis->lastPressed==PLAYLIST_PRESSED_LIST_NEW?PLEDIT_NEW_LIST_BUTTON_PRESSED:PLEDIT_NEW_LIST_BUTTON_UNPRESSED);
		y-=ELEMENT_HEIGHT(PLEDIT_NEW_LIST_BUTTON_PRESSED);
	} else {
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_LIST_LOAD);
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_LIST_SAVE);
		UNDEFINE_PRESSABLE(PLAYLIST_PRESSED_LIST_NEW);
	}
	y=WINDOW_PLAYLIST_HEIGHT-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+8;
	x=WINDOW_PLAYLIST_WIDTH-29-18+3;
	DEFINE_PRESSABLE(PLAYLIST_PRESSED_LIST_BUTTON,PLEDIT_LIST_BUTTON,x,y);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_LIST_BUTTON);

	return RETVAL_OK;
}

int window_playlist_refresh(tHandleWindowPlaylist* pThis)
{
	GdkPixbuf *pixbuf;
	pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,WINDOW_PLAYLIST_WIDTH,WINDOW_PLAYLIST_HEIGHT);
	window_playlist_draw(pThis,pixbuf);
	pthread_mutex_lock(&pThis->mutex);
	gdk_pixbuf_copy_area(pixbuf,0,0,WINDOW_PLAYLIST_WIDTH,WINDOW_PLAYLIST_HEIGHT,pThis->pixbuf,0,0);
	g_object_unref(pixbuf);	
	gtk_picture_set_pixbuf(GTK_PICTURE(pThis->picture),pThis->pixbuf);
	{
		
		gtk_widget_queue_draw(pThis->windowPlaylist);
	}
	pthread_mutex_unlock(&pThis->mutex);

	return RETVAL_OK;
}

int window_playlist_handle_press(tHandleWindowPlaylist *pThis,ePlaylistPressed pressable)
{
	switch(pressable)
	{
		case PLAYLIST_PRESSED_ADD_BUTTON:
			pThis->statusMenuAdd=(pThis->statusMenuAdd==MENU_OPEN)?MENU_CLOSE:MENU_OPEN;
			break;
		case PLAYLIST_PRESSED_REMOVE_BUTTON:
			pThis->statusMenuRem=(pThis->statusMenuRem==MENU_OPEN)?MENU_CLOSE:MENU_OPEN;
			break;
		case PLAYLIST_PRESSED_SELECTION_BUTTON:
			pThis->statusMenuSel=(pThis->statusMenuSel==MENU_OPEN)?MENU_CLOSE:MENU_OPEN;
			break;
		case PLAYLIST_PRESSED_MISCELLANEOUS_BUTTON:
			pThis->statusMenuMisc=(pThis->statusMenuMisc==MENU_OPEN)?MENU_CLOSE:MENU_OPEN;
			break;
		case PLAYLIST_PRESSED_LIST_BUTTON:
			pThis->statusMenuList=(pThis->statusMenuList==MENU_OPEN)?MENU_CLOSE:MENU_OPEN;
			break;
		default:
			printf("TODO: handle press %d\n",(int)pressable);
			break;
	}
	return RETVAL_OK;
}
ePlaylistPressed window_playlist_find_pressable(tHandleWindowPlaylist *pThis,int x,int y,int width,int height)
{
	int i;
	double scaleFactorX;
	double scaleFactorY;
	ePlaylistPressed pressed;

	pressed=PLAYLIST_PRESSED_NONE;

	scaleFactorX=width/WINDOW_PLAYLIST_WIDTH;
	scaleFactorY=height/WINDOW_PLAYLIST_HEIGHT;
	for (i=0;i<PLAYLIST_PRESSABLE_NUM;i++)
	{
		int x1;
		int x2;
		int y1;
		int y2;

		x1=pThis->windowPlaylist_pressable[i].posx;
		x2=x1+pThis->windowPlaylist_pressable[i].dimx;
		y1=pThis->windowPlaylist_pressable[i].posy;
		y2=y1+pThis->windowPlaylist_pressable[i].dimy;
		x1*=scaleFactorX;
		x2*=scaleFactorX;
		y1*=scaleFactorY;
		y2*=scaleFactorY;
		if (x>=x1 && x<x2 && y>=y1 && y<y2)
		{
			pressed=pThis->windowPlaylist_pressable[i].pressed;
		}
	}
	return pressed;
}
int window_playlist_event_pressed(GtkWidget *widget, double x,double y,guint event_button, gpointer data)
{
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)data;
	int width,height;
        width=gtk_widget_get_width(pThis->windowPlaylist);
        height=gtk_widget_get_height(pThis->windowPlaylist);
	pThis->lastPressed=window_playlist_find_pressable(pThis,(int)x,(int)y,width,height);
	window_playlist_refresh(pThis);
	return TRUE;	
}

int window_playlist_event_released(GtkWidget *widget, double x,double y,guint event_button, gpointer data)
{
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)data;
	int width,height;
	ePlaylistPressed pressed;
        width=gtk_widget_get_width(pThis->windowPlaylist);
        height=gtk_widget_get_height(pThis->windowPlaylist);
	pressed=window_playlist_find_pressable(pThis,(int)x,(int)y,width,height);

	if (pressed!=PLAYLIST_PRESSED_NONE && pressed==pThis->lastPressed)
	{
		window_playlist_handle_press(pThis,pressed);
	}
	pThis->lastPressed=PLAYLIST_PRESSED_NONE;
	window_playlist_refresh(pThis);
	return TRUE;	
}

int window_playlist_init(GtkApplication* app,tHandleWindowPlaylist* pThis,tHandleThemeManager *pHandleThemeManager)
{
	int i;
	memset(pThis,0,sizeof(tHandleWindowPlaylist));
	pThis->pHandleThemeManager=pHandleThemeManager;
	pthread_mutex_init(&pThis->mutex,NULL);	
	for (i=0;i<PLAYLIST_PRESSABLE_NUM;i++)
	{
		ePlaylistPressed idx;
		idx=(ePlaylistPressed)i;
		pThis->windowPlaylist_pressable[idx].pressed=idx;
	}

	pThis->scaleFactor=4;
	pThis->scrollPos=0;
	pThis->scrollLen=0;
	pThis->statusMenuAdd =MENU_CLOSE;
	pThis->statusMenuRem =MENU_CLOSE;
	pThis->statusMenuSel =MENU_CLOSE;
	pThis->statusMenuMisc=MENU_CLOSE;
	pThis->statusMenuList=MENU_CLOSE;
	

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

