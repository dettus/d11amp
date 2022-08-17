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
	memset(pThis,0,sizeof(tHandleGUI));

	

	theme_manager_init(&(pThis->handleThemeManager));
	theme_manager_load_from_directory(&(pThis->handleThemeManager),"theme/");


	window_equalizer_init(app,&(pThis->handleWindowEqualizer),&(pThis->handleThemeManager),pHandleDecoder);
	window_playlist_init(app,&(pThis->handleWindowPlaylist),&(pThis->handleThemeManager));
	window_main_init(app,&(pThis->handleWindowMain),&(pThis->handleThemeManager),pHandleAudioOutput,pHandleDecoder,&(pThis->handleWindowEqualizer),&(pThis->handleWindowPlaylist));
	return RETVAL_OK;
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
void gui_help()
{
	printf("GUI OPTIONS\n");
	printf("--gui.playlist.m3u=FILENAME.m3u Load a playlist from a .m3u file\n");
	printf("--gui.theme.dir=DIRECTORY       Load the theme from the DIRECTORY\n");
}

