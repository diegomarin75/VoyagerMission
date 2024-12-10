// PCX & BMP files managing library
#include <time.h>
#include <stdio.h>
#include <string.h>

// --------------------------------------------------------------------------
// Constants

//Error codes
#define GFILOK  1           //File ok
#define GFILERR 2           //File error
#define GFILMER 3           //Memory allocation error
#define GFILTYE 4           //File type error
#define GFILCAL 5           //Function call error
#define GFILNFO 6           //Not found
#define GFILFUL 7           //Cache memory is full

//File conversion constants
#define CVNULL   0          //No conversion
#define CVRGB555 1          //File conversion for 32K color modes
#define CVRGB565 2          //File conversion for 64K color modes

//File lock mode
#define GFLOCKED   0        //File stays in cache memory 
#define GFVOLATILE 1        //Memory for file can be reused
 
//Other
#define TYPEBMP   1         //BMP file
#define TYPEPCX   2         //PCX file
#define GMEMFIL   20        //Cache memory length (files)

// --------------------------------------------------------------------------
// Type definitions
typedef struct Rgb Rgb; //Palette register
typedef struct Gat Gat; //Graphic files attributes
typedef struct Cmr Cmr; //Cache memory register
typedef struct Cmi Cmi; //Cache memory interface

// --------------------------------------------------------------------------
// Structures

//Palette register
struct Rgb
{
    unsigned char r,g,b; 
};

//File attributes
struct Gat
{
    int  iAx;  //Width
    int  iAy;  //Heigth
    long lGSz; //Size (image definition part)
    int  iCol; //Colors
};

//Cache memory register
struct Cmr
{
    char sFile[255];     //File name
    Gat  *sGat;          //Attributes data
    Rgb  *sRgb;          //Palette data
    unsigned char *sImg; //Image data
    clock_t tTim;        //Time in which the file was loaded
    int iUsed;           //Used entry flag
    int iLock;           //Lock flag
};

//Cache memory interface structure
struct Cmi
{
    Gat  *sGat;          //Attributes data
    Rgb  *sRgb;          //Palette data
    unsigned char *sImg; //Image data
};

// --------------------------------------------------------------------------
// Function prototypes
int  PcxGetAtt(char *,Gat *);
int  PcxLoad(char *,unsigned char *,Rgb *,Gat *);
int  BmpGetAtt(char *,Gat *);
int  BmpLoad(char *,unsigned char *,Rgb *,Gat *);
int  BmpSave(char *,unsigned char *,Rgb *,Gat *);
void RGBColor(int,int,Rgb *,int *); 
void Conv256toRGB(int,Gat *,Rgb *,unsigned char *,unsigned char *);
void GMemInit(int);
void GMemClos(void);
int  GMemRead(char *,Cmi *,int);
int  GMemFree(char *);

// --------------------------------------------------------------------------
// Global variables
int _iFConv;        //File conversion mode
Cmr _sCmr[GMEMFIL]; //Cache memory registers

// --------------------------------------------------------------------------
// Get PCX file attributes
int PcxGetAtt(char *sFile,Gat *sGat)
{
    //Variables
    FILE *fFil;            //File pointer
    int  iA,iB;            //Size bytes
    int  iCol;             //Colors
    int  iBpp;             //Bytes per pixel
    int  iXmin,iXmax;      //File length X
    int  iYmin,iYmax;      //File length Y

    //Open file
    if((fFil=fopen(sFile,"rb"))==NULL) return(GFILERR);

    //Set file pointer on byte 4
    fseek(fFil,4,SEEK_SET);

    //Read file size
    iA=fgetc(fFil); iB=fgetc(fFil); iXmin=(unsigned)iA+256*(unsigned)iB;
    iA=fgetc(fFil); iB=fgetc(fFil); iYmin=(unsigned)iA+256*(unsigned)iB;
    iA=fgetc(fFil); iB=fgetc(fFil); iXmax=(unsigned)iA+256*(unsigned)iB;
    iA=fgetc(fFil); iB=fgetc(fFil); iYmax=(unsigned)iA+256*(unsigned)iB;

    //Get colors
    fseek(fFil,65,SEEK_SET);
    iBpp=fgetc(fFil);
    switch(iBpp)
    {
        case 1:  iCol=256; break;                //256 Color palette
        case 3:  iCol=0; break;                  //True color RGB
        default: fclose(fFil); return(GFILTYE);  //Not supported
    }
    
    //File attributes
    sGat->iAx=iXmax-iXmin+1; 
    sGat->iAy=iYmax-iYmin+1; 
    sGat->lGSz=sGat->iAx*sGat->iAy;
    sGat->iCol=iCol;

    //Close file
    fclose(fFil);
    
    //Return code
    return(GFILOK);

}

