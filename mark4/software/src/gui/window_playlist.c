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
#include "datastructures.h"
#include "gui_helpers.h"
#include "playlist.h"
#include "theme_manager.h"
#include "window_playlist.h"
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
static gboolean window_playlist_event_scroll(GtkEventControllerScroll *controller,double dx,double dy,GtkWidget *window);
static gboolean window_playlist_close(GtkWidget *widget,gpointer data);

	
static void window_playlist_filechooser_response(GtkNativeDialog *native,int response);
static void window_playlist_dirchooser_response(GtkNativeDialog *native,int response);
static void window_playlist_load_response(GtkNativeDialog *native,int response);
static void window_playlist_save_response(GtkNativeDialog *native,int response);

static void window_playlist_sort_by_filename(GSimpleAction *action, GVariant *parameter, gpointer user_data);
//static void window_playlist_sort_by_path(GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void window_playlist_sort_randomize(GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void window_playlist_sort_reverse(GSimpleAction *action, GVariant *parameter, gpointer user_data);

int window_playlist_resize(tHandleWindowPlaylist* pThis,int rows,int columns)
{
	int retval;
	int winheight;
	int winwidth;

	int rightmargin;
	GdkPixbuf *newPixbufBackground;
	GdkPixbuf *newPixbuf;
	GdkPixbuf *newPixbuf_handle;
	GdkPixbuf *newPixbuf_main;
	
	GdkPixbuf *ptr1;
	GdkPixbuf *ptr2;
	GdkPixbuf *ptr3;
	GdkPixbuf *ptr4;

	retval=RETVAL_OK;

	if (rows<MIN_PLAYLIST_ROWS)
	{
		rows=MIN_PLAYLIST_ROWS;
	}
	if (columns<MIN_PLAYLIST_COLS)
	{
		columns=MIN_PLAYLIST_COLS;
	}
	if (pThis->handleConfigValid)	// better safe than sorry...
	{
		config_setint(&(pThis->handleConfig),"rows",rows);
		config_setint(&(pThis->handleConfig),"columns",columns);
	}
	pThis->rows=rows;
	pThis->columns=columns;

	pThis->window_height=winheight=ELEMENT_HEIGHT(PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE)+rows*ELEMENT_HEIGHT(PLEDIT_LEFT_SIDE_FILLERS)+ELEMENT_HEIGHT(PLEDIT_BOTTOM_LEFT_CONTROL_BAR);
	pThis->window_width=winwidth=columns*ELEMENT_WIDTH(PLEDIT_TOP_FILLERS_ACTIVE);

	newPixbufBackground=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,winwidth,winheight);
	newPixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,winwidth,winheight);
	newPixbuf_handle=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,winwidth,WINDOW_PLAYLIST_HANDLE_HEIGHT);
	newPixbuf_main=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,winwidth,winheight-WINDOW_PLAYLIST_HANDLE_HEIGHT);

//	ptr3=pThis->picture;
	gtk_window_set_default_size(GTK_WINDOW(pThis->window),pThis->scaleFactor*winwidth,pThis->scaleFactor*winheight);

	ptr4=pThis->pixbuf_main;
	pThis->pixbuf_main=newPixbuf_main;
	ptr3=pThis->pixbuf_handle;
	pThis->pixbuf_handle=newPixbuf_handle;

	ptr2=pThis->pixbuf;
	pThis->pixbuf=newPixbuf;
	ptr1=pThis->pixbufBackground;
	pThis->pixbufBackground=newPixbufBackground;

	// at this point, the new references have been set.
	// the old ones are still "dangling".
	if (ptr4!=NULL)
	{
		g_object_unref(ptr4);
	}
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




	// since the window has been resized, the pressables might have moved
	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[16],ePRESSED_WINDOW_PLAYLIST_ADD,		PLEDIT_ADD_BUTTON);
	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[17],ePRESSED_WINDOW_PLAYLIST_REMOVE,		PLEDIT_REMOVE_BUTTON);
	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[18],ePRESSED_WINDOW_PLAYLIST_SELECTION,		PLEDIT_SELECTION_BUTTON);
	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[19],ePRESSED_WINDOW_PLAYLIST_MISCELLANEOUS,	PLEDIT_MISCELLANEOUS_BUTTON);
	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[20],ePRESSED_WINDOW_PLAYLIST_LIST,		PLEDIT_LIST_BUTTON);
	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[21],ePRESSED_WINDOW_PLAYLIST_RESIZE,		PLEDIT_RESIZE_CONTROL);

	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[22],ePRESSED_WINDOW_PLAYLIST_PREVIOUS,		PLEDIT_PREVIOUS_CONTROL);
	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[23],ePRESSED_WINDOW_PLAYLIST_PLAY,		PLEDIT_PLAY_CONTROL);
	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[24],ePRESSED_WINDOW_PLAYLIST_PAUSE,		PLEDIT_PAUSE_CONTROL);
	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[25],ePRESSED_WINDOW_PLAYLIST_STOP,		PLEDIT_STOP_CONTROL);
	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[26],ePRESSED_WINDOW_PLAYLIST_NEXT,		PLEDIT_NEXT_CONTROL);
	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[27],ePRESSED_WINDOW_PLAYLIST_OPEN,		PLEDIT_OPEN_CONTROL);

	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[28],ePRESSED_WINDOW_PLAYLIST_PAGEUP,		PLEDIT_PAGE_UP_BUTTON);
	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[29],ePRESSED_WINDOW_PLAYLIST_PAGEDOWN,		PLEDIT_PAGE_DOWN_BUTTON);


	rightmargin=ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_LEFT_BAR)+ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_SCROLL_GROOVE)+ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_RIGHT_BAR);
	pThis->list_posx=ELEMENT_WIDTH(PLEDIT_LEFT_SIDE_FILLERS);
	pThis->list_posy=ELEMENT_HEIGHT(PLEDIT_TOP_FILLERS_ACTIVE);
	pThis->list_dimx=pThis->window_width-pThis->list_posx-rightmargin;
	pThis->list_dimy=pThis->window_height-ELEMENT_HEIGHT(PLEDIT_PLAYLIST_TITLEBAR_ACTIVE)-ELEMENT_HEIGHT(PLEDIT_BOTTOM_FILLERS);
	
	
	gui_helpers_define_pressable_by_dimensions(&pThis->boundingBoxes[30],ePRESSED_WINDOW_PLAYLIST_SCROLLBAR,pThis->window_width-rightmargin,pThis->list_posy,rightmargin,pThis->list_dimy);
	gui_helpers_define_pressable_by_dimensions(&pThis->boundingBoxes[31],ePRESSED_WINDOW_PLAYLIST_MAIN,pThis->list_posx,pThis->list_posy,pThis->list_dimx,pThis->list_dimy);

	gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[32],ePRESSED_WINDOW_PLAYLIST_CLOSE,		PLEDIT_CLOSE_BUTTON_PRESSED);

