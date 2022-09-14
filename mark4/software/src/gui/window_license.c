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
#include "window_license.h"
#include "license.h"

static void window_license_clicked(GtkWidget *widget,gpointer data);

int window_license_init(tWindowLicense* pThis,GtkApplication *pApp)
{
	pThis->pApp=pApp;
	pThis->window=gtk_application_window_new(pApp);
	gtk_window_set_title(GTK_WINDOW(pThis->window),"d11amp License");
	
	pThis->box=gtk_box_new(GTK_ORIENTATION_VERTICAL,1);
	gtk_window_set_child(GTK_WINDOW(window),pThis->box);
	

	pThis->label=gtk_label_new("BSD-2-clause");
	pThis->textBuffer=gtk_text_buffer_new(NULL);
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(pThis->textBuffer),d11amp_license,strlen(d11amp_license));
	pThis->textview=gtk_text_view_new_with_buffer(GTK_TEXT_BUFFER(pThis->textBuffer));

	pThis->button=gtk_button_new_with_label("OK");
	gtk_box_append(GTK_BOX(pThis->box),pThis->label);
	gtk_box_append(GTK_BOX(pThis->box),pThis->textview);
	gtk_box_append(GTK_BOX(pThis->box),pThis->button);

	gtk_window_set_default_size(GTK_WINDOW(window),600,600);


	g_signal_connect (pThis->button, "clicked", G_CALLBACK (window_license_clicked), (void*)pThis);
	
}
int window_license_show(tWindowLicense* pThis)
{
	gtk_window_show(pThis->window);
}
int window_license_hide(tWindowLicense* pThis)
{
	gtk_window_hide(pThis->window);
}

static void window_license_clicked(GtkWidget *widget,gpointer user_data)
{
	tWindowLicense* pThis=(tWindowLicense*)user_data;
	window_license_hide(pThis);
}

