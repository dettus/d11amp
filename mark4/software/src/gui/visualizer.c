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
#include <stdio.h>
#include <math.h>


#define PERMUTE8(i)  (((i & 0x001) << 7) | ((i & 0x002) << 5) | ((i & 0x004) << 3) \
                | ((i & 0x008) << 1) | ((i & 0x010) >> 1) | ((i & 0x020) >> 3) \
                | ((i & 0x040) >> 5) | ((i & 0x080) >> 7))

#define PERMUTE9(i)  (((i & 0x001) << 8) | ((i & 0x002) << 6) | ((i & 0x004) << 4) \
                | ((i & 0x008) << 2) | ((i & 0x010) << 0) | ((i & 0x020) >> 2) \
                | ((i & 0x040) >> 4) | ((i & 0x080) >> 6) | ((i & 0x100) >> 8))

#define PERMUTE10(i) (((i & 0x001) << 9) | ((i & 0x002) << 7) | ((i & 0x004) << 5) \
                | ((i & 0x008) << 3) | ((i & 0x010) << 1) | ((i & 0x020) >> 1) \
                | ((i & 0x040) >> 3) | ((i & 0x080) >> 5) | ((i & 0x100) >> 7) \
                | ((i & 0x200) >> 9))

#define PERMUTE11(i) (((i & 0x001) << 10) | ((i & 0x002) << 8) | ((i & 0x004) << 6) \
                | ((i & 0x008) << 4) | ((i & 0x010) << 2) | ((i & 0x020) >> 0) \
                | ((i & 0x040) >> 2) | ((i & 0x080) >> 4) | ((i & 0x100) >> 6) \
                | ((i & 0x200) >> 8) | ((i & 0x400) >> 10))



int visualizer_init(tHandleVisualizer *pThis,tHandleThemeManager *pHandleThemeManager)
{
	int i;
	memset(pThis,0,sizeof(tHandleVisualizer));
	memset(pThis->oszilloscope,0,sizeof(pThis->oszilloscope));
	pThis->pHandleThemeManager=pHandleThemeManager;
	for (i=0;i<sizeof(pThis->visualizationDrawBuf);i+=4)
	{
		pThis->visualizationDrawBuf[i+3]=0xff;
	}
	pThis->visualizer=eVISUALIZER_OFF;


	for (i=0;i<VISUALIZER_FFTSIZE;i++)
	{
		pThis->omega_r[i]= cos(2*M_PI*i/VISUALIZER_FFTSIZE);
		pThis->omega_i[i]=-sin(2*M_PI*i/VISUALIZER_FFTSIZE);
	}
	for (i=0;i<VISUALIZER_FFTSIZE;i++)
	{
		pThis->energybuf[i]=0;
	}
	pThis->pastRingIdx=0;
	return RETVAL_OK;
}
int visualizer_fft(tHandleVisualizer *pThis,signed short *pPcm,double* pOut)
{
	int i;
	int sigma;
	int alpha;
	int beta;
	int betainc;
	int omega;
	int omegainc;
	int omegamask;
	int notmask;
	int mask;
	int n;


	double xr,xi;
	double yr,yi;
	int sigmas=0;

	n=VISUALIZER_FFTSIZE;
	switch(n)
	{
		case 256: 
			sigmas=8;
			omegamask=0x7f;
			notmask=0x1ff;
			break;
		case 512: 
			sigmas=9;
			omegamask=0xff;
			notmask=0x3ff;
			break;
		case 1024:
			sigmas=10;
			omegamask=0x1ff;
			notmask=0x7ff;
			break;
	//	case 2048:
		default:
			sigmas=11;
			omegamask=0x3ff;
			notmask=0xfff;
			break;
	}
	for (i=0;i<n;i++)
	{
		switch(n)
		{
			case 256:
				pThis->tmp_r[i]=pPcm[2*PERMUTE8(i)+0];
				pThis->tmp_i[i]=pPcm[2*PERMUTE8(i)+1];
				break;
			case 512:
				pThis->tmp_r[i]=pPcm[2*PERMUTE9(i)+0];
				pThis->tmp_i[i]=pPcm[2*PERMUTE9(i)+1];
				break;
			case 1024:
				pThis->tmp_r[i]=pPcm[2*PERMUTE10(i)+0];
				pThis->tmp_i[i]=pPcm[2*PERMUTE10(i)+1];
				break;
			case 2048:
				pThis->tmp_r[i]=pPcm[2*PERMUTE11(i)+0];
				pThis->tmp_i[i]=pPcm[2*PERMUTE11(i)+1];
				break;
			default:
				return RETVAL_NOK;
				break;

		}
	}
	betainc=1;
	omegainc=n/2;
	mask=0;

	for (i=0;i<n/2;i++)
	{
		alpha=(i<<1);
		beta=alpha | 0x01;

		xr=pThis->tmp_r[alpha];xi=pThis->tmp_i[alpha];
		yr=pThis->tmp_r[beta]; yi=pThis->tmp_i[beta];

		pThis->tmp_r[alpha]=yr+xr;pThis->tmp_i[alpha]=yi+xi;
		pThis->tmp_r[beta] =xr-yr;pThis->tmp_i[beta] =xi-yi;

	}

	for (sigma=1;sigma<sigmas;sigma++)
	{
		double omegar;
		double omegai;
		double mulr;
		double muli;
		omega=0;
		betainc<<=1;
		omegainc>>=1;
		mask=(mask<<1)|0x01;
		notmask=notmask<<1;
		for (i=0;i<n/2;i++)
		{
			alpha=((i&notmask)<<1) | (i&mask);
			beta=alpha|betainc;
			omegar=pThis->omega_r[omega];
			omegai=pThis->omega_i[omega];
			xr=pThis->tmp_r[alpha];xi=pThis->tmp_i[alpha];
			yr=pThis->tmp_r[beta]; yi=pThis->tmp_i[beta];

			mulr=omegar*yr-omegai*yi;
			muli=omegar*yi+omegai*yr;

			pThis->tmp_r[alpha]=(xr+mulr);pThis->tmp_i[alpha]=(xi+muli);
			pThis->tmp_r[beta] =(xr-mulr);pThis->tmp_i[beta] =(xi-muli);
			omega += omegainc;
			omega &= omegamask;
		}
	}
	for (i=0;i<n;i++)
	{
		pOut[2*i+0]=pThis->tmp_r[i];
		pOut[2*i+1]=pThis->tmp_i[i];
	}
	return RETVAL_OK;
}

