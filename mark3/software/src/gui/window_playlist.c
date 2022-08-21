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
#include "playlist.h"
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


int window_playlist_drawlist(tHandleWindowPlaylist* pThis,GdkPixbuf *pixbufDestination,int listx,int listy,int listwidth,int listheight)
{
	char linebuf[1024];
	tPlayListTheme *pPlayList;
	cairo_font_extents_t extents;
	cairo_surface_t *surface;
	cairo_t *cr;
	GdkPixbuf *pixbuf;
	char isCurrent;
	int linenum;
	int lineheight;
	int i;
	double color_normalBG_red;
	double color_normalBG_green;
	double color_normalBG_blue;

	double color_selectedBG_red;
	double color_selectedBG_green;
	double color_selectedBG_blue;


	double color_normal_red;
	double color_normal_green;
	double color_normal_blue;

	double color_current_red;
	double color_current_green;
	double color_current_blue;


	theme_manager_get_playlistTheme(pThis->pHandleThemeManager,&pPlayList);
	

	color_selectedBG_red=(double)pPlayList->color_selectedBG.red;	color_selectedBG_red/=255.0;
	color_selectedBG_green=(double)pPlayList->color_selectedBG.green;color_selectedBG_green/=255.0;
	color_selectedBG_blue=(double)pPlayList->color_selectedBG.blue;	color_selectedBG_blue/=255.0;

	color_current_red=(double)pPlayList->color_current.red;		color_current_red/=255.0;
	color_current_green=(double)pPlayList->color_current.green;	color_current_green/=255.0;
	color_current_blue=(double)pPlayList->color_current.blue;	color_current_blue/=255.0;

	color_normalBG_red=(double)pPlayList->color_normalBG.red;	color_normalBG_red/=255.0;
	color_normalBG_green=(double)pPlayList->color_normalBG.green;	color_normalBG_green/=255.0;
	color_normalBG_blue=(double)pPlayList->color_normalBG.blue;	color_normalBG_blue/=255.0;

	color_normal_red=(double)pPlayList->color_normal.red;		color_normal_red/=255.0;
	color_normal_green=(double)pPlayList->color_normal.green;	color_normal_green/=255.0;
	color_normal_blue=(double)pPlayList->color_normal.blue;		color_normal_blue/=255.0;

	surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,listwidth,listheight);
	cr=cairo_create(surface);

	cairo_set_source_rgba(cr,color_normalBG_blue,color_normalBG_green,color_normalBG_red,1);		// TODO: why is it the other way around?
	cairo_rectangle(cr,0,0,listwidth,listheight);
	cairo_stroke_preserve(cr);
	cairo_fill(cr);

        cairo_set_source_rgba(cr,color_normal_blue,color_normal_green,color_normal_red,1);

	// set the font
        cairo_select_font_face(cr, pPlayList->fontname,
                        CAIRO_FONT_SLANT_NORMAL,
                        CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size(cr, 8);	
        cairo_font_extents(cr, &extents);

	lineheight=(int)extents.height;

	// calculate the number of possible lines from the font size
	linenum=(listheight)/lineheight;
	playlist_resize(&(pThis->handlePlaylist),linenum);

	isCurrent=1;	// so that in the first iteration, the "current" color will be set
	for (i=0;i<linenum;i++)
	{
		cairo_move_to(cr, 0, (i+1)*lineheight);
		if (isCurrent)	// when the last iteration was the current one, the color is wrong. set it to normal
		{
			cairo_set_source_rgba(cr,color_normal_blue,color_normal_green,color_normal_red,1);		// TODO: why is it the other way around?
		}
		playlist_get_filename_by_line(&(pThis->handlePlaylist),linebuf,sizeof(linebuf),&isCurrent,i);
		if (isCurrent)
		{
			cairo_set_source_rgba(cr,color_current_blue,color_current_green,color_current_red,1);		// TODO: why is it the other way around?

		}
		cairo_show_text(cr, linebuf);
	}




	// now that the list has been prepared, convert it into a pixbuf
	pixbuf=gdk_pixbuf_new_from_data(cairo_image_surface_get_data(surface),GDK_COLORSPACE_RGB,TRUE,8,listwidth,listheight,listwidth*4,NULL,NULL);
	gdk_pixbuf_copy_area(pixbuf,0,0,listwidth,listheight,pixbufDestination,listx,listy);
	g_object_unref(pixbuf);
	cairo_destroy(cr);
	cairo_surface_destroy(surface);	
	
	return RETVAL_OK;	
}	