// --------------------------------------------------------------------------
// Load PCX file into memory buffer
int PcxLoad(char *sFile,unsigned char *sBuf,Rgb *sRgb,Gat *sGat)
{
    //Variables
    FILE *fFil;            //File pointer
    long l,lReps,lCount=0; //Algorithm variables
    unsigned char ucByte;  //Byte 

    //Return error if bytes per pixel is not 1
    if(sGat->iCol!=256) return (GFILCAL);
    
    //Open file
    if((fFil=fopen(sFile,"rb"))==NULL) return(GFILERR);

    //Read palette
    fseek(fFil,-(sGat->iCol*sizeof(Rgb)),SEEK_END);
    for(l=0;l<sGat->iCol;l++){
        sRgb[l].r=((unsigned char)fgetc(fFil))>>2;
        sRgb[l].g=((unsigned char)fgetc(fFil))>>2;
        sRgb[l].b=((unsigned char)fgetc(fFil))>>2;
    }

    //Decoding loop
    fseek(fFil,128,SEEK_SET);
    while(lCount<sGat->lGSz){
      ucByte=(unsigned char)fgetc(fFil);
      if(ucByte>=192){
        lReps=ucByte&0x3f; ucByte=(unsigned char)fgetc(fFil);
        for(l=0;l<lReps;l++) sBuf[lCount+l]=ucByte;
        lCount+=lReps;
      }
      else sBuf[lCount++]=ucByte;
    }

    //Close file
    fclose(fFil);
    
    //Return no error
    return(GFILOK);

}

// --------------------------------------------------------------------------
// Get BMP file attributes
int BmpGetAtt(char *sFile,Gat *sGat)
{
    //Variables
    FILE *fFil;            //File pointer
    int  iA,iB;            //Size bytes
    int  iXmin,iXmax;      //File length X
    int  iYmin,iYmax;      //File length Y
    int  iCol;             //Colors
    int  iBpp;             //Bytes per pixel

    //Open file
    if((fFil=fopen(sFile,"rb"))==NULL) return(GFILERR);

    //Read file size
    fseek(fFil,4,SEEK_SET);
    iA=fgetc(fFil); iB=fgetc(fFil); iXmin=(unsigned)iA+256*(unsigned)iB;
    iA=fgetc(fFil); iB=fgetc(fFil); iYmin=(unsigned)iA+256*(unsigned)iB;
    iA=fgetc(fFil); iB=fgetc(fFil); iXmax=(unsigned)iA+256*(unsigned)iB;
    iA=fgetc(fFil); iB=fgetc(fFil); iYmax=(unsigned)iA+256*(unsigned)iB;
    iA=fgetc(fFil); iB=fgetc(fFil); iCol= (unsigned)iA+256*(unsigned)iB;

    //Get colors
    iBpp=fgetc(fFil);
    switch(iBpp)
    {
        case 1:  iCol=256; break;                //256 Color palette
        case 3:  iCol=0; break;                  //True color RGB
        default: fclose(fFil); return(GFILTYE);  //Not supported
    }
    
    //Set attributes
    sGat->iAx=iXmax-iXmin+1; 
    sGat->iAy=iYmax-iYmin+1;
    sGat->lGSz=sGat->iAx*sGat->iAy; 
    sGat->iCol=iCol;

    //Close file
    fclose(fFil);
    
    //Return code
    return(GFILOK);

}

