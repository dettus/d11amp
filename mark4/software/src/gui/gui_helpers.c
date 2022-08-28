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
#include "gui_helpers.h"

ePressable gui_helpers_find_pressable(tPressableBoundingBox *pBoundingBoxes,int num, double pressx,double pressy, GtkWidget* window,int defaultwidth,int defaultheight)
{
	int i;
	double scaleX;
	double scaleY;
	ePressable retval;
	

	scaleX=(double)defaultwidth/(double)gtk_widget_get_width(window);
	scaleY=(double)defaultheight/(double)gtk_widget_get_height(window);


	retval=ePRESSED_NONE;
	for (i=0;i<num;i++)
	{
		double x1,x2;
		double y1,y2;

		x1=pBoundingBoxes[i].posx;
		x2=x1+pBoundingBoxes[i].dimx;
		y1=pBoundingBoxes[i].posy;
		y2=y1+pBoundingBoxes[i].dimy;

		x1*=scaleX;
		x2*=scaleX;
		y1*=scaleY;
		y2*=scaleY;


		if (pressx>=x1 && pressx<x2 && pressy>=y1 && pressy<y2)
		{
			retval=pBoundingBoxes[i].pressable;
		}
	}

	return retval;
}