// FIXME: for some reason, resizing the gdk pixbufs does not mean that the pictures are being resized as well...
	gtk_picture_set_pixbuf(GTK_PICTURE(pThis->picture_handle),pThis->pixbuf_handle);
	gtk_picture_set_pixbuf(GTK_PICTURE(pThis->picture_main),pThis->pixbuf_main);

	gtk_window_set_default_size(GTK_WINDOW(pThis->window),pThis->scaleFactor*winwidth,pThis->scaleFactor*winheight);



	return retval;
}

int window_playlist_init(tHandleWindowPlaylist* pThis,void* pControllerContext,tHandleThemeManager *pHandleThemeManager,GtkApplication* app,tHandlePlayList *pHandlePlayList)
{
	int retval;
	retval=RETVAL_OK;
		
	memset(pThis,0,sizeof(tHandleWindowPlaylist));
	pThis->handleConfigValid=0;
	pThis->scaleFactor=1;
	pThis->app=app;
	pThis->pControllerContext=pControllerContext;
	pThis->pHandlePlayList=pHandlePlayList;
	pThis->pHandleThemeManager=pHandleThemeManager;
	pThis->window=gtk_application_window_new(pThis->app);
	gtk_window_set_title(GTK_WINDOW(pThis->window),"d11amp playlist");
	gtk_window_set_resizable(GTK_WINDOW(pThis->window),FALSE);
	gtk_window_set_decorated(GTK_WINDOW(pThis->window),FALSE);

	pThis->pixbuf=NULL;
	pThis->picture=gtk_picture_new_for_pixbuf(NULL);
	pThis->picture_handle=gtk_picture_new_for_pixbuf(NULL);
	pThis->picture_main=gtk_picture_new_for_pixbuf(NULL);
	pThis->handle=gtk_window_handle_new();
	gtk_window_handle_set_child(GTK_WINDOW_HANDLE(pThis->handle),pThis->picture_handle);

	pThis->box=gtk_box_new(GTK_ORIENTATION_VERTICAL,0);

	gtk_box_append(GTK_BOX(pThis->box),pThis->handle);
	gtk_box_append(GTK_BOX(pThis->box),pThis->picture_main);
	
	gtk_box_set_homogeneous(GTK_BOX(pThis->box),FALSE);
	gtk_window_set_child(GTK_WINDOW(pThis->window),pThis->box);
	
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

	pThis->event_scroll=gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
	g_object_set_data(G_OBJECT(pThis->event_scroll),"pThis",pThis);	// add a pointer to the handle to the widget. this way it is available in the gesture callbacks
	g_signal_connect(pThis->event_scroll,"scroll",G_CALLBACK(window_playlist_event_scroll),pThis);
	gtk_widget_add_controller(pThis->window,pThis->event_scroll);

	g_signal_connect(G_OBJECT(pThis->window), "close_request", G_CALLBACK (window_playlist_close), (void*)pThis);

	pthread_mutex_init(&pThis->mutex,NULL);

	pThis->menuItemCnt=0;
	pThis->menu=g_menu_new();
#define	NEW_MENU_ITEM(callback_function, action_name1, action_name2, menu_label)	\
	pThis->action[pThis->menuItemCnt]=g_simple_action_new(action_name1,NULL);		\
	g_action_map_add_action(G_ACTION_MAP(app),G_ACTION(pThis->action[pThis->menuItemCnt]));	\
	g_signal_connect(pThis->action[pThis->menuItemCnt],"activate",G_CALLBACK(callback_function),pThis);	\
	pThis->menuitems[pThis->menuItemCnt]=g_menu_item_new(menu_label,action_name2);	\
	g_menu_append_item(pThis->menu,pThis->menuitems[pThis->menuItemCnt]);	\
	pThis->menuItemCnt++;


	NEW_MENU_ITEM(window_playlist_sort_by_filename,"window_playlist_sort_by_filename","app.window_playlist_sort_by_filename","Sort by Filename");
	//NEW_MENU_ITEM(window_playlist_sort_by_path,"window_playlist_sort_by_path","app.window_playlist_sort_by_path","Sort by Path+Filename");
	NEW_MENU_ITEM(window_playlist_sort_randomize,"window_playlist_sort_randomize","app.window_playlist_sort_randomize","Randomize");
	NEW_MENU_ITEM(window_playlist_sort_reverse,"window_playlist_sort_reverse","app.window_playlist_sort_reverse","Reverse");
		
	pThis->popUpMenu=gtk_popover_menu_new_from_model_full(G_MENU_MODEL(pThis->menu),GTK_POPOVER_MENU_NESTED);
	gtk_widget_set_parent(GTK_WIDGET(pThis->popUpMenu),pThis->box);


	pThis->show_full_path=1;	
	return retval;
}
int window_playlist_start(tHandleWindowPlaylist* pThis)
{
	int rows,columns;
	GtkWidget *pWidget;
	pThis->handleConfigValid=1;
	config_init(&(pThis->handleConfig),pThis->pControllerContext,"playlist.config");
	config_getint(&(pThis->handleConfig),"rows",&rows,pThis->rows);
	config_getint(&(pThis->handleConfig),"columns",&columns,pThis->columns);


	window_playlist_get_preferences_widget(pThis,&pWidget);
	controller_add_preferences_widget(pThis->pControllerContext,(void*)pWidget,"Playlist",window_playlist_populate_preferences,window_playlist_apply_preferences,(void*)pThis);

	return window_playlist_resize(pThis,rows,columns);
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
	int deltax;

	winheight=ROW_MARGIN_TOP+pThis->rows*ROW_HEIGHT+ROW_MARGIN_BOTTOM;
	winwidth=pThis->columns*COL_WIDTH;

	


	retval=RETVAL_OK;

	// first: draw the top frame         ------------------------
	x=0;
#if 0
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE,0,0);x+=ELEMENT_WIDTH(PLEDIT_UPPER_LEFT_CORNERPIECE_ACTIVE);
	while (x<pThis->window_width-ELEMENT_WIDTH(PLEDIT_UPPER_RIGHT_CORNERPIECE_ACTIVE))
	{
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_TOP_FILLERS_ACTIVE,x,0);x+=ELEMENT_WIDTH(PLEDIT_TOP_FILLERS_ACTIVE);
	}
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_UPPER_RIGHT_CORNERPIECE_ACTIVE,x,0);
	// then: add the title in the center ---------[title]--------
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_PLAYLIST_TITLEBAR_ACTIVE,winwidth/2-ELEMENT_WIDTH(PLEDIT_PLAYLIST_TITLEBAR_ACTIVE)/2,0);
#else
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_UPPER_LEFT_CORNERPIECE_INACTIVE,0,0);x+=ELEMENT_WIDTH(PLEDIT_UPPER_LEFT_CORNERPIECE_INACTIVE);
	while (x<pThis->window_width-ELEMENT_WIDTH(PLEDIT_UPPER_RIGHT_CORNERPIECE_INACTIVE))
	{
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_TOP_FILLERS_INACTIVE,x,0);x+=ELEMENT_WIDTH(PLEDIT_TOP_FILLERS_INACTIVE);
	}
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_UPPER_RIGHT_CORNERPIECE_INACTIVE,x,0);
	// then: add the title in the center ---------[title]--------
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_PLAYLIST_TITLEBAR_INACTIVE,winwidth/2-ELEMENT_WIDTH(PLEDIT_PLAYLIST_TITLEBAR_INACTIVE)/2,0);

