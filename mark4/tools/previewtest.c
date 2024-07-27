

#include "theme_manager.h"
#include "window_equalizer.h"
#include "window_main.h"
#include "window_playlist.h"

#include <gdk-pixbuf/gdk-pixbuf.h>

int main(int argc,char** argv)
{
	GdkPixbuf *pixbuf;
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
	err=NULL;
	snprintf(filename,1024,"%s_main_.png",argv[1]);
	pixbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,1,4,1000,1000);
	window_main_draw_background(&handleTheme,pixbuf,0);
	gdk_pixbuf_save(pixbuf,filename,"png",&err,NULL);
	

	
}