int window_playlist_draw(tHandleWindowPlaylist* pThis,GdkPixbuf *pixbufDestination)
{
	int x,x0;
	int y,y0,y1;

	int listx,listy;
	int listwidth,listheight;
	x=0;
	y=0;

	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,(pThis->statusActive==ACTIVE?PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE:PLEDIT_UPPER_LEFT_CORNERPIECE_INACTIVE));x+=ELEMENT_WIDTH(PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE);

	while (x<pThis->window_width-ELEMENT_WIDTH(PLEDIT_UPPER_RIGHT_CORNERPIECE_ACTIVE))
	{
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,(pThis->statusActive==ACTIVE?PLEDIT_TOP_FILLERS_ACTIVE:PLEDIT_TOP_FILLERS_INACTIVE));x+=ELEMENT_WIDTH(PLEDIT_TOP_FILLERS_ACTIVE);
	}

	x=pThis->window_width/2-ELEMENT_WIDTH(PLEDIT_PLAYLIST_TITLEBAR_INACTIVE)/2;
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,(pThis->statusActive==ACTIVE?PLEDIT_PLAYLIST_TITLEBAR_ACTIVE:PLEDIT_PLAYLIST_TITLEBAR_INACTIVE));
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,pThis->window_width-ELEMENT_WIDTH(PLEDIT_UPPER_RIGHT_CORNERPIECE_ACTIVE),y,(pThis->statusActive==ACTIVE?PLEDIT_UPPER_RIGHT_CORNERPIECE_ACTIVE:PLEDIT_UPPER_RIGHT_CORNERPIECE_INACTIVE));
	

	y0=y=ELEMENT_HEIGHT(PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE);
	x0=pThis->window_width-ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_LEFT_BAR)-ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_SCROLL_GROOVE)-ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_RIGHT_BAR);
	while (y<pThis->window_height-ELEMENT_HEIGHT(PLEDIT_BOTTOM_FILLERS))
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

	y1=y=pThis->window_height-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR);
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
	x=pThis->window_width-ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_RIGHT_BAR)-ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_SCROLL_GROOVE);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_SCROLL_BUTTON_PRESSED);
		

	y=y1;
	x=0;
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,0,y,PLEDIT_BOTTOM_LEFT_CONTROL_BAR);
	x+=ELEMENT_WIDTH(PLEDIT_BOTTOM_LEFT_CONTROL_BAR);
	while (x<pThis->window_width-ELEMENT_WIDTH(PLEDIT_BOTTOM_RIGHT_CONTROL_BAR))
	{
		theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_BOTTOM_FILLERS);
		x+=ELEMENT_WIDTH(PLEDIT_BOTTOM_FILLERS);
	}
	x=pThis->window_width-ELEMENT_WIDTH(PLEDIT_BOTTOM_RIGHT_CONTROL_BAR);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_BOTTOM_RIGHT_CONTROL_BAR);


	x=pThis->window_width-ELEMENT_WIDTH(PLEDIT_RESIZE_CONTROL);
	y=pThis->window_height-ELEMENT_WIDTH(PLEDIT_RESIZE_CONTROL)-2;
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_RESIZE_CONTROL);
	DEFINE_PRESSABLE(PLAYLIST_PRESSED_RESIZE_CONTROL,PLEDIT_RESIZE_CONTROL,x,y);
	

	// draw the playlist between the top left corner of the border and the bottom right one.
	listx=ELEMENT_WIDTH(PLEDIT_LEFT_SIDE_FILLERS);
	listy=ELEMENT_HEIGHT(PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE);
	listwidth=pThis->window_width-ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_LEFT_BAR)-ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_SCROLL_GROOVE)-ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_RIGHT_BAR)-listx;
	listheight=pThis->window_height-ELEMENT_HEIGHT(PLEDIT_BOTTOM_FILLERS)-ELEMENT_HEIGHT(PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE);
	window_playlist_drawlist(pThis,pixbufDestination,listx,listy,listwidth,listheight);

