/*

Copyright 2023, dettus@dettus.net

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
	*pWidget=gtk_box_new(GTK_ORIENTATION_VERTICAL,3);
	pThis->audioDeviceList=gtk_string_list_new(NULL);


	pThis->dropDown=gtk_drop_down_new(G_LIST_MODEL(pThis->audioDeviceList),NULL);
	gtk_box_append(GTK_BOX(*pWidget),pThis->dropDown);

	return RETVAL_OK;
}
int audiooutput_preferences_populate(tHandleAudioOutputPreferences* pThis,int currDevIdx)
{
	int i;
	int entry;
	int n;
	tAudioDeviceList list;
	audiooutput_portaudio_get_devicelist(&list);

	n=g_list_model_get_n_items(G_LIST_MODEL(pThis->audioDeviceList));
	for (i=0;i<n;i++)
	{
		gtk_string_list_remove(GTK_STRING_LIST(pThis->audioDeviceList),0);
	}

	entry=0;
	for (i=0;i<list.devicenum;i++)
	{
		if ((currDevIdx==-1 && list.idx[i]==list.defaultdevice) || list.idx[i]==currDevIdx)
		{
			entry=i;
		}
		gtk_string_list_append(GTK_STRING_LIST(pThis->audioDeviceList),list.name[i]);
	}
	gtk_drop_down_set_model(GTK_DROP_DOWN(pThis->dropDown),G_LIST_MODEL(pThis->audioDeviceList));
	gtk_drop_down_set_selected(GTK_DROP_DOWN(pThis->dropDown),entry);
	pThis->current_preferences.deviceIdx=currDevIdx;


	return RETVAL_OK;		
}
int audiooutput_preferences_apply(tHandleAudioOutputPreferences* pThis,tAudioOutputPreferences *pNextPrefs)
{
	int entry;	
	tAudioDeviceList list;

	entry=gtk_drop_down_get_selected(GTK_DROP_DOWN(pThis->dropDown));
	audiooutput_portaudio_get_devicelist(&list);
	pNextPrefs->deviceIdx=list.idx[entry];

	printf("entry %2d -> %3d [%s]\n",entry,list.idx[entry],list.name[entry]);
	
	return RETVAL_OK;
}
