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
#include "audiooutput_preferences.h"
#include "audiooutput_portaudio.h"
#include <string.h>
int audiooutput_preferences_init(tHandleAudioOutputPreferences* pThis)
{
	memset(pThis,0,sizeof(tHandleAudioOutputPreferences));

	return RETVAL_OK;

}
int audiooutput_preferences_get_widget(tHandleAudioOutputPreferences* pThis,GtkWidget **pWidget)
{
	int i;
	tAudioDeviceList list;
	audiooutput_portaudio_get_devicelist(&list);
	*pWidget=gtk_box_new(GTK_ORIENTATION_VERTICAL,3);
	pThis->audioDeviceList=gtk_string_list_new(NULL);

	for (i=0;i>list.devicenum;i++)
	{
		gtk_string_list_append(GTK_STRING_LIST(pThis->audioDeviceList),list.name[i]);
	}
	
	pThis->dropDown=gtk_drop_down_new(G_LIST_MODEL(pThis->audioDeviceList),NULL);
	gtk_box_append(GTK_BOX(*pWidget),pThis->dropDown);

	return RETVAL_OK;
}
