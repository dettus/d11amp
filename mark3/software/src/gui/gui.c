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

#include "gui.h"
#include <string.h>
#include <stdio.h>


int gui_init(tHandleGUI* pThis,GtkApplication *app,tHandleAudioOutput *pHandleAudioOutput,tHandleDecoder *pHandleDecoder,tOptions *pCommandLineOptions)
{
	int retval;
	int i;
	memset(pThis,0,sizeof(tHandleGUI));

	retval=RETVAL_OK;
	pThis->pHandleDecoder=pHandleDecoder;	

	theme_manager_init(&(pThis->handleThemeManager),pCommandLineOptions);
	window_equalizer_init(app,&(pThis->handleWindowEqualizer),&(pThis->handleThemeManager),pHandleDecoder);
	window_playlist_init(app,&(pThis->handleWindowPlaylist),&(pThis->handleThemeManager));
	window_main_init(app,&(pThis->handleWindowMain),&(pThis->handleThemeManager),pHandleAudioOutput,pHandleDecoder,&(pThis->handleWindowEqualizer),&(pThis->handleWindowPlaylist));

	for (i=pCommandLineOptions->gtkargc;i<pCommandLineOptions->argc;i++)
	{
		int l;
		int r;
		
		l=strlen(pCommandLineOptions->argv[i]);
		if (l>15 && strncmp("--gui.playlist.",pCommandLineOptions->argv[i],15)==0)
		{
			r=window_playlist_parse_commandline(&(pThis->handleWindowPlaylist),&(pCommandLineOptions->argv[i][15]));
			if (r==RETVAL_NOK_COMMANDLINE) fprintf(stderr,"UNKNOWN ARGUMENT [%s] \n",pCommandLineOptions->argv[i]);
			retval|=r;
		}
	}

	return retval;
}
int gui_show(tHandleGUI* pThis)
{
	int retval;
	retval=0;
	//retval|=window_equalizer_show(&(pThis->handleWindowEqualizer));
	//retval|=window_playlist_show(&(pThis->handleWindowPlaylist));
	retval|=window_main_show(&(pThis->handleWindowMain));
	return retval;
}
int gui_get_shuffle_repeat(tHandleGUI* pThis,int* pShuffle,int* pRepeat)
{
	return window_main_get_shuffle_repeat(&(pThis->handleWindowMain),pShuffle,pRepeat);
}

int gui_next(tHandleGUI* pThis)
{
	int shuffle,repeat;
	char filename[1024];
	int retval;
	window_main_get_shuffle_repeat(&(pThis->handleWindowMain),&shuffle,&repeat);
	if (shuffle)
	{
		retval=window_playlist_shuffle(&(pThis->handleWindowPlaylist),filename,sizeof(filename));
	} else {
		retval=window_playlist_next(&(pThis->handleWindowPlaylist),filename,sizeof(filename));
	}
	if (retval==RETVAL_OK) {retval= decoder_openfile(pThis->pHandleDecoder,filename);}
	if (retval==RETVAL_OK) {retval= decoder_set_state(pThis->pHandleDecoder,STATE_PLAY);}
	return retval;
	
}
int gui_prev(tHandleGUI* pThis)
{
	int shuffle,repeat;
	char filename[1024];
	int retval;
	window_main_get_shuffle_repeat(&(pThis->handleWindowMain),&shuffle,&repeat);
	if (shuffle)
	{
		retval=window_playlist_shuffle(&(pThis->handleWindowPlaylist),filename,sizeof(filename));
	} else {
		retval=window_playlist_prev(&(pThis->handleWindowPlaylist),filename,sizeof(filename));
	}
	if (retval==RETVAL_OK) {retval= decoder_openfile(pThis->pHandleDecoder,filename);}
	if (retval==RETVAL_OK) {retval= decoder_set_state(pThis->pHandleDecoder,STATE_PLAY);}
	return retval;
}
void gui_help()
{
	printf("GUI OPTIONS\n");
	printf("--gui.playlist.m3u=FILENAME.m3u Load a playlist from a .m3u file\n");
	printf("--gui.theme.dir=DIRECTORY       Load the theme from the DIRECTORY\n");
}

