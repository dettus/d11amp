/*
BSD 2-Clause License

Copyright (c) 2022, dettus
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <gtk/gtk.h>
#include "datastructures.h"
#include "main.h"
#include "debugging.h"


int main_init(tHandleMain *pThis)
{
	DEBUGGING_WRITE_VCD("initphase",1);
	audiooutput_init(&(pThis->handleAudioOutput));
	decoder_init(&(pThis->handleDecoder),&(pThis->handleAudioOutput));
	gui_init(&(pThis->handleGUI),&(pThis->handleDecoder),&(pThis->handleAudioOutput));
	printf("*** AT THIS STAGE: PLEASE CREATE A SYMLINK TO A DIRECTORY CALLED\n");
	printf("*** theme/, IN WHICH ALL THE NECESSARY .BMP FILES ARE BEING STORED\n");
	gui_load_theme_from_directory(&(pThis->handleGUI),"theme/");


	DEBUGGING_WRITE_VCD("initphase",0);
	
	gui_run(&(pThis->handleGUI));

	

	return RETVAL_OK;
}
int main(int argc,char** argv)
{
	tHandleMain* pHandleMain;
	#ifdef	D11AMP_DEBUGGING
	debugging_start_vcd("debugging.vcd");			
	#endif

	pHandleMain=(tHandleMain*)malloc(sizeof(tHandleMain));

	gtk_init(&argc,&argv);
	main_init(pHandleMain);

	gtk_main();
	return 0;
}
