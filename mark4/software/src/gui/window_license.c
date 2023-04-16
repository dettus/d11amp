/*
Copyright 2023, dettus@dettus.net

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
#include "window_license.h"
#include "license.h"
#include "version.h"
#include <string.h>

static gboolean window_license_clicked(GtkWidget *widget,gpointer data);

#define	CALC_WIDTH(a,x)   ((a)<(x)?(a):(x))
#define	CALC_HEIGHT(a,x)  ((a)+(x))
int window_license_init(tHandleWindowLicense* pThis,GtkApplication *pApp)
{
	char title[64];
	int width;
	int height;
	memset(pThis,0,sizeof(tHandleWindowLicense));
	pThis->pApp=pApp;
	pThis->window=gtk_application_window_new(pApp);
	snprintf(title,64,"d11amp %d.%d%d License",VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION);
	gtk_window_set_title(GTK_WINDOW(pThis->window),title);
	
	pThis->box=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
        gtk_box_set_homogeneous(GTK_BOX(pThis->box),FALSE);
	gtk_window_set_child(GTK_WINDOW(pThis->window),pThis->box);
	

	pThis->label=gtk_label_new("BSD-2-clause");
	pThis->textBuffer=gtk_text_buffer_new(NULL);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(pThis->textBuffer),d11amp_license,strlen(d11amp_license));
	pThis->textview=gtk_text_view_new_with_buffer(GTK_TEXT_BUFFER(pThis->textBuffer));

	pThis->button=gtk_button_new_with_label("OK");
	gtk_box_append(GTK_BOX(pThis->box),pThis->label);
	gtk_box_append(GTK_BOX(pThis->box),pThis->textview);
	gtk_box_append(GTK_BOX(pThis->box),pThis->button);

        gtk_widget_set_halign(pThis->label,GTK_ALIGN_FILL);
        gtk_widget_set_halign(pThis->textview,GTK_ALIGN_FILL);
        gtk_widget_set_halign(pThis->button,GTK_ALIGN_FILL);

        gtk_widget_set_valign(pThis->label,GTK_ALIGN_FILL);
        gtk_widget_set_valign(pThis->textview,GTK_ALIGN_FILL);
        gtk_widget_set_valign(pThis->button,GTK_ALIGN_FILL);

	width=WINDOW_MIN_WIDTH;
	height=WINDOW_MIN_HEIGHT;

//	width=CALC_WIDTH(width,gtk_widget_get_width(pThis->label));
//	width=CALC_WIDTH(width,gtk_widget_get_width(pThis->textview));
//	width=CALC_WIDTH(width,gtk_widget_get_width(pThis->button));

//	height=CALC_HEIGHT(height,gtk_widget_get_height(pThis->label));
//	height=CALC_HEIGHT(height,gtk_widget_get_height(pThis->textview));
//	height=CALC_HEIGHT(height,gtk_widget_get_height(pThis->button));


	gtk_window_set_default_size(GTK_WINDOW(pThis->window),width,height);


	g_signal_connect (pThis->button, "clicked", G_CALLBACK (window_license_clicked), (void*)pThis);
	g_signal_connect(G_OBJECT(pThis->window), "destroy", G_CALLBACK (window_license_clicked), (void*)pThis);
	g_signal_connect(G_OBJECT(pThis->window), "close_request", G_CALLBACK (window_license_clicked), (void*)pThis);
	return RETVAL_OK;	
}
int window_license_show(tHandleWindowLicense* pThis)
{
	gtk_widget_set_visible(pThis->window,TRUE);
	return RETVAL_OK;	
}
int window_license_hide(tHandleWindowLicense* pThis)
{
	gtk_widget_set_visible(pThis->window,FALSE);
	return RETVAL_OK;	
}

static gboolean window_license_clicked(GtkWidget *widget,gpointer user_data)
{
	tHandleWindowLicense* pThis=(tHandleWindowLicense*)user_data;
	window_license_hide(pThis);
	return TRUE;
}

