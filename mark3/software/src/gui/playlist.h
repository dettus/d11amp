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
#include "datastructures.h"

// the way the playlist is handled is by simply loading the full .m3u file into memory.
#define	PLAYLIST_BUFSIZE	(1<<21)	// 2MByte
typedef struct _tHandlePlaylist
{
// buffer for the loaded file
	char m3uBuf[PLAYLUST_BUFSIZE];
	int m3uSize;

// size of one page
	int lines_per_page;
// index of the beginning of the first line within the page
	int indexFirst;
// index of the end of the last line within the page
	int indexEnd;


// index of the beginning of the last played filename
	int currentEntry;
	int firstLineNum;	// first line number in the page
} tHandlePlaylist;

int playlist_init(tHandlePlaylist* pThis);
int playlist_load(tHandlePlaylist* pThis,char *filename);

// navigation
int playlist_resize(tHandlePlayList* pThis,int lines_per_page);
int playlist_scroll_top(tHandlePlaylist* pThis);
int playlist_scroll_back(tHandlePlayList* pThis,int linenum);
int playlist_scroll_forward(tHandlePlayList* pThis,int linenum);
int playlist_scroll_end(tHandlePlaylist* pThis);

// selecting one file
int playlist_get_filename_by_line(tHandlePlayList* pThis,char* pFilename,char* pCurrent,int linenum); // get the filename in a line. pCurrent=1 if it is the current filename (so that it can be highlighted
int playlist_shuffle(tHandlePlayList* pThis,char* pFilename);	// returns a random filename
int playlist_prev(tHandlePlayList* pThis,char* pFilename);	// returns the previous filename
int playlist_next(tHandlePlayList* pThis,char* pFilename);	// returns the next filename
#endif

