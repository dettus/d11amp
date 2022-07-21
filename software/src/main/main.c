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
int pressed=0;
int last_x=0x12345678;
int last_y=0x12345678;
static gboolean mouse_press_event(GtkWidget *widget,GdkEventButton* event,gpointer user_data)
{
	pressed=1;
	printf("x:%f y:%f\n",(int)event->x,(int)event->y);	
	return TRUE;
}
static gboolean mouse_release_event(GtkWidget *widget,GdkEventButton event,gpointer user_data)
{
	pressed=0;
	last_x=0x12345678;
	last_y=0x12345678;	
	return TRUE;
}
static gboolean mouse_moved(GtkWidget *widget,GdkEvent *event,gpointer user_data)
{
	gint windowx;
	gint windowy;
	gint curx;
	gint cury;
	int deltax;
	int deltay;
	GdkEventMotion* e=(GdkEventMotion*)event;

	gtk_window_get_position(GTK_WINDOW(widget),&windowx,&windowy);
	gdk_window_get_device_position(e->window, gdk_seat_get_pointer(gdk_display_get_default_seat(gtk_widget_get_display(widget))), &curx, &cury, 0);
	curx+=windowx;
	cury+=windowy;


	if (last_x==0x12345678)
	{
		last_x=curx;
	}

	if (last_y==0x12345678)
	{
		last_y=cury;
	}

	deltax=curx-last_x;
	deltay=cury-last_y;
	last_x=curx;
	last_y=cury;
	printf("curx:%d cury:%d deltax:%d deltay:%d\n",curx,cury,deltax,deltay);
	gtk_window_move(GTK_WINDOW(widget),windowx+deltax,windowy+deltay);

	return TRUE;
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
	gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
	layout = gtk_layout_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER (window), layout);
	gtk_widget_show(layout);

	topHandle.image=gtk_image_new();
	gtk_image_set_from_pixbuf(GTK_IMAGE(topHandle.image),topHandle.pixbuf);
	gtk_widget_queue_draw(topHandle.image);
	gtk_layout_put(GTK_LAYOUT(layout),topHandle.image, 0, 0);
	gtk_widget_show(topHandle.image);
	gtk_widget_show(window);
	g_signal_connect(window, "motion-notify-event",G_CALLBACK(mouse_moved), NULL);
	g_signal_connect(window, "destroy",G_CALLBACK(gtk_main_quit), NULL);  
        g_signal_connect(window, "size-allocate", G_CALLBACK(allocate_event), &(topHandle));
        g_signal_connect(window, "button-press-event", G_CALLBACK(mouse_press_event), &(topHandle));
        g_signal_connect(window, "button-release-event", G_CALLBACK(mouse_release_event), &(topHandle));
	gtk_window_move(GTK_WINDOW(window),15,15);

	gtk_main();

	return 0;
}

