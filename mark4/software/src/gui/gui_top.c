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

#include "gui_top.h"
#include <string.h>
int gui_top_init(tHandleGuiTop* pThis,void* pControllerContext,GtkApplication* app,tHandlePlayList* pHandlePlayList)
{
	int retval;

	memset(pThis,0,sizeof(tHandleGuiTop));

	pThis->app=app;
	pThis->pControllerContext=pControllerContext;

	retval=RETVAL_OK;	
	retval|=theme_manager_init(&(pThis->handleThemeManager));

	retval|=theme_manager_load_from_directory(&(pThis->handleThemeManager),"theme/");	


	retval|=window_main_init(&(pThis->handleWindowMain),pThis->pControllerContext,&(pThis->handleThemeManager),pThis->app);
	retval|=window_equalizer_init(&(pThis->handleWindowEqualizer),pThis->pControllerContext,&(pThis->handleThemeManager),pThis->app);
	retval|=window_playlist_init(&(pThis->handleWindowPlaylist),pThis->pControllerContext,&(pThis->handleThemeManager),pThis->app,pHandlePlayList);


	window_main_show(&(pThis->handleWindowMain));
	window_equalizer_show(&(pThis->handleWindowEqualizer));
	window_playlist_show(&(pThis->handleWindowPlaylist));

	return retval;
}

int gui_top_signal_new_theme(tHandleGuiTop* pThis)
{
	int retval;
	retval=RETVAL_OK;
	retval|=window_main_signal_new_theme(&(pThis->handleWindowMain));
	retval|=window_equalizer_signal_new_theme(&(pThis->handleWindowEqualizer));
	retval|=window_playlist_signal_new_theme(&(pThis->handleWindowPlaylist));

	return retval;
}

int gui_top_commandline_option(tHandleGuiTop* pThis,char* argument)
{
	int retval;
	int l;

	retval=RETVAL_NOK_COMMANDLINE;
	
	l=strlen(argument);
	if (strncmp("--gui.theme.dir=",argument,16)==0 && l>16)
	{
		retval=theme_manager_load_from_directory(&(pThis->handleThemeManager),&argument[16]);
		retval|=gui_top_signal_new_theme(pThis);
	}
	
	return retval;
}