#endif

	// add the borders on the left and on the right side |              |||	
	y=ROW_MARGIN_TOP;
	for (i=0;i<pThis->rows;i++)
	{
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_LEFT_SIDE_FILLERS,0,y);
	
		x=winwidth-COL_MARGIN_RIGHT;	
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_RIGHT_SIDE_FILLERS_LEFT_BAR,x,y);x+=ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_LEFT_BAR);
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_RIGHT_SIDE_FILLERS_SCROLL_GROOVE,x,y);x+=ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_SCROLL_GROOVE);
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_RIGHT_SIDE_FILLERS_RIGHT_BAR,x,y);x+=ELEMENT_WIDTH(PLEDIT_RIGHT_SIDE_FILLERS_RIGHT_BAR);
		y+=ROW_HEIGHT;
	}	
	// add the bottom menu
	x=0;
	y=winheight-ROW_MARGIN_BOTTOM;
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_BOTTOM_LEFT_CONTROL_BAR,x,y);x+=ELEMENT_WIDTH(PLEDIT_BOTTOM_LEFT_CONTROL_BAR);
	deltax=(winwidth-ELEMENT_WIDTH(PLEDIT_BOTTOM_RIGHT_CONTROL_BAR))-x;
	if (deltax>=ELEMENT_WIDTH(PLEDIT_VISUALIZATION_MINISCREEN))
	{
		theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_VISUALIZATION_MINISCREEN);
		deltax-=ELEMENT_WIDTH(PLEDIT_VISUALIZATION_MINISCREEN);
	}
	while (deltax>0)	// when the window is at its minimum width, the bottom left controlbar + bottom right controlbar are as wide as the window. otherwise, it might need some filling
	{
		retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_BOTTOM_FILLERS,x,y);x+=ELEMENT_WIDTH(PLEDIT_BOTTOM_FILLERS);
		deltax-=ELEMENT_WIDTH(PLEDIT_BOTTOM_FILLERS);
	}
	retval|=theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_BOTTOM_RIGHT_CONTROL_BAR);

	theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_PREVIOUS_CONTROL);
	theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_PLAY_CONTROL);
	theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_PAUSE_CONTROL);
	theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_STOP_CONTROL);
	theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_NEXT_CONTROL);
	theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_OPEN_CONTROL);

	theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_PAGE_UP_BUTTON);
	theme_manager_draw_element(pThis->pHandleThemeManager,pThis->pixbufBackground,PLEDIT_PAGE_DOWN_BUTTON);


	
	return retval;
}
int window_playlist_draw_status(tHandleWindowPlaylist *pThis,GdkPixbuf *destBuf)
{
	int retval;

	retval=RETVAL_OK;
	if (pThis->status.menu_add)
	{
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_DECORATION_BAR_ADD);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_ADD_FILE_BUTTON_UNPRESSED);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_ADD_DIR_BUTTON_UNPRESSED);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_ADD_URL_BUTTON_UNPRESSED);
		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[ 0],ePRESSED_WINDOW_PLAYLIST_ADD_FILE,PLEDIT_ADD_FILE_BUTTON_UNPRESSED);
		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[ 1],ePRESSED_WINDOW_PLAYLIST_ADD_DIR,PLEDIT_ADD_DIR_BUTTON_UNPRESSED);
		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[ 2],ePRESSED_WINDOW_PLAYLIST_ADD_URL,PLEDIT_ADD_URL_BUTTON_UNPRESSED);
	} else {
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[ 0],ePRESSED_WINDOW_PLAYLIST_ADD_FILE);
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[ 1],ePRESSED_WINDOW_PLAYLIST_ADD_DIR);
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[ 2],ePRESSED_WINDOW_PLAYLIST_ADD_URL);
	}
	if (pThis->status.menu_remove)
	{
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_DECORATION_BAR_REMOVE);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_MISC_REMOVE_BUTTON_UNPRESSED);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_CROP_BUTTON_UNPRESSED);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_REMOVE_FILE_BUTTON_UNPRESSED);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_REMOVE_ALL_BUTTON_UNPRESSED);

		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[ 3],ePRESSED_WINDOW_PLAYLIST_MISC_REMOVE_BUTTON,PLEDIT_MISC_REMOVE_BUTTON_UNPRESSED);
		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[ 4],ePRESSED_WINDOW_PLAYLIST_CROP_BUTTON,PLEDIT_CROP_BUTTON_UNPRESSED);
		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[ 5],ePRESSED_WINDOW_PLAYLIST_REMOVE_FILE,PLEDIT_REMOVE_FILE_BUTTON_UNPRESSED);
		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[ 6],ePRESSED_WINDOW_PLAYLIST_REMOVE_ALL,PLEDIT_REMOVE_ALL_BUTTON_UNPRESSED);
	} else {
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[ 3],ePRESSED_WINDOW_PLAYLIST_MISC_REMOVE_BUTTON);
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[ 4],ePRESSED_WINDOW_PLAYLIST_CROP_BUTTON);
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[ 5],ePRESSED_WINDOW_PLAYLIST_REMOVE_FILE);
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[ 6],ePRESSED_WINDOW_PLAYLIST_REMOVE_ALL);
	}
	if (pThis->status.menu_select)
	{
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_DECORATION_BAR_SELECTION);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_INVERT_SELECTION_BUTTON_UNPRESSED);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_SELECT_NONE_BUTTON_UNPRESSED);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_SELECT_ALL_BUTTON_UNPRESSED);

		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[ 7],ePRESSED_WINDOW_PLAYLIST_INVERT_SELECTION,PLEDIT_INVERT_SELECTION_BUTTON_UNPRESSED);
		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[ 8],ePRESSED_WINDOW_PLAYLIST_SELECT_NONE,PLEDIT_SELECT_NONE_BUTTON_UNPRESSED);
		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[ 9],ePRESSED_WINDOW_PLAYLIST_SELECT_ALL,PLEDIT_SELECT_ALL_BUTTON_UNPRESSED);
	} else {
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[ 7],ePRESSED_WINDOW_PLAYLIST_INVERT_SELECTION);
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[ 8],ePRESSED_WINDOW_PLAYLIST_SELECT_NONE);
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[ 9],ePRESSED_WINDOW_PLAYLIST_SELECT_ALL);
	}
	if (pThis->status.menu_misc)
	{
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_DECORATION_BAR_MISC);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_SORT_LIST_BUTTON_UNPRESSED);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_FILE_INFO_BUTTON_UNPRESSED);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_MISC_OPTIONS_BUTTON_UNPRESSED);
		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[10],ePRESSED_WINDOW_PLAYLIST_SORT_LIST,PLEDIT_SORT_LIST_BUTTON_UNPRESSED);
		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[11],ePRESSED_WINDOW_PLAYLIST_FILE_INFO,PLEDIT_FILE_INFO_BUTTON_UNPRESSED);
		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[12],ePRESSED_WINDOW_PLAYLIST_MISC_OPTIONS,PLEDIT_MISC_OPTIONS_BUTTON_UNPRESSED);
	} else {
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[10],ePRESSED_WINDOW_PLAYLIST_SORT_LIST);
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[11],ePRESSED_WINDOW_PLAYLIST_FILE_INFO);
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[12],ePRESSED_WINDOW_PLAYLIST_MISC_OPTIONS);
	}
	if (pThis->status.menu_list)
	{
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_DECORATION_BAR_LIST);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_NEW_LIST_BUTTON_UNPRESSED);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_SAVE_LIST_BUTTON_UNPRESSED);
		retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,PLEDIT_LOAD_LIST_BUTTON_UNPRESSED);
		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[13],ePRESSED_WINDOW_PLAYLIST_NEW_LIST,PLEDIT_NEW_LIST_BUTTON_UNPRESSED);
		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[14],ePRESSED_WINDOW_PLAYLIST_SAVE_LIST,PLEDIT_SAVE_LIST_BUTTON_UNPRESSED);
		retval|=gui_helpers_define_pressable_by_element(pThis->window_width,pThis->window_height,&pThis->boundingBoxes[15],ePRESSED_WINDOW_PLAYLIST_LOAD_LIST,PLEDIT_LOAD_LIST_BUTTON_UNPRESSED);
	} else {
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[13],ePRESSED_WINDOW_PLAYLIST_NEW_LIST);
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[14],ePRESSED_WINDOW_PLAYLIST_SAVE_LIST);
		retval|=gui_helpers_undefine_pressable(&pThis->boundingBoxes[15],ePRESSED_WINDOW_PLAYLIST_LOAD_LIST);
	}
	retval|=theme_manager_draw_element_at(pThis->pHandleThemeManager,destBuf,PLEDIT_SCROLL_BUTTON_UNPRESSED,pThis->window_width+ELEMENT_DESTX(PLEDIT_SCROLL_BUTTON_UNPRESSED),pThis->scrollbarY);

	return retval;
}
int window_playlist_draw_pressable(tHandleWindowPlaylist *pThis,GdkPixbuf *destBuf)
{
	#define	CASEBLOCK(pressableid,elementid)	\
		case (pressableid):	\
			retval|=theme_manager_draw_element(pThis->pHandleThemeManager,destBuf,(elementid));	\
			break;		


	int retval;
	retval=RETVAL_OK;

	switch (pThis->lastPressed)
	{
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_ADD_FILE,PLEDIT_ADD_FILE_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_ADD_DIR,PLEDIT_ADD_DIR_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_ADD_URL,PLEDIT_ADD_URL_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_MISC_REMOVE_BUTTON,PLEDIT_MISC_REMOVE_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_CROP_BUTTON,PLEDIT_CROP_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_REMOVE_FILE,PLEDIT_REMOVE_FILE_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_REMOVE_ALL,PLEDIT_REMOVE_ALL_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_INVERT_SELECTION,PLEDIT_INVERT_SELECTION_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_SELECT_NONE,PLEDIT_SELECT_NONE_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_SELECT_ALL,PLEDIT_SELECT_ALL_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_SORT_LIST,PLEDIT_SORT_LIST_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_FILE_INFO,PLEDIT_FILE_INFO_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_MISC_OPTIONS,PLEDIT_MISC_OPTIONS_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_NEW_LIST,PLEDIT_NEW_LIST_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_SAVE_LIST,PLEDIT_SAVE_LIST_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_LOAD_LIST,PLEDIT_LOAD_LIST_BUTTON_PRESSED);
		CASEBLOCK(ePRESSED_WINDOW_PLAYLIST_CLOSE,PLEDIT_CLOSE_BUTTON_PRESSED);
		case ePRESSED_WINDOW_PLAYLIST_SCROLLBAR:
			break;
		default:
			break;
	}

	return retval;
}
int window_playlist_draw_main(tHandleWindowPlaylist *pThis,GdkPixbuf *destBuf)
{
	int retval;

	// pThis->list_posx;
	// pThis->list_posy;
	// pThis->list_dimx;
	// pThis->list_dimy;

	tPlayListTheme *pPlayList;
	cairo_font_extents_t extents;
	cairo_surface_t *surface;
	cairo_t *cr;
	GdkPixbuf *pixbuf;

	int lineheight;
	
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

	int i;
	int currentEntry;
	retval=RETVAL_OK;

	retval|=playlist_get_numbers(pThis->pHandlePlayList,&(pThis->list_numberOfEntries),&currentEntry);


	retval|=theme_manager_get_playListTheme(pThis->pHandleThemeManager,&pPlayList);
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

	surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32,pThis->list_dimx,pThis->list_dimy);
	cr=cairo_create(surface);

	cairo_set_source_rgba(cr,color_normalBG_blue,color_normalBG_green,color_normalBG_red,1);		// TODO: why is it the other way around?
	cairo_rectangle(cr,0,0,pThis->list_dimx,pThis->list_dimy);
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
	pThis->list_entriesPerPage=(int)(pThis->list_dimy/lineheight);
