#include "pixbufloader.h"
#define	MAGIC	0x79486568	// "yHeh" = The place where I grew up.
typedef struct _tTopHandle
{
	tHandlePixbufLoader	hPixbufLoader;	
} tTopHandle;

tTopHandle topHandle;	// I DO NOT LIKE TO PLACE MEMORY IN THIS SHARED REGION. At least, this one is structured
int main(int argc,char** argv)
{
	GdkPixbuf *testbuf;
	GError *err=NULL;
	
	pixbufloader_init(&(topHandle.hPixbufLoader),argv[1]);
	testbuf=gdk_pixbuf_new(GDK_COLORSPACE_RGB,TRUE, 8,275,116);
	pixbufloader_addelement(&(topHandle.hPixbufLoader),testbuf,0,14,MAIN_MAIN_DISPLAY);	
	gdk_pixbuf_save(testbuf,"test.png","png",NULL);

	return 0;
}

