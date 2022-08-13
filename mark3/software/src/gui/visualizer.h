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

#ifndef	VISUALIZER_H
#define	VISUALIZER_H
#include "datastructures.h"
#include <pthread.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#define	VISUALIZER_FFTSIZE	256
#define	VISUALIZER_WIDTH	76
#define	VISUALIZER_HEIGHT	15
typedef enum
{
	eVISUALIZER_OSZILLOSCOPE=0,
	eVISUALIZER_FFT=1,
	eVISUALIZER_WATERFALL=2,
	eVISUALIZER_OFF=3
} eVisualizer;



typedef struct _tHandleVisualizer
{
// memory for the vizualization
	unsigned char visualizationDrawBuf[VISUALIZER_WIDTH*VISUALIZER_HEIGHT*4];
	eVisualizer visualizer;
	pthread_mutex_t mutex;
	double omega_r[VISUALIZER_FFTSIZE];
	double omega_i[VISUALIZER_FFTSIZE];
	double tmp_r[VISUALIZER_FFTSIZE];
	double tmp_i[VISUALIZER_FFTSIZE];

	double energybuf[VISUALIZER_FFTSIZE];
		
} tHandleVisualizer;

int visualizer_init(tHandleVisualizer *pThis);
int visualizer_cycle(tHandleVisualizer *pThis);
int visualizer_render(tHandleVisualizer *pThis,GdkPixbuf* pixbufDestination,int destx,int desty);
int visualizer_newPcm(tHandleVisualizer *pThis,signed short* pPcm,int n);


#endif