//	for (i=0;i<pThis->list_entriesPerPage;i++)
	for (i=pThis->list_entriesPerPage-1;i>=0;i--)		// TODO: for some reason it is beneficial to draw from the bottom up.
	{
		int index;

		index=i+pThis->list_topIndex;
		if (index<pThis->list_numberOfEntries)
		{
			tSongInfo songInfo;
			char selected;
			int pathname_end;
			retval|=playlist_read_entry(pThis->pHandlePlayList,index,&songInfo,&selected);
			if (selected)	// if the entry is selected, 
			{
			        cairo_set_source_rgba(cr,color_selectedBG_blue,color_selectedBG_green,color_selectedBG_red,1);                // TODO: why is it the other way around?
				cairo_rectangle(cr,0,i*lineheight,pThis->list_dimx,lineheight);	// change the background for this line. TODO: this is too high
				cairo_stroke_preserve(cr);
				cairo_fill(cr);
			}
			cairo_move_to(cr, 0, (i+1)*lineheight);
			if (currentEntry==index)	// write the current one in a different color
			{
				cairo_set_source_rgba(cr,color_current_blue,color_current_green,color_current_red,1);		// TODO: why is it the other way around?
			} else {
				cairo_set_source_rgba(cr,color_normal_blue,color_normal_green,color_normal_red,1);		// TODO: why is it the other way around?
			}
			pathname_end=0;
			if (!pThis->show_full_path)
			{
				int j;
				for (j=0;j<strlen(songInfo.filename)-1;j++)
				{
					if (songInfo.filename[j]=='/')
					{
						pathname_end=j+1;
					}
				}
			}	
			cairo_show_text(cr, &songInfo.filename[pathname_end]);
		}
	}
	pixbuf=gdk_pixbuf_new_from_data(cairo_image_surface_get_data(surface),GDK_COLORSPACE_RGB,TRUE,8,pThis->list_dimx,pThis->list_dimy,pThis->list_dimx*4,NULL,NULL);
	gdk_pixbuf_copy_area(pixbuf,0,0,pThis->list_dimx,pThis->list_dimy,destBuf,pThis->list_posx,pThis->list_posy);
	g_object_unref(pixbuf);
	cairo_destroy(cr);
	cairo_surface_destroy(surface);	

	if (pThis->list_numberOfEntries)
	{
		pThis->scrollbarY=pThis->list_posy+(pThis->list_topIndex*(pThis->list_dimy-ELEMENT_HEIGHT(PLEDIT_SCROLL_BUTTON_UNPRESSED)-1))/pThis->list_numberOfEntries;
	} else {
		pThis->scrollbarY=pThis->list_posy;
	}


	return retval;
}
int window_playlist_draw(tHandleWindowPlaylist *pThis,GdkPixbuf *destBuf)
{
	int retval;
	retval=RETVAL_OK;
	gdk_pixbuf_copy_area(pThis->pixbufBackground,0,0,pThis->window_width,pThis->window_height,destBuf,0,0);
	retval|=window_playlist_draw_main(pThis,destBuf);
	retval|=window_playlist_draw_status(pThis,destBuf);
	retval|=window_playlist_draw_pressable(pThis,destBuf);

	return retval;
}


