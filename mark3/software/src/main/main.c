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
#include "constants.h"
#include "decoder.h"
#include "gui.h"
#include <pthread.h>

typedef struct _tHandleMain
{
	tHandleAudioOutput handleAudioOutput;
	tHandleDecoder handleDecoder;
	tHandleGUI handleGUI;
	pthread_t thread;
	tOptions commandLineOptions;
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
				
			gui_next(&(pThis->handleGUI));
		}
	}
}

static void activate(GtkApplication *app, gpointer user_data)
{
	tHandleMain* pThis=(tHandleMain*)user_data;

	audiooutput_init(&(pThis->handleAudioOutput),&(pThis->commandLineOptions));
	decoder_init(&(pThis->handleDecoder),&(pThis->handleAudioOutput),&(pThis->commandLineOptions));
	gui_init(&(pThis->handleGUI),app,&(pThis->handleAudioOutput),&(pThis->handleDecoder),&(pThis->commandLineOptions));
	gui_show(&(pThis->handleGUI));


	pthread_create(&pThis->thread,NULL,main_thread,(void*)pThis);
}

tHandleMain handleMain;
void printheader()
{
	printf("*** d11amp %d.%d%d\n",VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION);
	printf("*** (C)opyright 2022\n");
	printf("*** by dettus@dettus.net\n");
	printf("************************\n");
	printf("\n");
}
int main(int argc,char** argv)
{
	GtkApplication *app;
	int status;
	int i;
	int gtkargc;
	memset(&handleMain,0,sizeof(tHandleMain));

	if (argc==1)
	{
		printheader();
		printf("PLEASE RUN WITH \n");
		printf("  %s --gui.theme.dir=THEMEDIR/\n",argv[0]);
		printf("where THEMEDIR contains the .bmp files from an unzipped .wsz file\n");	
		printf(" mkdir THEMEDIR\n");
		printf(" cd THEMEDIR\n");
		printf(" wget -c https://qmmp.ylsoftware.com/files/skins/winamp-skins/winamp_classic.wsz\n");
		printf(" unzip winamp_classic.wsz\n");
		printf(" cd ..\n");
		printf(" %s --gui.theme.dir=THEMEDIR/\n",argv[0]);
		return 1;
	}

	// so... the idea is that gtk has its commandline option, 
	// but d11amp does as well.
	// what I am doing here is to reorder the options, so that the
	// gtk ones are at the beginning. at the same time, the count
	// is being decreased.

	// step 1: tag the d11amp options by simply overwriting the first 
	// character with 0.
	for (i=0;i<argc;i++)
	{
		if (strncmp(argv[i],"--help",6)==0 && strlen(argv[i])==6)
		{
			printheader();
			printf("Please run with %s [OPTIONS]\n",argv[0]);
			printf("Where [OPTIONS] are\n");
			printf("\n");
			audiooutput_help();
			decoder_help();
			gui_help();
			printf("OTHER OPTIONS\n");			
			printf("-bsd                    shows the license\n");
			printf("--help                  shows this screen\n");
			printf("--help-gapplication     shows more help\n");
			printf("--version               shows %d.%d%d\n",VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION);
			return 0;
		}
		if (strncmp(argv[i],"-bsd",4)==0)
		{
			printheader();
			printf("%s\n",d11amp_license_text);
			return 0;
		}
		if (strncmp(argv[i],"--version",9)==0)
		{
			printf("%d.%d%d\n",VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION);
			return 0;
		}
		if (strncmp(argv[i],"--audiooutput",13)==0)
		{
			argv[i][0]=0;	
		}
		if (strncmp(argv[i],"--gui",5)==0)
		{
			argv[i][0]=0;	
		}
		if (strncmp(argv[i],"--decoder",9)==0)
		{
			argv[i][0]=0;	
		}
	}	

	// step 2: move the arguments for d11amp to the end.
	i=0;
	gtkargc=argc;
	while (i<gtkargc)
	{
		int j;

		if (argv[i][0]==0)
		{
			for (j=i;j<gtkargc-1;j++)
			{
				char *xchng;
				xchng=argv[j];	// remember this argument
				argv[j]=argv[j+1];	// move the others one to the left
				argv[j+1]=xchng;
			}
			gtkargc--;
		} else {
			i++;
		}
	}
	// step 3: remove the tags
	for (i=gtkargc;i<argc;i++)
	{
		argv[i][0]='-';
	}

	// step 4: make the commandline options available to the modules
	handleMain.commandLineOptions.argc=argc;
	handleMain.commandLineOptions.gtkargc=gtkargc;
	handleMain.commandLineOptions.argv=argv;
	

	app=gtk_application_new("net.dettus.d11amp",G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app,"activate",G_CALLBACK(activate),&handleMain);

	status=g_application_run(G_APPLICATION(app),gtkargc,argv);
	g_object_unref(app);
	return status;	
}
