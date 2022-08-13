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
#include <gtk/gtk.h>
#include "decoder.h"
#include "gui.h"
#include <pthread.h>

typedef struct _tHandleMain
{
	tHandleAudioOutput handleAudioOutput;
	tHandleDecoder handleDecoder;
	tHandleGUI handleGUI;
	pthread_t thread;
} tHandleMain;

void* main_thread(void* user_data)
{
	tHandleMain* pThis=(tHandleMain*)user_data;
	eDecoderState decState;
	
	while (1)
	{
		usleep(100000);
		decoder_get_state(&(pThis->handleDecoder),&decState);
		if (decState==STATE_EOF)
		{
			int shuffle,repeat;
			gui_get_shuffle_repeat(&(pThis->handleGUI),&shuffle,&repeat);
			if (repeat)
			{
				decoder_set_state(&(pThis->handleDecoder),STATE_PLAY);
			}
		}
	}
}

static void activate(GtkApplication *app, gpointer user_data)
{
	tHandleMain* pThis=(tHandleMain*)user_data;

	audiooutput_init(&(pThis->handleAudioOutput));
	decoder_init(&(pThis->handleDecoder),&(pThis->handleAudioOutput));
	gui_init(&(pThis->handleGUI),app,&(pThis->handleAudioOutput),&(pThis->handleDecoder));
	gui_show(&(pThis->handleGUI));


	pthread_create(&pThis->thread,NULL,main_thread,(void*)pThis);
}

tHandleMain handleMain;
int main(int argc,char** argv)
{
	GtkApplication *app;
	int status;

	app=gtk_application_new("net.dettus.d11amm",G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app,"activate",G_CALLBACK(activate),&handleMain);

	status=g_application_run(G_APPLICATION(app),argc,argv);
	g_object_unref(app);
	return status;	
}
