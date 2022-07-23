#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc,char** argv)
{
	int i;
	char buf[16];
	tHandleAudio handle_audio;

	audio_init(&handle_audio);
	printf("initialized.\n");
	sleep(1);
	audio_startfile(&handle_audio,argv[1]);

	printf("please press enter\n");
	fgets(buf,sizeof(buf),stdin);	
	
}