/////////////////////////////// the menues
// the add menu
	y=pThis->window_height-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+7;
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
	
	y=pThis->window_height-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+8;
	x=14;
	DEFINE_PRESSABLE(PLAYLIST_PRESSED_ADD_BUTTON,PLEDIT_ADD_BUTTON,x,y);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_ADD_BUTTON);
	


	y=pThis->window_height-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+7;
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
	y=pThis->window_height-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+8;
	x=43;
	DEFINE_PRESSABLE(PLAYLIST_PRESSED_REMOVE_BUTTON,PLEDIT_REMOVE_BUTTON,x,y);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_REMOVE_BUTTON);
	

	y=pThis->window_height-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+7;
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
	y=pThis->window_height-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+8;
	x=72;
	DEFINE_PRESSABLE(PLAYLIST_PRESSED_SELECTION_BUTTON,PLEDIT_SELECTION_BUTTON,x,y);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_SELECTION_BUTTON);
	



	y=pThis->window_height-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+7;
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
	y=pThis->window_height-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+8;
	x=101;
	DEFINE_PRESSABLE(PLAYLIST_PRESSED_MISCELLANEOUS_BUTTON,PLEDIT_MISCELLANEOUS_BUTTON,x,y);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_MISCELLANEOUS_BUTTON);


	y=pThis->window_height-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+7;
	x=pThis->window_width-29-18;
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
	y=pThis->window_height-ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR)+8;
	x=pThis->window_width-29-18+3;
	DEFINE_PRESSABLE(PLAYLIST_PRESSED_LIST_BUTTON,PLEDIT_LIST_BUTTON,x,y);
	theme_manager_addelement(pThis->pHandleThemeManager,pixbufDestination,x,y,PLEDIT_LIST_BUTTON);


	return RETVAL_OK;
}

int window_playlist_refresh(tHandleWindowPlaylist* pThis)
{
	GdkPixbuf *pixbuf;
	pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,pThis->window_width,pThis->window_height);
	window_playlist_draw(pThis,pixbuf);
	pthread_mutex_lock(&pThis->mutex);
	gdk_pixbuf_copy_area(pixbuf,0,0,pThis->window_width,pThis->window_height,pThis->pixbuf,0,0);
	g_object_unref(pixbuf);	
	gtk_picture_set_pixbuf(GTK_PICTURE(pThis->picture),pThis->pixbuf);
	{
		
	//	gtk_widget_queue_draw(pThis->windowPlaylist);
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

	scaleFactorX=width/pThis->window_width;
	scaleFactorY=height/pThis->window_height;
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
void window_playlist_resize(tHandleWindowPlaylist* pThis)
{
	g_object_unref(pThis->picture);
	g_object_unref(pThis->pixbuf);
	pThis->pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,pThis->window_width,pThis->window_height);
	window_playlist_draw(pThis,pThis->pixbuf);
	pThis->picture=gtk_picture_new_for_pixbuf(pThis->pixbuf);
	gtk_window_set_default_size(GTK_WINDOW(pThis->windowPlaylist),pThis->window_width*pThis->scaleFactor,pThis->window_height*pThis->scaleFactor);
	gtk_window_set_child(GTK_WINDOW(pThis->windowPlaylist),pThis->picture);

}
int window_playlist_event_pressed(GtkWidget *widget, double x,double y,guint event_button, gpointer data)
{
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)data;
	int width,height;
        width=gtk_widget_get_width(pThis->windowPlaylist);
        height=gtk_widget_get_height(pThis->windowPlaylist);
	printf("x:%d y:%d width:%d height:%d\n",(int)x,(int)y,width,height);
	pThis->lastPressed=window_playlist_find_pressable(pThis,(int)x,(int)y,width,height);
	pThis->pressedX=x;
	pThis->pressedY=y;
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
	printf("x:%d y:%d width:%d height:%d\n",(int)x,(int)y,width,height);

	if (pThis->lastPressed==PLAYLIST_PRESSED_RESIZE_CONTROL)
	{
		int deltax;
		int deltay;

		int stepsX;
		int stepsY;

		int newwidth;
		int newheight;
		

		deltax=x-pThis->pressedX;
		deltay=y-pThis->pressedY;

		printf("delta:%d %d\n",deltax,deltay);

		stepsX=deltax/(5*pThis->scaleFactor);
		stepsY=deltay/(5*pThis->scaleFactor);

		newwidth=pThis->window_width+stepsX*5;
		newheight=pThis->window_height+stepsY*5;

		if (newwidth<WINDOW_PLAYLIST_WIDTH)
		{
			newwidth=WINDOW_PLAYLIST_WIDTH;
		}
		if (newheight<WINDOW_PLAYLIST_HEIGHT)
		{
			newheight=WINDOW_PLAYLIST_HEIGHT;
		}
		pThis->window_width=newwidth;
		pThis->window_height=newheight;

		

		
		window_playlist_resize(pThis);
	} else {
		pressed=window_playlist_find_pressable(pThis,(int)x,(int)y,width,height);

		if (pressed!=PLAYLIST_PRESSED_NONE && pressed==pThis->lastPressed)
		{
			window_playlist_handle_press(pThis,pressed);
		}
	}
	pThis->lastPressed=PLAYLIST_PRESSED_NONE;
	window_playlist_refresh(pThis);
	return TRUE;	
}

