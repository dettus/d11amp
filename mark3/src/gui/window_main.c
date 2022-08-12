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
#include "window_main.h"

int window_main_event_pressed(GtkWidget *widget, double x,double y,guint event_button, gpointer data)
{
	tHandleWindowMain* pThis=(tHandleWindowMain*)data;
}
int window_main_event_released(GtkWidget *widget, double x,double y,guint event_button, gpointer data)
{
	tHandleWindowMain* pThis=(tHandleWindowMain*)data;
}

int window_main_init(GtkApplication* app,tHandleWindowMain* pThis)
{
	GError *err=NULL;
	memset(pThis,0,sizeof(tHandleWindowMain));


	pThis->scaleFactor=4;
	pThis->pixbuf=gdk_pixbuf_new();
	pThis->picture=gtk_picture_new_for_pixbuf(pixbuf);
	pThis->windowMain=gtk_application_window_new(app);
	gtk_window_set_child(GTK_WINDOW(pThis->windowMain),pThis->picture);
	gtk_window_set_title(GTK_WINDOW(pThis->windowMain),"d11amp main");
	gtk_window_set_default_size(GTK_WINDOW(pThis->windowMain),275*pThis->scaleFactor,116*pThis->scaleFactor);

	gesture=gtk_gesture_click_new();
	g_signal_connect(gesture,"pressed",G_CALLBACK(window_main_event_pressed),pThis->windowMain);
	g_signal_connect(gesture,"released",G_CALLBACK(window_main_event_released),pThis->windowMain);
	return RETVAL_OK;
}

int window_main_show(tHandleWindowMain* pThis)
{
	gtk_widget_show(pThis->windowMain);
	return RETVAL_OK;
}
