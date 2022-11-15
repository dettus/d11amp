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
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include "controller.h"
#include "datastructures.h"
#include "version.h"


static void activate(GtkApplication *app, gpointer user_data)
{
	int retval;
	int bytes;
	tArguments* arguments_d11amp=(tArguments*)user_data;
	void *pControllerContext;

	
	retval=RETVAL_OK;	
	retval|=controller_getBytes(&bytes);
	printf("allocating %d bytes\n",bytes);
	pControllerContext=malloc(bytes);
	retval|=controller_init(pControllerContext,(void*)app);
	retval|=controller_commandline_options(pControllerContext,arguments_d11amp);
	retval|=controller_start(pControllerContext);

	if (retval)
	{
		fprintf(stderr,"ERROR initializing modules\n");
		exit(1);
	}
	
	

//	pthread_create(&pThis->thread,NULL,main_thread,(void*)pThis);
}



int main(int argc,char** argv)
{
	GtkApplication *app;
	int status;
	int i;
	tArguments arguments_d11amp;
	tArguments arguments_gtk;

	arguments_d11amp.argv=malloc(argc*sizeof(char*));
	arguments_d11amp.argc=1;
	arguments_d11amp.argv[0]=argv[0];

	arguments_gtk.argv=malloc(argc*sizeof(char*));
	arguments_gtk.argc=1;
	arguments_gtk.argv[0]=argv[0];


	if (argc==1)
	{
		printf("This version is not able to handle themes from .wsz files or has a default\n");
		printf("theme. Therefore, it is IMPERATIVE that you download one from a website,\n");
		printf("(for example https://skins.webamp.org/) unpack it, and load the theme from\n");
		printf("that directory.\n");
		printf("\n");
		printf("Since the .wsz files are simple ZIP files, albeit flat ones, they can be\n");
		printf("unpacked as followed:\n");
		printf("\n");
		printf("%% mkdir base-2.91\n");
		printf("%% cd base-2.91\n");
		printf("%% unzip ../base-2.91.wsz\n");
		printf("%% cd ..\n");
		printf("%% ./d11amp --gui.theme.dir=base-2.91/\n");
		printf("\n");
		printf("The directory should be the one with all the .BMP files.\n");
		printf("\n");
		exit(1);
	}
// the commandline arguments need to be sorted into two groups.
// d11amp and gtk. the controller_commandline_parse serves a dual role for this.
	for (i=1;i<argc;i++)
	{
		int retval;
		retval=controller_commandline_parse(NULL,argv[0],argv[i]);
		switch(retval)
		{
			case RETVAL_DONE:
				return 0;
				break;
			case RETVAL_OK_CONTINUE:
				arguments_d11amp.argv[arguments_d11amp.argc++]=argv[i];
				break;
			default:
				arguments_gtk.argv[arguments_gtk.argc++]=argv[i];
				break;
		}	
	}

	
	
	app=gtk_application_new("net.dettus.d11amp",G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app,"activate",G_CALLBACK(activate),&arguments_d11amp);

	status=g_application_run(G_APPLICATION(app),arguments_gtk.argc,arguments_gtk.argv);
	g_object_unref(app);

	return status;
}
