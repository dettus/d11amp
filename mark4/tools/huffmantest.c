#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define	TOTAL_NUM	19
typedef struct _tDefaultThemePackedDir
{
	char filename[13];
	int len;
	int start;
} tDefaultThemePackedDir;

#define	SYMBOLNUM	256
#define	MAXPATTERNLEN	64

#define	NODEMARKER	0x0000		
#define	LEAFMARKER	0xff00

typedef struct _tHuffman
{
        unsigned short huffmantree[2*2*SYMBOLNUM];
        int nodecnt;
        unsigned char bitpattern[SYMBOLNUM][MAXPATTERNLEN];
        int patternlen[SYMBOLNUM];
} tHuffman;

void mktree(unsigned char* pInBuf,int len,unsigned short *pOutBuf,int *nodecnt)
{
	int i;
	int ncnt;
	int nweight[SYMBOLNUM]={0};	// node weight
	int lweight[SYMBOLNUM]={0};	// leave weight
	int done;

	ncnt=0;
	// first: histogram over the input buffer
	for (i=0;i<len;i++)
	{
		lweight[pInBuf[i]]++;	
	}
	
	// second: repeatedly, combine the two least used leaves into a node. then combine the nodes.
	done=0;
	while (!done)
	{
		int min1,min2;
		int minleaf1,minleaf2;
		int minnode1,minnode2;
		int leafdonecnt;
		int nodedonecnt;


		min1=min2=0;
		minleaf1=minleaf2=minnode1=minnode2=-1;
		// find the two least used leaves
		leafdonecnt=0;
		for (i=0;i<SYMBOLNUM;i++)
		{
			if (lweight[i]==0)
			{
				leafdonecnt++;
			} else {
				if (min1==0 || min1>lweight[i])
				{
					minleaf2=minleaf1;	
					min2=min1;
					minleaf1=i;
					min1=lweight[i];
				}
				else if (min2==0 || min2>lweight[i])
				{
					minleaf2=i;
					min2=lweight[i];
				}
			}
		}
		// find a node, which is still lighter than the lightest leaves.
		nodedonecnt=0;
		for (i=0;i<ncnt;i++)
		{
			if (nweight[i]!=0)
			{
				nodedonecnt++;
				if (min1==0 || min1>nweight[i])
				{
					minleaf2=minleaf1;
					minnode2=minnode1;

					min2=min1;
					minnode1=i;
					min1=nweight[i];
				} else if (min2==0 || min2>nweight[i]) {
					min2=nweight[i];
					minnode2=i;
				}
			}
		}
		// if there is only one single node with a weight, the tree is complete
		if (leafdonecnt!=SYMBOLNUM || nodedonecnt!=1)
		{
			// combine the two least used nodes OR leaves into a new node
			// left branch of the tree
			if (minnode1!=-1)
			{
				pOutBuf[2*ncnt+0]=minnode1|NODEMARKER;
				nweight[ncnt]=nweight[minnode1];	// move the weight of the node to the new one
				nweight[minnode1]=0;			// we do not need the weight from the previous one
			} else {	// it was a leaf
				pOutBuf[2*ncnt+0]=minleaf1|LEAFMARKER;	// mark it as leaf
				nweight[ncnt]=lweight[minleaf1];	// move the weight of the leaf to the new node
				lweight[minleaf1]=0;			// no need to rememebr the weight of the leaf
			}
		// right branches
			if (minnode2!=-1)
			{
				pOutBuf[2*ncnt+1]=minnode2|NODEMARKER;
				nweight[ncnt]+=nweight[minnode2];	// move the weight of the node to the new one
				nweight[minnode2]=0;			// we do not need the weight from the previous one
			} else {	// it was a leaf
				pOutBuf[2*ncnt+1]=minleaf2|LEAFMARKER;	// mark it as leaf
				nweight[ncnt]+=lweight[minleaf2];	// move the weight of the leaf to the new node
				lweight[minleaf2]=0;			// no need to rememebr the weight of the leaf
			}
			ncnt++;
		} else {
			done=1;
		}
	}
	*nodecnt=ncnt;	
}


// try to find the terminal symbol inside the huffman tree
int traceback_bitpattern(int node,tHuffman *pHuffman,unsigned char symbol)
{
	int found;
	unsigned short bleft,bright;

	bleft =pHuffman->huffmantree[2*node+0];
	bright=pHuffman->huffmantree[2*node+1];

	if (bleft&LEAFMARKER && (symbol==(bleft&0xff)))
	{
		pHuffman->bitpattern[symbol][0]=1;	// left branch	 --> 1
		pHuffman->patternlen[symbol]=1;
		return 1;				// FOUND IT!	
	}
	if (bright&LEAFMARKER && (symbol==(bright&0xff)))
	{
		pHuffman->bitpattern[symbol][0]=0;	// right branch --> 0
		pHuffman->patternlen[symbol]=1;
		return 1;				// FOUND IT!	
	}
	
	if (!(bleft&LEAFMARKER))	// it is a branch. follow it
	{
		if (traceback_bitpattern(bleft,pHuffman,symbol))		// recursive search
		{
			pHuffman->bitpattern[symbol][pHuffman->patternlen[symbol]]=1;	// it was in the left branch --> 1
			pHuffman->patternlen[symbol]++;
			return 1;
		}
	}	

	if (!(bright&LEAFMARKER))	// it is a branch. follow it
	{
		if (traceback_bitpattern(bright,pHuffman,symbol))	// recursive search
		{
			pHuffman->bitpattern[symbol][pHuffman->patternlen[symbol]]=1;	// it was in the right branch --> 0
			pHuffman->patternlen[symbol]++;
			return 1;
		}
	}

	return 0;	// no match found.
}

void traceback(tHuffman* pHuffman)
{
	int i;
	
	for (i=0;i<SYMBOLNUM;i++)
	{
		traceback_bitpattern(pHuffman->nodecnt-1,pHuffman,i);
	}
}

unsigned char inbuf[32768];
unsigned char outbuf[32768];
int main(int argc,char** argv)
{
	FILE *f;
	
	tDefaultThemePackedDir dir[TOTAL_NUM];
	tHuffman huffman;
	int bytes;
	int i;
	int bits;
	int histogram[256]={0};

	if (argc!=2)
	{
		printf("please run with %s debug.packed.bin\n",argv[0]);
		return -1;
	}
	f=fopen(argv[1],"rb");
	fread(dir,sizeof(tDefaultThemePackedDir),TOTAL_NUM,f);
	bytes=fread(inbuf,sizeof(char),sizeof(inbuf),f);
	fclose(f);	
	memset(&huffman,0,sizeof(tHuffman));

	printf("read %d bytes\n",bytes);
	printf("generating tree\n");

	mktree(inbuf,bytes,huffman.huffmantree,&huffman.nodecnt);
	printf("nodes:%d\n",huffman.nodecnt);
	traceback(&huffman);

	printf("calculating potential...\n");
	for (i=0;i<bytes;i++)
	{
		histogram[inbuf[i]]++;	
	}
	bits=0;
	for (i=0;i<SYMBOLNUM;i++)
	{
		bits+=histogram[i]*huffman.patternlen[i];
	}
	printf("%d bytes -> %d bits (%d bytes)\n",bytes,bits,(bits+7)/8);
	
	
	
	
	return 0;	
}
