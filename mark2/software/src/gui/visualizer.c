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
#include "visualizer.h"
int visualizer_init(tHandleVisualizer *pThis)
{
	int i;
	memset(pThis,0,sizeof(tHandleVisualizer));
	for (i=0;i<sizeof(pThis->visualizationDrawBuf);i+=4)
	{
		pThis->visualizationDrawBuf[i+3]=0xff;
	}
	pThis->visualizer=eVISUALIZER_OFF;
	return RETVAL_OK;
}
int visualizer_cycle(tHandleVisualizer *pThis)
{
	pthread_mutex_lock(&(pThis->mutex));
	if (pThis->visualizer==eVISUALIZER_OFF)
	{
		pThis->visualizer=eVISUALIZER_OSZILLOSCOPE;
	} else {
		//pThis->visualizer++;// TODO
		pThis->visualizer=eVISUALIZER_OFF;
	}	
	pthread_mutex_unlock(&(pThis->mutex));
	return RETVAL_OK;
}
int visualizer_render(tHandleVisualizer *pThis,GdkPixbuf** pPixbuf)
{
	if (*pPixbuf==NULL)
	{
		g_object_unref(*pPixbuf);
		*pPixbuf=NULL;
	}
	pthread_mutex_lock(&(pThis->mutex));
	*pPixbuf=gdk_pixbuf_new_from_data(pThis->visualizationDrawBuf,GDK_COLORSPACE_RGB,TRUE,8,76,15,76*4,NULL,NULL);
	pthread_mutex_unlock(&(pThis->mutex));
	return RETVAL_OK;
}
int visualizer_newPcm(tHandleVisualizer *pThis,signed short* pPcm,int n)
{
	int x,y;
	int width;
	int height;
	int accu_value;
	int accu_x;
	int m;
	int max;
	int i;
	int ylast,ynext;
	width=76;
	height=15;

	pthread_mutex_lock(&(pThis->mutex));
	switch(pThis->visualizer)
	{
		case eVISUALIZER_OFF:
			memset(pThis->visualizationDrawBuf,0,sizeof(pThis->visualizationDrawBuf));
			for (i=0;i<sizeof(pThis->visualizationDrawBuf);i+=4)
			{
				pThis->visualizationDrawBuf[i+3]=0xff;
			}
			break;
		//case eVISUALIZER_OSZILLOSCOPE:
		default:
			memset(pThis->visualizationDrawBuf,0,sizeof(pThis->visualizationDrawBuf));
			for (i=0;i<sizeof(pThis->visualizationDrawBuf);i+=4)
			{
				pThis->visualizationDrawBuf[i+3]=0xff;
			}
			x=0;
			accu_value=0;
			accu_x=0;
			m=0;		
			max=0;
			for (i=0;i<n;i++)
			{
				if (pPcm[i]>max) max=pPcm[i];
				if (-pPcm[i]>max) max=-pPcm[i];
			}
			if (max<4096) 
			{
				max=4096;
			}
			for (i=0;i<n && x<width;i++)
			{
				accu_value+=pPcm[i];
				accu_x+=width;
				m++;
				if (accu_x>=n)
				{
					accu_value/=m;
					y=pPcm[i];
					if (max>height/2)
					{
						y*=(height/2);
						y/=max;
					}
					y+=height/2;
					if (y<0) y=0;
					if (y>=height) y=height-1;
					if (x>=width-1) x=width-1;
					if (x==0) ylast=y;
					ynext=y;
					if (ylast<y) 
					{
						ylast^=y;
						y^=ylast;
						ylast^=y;
					}
					do
					{
						pThis->visualizationDrawBuf[0+4*(x+width*y)]=0xff;	
						pThis->visualizationDrawBuf[1+4*(x+width*y)]=0xff;	
						pThis->visualizationDrawBuf[2+4*(x+width*y)]=0xff;	
						y++;
					}
					while (y<ylast);
					ylast=ynext;
					m=0;
					x++;		
					accu_value=0;
					accu_x-=n;
				}	
				
			}
		break;
	}	
	pthread_mutex_unlock(&(pThis->mutex));
	return RETVAL_OK;
}


