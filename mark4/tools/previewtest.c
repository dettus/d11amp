

#include "theme_manager.h"
#include "window_equalizer.h"
#include "window_main.h"
#include "window_playlist.h"

#include <gdk-pixbuf/gdk-pixbuf.h>

int main(int argc,char** argv)
{
	GdkPixbuf *pixbuf;
	GdkPixbuf *pixbuf_main;
	GdkPixbuf *pixbuf_equalizer;
	GdkPixbuf *pixbuf_playlist;
	tHandleThemeManager handleTheme;
	char filename[1024];
	GError *err;
	if (argc!=2)
	{
		fprintf(stderr,"please run with %s INPUTFILE.wsz\n",argv[0]);
		return 1;
	}

	theme_manager_init(&handleTheme,NULL);
	theme_manager_load_from_wsz(&handleTheme,argv[1]);
	theme_manager_load_from_directory(&handleTheme,"./theme");



	pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,1,8,WINDOW_MAIN_WIDTH,2*WINDOW_MAIN_HEIGHT+WINDOW_EQUALIZER_HEIGHT);


	err=NULL;
	snprintf(filename,1024,"%s_main_.png",argv[1]);
	pixbuf_main=gdk_pixbuf_new(GDK_COLORSPACE_RGB,1,8,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);
	window_main_draw_background(&handleTheme,pixbuf_main,0);
//	gdk_pixbuf_save(pixbuf_main,filename,"png",&err,NULL);
	
	err=NULL;
	snprintf(filename,1024,"%s_equalizer_.png",argv[1]);
	pixbuf_equalizer=gdk_pixbuf_new(GDK_COLORSPACE_RGB,1,8,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT);
	window_equalizer_draw_background(&handleTheme,pixbuf_equalizer);
//	gdk_pixbuf_save(pixbuf_equalizer,filename,"png",&err,NULL);
	
	err=NULL;
	snprintf(filename,1024,"%s_playlist_.png",argv[1]);
	pixbuf_playlist=gdk_pixbuf_new(GDK_COLORSPACE_RGB,1,8,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT);
	window_playlist_draw_background(&handleTheme,pixbuf_playlist,2,11);
//	gdk_pixbuf_save(pixbuf_playlist,filename,"png",&err,NULL);


	gdk_pixbuf_copy_area(pixbuf_main,0,0,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,pixbuf,0,0);
	gdk_pixbuf_copy_area(pixbuf_equalizer,0,0,WINDOW_EQUALIZER_WIDTH,WINDOW_EQUALIZER_HEIGHT,pixbuf,0,WINDOW_MAIN_HEIGHT);
	gdk_pixbuf_copy_area(pixbuf_playlist,0,0,WINDOW_MAIN_WIDTH,WINDOW_MAIN_HEIGHT,pixbuf,0,WINDOW_EQUALIZER_HEIGHT+WINDOW_MAIN_HEIGHT);
	snprintf(filename,1024,"%s.png",argv[1]);
	gdk_pixbuf_save(pixbuf,filename,"png",&err,NULL);

	

	
}
