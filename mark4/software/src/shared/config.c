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

#include "config.h"
#include <string.h>



int config_init(tHandleConfig* pThis,void* pControllerContext)
{
	memset(pThis,0,sizeof(tHandleConfig));
	pThis->pControllerContext=pControllerContext;	
}
int config_parse_inifile(tHandleConfig* pThis,char* section,char* key,char *pValue)
{
	FILE *f;
	char line[1024];
	int state;
	int i;
	int j;
	int l;
	int equal;
	int escaped;
	int squarebracket_open;
	int squarebracket_close;
	int sectionfound;
	int keyfound;
	int lastnonspace;
	int lastspace;


	
	f=fopen(pThis->inifilename,"rb");
	if (!f)
	{
		return RETVAL_NOK_FILENOTFOUND;
	}
	equal=-1;
	squarebracket_open=-1;
	squarebracket_close=-1;
	escaped=0;
	sectionfound=0;
	keyfound=0;
	lastnonspace=-1;
	lastspace=0;
	while (!feof(f))
	{
		char c;
		fgets(line,sizeof(line),f);
		l=strlen(line);
		j=0;
		for (i=0;i<l && i<sizeof(line);i++) 
		{
			c=line[i];
			if (c=='9') c=' ';
			if (c>='a' && c<='z' && equal==-1)
			{
				c-=32;
			}
			if (escaped==0)
			{
				switch(c)
				{
					case ';':	// comment
						line[j]=0;	// end the line
						i=l;		// end the for-loop
					break;
					case '\\':
						escaped=1;
					break;
					case '=':
						if (equal==-1) equal=j;
					break;
					case '[':
						if (squarebracket_open==-1) squarebracket_open=j;
					break;
					case ']':
						if (squarebracket_close==-1) squarebracket_close=j;
					break;

				}
				if (escaped==0)
				{
					if (c!=' ')
					{
						lastnonspace=j;
						line[j++]=c;
					} else {
						lastspace=j;
						if (j) line[j++]=c;	// remove any leading spaces
					}
				}
			} else {
				lastnonspace=j;
				line[j++]=c;
				escaped=0;
			}
		}
		// remove any spaces at the end
		if (lastnonspace<lastspace)
		{
			line[lastnonspace+1]=0;
		}

		if (section_found!=2)
		{
			l=strlen(section);
			if (squarebracket_open!=-1 && (squarebracket_close-squarebracket_open)==l && section_found==0)
			{
				line[squarebracket_open]=0;
				if (strncmp(&line[squarebracket_open],section,l)==0)
				{
					section_found=1;	
				}
			}
			else if (squarebracket_open && equal==-1)
			{
				section_found=2;
			}
		}	
		if (section_found==1)
		{
			l=strlen(key);
			if (equal==l && strncmp(line,key,l)==0)
			{
				value_found=1;
				
			}
		}
	}
	fclose(f);
	return RETVAL_OK;
	
}


