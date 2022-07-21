#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "elements.h"
#include "pixbufloader.h"
#define	MAGIC	0x79486568	// "yHeh" = The place where I grew up.
typedef struct _tTopHandle
{
	unsigned int magic;
	void* hPixbufLoader;
        int width;
        int height;
        GtkWidget* image;
        GdkPixbuf *pixbuf;
        GdkPixbuf *scaled;
} tTopHandle;

tTopHandle topHandle;	// I DO NOT LIKE TO PLACE MEMORY IN THIS SHARED REGION. At least, this one is structured

void redraw_image(GtkWidget *widget,int width,int height,void* handle)
{
	tTopHandle* pThis=(tTopHandle*)handle;

	g_object_unref(pThis->scaled);
	pThis->width=width;
	pThis->height=height;
	pThis->scaled=gdk_pixbuf_scale_simple(pThis->pixbuf,width,height,GDK_INTERP_NEAREST);
	gtk_image_set_from_pixbuf(GTK_IMAGE(pThis->image),pThis->scaled);
	gtk_widget_queue_draw(pThis->image);
	gtk_widget_queue_draw(widget);
}
void allocate_event(GtkWidget *widget, GtkAllocation *allocation, gpointer user_data)
{
	int width,height;
	tTopHandle *pThis=(tTopHandle*)user_data;
	width=(allocation->width);
	height=(allocation->height);
	pThis->width=width;
	pThis->height=height;
	redraw_image(widget,width,height,pThis);
}



int main(int argc, char *argv[]) 
{
	GtkWidget *window;
	GtkWidget *layout;
	int bytes;

	pixbufloader_getsize(&bytes);
	topHandle.magic=MAGIC;
	topHandle.hPixbufLoader=malloc(bytes);
	topHandle.image=NULL;
	topHandle.pixbuf=NULL;
	topHandle.scaled=NULL;	
	

	gtk_init(&argc, &argv);

	pixbufloader_open(topHandle.hPixbufLoader,"../wsz");
	pixbufloader_initmainwindow(topHandle.hPixbufLoader,&(topHandle.pixbuf));

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), 275, 116);
//	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
//	gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
	layout = gtk_layout_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER (window), layout);
	gtk_widget_show(layout);

	topHandle.image=gtk_image_new();
	gtk_image_set_from_pixbuf(GTK_IMAGE(topHandle.image),topHandle.pixbuf);
	gtk_widget_queue_draw(topHandle.image);
	gtk_layout_put(GTK_LAYOUT(layout),topHandle.image, 0, 0);
	gtk_widget_show(topHandle.image);
	gtk_widget_show(window);

	g_signal_connect(window, "destroy",
			G_CALLBACK(gtk_main_quit), NULL);  
        g_signal_connect(window, "size-allocate", G_CALLBACK(allocate_event), &(topHandle));

	gtk_main();

	return 0;
}

