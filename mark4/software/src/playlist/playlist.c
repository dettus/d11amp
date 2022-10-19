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

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "datastructures.h"
#include "playlist.h"

int playlist_init(tHandlePlayList* pThis)
{
	memset(pThis,0,sizeof(tHandlePlayList));	
	return RETVAL_OK;
}
int playlist_load_m3u(tHandlePlayList* pThis,char* filename)
{
	FILE *f;
	int i;
	char c;
	char directory[1024];
	char tmp[2048];
	char line[1024];

	strncpy(directory,filename,1024);
	
	c=1;
	for (i=strlen(directory)-1;i>=0 && c!=0;i--)
	{
		c=directory[i];
		if (c=='/')
		{
			c=0;
		}
		directory[i]=c;
	}

	f=fopen(filename,"rb");
	if (!f)
	{
		return RETVAL_NOK;
	}
	pThis->numberOfEntries=0;
	pThis->currentEntry=0;
	fgets(line,sizeof(line),f);
	while (!feof(f))
	{
		int i;
		for (i=0;i<strlen(line);i++)
		{
			if (line[i]<' ')
			{
				line[i]=0;
			}
		}
		if (line[0]!=0)
		{
			if (line[0]=='/')	// this is an absolute path
			{
				snprintf(tmp,2048,"%s",line);
			} else {	// this is a relative entry
				snprintf(tmp,2048,"%s/%s",directory,line);
			}
			strncpy(pThis->songInfos[pThis->numberOfEntries].filename,tmp,1024);
			pThis->numberOfEntries++;
		}
		fgets(line,sizeof(line),f);
	}
	fclose(f);

	return RETVAL_OK;
}
int playlist_get_numbers(tHandlePlayList* pThis,int *pNumberOfEntries,int* pCurrentEntry)
{
	*pNumberOfEntries=pThis->numberOfEntries;
	*pCurrentEntry=pThis->currentEntry;
	return RETVAL_OK;
}
int playlist_set_current_entry(tHandlePlayList* pThis,int currentEntry)
{
	pThis->currentEntry=currentEntry;
	return RETVAL_OK;
}
int playlist_read_entry(tHandlePlayList* pThis,int index,tSongInfo *pSongInfo,char *pMarked)
{
	if (pSongInfo!=NULL)
	{
		if (index>pThis->numberOfEntries)
		{
			return RETVAL_NOK;
		}
		memcpy(pSongInfo,&pThis->songInfos[index],sizeof(tSongInfo));
	}
	if (pMarked!=NULL)
	{
		*pMarked=pThis->playListSelected[index];
	}

	return RETVAL_OK;
}

int playlist_commandline_option(tHandlePlayList* pThis,char* argument)
{
	int retval;
	int l;

	retval=RETVAL_NOK_COMMANDLINE;

	l=strlen(argument);
	if (strncmp("--playlist.m3u=",argument,15)==0 && l>15)
	{
		retval=playlist_load_m3u(pThis,&argument[15]);
	}
	
	return retval;
}
int playlist_add_entry(tHandlePlayList* pThis,tSongInfo* pSongInfo)
{
	int retval;

	retval=RETVAL_OK;
	if (pThis->numberOfEntries<PLAYLIST_MAX_INDEX)
	{
		memcpy(&pThis->songInfos[pThis->numberOfEntries],pSongInfo,sizeof(tSongInfo));
		pThis->numberOfEntries++;
	}
	return retval;
}
int playlist_add_dir(tHandlePlayList* pThis,char* directory)
{
	DIR *d;
	int l;
	int retval;
	struct dirent *dir;
	tSongInfo songInfo;
	retval=RETVAL_OK;
	d = opendir(directory);
	if (d)
	{ 
		while ((dir = readdir(d)) != NULL)
		{
			l=strlen(dir->d_name);

			if (dir->d_type==DT_REG && l>=4 && l<1023)
			{
				if (dir->d_name[l-4]=='.' && 
					(dir->d_name[l-3]=='m' || dir->d_name[l-3]=='M') && 
					(dir->d_name[l-2]=='p' || dir->d_name[l-2]=='P') && 
					(dir->d_name[l-1]=='3'))
				{
				//	strncpy(songInfo.filename,dir->d_name,strlen(dir->d_name));
					memset(&songInfo,0,sizeof(tSongInfo));
					snprintf(songInfo.filename,1024,"%s/%s",directory,dir->d_name);
					retval|=playlist_add_entry(pThis,&songInfo);
				}
			}	
		}
		closedir(d);
	}
	return retval;
}
int playlist_remove_all(tHandlePlayList* pThis)
{
	memset(pThis,0,sizeof(tHandlePlayList));
	return RETVAL_OK;
}
int playlist_remove_single(tHandlePlayList* pThis,int index)
{
	int j;
	pThis->numberOfEntries--;
	for (j=index;j<pThis->numberOfEntries;j++)
	{
		memcpy(&pThis->songInfos[j],&pThis->songInfos[j+1],sizeof(tSongInfo));
		memcpy(&pThis->playListSelected[j],&pThis->playListSelected[j+1],sizeof(char));
	}
	if (pThis->currentEntry>index)
	{
		pThis->currentEntry--;	
	}
	return RETVAL_OK;
}

int playlist_remove_selected(tHandlePlayList* pThis)
{
	int i;
	int retval;

	retval=RETVAL_OK;
	i=0;
	while (i<pThis->numberOfEntries)
	{
		if (pThis->playListSelected[i])
		{
			retval|=playlist_remove_single(pThis,i);
		} else {
			i++;
		}
	}
	return retval;
}

int playlist_select_toggle(tHandlePlayList* pThis,int index)
{
	if (index<pThis->numberOfEntries)
	{
		pThis->playListSelected[index]=1-pThis->playListSelected[index];
	}
	return RETVAL_OK;
}
int playlist_select_action(tHandlePlayList* pThis,ePLAYLIST_SELECT_ACTION action)
{
	int i;
	switch(action)
	{
		case ePLAYLIST_SELECT_ALL:
		for (i=0;i<pThis->numberOfEntries;i++)
		{
			pThis->playListSelected[i]=1;
		}	
		break;
		case ePLAYLIST_SELECT_NONE:
		for (i=0;i<pThis->numberOfEntries;i++)
		{
			pThis->playListSelected[i]=0;
		}	
		break;
		case ePLAYLIST_SELECT_INV:
		for (i=0;i<pThis->numberOfEntries;i++)
		{
			pThis->playListSelected[i]=1-pThis->playListSelected[i];
		}	
		break;
		default:
			return RETVAL_NOK;
	}
	return RETVAL_OK;
}
