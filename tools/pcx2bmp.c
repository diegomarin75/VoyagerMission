#include <malloc.h>
#include "include/gfiles.c"

void main(int argc,char **argv)
{
    //Variables
    int iAx,iAy;
    char *sBuf;
    Rgb sRgb[256];
    Gat sGat;
    
    //Check args
    if(argc!=3)
    {
        printf("Usage is: %s <file.pcx> <file.bmp>\n",argv[0]);
        exit(0);
    }
    
    //Action mesaje
    printf("Converting %s to %s...\n",argv[1],argv[2]);
    
    //Load Pcx
    if(PcxGetAtt(argv[1],&sGat)!=GFILOK)
    { 
        printf("PcxGetSize(): No file\n"); exit(0); 
    }
    if((sBuf=malloc(sGat.iAx*sGat.iAy))==NULL)
    { 
        printf("Not enough memory\n"); exit(0); 
    }
    if(PcxLoad(argv[1],sBuf,sRgb,&sGat)!=GFILOK)
    { 
        printf("PcxLoad(): No file\n"); exit(0); 
    }
    
    //Bmp save
    if(BmpSave(argv[2],sBuf,sRgb,&sGat)!=GFILOK)
    {
        printf("BmpSave(): No file\n"); exit(0);
    }
    
}