int window_playlist_init(GtkApplication* app,tHandleWindowPlaylist* pThis,tHandleThemeManager *pHandleThemeManager)
{
	int i;
	memset(pThis,0,sizeof(tHandleWindowPlaylist));
	playlist_init(&(pThis->handlePlaylist));
	pThis->pHandleThemeManager=pHandleThemeManager;
	pthread_mutex_init(&pThis->mutex,NULL);	
	for (i=0;i<PLAYLIST_PRESSABLE_NUM;i++)
	{
		ePlaylistPressed idx;
		idx=(ePlaylistPressed)i;
		pThis->windowPlaylist_pressable[idx].pressed=idx;
	}
	pThis->windowPlaylist=gtk_application_window_new(app);

	pThis->scaleFactor=4;
	pThis->scrollPos=0;
	pThis->scrollLen=0;
	pThis->statusMenuAdd =MENU_CLOSE;
	pThis->statusMenuRem =MENU_CLOSE;
	pThis->statusMenuSel =MENU_CLOSE;
	pThis->statusMenuMisc=MENU_CLOSE;
	pThis->statusMenuList=MENU_CLOSE;

	pThis->window_height=WINDOW_PLAYLIST_HEIGHT;
	pThis->window_width=WINDOW_PLAYLIST_WIDTH;
	pThis->pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,pThis->window_width,pThis->window_height);	// alibi
	pThis->picture=gtk_picture_new_for_pixbuf(pThis->pixbuf);
	window_playlist_resize(pThis);
	

	window_playlist_draw(pThis,pThis->pixbuf);
	pThis->picture=gtk_picture_new_for_pixbuf(pThis->pixbuf);
	gtk_window_set_default_size(GTK_WINDOW(pThis->windowPlaylist),pThis->window_width*pThis->scaleFactor,pThis->window_height*pThis->scaleFactor);
	gtk_window_set_child(GTK_WINDOW(pThis->windowPlaylist),pThis->picture);
	gtk_window_set_title(GTK_WINDOW(pThis->windowPlaylist),"d11amp playlist");

	pThis->gesture=gtk_gesture_click_new();
	g_signal_connect(pThis->gesture,"pressed",G_CALLBACK(window_playlist_event_pressed),&(pThis->windowPlaylist));
	g_signal_connect(pThis->gesture,"released",G_CALLBACK(window_playlist_event_released),&(pThis->windowPlaylist));
	gtk_widget_add_controller(pThis->windowPlaylist,GTK_EVENT_CONTROLLER(pThis->gesture));




	return RETVAL_OK;
}
int window_playlist_load(tHandleWindowPlaylist* pThis,char* filename)
{
	int retval;
	retval=playlist_load(&(pThis->handlePlaylist),filename);
	window_playlist_refresh(pThis);
	return retval;
}
int window_playlist_show(tHandleWindowPlaylist* pThis)
{
	gtk_widget_show(pThis->windowPlaylist);
	window_playlist_refresh(pThis);
	return RETVAL_OK;
}
int window_playlist_hide(tHandleWindowPlaylist* pThis)
{
	gtk_widget_hide(pThis->windowPlaylist);
	return RETVAL_OK;
}
int window_playlist_parse_commandline(tHandleWindowPlaylist* pThis,char* argument)
{
	int l;
	int retval;
	l=strlen(argument);
	if (l>4 && strncmp("m3u=",argument,4)==0)
	{
		retval=playlist_load(&(pThis->handlePlaylist),&argument[4]);
	} else {
		return RETVAL_NOK_COMMANDLINE;
	}
	return retval;

}

int window_playlist_append(tHandleWindowPlaylist *pThis,char* filename)
{
	playlist_append(&(pThis->handlePlaylist),filename);
	window_playlist_refresh(pThis);
	return RETVAL_OK;	
}

int window_playlist_shuffle(tHandleWindowPlaylist *pThis,char* pFilename,int size)
{
	playlist_shuffle(&(pThis->handlePlaylist),pFilename,size,1);
	playlist_jump_to_current(&(pThis->handlePlaylist));
	window_playlist_refresh(pThis);
	return RETVAL_OK;
}
int window_playlist_next(tHandleWindowPlaylist *pThis,char* pFilename,int size)
{
	playlist_next(&(pThis->handlePlaylist),pFilename,size,1);
	playlist_jump_to_current(&(pThis->handlePlaylist));
	window_playlist_refresh(pThis);
	
	return RETVAL_OK;
}
int window_playlist_prev(tHandleWindowPlaylist *pThis,char* pFilename,int size)
{
	playlist_prev(&(pThis->handlePlaylist),pFilename,size,1);
	playlist_jump_to_current(&(pThis->handlePlaylist));
	window_playlist_refresh(pThis);
	return RETVAL_OK;

}