int visualizer_cycle(tHandleVisualizer *pThis)
{
	pthread_mutex_lock(&(pThis->mutex));
	switch (pThis->visualizer)
	{
		case eVISUALIZER_OFF:
			pThis->visualizer=eVISUALIZER_OSZILLOSCOPE;
			break;
		case eVISUALIZER_OSZILLOSCOPE:
			pThis->visualizer=eVISUALIZER_FFT;
			break;
		case eVISUALIZER_FFT:
			pThis->visualizer=eVISUALIZER_WATERFALL;
			break;
		default:
			pThis->visualizer=eVISUALIZER_OFF;
			break;
	}
	pthread_mutex_unlock(&(pThis->mutex));
	return RETVAL_OK;
}

int visualizer_render(tHandleVisualizer *pThis,GdkPixbuf* pixbufDestination,int destx,int desty)
{
	GdkPixbuf *pixbuf;
	pthread_mutex_lock(&(pThis->mutex));
	if (pThis->visualizer!=eVISUALIZER_OFF)
	{
		pixbuf=gdk_pixbuf_new_from_data(pThis->visualizationDrawBuf,GDK_COLORSPACE_RGB,TRUE,8,VISUALIZER_WIDTH,VISUALIZER_HEIGHT,VISUALIZER_WIDTH*4,NULL,NULL);
		gdk_pixbuf_copy_area(pixbuf,0,0,VISUALIZER_WIDTH,VISUALIZER_HEIGHT,pixbufDestination,destx,desty);
		g_object_unref(pixbuf);
	}
	pthread_mutex_unlock(&(pThis->mutex));
	return RETVAL_OK;
}


