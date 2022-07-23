#include "pixbufloader.h"
#include "mainwindow.h"
#define	MAGIC	0x79486568	// "yHeh" = The place where I grew up.
typedef struct _tTopHandle
{
	tHandlePixbufLoader	hPixbufLoader;	
	tHandleMainWindow	hMainWindow;
	tHandleAudio		hAudio;
} tTopHandle;

tTopHandle topHandle;	// I DO NOT LIKE TO PLACE MEMORY IN THIS SHARED REGION. At least, this one is structured
int main(int argc,char** argv)
{
	
	pixbufloader_init(&(topHandle.hPixbufLoader),argv[argc-1]);
	audio_init(&(topHandle.hAudio));
	argc--;
	gtk_init(&argc,&argv);
	mainwindow_init(&(topHandle.hMainWindow),&(topHandle.hPixbufLoader),&(topHandle.hAudio));
	gtk_main();


	return 0;
}

