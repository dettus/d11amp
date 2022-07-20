#include <gtk/gtk.h>
#include "pixbufloader.h"

int main(int argc, char *argv[]) 
{
	tPixbufs pixbufs;
	GtkWidget *window;
	GtkWidget *image;
	GtkWidget *layout;

	gtk_init(&argc, &argv);

	loadpixbuf(&pixbufs,"../wsz");

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	layout = gtk_layout_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER (window), layout);
	gtk_widget_show(layout);

	image=gtk_image_new();
	gtk_image_set_from_pixbuf(GTK_IMAGE(image),pixbufs.mainbmp);
	gtk_widget_queue_draw(image);
	gtk_widget_show(image);
	gtk_layout_put(GTK_LAYOUT(layout),image,20,20);	
	gtk_widget_show(window);

	g_signal_connect(window, "destroy",
			G_CALLBACK(gtk_main_quit), NULL);  

	gtk_main();

	return 0;
}

