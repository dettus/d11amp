#include "pixbufloader.h"
#include "mainwindow.h"
#define	MAGIC	0x79486568	// "yHeh" = The place where I grew up.
typedef struct _tTopHandle
{
	tHandlePixbufLoader	hPixbufLoader;	
	tHandleMainWindow	hMainWindow;
	tHandleDecoderMain	hDecoderMain;
	tHandleAudioOut		hAudioOut;
} tTopHandle;

void* main_thread(void* handle)
{
	tTopHandle* pThis=(tTopHandle*)handle;
	while (1)
	{
		usleep(100000);
		{
			int pos;
			int len;
			char* songinfo;
			int bitrate;
			int rate;
			int channels;
			decode_getpos(&(pThis->hDecoderMain),&len,&pos);
			mainwindow_setpos(&(pThis->hMainWindow),len,pos);
			decode_getsonginfo(&(pThis->hDecoderMain),&songinfo,&bitrate,&rate,&channels);
			mainwindow_setsonginfo(&(pThis->hMainWindow),songinfo,bitrate,rate,channels);

		}
		
		
	}
}


tTopHandle topHandle;	// I DO NOT LIKE TO PLACE MEMORY IN THIS SHARED REGION. At least, this one is structured
int main(int argc,char** argv)
{
	pthread_t mainthread;
	
	pixbufloader_init(&(topHandle.hPixbufLoader),argv[argc-1]);
	audioout_init(&(topHandle.hAudioOut));
	decode_init(&(topHandle.hDecoderMain),&(topHandle.hAudioOut));
	argc--;
	gtk_init(&argc,&argv);
	mainwindow_init(&(topHandle.hMainWindow),&(topHandle.hPixbufLoader),&(topHandle.hDecoderMain));

	pthread_create(&mainthread,NULL,&main_thread,(void*)&topHandle);
	gtk_main();


	return 0;
}