int window_playlist_refresh(tHandleWindowPlaylist* pThis)
{
	int retval;
	retval=RETVAL_OK;
	retval|=window_playlist_draw(pThis,pThis->pixbuf);

	gdk_pixbuf_copy_area(pThis->pixbuf,0,0,pThis->window_width,WINDOW_PLAYLIST_HANDLE_HEIGHT,pThis->pixbuf_handle,0,0);
	gdk_pixbuf_copy_area(pThis->pixbuf,0,WINDOW_PLAYLIST_HANDLE_HEIGHT,pThis->window_width,pThis->window_height-WINDOW_PLAYLIST_HANDLE_HEIGHT,pThis->pixbuf_main,0,0);

	gtk_picture_set_pixbuf(GTK_PICTURE(pThis->picture_handle),pThis->pixbuf_handle);
	gtk_picture_set_pixbuf(GTK_PICTURE(pThis->picture_main),pThis->pixbuf_main);
	

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
int window_playlist_signal_jump_to_entry(tHandleWindowPlaylist* pThis,int currentEntry)
{
	int retval;
	retval=RETVAL_OK;
	retval|=window_playlist_refresh(pThis);
	
	
	return retval;
}
int window_playlist_signal_scalefactor(tHandleWindowPlaylist* pThis,int scaleFactor)
{
	int retval;
	retval=RETVAL_OK;
	pThis->scaleFactor=scaleFactor;
	gtk_window_set_default_size(GTK_WINDOW(pThis->window),scaleFactor*pThis->window_width,scaleFactor*pThis->window_height);
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
	ePressable pressed;
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)g_object_get_data(G_OBJECT(gesture),"pThis");
	pressed=gui_helpers_find_pressable(pThis->boundingBoxes,PRESSABLE_PLAYLIST_NUM,x,y,window,pThis->window_width,pThis->window_height);
	pThis->lastPressed=pressed;
	pThis->pressedX=x;
	pThis->pressedY=y;
	pThis->resizeCols=pThis->columns;
	pThis->resizeRows=pThis->rows;
	pThis->resizeWidth=gtk_widget_get_width(window);
	pThis->resizeHeight=gtk_widget_get_height(window);
	window_playlist_refresh(pThis);
}
static void window_playlist_event_released(GtkGestureClick *gesture, int n_press, double x, double y, GtkWidget *window)
{
	ePressable released;
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)g_object_get_data(G_OBJECT(gesture),"pThis");
	released=gui_helpers_find_pressable(pThis->boundingBoxes,PRESSABLE_PLAYLIST_NUM,x,y,window,pThis->window_width,pThis->window_height);

	pThis->status.menu_add=0;
	pThis->status.menu_remove=0;
	pThis->status.menu_select=0;
	pThis->status.menu_misc=0;
	pThis->status.menu_list=0;

	if (released==pThis->lastPressed && released!=ePRESSED_NONE)
	{
		switch(released)
		{
			case ePRESSED_WINDOW_PLAYLIST_ADD:
				pThis->status.menu_add=1-pThis->status.menu_add;
			break;
			case ePRESSED_WINDOW_PLAYLIST_REMOVE:
				pThis->status.menu_remove=1-pThis->status.menu_remove;
			break;
			case ePRESSED_WINDOW_PLAYLIST_SELECTION:
				pThis->status.menu_select=1-pThis->status.menu_select;
			break;
			case ePRESSED_WINDOW_PLAYLIST_MISCELLANEOUS:
				pThis->status.menu_misc=1-pThis->status.menu_misc;
			break;
			case ePRESSED_WINDOW_PLAYLIST_LIST:
				pThis->status.menu_list=1-pThis->status.menu_list;
			break;
	
			case ePRESSED_WINDOW_PLAYLIST_PREVIOUS:
				controller_event(pThis->pControllerContext,eEVENT_PLAY_PREV_FILE,NULL);
				break;
			case ePRESSED_WINDOW_PLAYLIST_PLAY:
				controller_event(pThis->pControllerContext,eEVENT_PLAY,NULL);
				break;
			case ePRESSED_WINDOW_PLAYLIST_PAUSE:
				controller_event(pThis->pControllerContext,eEVENT_PAUSE,NULL);
				break;
			case ePRESSED_WINDOW_PLAYLIST_STOP:
				controller_event(pThis->pControllerContext,eEVENT_STOP,NULL);
				break;
			case ePRESSED_WINDOW_PLAYLIST_NEXT:
				controller_event(pThis->pControllerContext,eEVENT_PLAY_NEXT_FILE,NULL);
				break;

			case ePRESSED_WINDOW_PLAYLIST_MAIN:
				{
					int index;

					index=pThis->list_topIndex+gui_helpers_relative_value(0,pThis->list_entriesPerPage,pThis->list_posy,pThis->list_posy+pThis->list_dimy,1,x,y,window,pThis->window_width,pThis->window_height);
					if (n_press==1)
					{
						playlist_select_toggle(pThis->pHandlePlayList,index);
						window_playlist_refresh(pThis);
					}
					if (n_press==2)
					{
						tPayload payload;
						tSongInfo songInfo;

						playlist_select_toggle(pThis->pHandlePlayList,index);
						window_playlist_refresh(pThis);
						playlist_read_entry(pThis->pHandlePlayList,index,&songInfo,NULL);
						
						payload.filename=songInfo.filename;
						playlist_set_current_entry(pThis->pHandlePlayList,index);
				                controller_event(pThis->pControllerContext,eEVENT_OPEN_FILE,&payload);
					}
				}
				break;


			case ePRESSED_WINDOW_PLAYLIST_ADD_FILE:
				{
					GtkFileChooserNative *fileChooser;
					fileChooser=gtk_file_chooser_native_new("Open File",
							GTK_WINDOW(pThis->window),
							GTK_FILE_CHOOSER_ACTION_OPEN,
							"_Open",
							"_Cancel");

					g_object_set_data(G_OBJECT(fileChooser),"pThis",pThis);
					g_signal_connect(fileChooser,"response",G_CALLBACK(window_playlist_filechooser_response),NULL);
					gtk_native_dialog_show(GTK_NATIVE_DIALOG(fileChooser));
				}
				break;
			case ePRESSED_WINDOW_PLAYLIST_ADD_DIR:
				{
					GtkFileChooserNative *fileChooser;
					fileChooser=gtk_file_chooser_native_new("Open Directory",
							GTK_WINDOW(pThis->window),
							GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
							"Choose",
							"_Cancel");

					g_object_set_data(G_OBJECT(fileChooser),"pThis",pThis);
					g_signal_connect(fileChooser,"response",G_CALLBACK(window_playlist_dirchooser_response),NULL);
					gtk_native_dialog_show(GTK_NATIVE_DIALOG(fileChooser));
				}
				break;

			case ePRESSED_WINDOW_PLAYLIST_NEW_LIST:
				pThis->status.menu_list=0;
			case ePRESSED_WINDOW_PLAYLIST_REMOVE_ALL:
				{
					playlist_remove_all(pThis->pHandlePlayList);
					window_playlist_refresh(pThis);
				}
				break;
//			case ePRESSED_WINDOW_PLAYLIST_MISC_REMOVE_BUTTON:
//				remove double files
//				remove physical files
//				remove files that do not exist
//				break;

			case ePRESSED_WINDOW_PLAYLIST_CROP_BUTTON:
				{
					playlist_remove_selected(pThis->pHandlePlayList);
					window_playlist_refresh(pThis);
				}
				break;

			case ePRESSED_WINDOW_PLAYLIST_INVERT_SELECTION:
				{
					playlist_select_action(pThis->pHandlePlayList,ePLAYLIST_SELECT_INV);
					window_playlist_refresh(pThis);
				}
				break;
			case ePRESSED_WINDOW_PLAYLIST_SELECT_NONE:
				{
					playlist_select_action(pThis->pHandlePlayList,ePLAYLIST_SELECT_NONE);
					window_playlist_refresh(pThis);
				}
				break;
			case ePRESSED_WINDOW_PLAYLIST_SELECT_ALL:
				{
					playlist_select_action(pThis->pHandlePlayList,ePLAYLIST_SELECT_ALL);
					window_playlist_refresh(pThis);
				}
				break;

			case ePRESSED_WINDOW_PLAYLIST_SAVE_LIST:
				{
					GtkFileChooserNative *fileChooser;
					fileChooser=gtk_file_chooser_native_new("Save playlist ",
							GTK_WINDOW(pThis->window),
							GTK_FILE_CHOOSER_ACTION_SAVE,
							"Save",
							"_Cancel");

					g_object_set_data(G_OBJECT(fileChooser),"pThis",pThis);
					gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (fileChooser), ("Untitled document"));
					g_signal_connect(fileChooser,"response",G_CALLBACK(window_playlist_save_response),NULL);
					gtk_native_dialog_show(GTK_NATIVE_DIALOG(fileChooser));
				}
				break;
			case ePRESSED_WINDOW_PLAYLIST_LOAD_LIST:

				{
					GtkFileChooserNative *fileChooser;
					fileChooser=gtk_file_chooser_native_new("Load Playlist ",
							GTK_WINDOW(pThis->window),
							GTK_FILE_CHOOSER_ACTION_OPEN,
							"_Load",
							"_Cancel");

					g_object_set_data(G_OBJECT(fileChooser),"pThis",pThis);
					g_signal_connect(fileChooser,"response",G_CALLBACK(window_playlist_load_response),NULL);
					gtk_native_dialog_show(GTK_NATIVE_DIALOG(fileChooser));
				}
				break;
			case ePRESSED_WINDOW_PLAYLIST_SORT_LIST:
				gtk_widget_show(GTK_WIDGET(pThis->popUpMenu));
				break;
			case ePRESSED_WINDOW_PLAYLIST_RESIZE:
				window_playlist_refresh_background(pThis);
				window_playlist_refresh(pThis);
				break;	
			case ePRESSED_WINDOW_PLAYLIST_CLOSE:
				window_playlist_close(pThis->window,pThis);	
				break;
			default:
			break;
		}
	}


	pThis->lastPressed=ePRESSED_NONE;
	window_playlist_refresh(pThis);
}
static void window_playlist_event_drag_begin(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
//	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)g_object_get_data(G_OBJECT(gesture),"pThis");
}
static void window_playlist_event_drag_end(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)g_object_get_data(G_OBJECT(gesture),"pThis");
	if (pThis->lastPressed==ePRESSED_WINDOW_PLAYLIST_RESIZE)
	{
		double width;
		double height;
		double scaleX;
		double scaleY;
		int newcols,newrows;
		gtk_window_set_title(GTK_WINDOW(pThis->window),"d11amp playlist");
		pthread_mutex_lock(&pThis->mutex);


		width=gtk_widget_get_width(window);
		height=gtk_widget_get_height(window);
		scaleX=width/pThis->window_width;
		scaleY=height/pThis->window_height;

		

		newrows=(int)(y/(PLAYLIST_ROW_HEIGHT*scaleY))+pThis->resizeRows;
		newcols=(int)(x/(PLAYLIST_COL_WIDTH*scaleX))+pThis->resizeCols;;

		if (newrows!=pThis->rows || newcols!=pThis->columns)
		{
			gtk_window_set_title(GTK_WINDOW(pThis->window),"d11amp playlist");
			window_playlist_resize(pThis,newrows,newcols);
		}

		window_playlist_refresh_background(pThis);
		window_playlist_refresh(pThis);


		

		pthread_mutex_unlock(&pThis->mutex);
	}
}

