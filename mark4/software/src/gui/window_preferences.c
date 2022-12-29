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

#include "window_preferences.h"
static gboolean window_preferences_okay_clicked(GtkWidget *widget,gpointer data);
static gboolean window_preferences_apply_clicked(GtkWidget *widget,gpointer data);
static gboolean window_preferences_cancel_clicked(GtkWidget *widget,gpointer data);

int window_preferences_init(tHandleWindowPreferences *pThis,void *pControllerContext,GtkApplication *app)
{
	int retval;
	GtkWidget *layout_vbox;
	GtkWidget *buttons_hbox;
	retval=RETVAL_OK;

	memset(pThis,0,sizeof(tHandleWindowPreferences));
	pThis->app=app;
	pThis->pControllerContext=pControllerContext;

	pThis->window=gtk_application_window_new(pThis->app);
	layout_vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL,3);
	gtk_box_set_homogeneous(GTK_BOX(layout_vbox),TRUE);
	gtk_window_set_child(GTK_WINDOW(pThis->window),layout_vbox);
	pThis->notebook=gtk_notebook_new();

	gtk_widget_set_valign(pThis->notebook,GTK_ALIGN_FILL);
	gtk_widget_set_halign(pThis->notebook,GTK_ALIGN_FILL);
	gtk_box_append(GTK_BOX(layout_vbox),pThis->notebook);


	g_signal_connect(G_OBJECT(pThis->window), "destroy", G_CALLBACK (window_preferences_cancel_clicked), (void*)pThis);
	g_signal_connect(G_OBJECT(pThis->window), "close_request", G_CALLBACK (window_preferences_cancel_clicked), (void*)pThis);


	buttons_hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,3);
	gtk_widget_set_valign(buttons_hbox,GTK_ALIGN_END);
	gtk_widget_set_halign(buttons_hbox,GTK_ALIGN_FILL);
	gtk_box_set_homogeneous(GTK_BOX(buttons_hbox),TRUE);

	pThis->button_okay  =gtk_button_new_with_label("Okay");
	pThis->button_apply =gtk_button_new_with_label("Apply");
	pThis->button_cancel=gtk_button_new_with_label("Cancel");

	g_signal_connect(pThis->button_okay,  "clicked",G_CALLBACK(window_preferences_okay_clicked),(void*)pThis);
	g_signal_connect(pThis->button_apply, "clicked",G_CALLBACK(window_preferences_apply_clicked),(void*)pThis);
	g_signal_connect(pThis->button_cancel,"clicked",G_CALLBACK(window_preferences_cancel_clicked),(void*)pThis);

	gtk_box_append(GTK_BOX(buttons_hbox),pThis->button_okay  );
	gtk_box_append(GTK_BOX(buttons_hbox),gtk_label_new(""));
	gtk_box_append(GTK_BOX(buttons_hbox),pThis->button_apply );
	gtk_box_append(GTK_BOX(buttons_hbox),gtk_label_new(""));
	gtk_box_append(GTK_BOX(buttons_hbox),pThis->button_cancel);


	gtk_box_append(GTK_BOX(layout_vbox),buttons_hbox);
	

	return retval;
	
}
int window_preferences_show(tHandleWindowPreferences *pThis)
{
	// TODO: call the controller to set the preferences widgets to its current values
	gtk_widget_show(pThis->window);
	return RETVAL_OK;
}
int window_preferences_hide(tHandleWindowPreferences *pThis)
{
	gtk_widget_hide(pThis->window);
	return RETVAL_OK;
}
int window_preferences_add_page(tHandleWindowPreferences *pThis,GtkWidget* pWidget,char* label)
{
	gtk_notebook_append_page(GTK_NOTEBOOK(pThis->notebook),pWidget,gtk_label_new(label));
	return RETVAL_OK;
}


/// callbacks
static gboolean window_preferences_apply_clicked(GtkWidget *widget,gpointer data)
{
	// TODO: call the controller to set the modules to the settings

	return TRUE;
}
static gboolean window_preferences_cancel_clicked(GtkWidget *widget,gpointer data)
{
	tHandleWindowPreferences* pThis=(tHandleWindowPreferences*)data;
	window_preferences_hide(pThis);	
	return TRUE;
}
static gboolean window_preferences_okay_clicked(GtkWidget *widget,gpointer data)
{
	window_preferences_apply_clicked(widget,data);	// apply the changes
	window_preferences_cancel_clicked(widget,data);	// just close the window
	return TRUE;
}