// --------------------------------------------------------------------------
// Load BMP file into memory buffer
int BmpLoad(char *sFile,unsigned char *sBuf,Rgb *sRgb,Gat *sGat)
{
    //Variables
    FILE *fFil; //File pointer
    long l;     //Algorithm variables

    //Call error
    if(sGat->iCol!=256) return(GFILCAL);
    
    //Open file
    if((fFil=fopen(sFile,"rb"))==NULL) return(GFILERR);

    //Reading loop
    fseek(fFil,128,SEEK_SET);
    for(l=0;l<sGat->lGSz;l++) sBuf[l]=(unsigned char)fgetc(fFil);
    
    //Read palette
    for(l=0;l<sGat->iCol;l++ ){
        sRgb[l].r=(unsigned char)fgetc(fFil);
        sRgb[l].g=(unsigned char)fgetc(fFil);
        sRgb[l].b=(unsigned char)fgetc(fFil);
    }

    //Close file
    fclose(fFil);
    
    //Return no error
    return(GFILOK); 

}

// --------------------------------------------------------------------------
// Save BMP file
int BmpSave(char *sFile,unsigned char *sBuf,Rgb *sRgb,Gat *sGat)
{
    //Variables
    FILE *fFil;            //File pointer
    unsigned char c;       //Unsigned char
    long l;                //Counter

    //Call error
    if(sGat->iCol!=256) return(GFILCAL);
    
    //Open file
    if((fFil=fopen(sFile,"wb"))==NULL) return(GFILERR);

    //Set BMP mark
    fprintf(fFil,"BMP0");
    
    //Set file attributes
    c=0;                 fwrite((void *)&c,sizeof(c),1,fFil);
    c=0;                 fwrite((void *)&c,sizeof(c),1,fFil);
    c=0;                 fwrite((void *)&c,sizeof(c),1,fFil);
    c=0;                 fwrite((void *)&c,sizeof(c),1,fFil);
    c=(sGat->iAx-1)%256; fwrite((void *)&c,sizeof(c),1,fFil);
    c=(sGat->iAx-1)/256; fwrite((void *)&c,sizeof(c),1,fFil);
    c=(sGat->iAy-1)%256; fwrite((void *)&c,sizeof(c),1,fFil);
    c=(sGat->iAy-1)/256; fwrite((void *)&c,sizeof(c),1,fFil);
    c=sGat->iCol;        fwrite((void *)&c,sizeof(c),1,fFil);
    
    //Set zeros
    for(l=0;l<119;l++) fprintf(fFil,"%c",0);
    
    //Write Image
    for(l=0;l<sGat->lGSz;l++) fwrite((void *)&(sBuf[l]),sizeof(char),1,fFil);

    //Write palette
    for(l=0;l<sGat->iCol;l++){
        fwrite((void *)&sRgb[l].r,sizeof(char),1,fFil);
        fwrite((void *)&sRgb[l].g,sizeof(char),1,fFil);
        fwrite((void *)&sRgb[l].b,sizeof(char),1,fFil);
    }

    //Close File
    fclose(fFil);
    
    //Return no error
    return(GFILOK); 

}

// --------------------------------------------------------------------------
// Color conversion from index to RGB 555 / 565
void RGBColor(int iFConv,     //Conversion mode
              int iIndex,     //Indexed color
              Rgb *sRgb,      //Color palette
              int *iRgbColor) //RGBColor 
{
    //Variables
    unsigned int uiRed,uiGreen,uiBlue; //RGB components
    
    //Read color components
    uiRed  =sRgb[iIndex].r;
    uiGreen=sRgb[iIndex].g;
    uiBlue =sRgb[iIndex].b;
    
    //Jump on conversion mode
    switch(iFConv)
    {
        //Convert to RGB 555
        case CVRGB555: uiRed  =((uiRed>>1)<<10);
                       uiGreen=((uiGreen>>1)<<5);
                       uiBlue =(uiBlue>>1);
                       break;
        
        //Convert to RGB 565
        case CVRGB565: uiRed  =((uiRed>>1)<<11);
                       uiGreen=(uiGreen<<5);
                       uiBlue =(uiBlue>>1);
                       break;
    }            
    
    //Return RGB color
    *iRgbColor =uiRed|uiGreen|uiBlue;

}