static void window_playlist_event_drag_update(GtkGestureDrag *gesture, double x, double y, GtkWidget *window)
{
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)g_object_get_data(G_OBJECT(gesture),"pThis");
	if (pThis->lastPressed==ePRESSED_WINDOW_PLAYLIST_RESIZE)
	{
		double width;
		double height;
		double scaleX;
		double scaleY;
		int newcols,newrows;
		pthread_mutex_lock(&pThis->mutex);


		width=gtk_widget_get_width(window);
		height=gtk_widget_get_height(window);
		scaleX=width/pThis->window_width;
		scaleY=height/pThis->window_height;



		newrows=(int)(y/(PLAYLIST_ROW_HEIGHT*scaleY))+pThis->resizeRows;
		newcols=(int)(x/(PLAYLIST_COL_WIDTH*scaleX))+pThis->resizeCols;;

		if (newrows!=pThis->rows || newcols!=pThis->columns)
		{
//			window_playlist_resize(pThis,newrows,newcols);
			char fixme[64];
			snprintf(fixme,64,"FIXME: %d x %d\n",newcols,newrows);
			gtk_window_set_title(GTK_WINDOW(pThis->window),fixme);
			

		}
		window_playlist_refresh_background(pThis);
		window_playlist_refresh(pThis);
		pthread_mutex_unlock(&pThis->mutex);
	}
	if (pThis->lastPressed==ePRESSED_WINDOW_PLAYLIST_SCROLLBAR && pThis->list_numberOfEntries!=0)
	{
		int newTopIndex;
		newTopIndex=gui_helpers_relative_value(0,(pThis->list_numberOfEntries-1),pThis->list_posy,pThis->list_posy+pThis->list_dimy,1,pThis->pressedX+x,pThis->pressedY+y,window,pThis->window_width,pThis->window_height);
		if (newTopIndex>=0 && newTopIndex<=pThis->list_numberOfEntries)
		{
			pThis->list_topIndex=newTopIndex;
		}
		window_playlist_refresh(pThis);
	}
}
static gboolean window_playlist_event_scroll(GtkEventControllerScroll *controller,double dx,double dy,GtkWidget *window)
{
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)g_object_get_data(G_OBJECT(controller),"pThis");
	int newTopIndex;

	newTopIndex=pThis->list_topIndex+(int)dy;
	if (newTopIndex>=pThis->list_numberOfEntries)
	{
		newTopIndex=pThis->list_numberOfEntries-1;
	}
	if (newTopIndex<0) 
	{
		newTopIndex=0;
	}
	pThis->list_topIndex=newTopIndex;
	window_playlist_refresh(pThis);

	return TRUE;

}

