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
#include "controller.h"
#include <string.h>
int gui_top_init(tHandleGuiTop* pThis,void* pControllerContext,GtkApplication* app,tHandlePlayList* pHandlePlayList)
{
	int retval;

	memset(pThis,0,sizeof(tHandleGuiTop));

	pThis->app=app;
	pThis->pControllerContext=pControllerContext;

	retval=RETVAL_OK;	
	retval|=theme_manager_init(&(pThis->handleThemeManager),pControllerContext);



	retval|=window_main_init(&(pThis->handleWindowMain),pThis->pControllerContext,&(pThis->handleThemeManager),pThis->app);
	retval|=window_equalizer_init(&(pThis->handleWindowEqualizer),pThis->pControllerContext,&(pThis->handleThemeManager),pThis->app);
	retval|=window_playlist_init(&(pThis->handleWindowPlaylist),pThis->pControllerContext,&(pThis->handleThemeManager),pThis->app,pHandlePlayList);
	
	retval|=window_preferences_init(&(pThis->handleWindowPreferences),pThis->pControllerContext,pThis->app);


	return retval;
}

int gui_top_start(tHandleGuiTop* pThis)
{
	tPayload payload;
	char themedir[2048];
	char configDir[1024];
	int retval;


	config_init(&(pThis->handleConfig),pThis->pControllerContext,"gui.config");
	config_getint(&(pThis->handleConfig),"scalefactor",&(payload.scaleFactor),1);
	controller_event(pThis->pControllerContext,eEVENT_SCALE,&payload);


	controller_get_config_dir(pThis->pControllerContext,configDir);
	snprintf(themedir,2048,"%s/theme/",configDir);

	retval=theme_manager_load_from_directory(&(pThis->handleThemeManager),themedir);
	window_main_start(&(pThis->handleWindowMain));
	window_equalizer_start(&(pThis->handleWindowEqualizer));
	window_playlist_start(&(pThis->handleWindowPlaylist));



	window_main_show(&(pThis->handleWindowMain));
	config_getbool(&(pThis->handleConfig),"showplaylist",&(payload.hide0show1),0);
	controller_event(pThis->pControllerContext,eEVENT_WINDOW_PLAYLIST,&payload);
	config_getbool(&(pThis->handleConfig),"showequalizer",&(payload.hide0show1),0);
	controller_event(pThis->pControllerContext,eEVENT_WINDOW_EQUALIZER,&payload);
	
	return retval;
}
int gui_top_signal_window_playlist(tHandleGuiTop* pThis,int hide0show1)
{
	if (hide0show1)
	{
		window_playlist_show(&(pThis->handleWindowPlaylist));
	} else {
		window_playlist_hide(&(pThis->handleWindowPlaylist));
	}
	window_main_signal_playlist(&(pThis->handleWindowMain),hide0show1);
	config_setbool(&(pThis->handleConfig),"showplaylist",hide0show1);
	return RETVAL_OK;
}
int gui_top_signal_window_equalizer(tHandleGuiTop* pThis,int hide0show1)
{
	if (hide0show1)
	{
		window_equalizer_show(&(pThis->handleWindowEqualizer));
	} else {
		window_equalizer_hide(&(pThis->handleWindowEqualizer));
	}
	window_main_signal_equalizer(&(pThis->handleWindowMain),hide0show1);
	config_setbool(&(pThis->handleConfig),"showequalizer",hide0show1);
	return RETVAL_OK;
}
int gui_top_signal_window_preferences(tHandleGuiTop* pThis,int hide0show1)
{
	int retval;
	if (hide0show1)
	{
		retval=window_preferences_show(&(pThis->handleWindowPreferences));
	} else {
		retval=window_preferences_hide(&(pThis->handleWindowPreferences));
	}
	return retval;	

}
int gui_top_signal_scale(tHandleGuiTop* pThis,int scaleFactor)
{
	config_setint(&(pThis->handleConfig),"scalefactor",scaleFactor);
	window_equalizer_signal_scalefactor(&(pThis->handleWindowEqualizer),scaleFactor);
	window_main_signal_scalefactor(&(pThis->handleWindowMain),scaleFactor);
	window_playlist_signal_scalefactor(&(pThis->handleWindowPlaylist),scaleFactor);
	
	return RETVAL_OK;

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
	if (strncmp("--gui.theme.dumptemplate=",argument,25)==0 && l>25)
	{
		retval=theme_manager_write_template(&argument[25]);
		return RETVAL_DONE;
	}
	if (strncmp("--gui.theme.dumpdefault=",argument,24)==0 && l>24)
	{
		retval=theme_manager_write_default(&argument[24]);
		return RETVAL_DONE;
	}
	if (strncmp("--gui.theme.dir=",argument,16)==0 && l>16)
	{
		retval=theme_manager_load_from_directory(&(pThis->handleThemeManager),&argument[16]);
		retval|=gui_top_signal_new_theme(pThis);
	}
	if (strncmp("--gui.theme.wsz=",argument,16)==0 && l>16)
	{
		retval=theme_manager_load_from_wsz(&(pThis->handleThemeManager),&argument[16]);
		retval|=gui_top_signal_new_theme(pThis);
	}
	
	return retval;
}

int gui_top_add_preferences_page(tHandleGuiTop* pThis,GtkWidget* pWidget,char* label)
{
	return window_preferences_add_page(&(pThis->handleWindowPreferences),pWidget,label);
}