int visualizer_newPcm(tHandleVisualizer *pThis,signed short* pPcm,int n)
{
	tVisColor *pVisColors=NULL;
	int x,y;
	int width;
	int height;
//	int accu_value;
	int accu_x;
	int m;
	double max;
	int i,j;
	int ylast,ynext;
	double fftout[VISUALIZER_FFTSIZE*2];
	double energy[VISUALIZER_FFTSIZE];
	width=VISUALIZER_WIDTH;
	height=VISUALIZER_HEIGHT;

	pthread_mutex_lock(&(pThis->mutex));
	theme_manager_get_viscolors(pThis->pHandleThemeManager,&pVisColors);
	switch(pThis->visualizer)
	{
		case eVISUALIZER_OFF:
		memset(pThis->visualizationDrawBuf,0,sizeof(pThis->visualizationDrawBuf));
		for (i=0;i<sizeof(pThis->visualizationDrawBuf);i+=4)
		{
			pThis->visualizationDrawBuf[i+0]=pVisColors[0].red;
			pThis->visualizationDrawBuf[i+1]=pVisColors[0].green;
			pThis->visualizationDrawBuf[i+2]=pVisColors[0].blue;
			pThis->visualizationDrawBuf[i+3]=0xff;
		}
		break;
		case eVISUALIZER_OSZILLOSCOPE:
			memset(pThis->visualizationDrawBuf,0,sizeof(pThis->visualizationDrawBuf));
			for (i=0;i<sizeof(pThis->visualizationDrawBuf);i+=4)
			{
				pThis->visualizationDrawBuf[i+0]=pVisColors[0].red;
				pThis->visualizationDrawBuf[i+1]=pVisColors[0].green;
				pThis->visualizationDrawBuf[i+2]=pVisColors[0].blue;
				pThis->visualizationDrawBuf[i+3]=0xff;
			}
			for (i=1;i<VISUALIZER_HEIGHT;i+=2)
			{
				int j;
				for (j=1;j<VISUALIZER_WIDTH;j+=2)
				{
					int p;
					p=i*VISUALIZER_WIDTH+j;
					pThis->visualizationDrawBuf[p*4+0]=pVisColors[1].red;
					pThis->visualizationDrawBuf[p*4+1]=pVisColors[1].green;
					pThis->visualizationDrawBuf[p*4+2]=pVisColors[1].blue;
					pThis->visualizationDrawBuf[p*4+3]=0xff;

				}
			}
			for (i=0;i<sizeof(pThis->oszilloscope);i++)
			{
				if (pThis->oszilloscope[i]!=0)
				{
					pThis->oszilloscope[i]=(pThis->oszilloscope[i]+1)%23;	// 0 is tbackground. 18 is the brightest, 22 is the dimmest
				}
			}
			for (i=0;i<sizeof(pThis->visualizationDrawBuf);i+=4)
			{
				pThis->visualizationDrawBuf[i+3]=0xff;
			}
			x=0;
			//accu_value=0;
			accu_x=0;
			m=0;		
			max=0;
			n/=2;
			for (i=0;i<n;i++)
			{
				pPcm[i]=(pPcm[2*i+0]+pPcm[2*i+1])/2;		// stereo->mono downmix
				if (pPcm[i]>max) max=pPcm[i];
				if (-pPcm[i]>max) max=-pPcm[i];
			}
			if (max<4096) 
			{
				max=4096;
			}
			ylast=0;	
			for (i=0;i<n && x<width;i++)
			{
				//accu_value+=pPcm[i];
				accu_x+=width;
				m++;
				if (accu_x>=n)
				{
					//accu_value/=m;
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
					pThis->oszilloscope[x+width*y]=18;	// brightest color
					ynext=y;
					if (ylast<y) 
					{
						ylast^=y;
						y^=ylast;
						ylast^=y;
					}
					ylast=ynext;
					m=0;
					x++;		
					//accu_value=0;
					accu_x-=n;
				}
				
			}
			for (i=0;i<VISUALIZER_WIDTH*VISUALIZER_HEIGHT;i++)
			{
				if (pThis->oszilloscope[i]!=0)
				{
					pThis->visualizationDrawBuf[4*i+0]=pVisColors[pThis->oszilloscope[i]].red;
					pThis->visualizationDrawBuf[4*i+1]=pVisColors[pThis->oszilloscope[i]].green;
					pThis->visualizationDrawBuf[4*i+2]=pVisColors[pThis->oszilloscope[i]].blue;
				}
			}
			
		break;
		case eVISUALIZER_FFT:
			memset(pThis->visualizationDrawBuf,0,sizeof(pThis->visualizationDrawBuf));
			for (i=0;i<sizeof(pThis->visualizationDrawBuf);i+=4)
			{
				pThis->visualizationDrawBuf[i+0]=pVisColors[0].red;
				pThis->visualizationDrawBuf[i+1]=pVisColors[0].green;
				pThis->visualizationDrawBuf[i+2]=pVisColors[0].blue;
				pThis->visualizationDrawBuf[i+3]=0xff;
			}
			for (i=1;i<VISUALIZER_HEIGHT;i+=2)
			{
				int j;
				for (j=1;j<VISUALIZER_WIDTH;j+=2)
				{
					int p;
					p=i*VISUALIZER_WIDTH+j;
					pThis->visualizationDrawBuf[p*4+0]=pVisColors[1].red;
					pThis->visualizationDrawBuf[p*4+1]=pVisColors[1].green;
					pThis->visualizationDrawBuf[p*4+2]=pVisColors[1].blue;
					pThis->visualizationDrawBuf[p*4+3]=0xff;

				}
			}
			visualizer_fft(pThis,pPcm,fftout);
			max=0;
			for (i=0;i<VISUALIZER_FFTSIZE;i++)
			{
				double e;
				e=(fftout[i*2+0]*fftout[i*2+0]+fftout[i*2+1]*fftout[i*2+1]);
				pThis->energybuf[i]=(0.6*e+0.4*pThis->energybuf[i]);
				energy[i]=pThis->energybuf[i];
				
			}
			if (energy[0]==0) energy[0]=1;	// avoid division by zero
			for (i=1;i<VISUALIZER_FFTSIZE;i++)
			{
				energy[i]+=energy[VISUALIZER_FFTSIZE-i];
		//		energy[i]/=(2*energy[0]);
				if (energy[i]>max)
				{
					max=energy[i];
				}
			}
			//if (max<25304637553448.0) max=25304637553448.0;
			max=0;
			if (max<253046375534.0) max=253046375534.0;
//			max=energy[0]/16;
			for (i=0;i<SPECTRUMBARS;i++)
			{
				double y;
				int x;
				y=(energy[i+1]*15)/max;
				if (y>15) y=15;
				pThis->pastRing[pThis->pastRingIdx][i]=y;
				for (x=i*BARWIDTH+1;x<i*BARWIDTH+BARWIDTH;x++)
				{
					for (j=0;j<y;j++)
					{
						pThis->visualizationDrawBuf[0+4*(x+width*(14-j))]=pVisColors[14-j+2].red;
						pThis->visualizationDrawBuf[1+4*(x+width*(14-j))]=pVisColors[14-j+2].green;
						pThis->visualizationDrawBuf[2+4*(x+width*(14-j))]=pVisColors[14-j+2].blue;
					}
				}
			}
			for (i=0;i<SPECTRUMBARS;i++)
			{
				int j;
				int max;
				int x;

				max=0;
				for (j=0;j<SPECTRUMPAST;j++)
				{
					if (pThis->pastRing[j][i]>max) max=pThis->pastRing[j][i];
				}
				pThis->pastRingIdx=(pThis->pastRingIdx+1)%SPECTRUMPAST;
				for (x=i*BARWIDTH+1;x<i*BARWIDTH+BARWIDTH;x++)
				{
					pThis->visualizationDrawBuf[0+4*(x+width*(14-max))]=pVisColors[23].red;		// 23=analyzer peak dots
					pThis->visualizationDrawBuf[1+4*(x+width*(14-max))]=pVisColors[23].green;	// 23=analyzer peak dots
					pThis->visualizationDrawBuf[2+4*(x+width*(14-max))]=pVisColors[23].blue;	// 23=analyzer peak dots
				}
			}
		break;
		case eVISUALIZER_WATERFALL:
			memmove(pThis->visualizationDrawBuf,&(pThis->visualizationDrawBuf[76*4]),sizeof(pThis->visualizationDrawBuf)-76*4);
			for (i=0;i<sizeof(pThis->visualizationDrawBuf);i+=4)
			{
				pThis->visualizationDrawBuf[i+3]=0xff;
			}
			visualizer_fft(pThis,pPcm,fftout);
			max=0;
			for (i=0;i<VISUALIZER_FFTSIZE;i++)
			{
				double e;
				e=(fftout[i*2+0]*fftout[i*2+0]+fftout[i*2+1]*fftout[i*2+1]);
				pThis->energybuf[i]=(0.6*e+0.4*pThis->energybuf[i]);
				energy[i]=pThis->energybuf[i];
			}
			if (energy[0]==0) energy[0]=1;	// avoid division by zero
			for (i=1;i<VISUALIZER_FFTSIZE;i++)
			{
				energy[i]+=energy[VISUALIZER_FFTSIZE-i];
		//		energy[i]/=(2*energy[0]);
				if (energy[i]>max)
				{
					max=energy[i];
				}
			}
			max=0;
			//if (max<25304637553448.0) max=25304637553448.0;
			if (max<253046375534.0) max=253046375534.0;
//			max=energy[0]/16;
			for (i=0;i<76;i++)
			{
				double y;
				y=(energy[i+1]*14)/max;
				if (y>14) y=14;
				pThis->visualizationDrawBuf[0+4*(i+width*(14))]=pVisColors[14-(int)y+2].red;
				pThis->visualizationDrawBuf[1+4*(i+width*(14))]=pVisColors[14-(int)y+2].green;
				pThis->visualizationDrawBuf[2+4*(i+width*(14))]=pVisColors[14-(int)y+2].blue;
			}
		break;
		default:
		break;

	}	
	pthread_mutex_unlock(&(pThis->mutex));
	return RETVAL_OK;
}


