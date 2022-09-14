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
#include "license.h"
#include "version.h"

void print_header()
{
	printf("*** d11amp %d.%d%d\n",VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION);
	printf("*** dettus@dettus.net\n");
	printf("*** (C)opyright 2022\n");
	printf("********************\n");
	printf("\n");
}
void print_license()
{
	printf("%s",d11amp_license);
}
void print_help(char* argv0)
{
	printf("Please run with\n");
	printf("\n");
	printf("%s [OPTIONS]\n",argv0);
	printf("\n");
	printf("Where [OPTIONS] are\n");
	printf("AUDIO options\n");
	printf("  --audiooutput.portaudio.device=NUMBER Set the audio output device\n");
	printf("                                        (Use ? to see a list\n");
	printf("GUI options\n");
	printf("  --gui.theme.dir=DIRECTORY/   Load a theme from this directory\n");
	printf("Playlist options\n");
	printf("  --playlist.m3u=PLAYLIST.m3u  Loads playlist from an .m3u file\n");
	printf("Other options\n");
	printf("  --bsd                        Prints the license\n");
	printf("  --help                       Shows this help\n");
	printf("  --help-all                   Shows more GTK related help\n");
	printf("  --version                    Prints of %d.%d%d\n",VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION);
	printf("\n");
	printf("For example: %s --gui.theme.dir=theme/ --playlist.m3u=playlist.m3u\n",argv0);
}
void print_version()
{
	printf("%d.%d%d\n",VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION);
}

