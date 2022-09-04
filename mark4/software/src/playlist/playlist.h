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

#ifndef	PLAYLIST_H
#define	PLAYLIST_H

#define	PLAYLIST_MAX_INDEX	16384	// maximum number of entries in a playlist
#define	PLAYLIST_BUF_SIZE	(PLAYLIST_MAX_INDEX*512)	// assuming that the average filename has a length of 512 bytes

typedef struct _tHandlePlayList
{
	char playListBuf[PLAYLIST_BUF_SIZE];
	int playListPointer[PLAYLIST_MAX_INDEX];	// index pointer to the start of each entry
	char playListMarked[PLAYLIST_MAX_INDEX];// =1: the entry has been marked
	int entryNum;				// number of entries in the Buffer
	int endPointer;		// pointer to the end of the buffer
} tHandlePlayList;

int playlist_init(tHandlePlayList* pThis);
int playlist_load_m3u(tHandlePlayList* pThis,char* filename);
int playlist_get_number_of_entries(tHandlePlayList* pThis,int *pNumber);
int playlist_read_entry(tHandlePlayList* pThis,int index,tSongInfo *pSongInfo);
//int playlist_add_entry(tHandlePlayList* pThis,tSongInfo *pSongInfo);
//int playlist_remove_entry(tHandlePlayList* pThis,int index);
//int playlist_sort(tHandlePlayList* pThis);


#endif

