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
int gui_init(tHandleGUI* pHandleGUI,tHandleDecoder* pHandleDecoder,tHandleAudioOutput* pHandleAudioOutput)
{
	int retval;
	retval=pixbufloader_init(&(pHandleGUI->handlePixbufLoader));

	// TODO: load default theme


	retval=window_main_init(&(pHandleGUI->handleWindowMain),&(pHandleGUI->handlePixbufLoader),pHandleDecoder,pHandleAudioOutput);
	return retval;
}

int gui_load_theme_from_directory(tHandleGUI* pHandleGUI,char* directory)
{
	int retval;
	tSongInfo songInfo;
	retval=pixbufloader_load_from_directory(&(pHandleGUI->handlePixbufLoader),directory);
// loading a new gui changes the font, so the song info has to be redrawn
	// TODO
	memcpy(&songInfo,&(pHandleGUI->handleWindowMain.songInfo_drawn),sizeof(tSongInfo));
	memset(&(pHandleGUI->handleWindowMain.songInfo_drawn),0,sizeof(tSongInfo));
	window_main_refresh_songinfo(&(pHandleGUI->handleWindowMain),songInfo);
	return retval;
}

int gui_run(tHandleGUI* pHandleGUI)
{
	int retval;
	retval=window_main_run(&(pHandleGUI->handleWindowMain));	
	return retval;
}
int gui_get_shuffle_repeat(tHandleGUI* pHandleGUI,int* pShuffle,int* pRepeat)
{
	return window_main_get_shuffle_repeat(&(pHandleGUI->handleWindowMain),pShuffle,pRepeat);
}
