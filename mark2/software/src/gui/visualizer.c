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

const signed int cTwiddle_real[256]={32767,32757,32727,32678,32609,32520,32412,32284,32137,31970,31785,31580,31356,31113,30851,30571,30272,29955,29621,29268,28897,28510,28105,27683,27244,26789,26318,25831,25329,24811,24278,23731,23169,22594,22004,21402,20787,20159,19519,18867,18204,17530,16845,16150,15446,14732,14009,13278,12539,11792,11038,10278,9511,8739,7961,7179,6392,5601,4807,4011,3211,2410,1607,804,0,-804,-1607,-2410,-3211,-4011,-4807,-5601,-6392,-7179,-7961,-8739,-9511,-10278,-11038,-11792,-12539,-13278,-14009,-14732,-15446,-16150,-16845,-17530,-18204,-18867,-19519,-20159,-20787,-21402,-22004,-22594,-23169,-23731,-24278,-24811,-25329,-25831,-26318,-26789,-27244,-27683,-28105,-28510,-28897,-29268,-29621,-29955,-30272,-30571,-30851,-31113,-31356,-31580,-31785,-31970,-32137,-32284,-32412,-32520,-32609,-32678,-32727,-32757,-32767,-32757,-32727,-32678,-32609,-32520,-32412,-32284,-32137,-31970,-31785,-31580,-31356,-31113,-30851,-30571,-30272,-29955,-29621,-29268,-28897,-28510,-28105,-27683,-27244,-26789,-26318,-25831,-25329,-24811,-24278,-23731,-23169,-22594,-22004,-21402,-20787,-20159,-19519,-18867,-18204,-17530,-16845,-16150,-15446,-14732,-14009,-13278,-12539,-11792,-11038,-10278,-9511,-8739,-7961,-7179,-6392,-5601,-4807,-4011,-3211,-2410,-1607,-804,0,804,1607,2410,3211,4011,4807,5601,6392,7179,7961,8739,9511,10278,11038,11792,12539,13278,14009,14732,15446,16150,16845,17530,18204,18867,19519,20159,20787,21402,22004,22594,23169,23731,24278,24811,25329,25831,26318,26789,27244,27683,28105,28510,28897,29268,29621,29955,30272,30571,30851,31113,31356,31580,31785,31970,32137,32284,32412,32520,32609,32678,32727,32757};
const signed int cTwiddle_imag[256]={0,-804,-1607,-2410,-3211,-4011,-4807,-5601,-6392,-7179,-7961,-8739,-9511,-10278,-11038,-11792,-12539,-13278,-14009,-14732,-15446,-16150,-16845,-17530,-18204,-18867,-19519,-20159,-20787,-21402,-22004,-22594,-23169,-23731,-24278,-24811,-25329,-25831,-26318,-26789,-27244,-27683,-28105,-28510,-28897,-29268,-29621,-29955,-30272,-30571,-30851,-31113,-31356,-31580,-31785,-31970,-32137,-32284,-32412,-32520,-32609,-32678,-32727,-32757,-32767,-32757,-32727,-32678,-32609,-32520,-32412,-32284,-32137,-31970,-31785,-31580,-31356,-31113,-30851,-30571,-30272,-29955,-29621,-29268,-28897,-28510,-28105,-27683,-27244,-26789,-26318,-25831,-25329,-24811,-24278,-23731,-23169,-22594,-22004,-21402,-20787,-20159,-19519,-18867,-18204,-17530,-16845,-16150,-15446,-14732,-14009,-13278,-12539,-11792,-11038,-10278,-9511,-8739,-7961,-7179,-6392,-5601,-4807,-4011,-3211,-2410,-1607,-804,0,804,1607,2410,3211,4011,4807,5601,6392,7179,7961,8739,9511,10278,11038,11792,12539,13278,14009,14732,15446,16150,16845,17530,18204,18867,19519,20159,20787,21402,22004,22594,23169,23731,24278,24811,25329,25831,26318,26789,27244,27683,28105,28510,28897,29268,29621,29955,30272,30571,30851,31113,31356,31580,31785,31970,32137,32284,32412,32520,32609,32678,32727,32757,32767,32757,32727,32678,32609,32520,32412,32284,32137,31970,31785,31580,31356,31113,30851,30571,30272,29955,29621,29268,28897,28510,28105,27683,27244,26789,26318,25831,25329,24811,24278,23731,23169,22594,22004,21402,20787,20159,19519,18867,18204,17530,16845,16150,15446,14732,14009,13278,12539,11792,11038,10278,9511,8739,7961,7179,6392,5601,4807,4011,3211,2410,1607,804};


#define	CMUL_REAL(ar,ai, br,bi) ((ar)*(br)-(ai)*(bi))
#define	CMUL_IMAG(ar,ai, br,bi) ((ar)*(bi)+(ai)*(br))
int visualizer_fft256(signed short *pPcm,signed short *pOut)
{
	int i;
	int j;
	int yr,yi;

	for (i=0;i<512;i+=2)
	{
		yr=0;
		yi=0;
		for (j=0;j<256;j++)
		{
			yr+=CMUL_REAL(pPcm[i+0],pPcm[i+1], cTwiddle_real[(i*j)%256],cTwiddle_imag[(i*j)%256]);
			yi+=CMUL_IMAG(pPcm[i+0],pPcm[i+1], cTwiddle_real[(i*j)%256],cTwiddle_imag[(i*j)%256]);
		}
		yr/=32767;
		yi/=32767;
		pOut[i+0]=yr;
		pOut[i+1]=yi;
	}
	return RETVAL_OK;
}
int visualizer_spectrum256(signed short *pFFT,unsigned short *pEnergy)
{
	int i;
	signed long long x1;
	signed long long x2;
	signed long long x3;
	signed long long x4;
	signed long long y;
	
	for (i=0;i<128;i++)
	{
		x1=pFFT[i*2+0];
		x2=pFFT[i*2+1];
		x3=pFFT[511-(i*2+0)];
		x4=pFFT[511-(i*2+1)];

		y=(x1*x1)+(x2*x2);
		y+=(x3*x3)+(x4*x4);
// TODO: sqrt
		pEnergy[i]=y/(32767*4);
	}
	return RETVAL_OK;
}
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