// --------------------------------------------------------------------------
// Indexed 256 to RGB conversion (565 & 555)
void Conv256toRGB(int iFConv,           //Conversion mode
                  Gat *sGat,            //File attributes
                  Rgb *sRgb,            //256 color palette
                  unsigned char *sSrc,  //File content (Source)
                  unsigned char *sDst)  //Destination
{
    //Variables
    long lIndex;          //Buffer index
    unsigned char ucByte; //Byte
    int iColor;           //RGB color
    
    //Conversion loop
    for(lIndex=0;lIndex<sGat->lGSz;lIndex++)
    {
        RGBColor(iFConv,sSrc[lIndex],sRgb,&iColor);    
        ucByte=(0xFF00&iColor)>>8; sDst[2*lIndex+1]=ucByte;
        ucByte=(0x00FF&iColor);    sDst[2*lIndex+0]=ucByte;
    }
}

// --------------------------------------------------------------------------
// Initialize cache memory
void GMemInit(int iFConv)
{
    //Variables
    int i; //Counter
    
    //Init loop
    for(i=0;i<GMEMFIL;i++) memset((void *)&_sCmr[i],0,sizeof(Cmr));
    
    //Init pointers to NULL
    for(i=0;i<GMEMFIL;i++)
    {
        _sCmr[i].sGat=NULL;
        _sCmr[i].sImg=NULL;
        _sCmr[i].sRgb=NULL;
    }
    
    //Set file conversion mode
    _iFConv=iFConv;

}

// --------------------------------------------------------------------------
// Close cache memory
void GMemClos(void)
{
    //Variables
    int i; //Counter
    
    //Close loop
    for(i=0;i<GMEMFIL;i++) 
    {
        if(_sCmr[i].iUsed)
        {
            if(_sCmr[i].sGat!=NULL) free(_sCmr[i].sGat);
            if(_sCmr[i].sRgb!=NULL) free(_sCmr[i].sRgb);
            if(_sCmr[i].sImg!=NULL) free(_sCmr[i].sImg);
        }
    }

}