static gboolean window_playlist_close(GtkWidget *widget,gpointer user_data)
{
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)user_data;
	tPayload payload;
	payload.hide0show1=0;
	controller_event(pThis->pControllerContext,eEVENT_WINDOW_PLAYLIST,&payload);
	return TRUE;
}

static void window_playlist_filechooser_response(GtkNativeDialog *native,int response)
{
	if (response==GTK_RESPONSE_ACCEPT)
	{
		//tPayload payload;
		tSongInfo songInfo;
		GtkFileChooser *fileChooser=GTK_FILE_CHOOSER(native);
		GFile *chosen=gtk_file_chooser_get_file(fileChooser);
		tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)g_object_get_data(G_OBJECT(native),"pThis");
		pThis->status.menu_add=0;
		
		strncpy(songInfo.filename,(char*)g_file_get_parse_name(chosen),sizeof(songInfo.filename));
		playlist_add_entry(pThis->pHandlePlayList,&songInfo);
		window_playlist_refresh(pThis);
	}
	g_object_unref(native);
}

static void window_playlist_dirchooser_response(GtkNativeDialog *native,int response)
{
	if (response==GTK_RESPONSE_ACCEPT)
	{
		//int retval;
		GtkFileChooser *fileChooser=GTK_FILE_CHOOSER(native);
		GFile *chosen=gtk_file_chooser_get_file(fileChooser);
		tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)g_object_get_data(G_OBJECT(native),"pThis");
		pThis->status.menu_add=0;
		
		playlist_add_dir(pThis->pHandlePlayList,(char*)g_file_get_parse_name(chosen));
		window_playlist_refresh(pThis);
	}
	g_object_unref(native);
}

