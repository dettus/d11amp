#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "elements.h"
#include "pixbufloader.h"

int main(int argc, char *argv[]) 
{
	tPixbufs pixbufs;
	GdkPixbuf *mypixbuf;
	GtkWidget *window;
	GtkWidget *image;
	GtkWidget *layout;

	gtk_init(&argc, &argv);

	loadpixbuf(&pixbufs,"../wsz");
	mypixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB, 1, 8,275,116);
	gdk_pixbuf_copy_area(pixbufs.mainbmp, 0,0,275,116,mypixbuf,0,0);


//	mypixbuf=gdk_pixbuf_copy(pixbufs.mainbmp);
	gdk_pixbuf_copy_area(pixbufs.titlebarbmp,cElements[TITLEBAR_NORMAL_TITLEBAR_ACTIVE].startx,cElements[TITLEBAR_NORMAL_TITLEBAR_ACTIVE].starty,cElements[TITLEBAR_NORMAL_TITLEBAR_ACTIVE].dimx,cElements[TITLEBAR_NORMAL_TITLEBAR_ACTIVE].dimy, mypixbuf, 0,0);
	gdk_pixbuf_copy_area(pixbufs.mainbmp,cElements[MAIN_MAIN_DISPLAY].startx,cElements[MAIN_MAIN_DISPLAY].starty,cElements[MAIN_MAIN_DISPLAY].dimx,cElements[MAIN_MAIN_DISPLAY].dimy, mypixbuf, 0,14);
	gdk_pixbuf_copy_area(pixbufs.titlebarbmp,cElements[TITLEBAR_CLUTTERBAR_SHOWN].startx,cElements[TITLEBAR_CLUTTERBAR_SHOWN].starty,cElements[TITLEBAR_CLUTTERBAR_SHOWN].dimx,cElements[TITLEBAR_CLUTTERBAR_SHOWN].dimy, mypixbuf, 10,22);
	gdk_pixbuf_copy_area(pixbufs.numbersbmp,cElements[NUMBERS_1].startx,cElements[NUMBERS_1].starty,cElements[NUMBERS_1].dimx,cElements[NUMBERS_1].dimy, mypixbuf, 48,26);
	gdk_pixbuf_copy_area(pixbufs.numbersbmp,cElements[NUMBERS_3].startx,cElements[NUMBERS_3].starty,cElements[NUMBERS_3].dimx,cElements[NUMBERS_3].dimy, mypixbuf, 60,26);
	gdk_pixbuf_copy_area(pixbufs.numbersbmp,cElements[NUMBERS_3].startx,cElements[NUMBERS_3].starty,cElements[NUMBERS_3].dimx,cElements[NUMBERS_3].dimy, mypixbuf, 78,26);
	gdk_pixbuf_copy_area(pixbufs.numbersbmp,cElements[NUMBERS_7].startx,cElements[NUMBERS_7].starty,cElements[NUMBERS_7].dimx,cElements[NUMBERS_7].dimy, mypixbuf, 90,26);
	gdk_pixbuf_copy_area(pixbufs.playpausbmp,cElements[PLAYPAUSE_PLAY_INDICATOR].startx,cElements[PLAYPAUSE_PLAY_INDICATOR].starty,cElements[PLAYPAUSE_PLAY_INDICATOR].dimx,cElements[PLAYPAUSE_PLAY_INDICATOR].dimy, mypixbuf, 24,18);
	gdk_pixbuf_copy_area(pixbufs.monosterbmp,cElements[MONOSTER_STEREO_ACTIVE].startx,cElements[MONOSTER_STEREO_ACTIVE].starty,cElements[MONOSTER_STEREO_ACTIVE].dimx,cElements[MONOSTER_STEREO_ACTIVE].dimy, mypixbuf, 212,41);
	gdk_pixbuf_copy_area(pixbufs.monosterbmp,cElements[MONOSTER_MONO_INACTIVE].startx,cElements[MONOSTER_MONO_INACTIVE].starty,cElements[MONOSTER_MONO_INACTIVE].dimx,cElements[MONOSTER_MONO_INACTIVE].dimy, mypixbuf, 239,41);
	gdk_pixbuf_copy_area(pixbufs.volumebmp,cElements[VOLUME_023_025].startx,cElements[VOLUME_023_025].starty,cElements[VOLUME_023_025].dimx,cElements[VOLUME_023_025].dimy, mypixbuf, 107,57);
	gdk_pixbuf_copy_area(pixbufs.volumebmp,cElements[VOLUME_SLIDER_UNPRESSED].startx,cElements[VOLUME_SLIDER_UNPRESSED].starty,cElements[VOLUME_SLIDER_UNPRESSED].dimx,cElements[VOLUME_SLIDER_UNPRESSED].dimy, mypixbuf, 117,57);
	gdk_pixbuf_copy_area(pixbufs.balancebmp,cElements[BALANCE_74LEFTORRIGHT].startx,cElements[BALANCE_74LEFTORRIGHT].starty,cElements[BALANCE_74LEFTORRIGHT].dimx,cElements[BALANCE_74LEFTORRIGHT].dimy, mypixbuf, 177,57);
	gdk_pixbuf_copy_area(pixbufs.balancebmp,cElements[BALANCE_SLIDER_PRESSED].startx,cElements[BALANCE_SLIDER_PRESSED].starty,cElements[BALANCE_SLIDER_PRESSED].dimx,cElements[BALANCE_SLIDER_PRESSED].dimy, mypixbuf, 190,57);
	gdk_pixbuf_copy_area(pixbufs.shufrepbmp,cElements[SHUFREP_EQUALIZER_UNPRESSED].startx,cElements[SHUFREP_EQUALIZER_UNPRESSED].starty,cElements[SHUFREP_EQUALIZER_UNPRESSED].dimx,cElements[SHUFREP_EQUALIZER_UNPRESSED].dimy, mypixbuf, 219,58);
	gdk_pixbuf_copy_area(pixbufs.shufrepbmp,cElements[SHUFREP_PLAYLIST_UNPRESSED].startx,cElements[SHUFREP_PLAYLIST_UNPRESSED].starty,cElements[SHUFREP_PLAYLIST_UNPRESSED].dimx,cElements[SHUFREP_PLAYLIST_UNPRESSED].dimy, mypixbuf, 242,58);
	gdk_pixbuf_copy_area(pixbufs.posbarbmp,cElements[POSBAR_SONG_PROGRESS_BAR].startx,cElements[POSBAR_SONG_PROGRESS_BAR].starty,cElements[POSBAR_SONG_PROGRESS_BAR].dimx,cElements[POSBAR_SONG_PROGRESS_BAR].dimy, mypixbuf, 16,72);
	gdk_pixbuf_copy_area(pixbufs.posbarbmp,cElements[POSBAR_SONG_SLIDER_UNPRESSED].startx,cElements[POSBAR_SONG_SLIDER_UNPRESSED].starty,cElements[POSBAR_SONG_SLIDER_UNPRESSED].dimx,cElements[POSBAR_SONG_SLIDER_UNPRESSED].dimy, mypixbuf, 76,72);
	gdk_pixbuf_copy_area(pixbufs.shufrepbmp,cElements[SHUFREP_SHUFFLE_UNPRESSED].startx,cElements[SHUFREP_SHUFFLE_UNPRESSED].starty,cElements[SHUFREP_SHUFFLE_UNPRESSED].dimx,cElements[SHUFREP_SHUFFLE_UNPRESSED].dimy, mypixbuf, 164,89);
	gdk_pixbuf_copy_area(pixbufs.shufrepbmp,cElements[SHUFREP_REPEAT_UNPRESSED].startx,cElements[SHUFREP_REPEAT_UNPRESSED].starty,cElements[SHUFREP_REPEAT_UNPRESSED].dimx,cElements[SHUFREP_REPEAT_UNPRESSED].dimy, mypixbuf, 211,89);

	gdk_pixbuf_copy_area(pixbufs.cbuttonsbmp,cElements[CBUTTONS_PREV_BUTTON_UNPRESSED].startx,cElements[CBUTTONS_PREV_BUTTON_UNPRESSED].starty,cElements[CBUTTONS_PREV_BUTTON_UNPRESSED].dimx,cElements[CBUTTONS_PREV_BUTTON_UNPRESSED].dimy, mypixbuf,16,88);
	gdk_pixbuf_copy_area(pixbufs.cbuttonsbmp,cElements[CBUTTONS_PLAY_BUTTON_PRESSED].startx,cElements[CBUTTONS_PLAY_BUTTON_PRESSED].starty,cElements[CBUTTONS_PLAY_BUTTON_PRESSED].dimx,cElements[CBUTTONS_PLAY_BUTTON_PRESSED].dimy, mypixbuf,39,88);
	gdk_pixbuf_copy_area(pixbufs.cbuttonsbmp,cElements[CBUTTONS_PAUSE_BUTTON_UNPRESSED].startx,cElements[CBUTTONS_PAUSE_BUTTON_UNPRESSED].starty,cElements[CBUTTONS_PAUSE_BUTTON_UNPRESSED].dimx,cElements[CBUTTONS_PAUSE_BUTTON_UNPRESSED].dimy, mypixbuf,62,88);
	gdk_pixbuf_copy_area(pixbufs.cbuttonsbmp,cElements[CBUTTONS_STOP_BUTTON_UNPRESSED].startx,cElements[CBUTTONS_STOP_BUTTON_UNPRESSED].starty,cElements[CBUTTONS_STOP_BUTTON_UNPRESSED].dimx,cElements[CBUTTONS_STOP_BUTTON_UNPRESSED].dimy, mypixbuf,85,88);
	gdk_pixbuf_copy_area(pixbufs.cbuttonsbmp,cElements[CBUTTONS_NEXT_BUTTON_UNPRESSED].startx,cElements[CBUTTONS_NEXT_BUTTON_UNPRESSED].starty,cElements[CBUTTONS_NEXT_BUTTON_UNPRESSED].dimx,cElements[CBUTTONS_NEXT_BUTTON_UNPRESSED].dimy, mypixbuf,108,88);
	gdk_pixbuf_copy_area(pixbufs.cbuttonsbmp,cElements[CBUTTONS_OPEN_BUTTON_UNPRESSED].startx,cElements[CBUTTONS_OPEN_BUTTON_UNPRESSED].starty,cElements[CBUTTONS_OPEN_BUTTON_UNPRESSED].dimx,cElements[CBUTTONS_OPEN_BUTTON_UNPRESSED].dimy, mypixbuf,136,89);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	layout = gtk_layout_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER (window), layout);
	gtk_widget_show(layout);

	image=gtk_image_new();
	
	gtk_image_set_from_pixbuf(GTK_IMAGE(image),mypixbuf);
	gtk_widget_queue_draw(image);
	gtk_layout_put(GTK_LAYOUT(layout),image, 0, 0);
	gtk_widget_show(image);
	gtk_widget_show(window);

	g_signal_connect(window, "destroy",
			G_CALLBACK(gtk_main_quit), NULL);  

	gtk_main();

	return 0;
}