// --------------------------------------------------------------------------
// Load graphic file (with cache memory)
int GMemRead(char *sFile,Cmi *sCmi,int iLock)
{
    //Variables
    int i;                       //Counter
    int iIdx;                    //Index
    int iFound;                  //Searching flag
    int iOld;                    //Oldest file
    int iLen;                    //File name length
    int iType;                   //File type
    unsigned char *sPtr0,*sPtr1; //Buffer pointers
    
    //File type
    iType=0;
    iLen=strlen(sFile);
    if(sFile[iLen-4]=='.' && sFile[iLen-3]=='b' && 
       sFile[iLen-2]=='m' && sFile[iLen-1]=='p' ) iType=TYPEBMP;
    if(sFile[iLen-4]=='.' && sFile[iLen-3]=='p' && 
       sFile[iLen-2]=='c' && sFile[iLen-1]=='x' ) iType=TYPEPCX;
    if(!iType) return(GFILTYE);
    
    //Search file in cache registers
    for(i=0,iFound=0;i<GMEMFIL;i++)
    {
        if(_sCmr[i].iUsed)
        {
            if(!strcmp(sFile,_sCmr[i].sFile))
            {
                sCmi->sGat=_sCmr[i].sGat;
                sCmi->sRgb=_sCmr[i].sRgb;
                sCmi->sImg=_sCmr[i].sImg;
                iFound=1;
                break;
            }
        }
    }
    if(iFound) return(GFILOK);
    
    //Look for a free cache register
    for(i=0,iFound=0;i<GMEMFIL;i++) if(!_sCmr[i].iUsed){ iFound=1; break; }
    iIdx=i;
    
    //If all cache registers are full free the first available file
    if(!iFound)
    {
        //Search for first available & non locked register
        for(i=0,iFound=0,iOld=0;i<GMEMFIL;i++)
        {
            if(!_sCmr[i].iLock)
            {
                iFound=1;
                iOld=i;  
                break;              
            }
        }
        if(!iFound) return(GFILFUL);
        
        //Free register
        free(_sCmr[iOld].sGat); _sCmr[iOld].sGat=NULL;
        free(_sCmr[iOld].sImg); _sCmr[iOld].sImg=NULL;
        if(_sCmr[iOld].sRgb!=NULL)
        { 
            free(_sCmr[iOld].sRgb); 
            _sCmr[iOld].sRgb=NULL;
        } 
        iIdx=iOld;
    }
    
    //Load file attributes 
    _sCmr[iIdx].sGat=(Gat *)malloc(sizeof(Gat)); 
    if(_sCmr[iIdx].sGat==NULL) 
    {
        return(GFILMER);
    }
    if(iType==TYPEBMP) BmpGetAtt(sFile,_sCmr[iIdx].sGat);
    if(iType==TYPEPCX) PcxGetAtt(sFile,_sCmr[iIdx].sGat);

    //Load file image data & palette
    _sCmr[iIdx].sRgb=(Rgb *)malloc(_sCmr[iIdx].sGat->iCol*sizeof(Rgb)); 
    if(_sCmr[iIdx].sRgb==NULL) 
    {
        free(_sCmr[iIdx].sGat); _sCmr[iIdx].sGat=NULL;
        return(GFILMER);
    }
    _sCmr[iIdx].sImg=(unsigned char *)malloc(_sCmr[iIdx].sGat->lGSz); 
    if(_sCmr[iIdx].sImg==NULL) 
    {
        free(_sCmr[iIdx].sGat); _sCmr[iIdx].sGat=NULL;
        free(_sCmr[iIdx].sRgb); _sCmr[iIdx].sRgb=NULL;
        return(GFILMER);
    }
    if(iType==TYPEBMP) 
    {
        BmpLoad(sFile,_sCmr[iIdx].sImg,_sCmr[iIdx].sRgb,_sCmr[iIdx].sGat);
    }
    if(iType==TYPEPCX) 
    {
        PcxLoad(sFile,_sCmr[iIdx].sImg,_sCmr[iIdx].sRgb,_sCmr[iIdx].sGat);
    }
    
    //Perform file conversion if needed
    if(_iFConv==CVRGB555 || _iFConv==CVRGB565)
    {
        //Get memory to allocate file
        if((sPtr0=(unsigned char *)malloc(2*_sCmr[iIdx].sGat->lGSz))==NULL) 
        {
            free(_sCmr[iIdx].sGat); _sCmr[iIdx].sGat=NULL;
            free(_sCmr[iIdx].sRgb); _sCmr[iIdx].sRgb=NULL;
            free(_sCmr[iIdx].sImg); _sCmr[iIdx].sImg=NULL;
            return(GFILMER);
        }
        
        //Perform conversion
        Conv256toRGB(_iFConv,_sCmr[iIdx].sGat,_sCmr[iIdx].sRgb,
                     _sCmr[iIdx].sImg,sPtr0);
           
        //Change pointer & free unused buffer
        sPtr1=_sCmr[iIdx].sImg;
        _sCmr[iIdx].sImg=sPtr0;
        free(sPtr1);
        
        //Free palette
        free(_sCmr[iIdx].sRgb); _sCmr[iIdx].sRgb=NULL;
        
        //Enlarge image buffer
        _sCmr[iIdx].sGat->lGSz*=2;
          
    }
    
    //Set other fields of the cache register
    sprintf(_sCmr[iIdx].sFile,sFile);
    _sCmr[iIdx].iUsed=1;
    _sCmr[iIdx].iLock=(iLock==GFLOCKED?1:0);
    
    //Return cache register
    sCmi->sGat=_sCmr[iIdx].sGat;
    sCmi->sRgb=_sCmr[iIdx].sRgb;
    sCmi->sImg=_sCmr[iIdx].sImg;
    return(GFILOK);

}    

// --------------------------------------------------------------------------
// Frees memory for a cache record
int GMemFree(char *sFile)
{
    //Variables
    int i;      //Counter
    int iFound; //Found flag
    
    //Find cache record and free it
    for(i=0,iFound=0;i<GMEMFIL;i++)
    {
        if(_sCmr[i].iUsed)
        {
            if(!strcmp(sFile,_sCmr[i].sFile))
            {
                if(_sCmr[i].sGat!=NULL) free(_sCmr[i].sGat);
                if(_sCmr[i].sRgb!=NULL) free(_sCmr[i].sRgb);
                if(_sCmr[i].sImg!=NULL) free(_sCmr[i].sImg);
                memset((void *)&_sCmr[i],0,sizeof(Cmr));
                iFound=1;
                break;
            }
        }
    }
    
    //Return code
    if(iFound) return(GFILOK); else return(GFILNFO);

}