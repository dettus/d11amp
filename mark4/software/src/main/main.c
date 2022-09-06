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

typedef struct _tArguments
{
	int argc;
	char** argv;
} tArguments;

int commandline_parse(char* argument,int sort0parse1)
{
	int l;
	int retval;

	retval=0;
	l=strlen(argument);
	if (l==5 && strncmp("--bsd",argument,l)==0)
	{
		retval=1;
	}
	if (l==6 && strncmp("--help",argument,l)==0)
	{
		retval=1;
	}
	return retval;
}

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
	retval|=controller_event(pControllerContext,eEVENT_ACTIVATE,NULL);
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

	for (i=1;i<argc;i++)
	{
		if (commandline_parse(argv[i],0))
		{
			arguments_d11amp.argv[arguments_d11amp.argc++]=argv[i];
		} else {
			arguments_gtk.argv[arguments_gtk.argc++]=argv[i];
		}	
	}

	
	
	app=gtk_application_new("net.dettus.d11amp",G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app,"activate",G_CALLBACK(activate),&arguments_d11amp);

	status=g_application_run(G_APPLICATION(app),arguments_gtk.argc,arguments_gtk.argv);
	g_object_unref(app);

	return status;
}
