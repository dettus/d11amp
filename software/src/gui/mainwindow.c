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

void mainwindow_redraw(GtkWidget* widget,int width,int height,tHandleMainWindow* pThis)
{
	// TODO: add the elements here
	printf("width:%d height:%d   %p\n",width,height,pThis->mainPixbuf);
	//pixbufloader_addelement(pThis->pHandlePixbufLoader,pThis->mainPixbuf,0,14,MAIN_MAIN_DISPLAY);
	gdk_pixbuf_save(pThis->mainPixbuf,"test.png","png",NULL);
	gtk_image_set_from_pixbuf(GTK_IMAGE(pThis->mainImage),pThis->mainPixbuf);
	gtk_widget_queue_draw(pThis->mainImage);
	gtk_widget_queue_draw(widget);
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


	mainwindow_redraw(widget,pThis->width,pThis->height,pThis);	
}

int mainwindow_init(tHandleMainWindow* pThis,tHandlePixbufLoader *pPixbuf)
{
	// step 1: create the pixbuf as the "drawing area"
	pThis->pHandlePixbufLoader=pPixbuf;
	pThis->mainPixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE,8,275,116);
	printf("%p\n",pThis->mainPixbuf);



	pThis->mainWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(pThis->mainWindow),275,116);
	pThis->layout=gtk_layout_new(NULL,NULL);
	gtk_container_add(GTK_CONTAINER(pThis->mainWindow),pThis->layout);
	gtk_widget_show(pThis->layout);
	pThis->mainImage=gtk_image_new();
	gtk_image_set_from_pixbuf(GTK_IMAGE(pThis->mainImage),pThis->mainPixbuf);
	gtk_widget_queue_draw(pThis->mainImage);
	gtk_layout_put(GTK_LAYOUT(pThis->layout),pThis->mainImage,0,0);
	gtk_widget_show(pThis->mainImage);
	gtk_widget_show(pThis->mainWindow);
	pThis->scalefactor=1;
	g_signal_connect(pThis->mainWindow, "size-allocate", G_CALLBACK(mainwindow_allocate), pThis);
	gtk_window_set_default_size(GTK_WINDOW(pThis->mainWindow), 275*pThis->scalefactor, 116*pThis->scalefactor);
        g_signal_connect(pThis->mainWindow, "destroy",G_CALLBACK(gtk_main_quit), NULL);





	return MAINWINDOW_OK;
	
}