static void window_playlist_load_response(GtkNativeDialog *native,int response)
{
	if (response==GTK_RESPONSE_ACCEPT)
	{
		//tPayload payload;
		GtkFileChooser *fileChooser=GTK_FILE_CHOOSER(native);
		GFile *chosen=gtk_file_chooser_get_file(fileChooser);
		tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)g_object_get_data(G_OBJECT(native),"pThis");
		pThis->status.menu_list=0;
		
		playlist_load_m3u(pThis->pHandlePlayList,g_file_get_parse_name(chosen));
		window_playlist_refresh(pThis);
	}
	g_object_unref(native);
}

static void window_playlist_save_response(GtkNativeDialog *native,int response)
{
	if (response==GTK_RESPONSE_ACCEPT)
	{
		//tPayload payload;
		GtkFileChooser *fileChooser=GTK_FILE_CHOOSER(native);
		GFile *chosen=gtk_file_chooser_get_file(fileChooser);
		tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)g_object_get_data(G_OBJECT(native),"pThis");
		pThis->status.menu_list=0;
		
		playlist_save_m3u(pThis->pHandlePlayList,g_file_get_parse_name(chosen),1);
		window_playlist_refresh(pThis);
	}
	g_object_unref(native);
}
static void window_playlist_sort_by_filename(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)user_data;
	playlist_sort(pThis->pHandlePlayList,ePLAYLIST_SORT_FILENAME);
	window_playlist_refresh(pThis);
}
//static void window_playlist_sort_by_path(GSimpleAction *action, GVariant *parameter, gpointer user_data)
//{
//	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)user_data;
//	playlist_sort(pThis->pHandlePlayList,ePLAYLIST_SORT_PATH);
//	window_playlist_refresh(pThis);
//}

static void window_playlist_sort_randomize(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)user_data;
	playlist_randomize(pThis->pHandlePlayList);
	window_playlist_refresh(pThis);
}
static void window_playlist_sort_reverse(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)user_data;
	playlist_reverse(pThis->pHandlePlayList);
	window_playlist_refresh(pThis);
}

int window_playlist_get_preferences_widget(tHandleWindowPlaylist* pThis,GtkWidget** pWidget)
{
	int retval;
	retval=RETVAL_OK;

	*pWidget=gtk_box_new(GTK_ORIENTATION_VERTICAL,3);
	pThis->pref_check=gtk_check_button_new_with_label("Show full path");
	gtk_box_append(GTK_BOX(*pWidget),pThis->pref_check);

	return retval;
}

int window_playlist_populate_preferences(void* pContext)
{
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)pContext;
	int retval;
	retval=RETVAL_OK;

	gtk_check_button_set_active(GTK_CHECK_BUTTON(pThis->pref_check),pThis->show_full_path);
	return retval;
}
int window_playlist_apply_preferences(void* pContext)
{
	tHandleWindowPlaylist* pThis=(tHandleWindowPlaylist*)pContext;
	pThis->show_full_path=gtk_check_button_get_active(GTK_CHECK_BUTTON(pThis->pref_check));
	window_playlist_refresh(pThis);

	return RETVAL_OK;
}
	
