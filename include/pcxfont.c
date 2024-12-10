// PCX font management

// ---------------------------------------------------------------------------
// Constants

// PcxFntPutStr() options
#define PXFCENTERED  1  //Centered attribute

// Error codes
#define PCXFNTOK   0 //No error
#define PCXFNTFULL 1 //Font table full
#define PCXFNTREAD 2 //File read error

// Other
#define PCXFONTMAX  10  //Pcx fonts table size
#define PCXFILENAME 255 //Pcx font file name

// ---------------------------------------------------------------------------
// Types
typedef struct Pxf Pxf; //Pcx font

// ---------------------------------------------------------------------------
// Structures

// PCX font 
struct Pxf
{
    char sFile[PCXFILENAME];  //PCX font file name
    int iAx,iAy;              //Font size
    int iUsed;                //Record used flag
    unsigned char *sPtr[256]; //Char map
};

// ---------------------------------------------------------------------------
// Global variables

// PCX font index
Pxf _sPxf[PCXFONTMAX];

// ---------------------------------------------------------------------------
// Function prototypes
int  LoadPcxFont(char *,int,int,int,int,int *); 
void ClosePcxFont(int);
void PcxFntPutChar(int,int,int,int); 
void PcxFntPutStr(int,int,int,char *,int); 

// ---------------------------------------------------------------------------
// Load PCX font into memory
int  LoadPcxFont(char *sFile,     //File name
                 int iAx,int iAy, //Char size
                 int iStart,      //Char start
                 int iNum,        //Number of chars in font
                 int *iHandle)    //Font handle
{
    //Variables
    int i;      //Counter
    int iIdx;   //Index
    int iFound; //Found flag
    Cmi sCmi;   //Cache memory inteface
    
    //Search for free font record
    for(i=0,iFound=0;i<PCXFONTMAX;i++) if(!_sPxf[i].iUsed){ iIdx=i; iFound=1; }
    if(!iFound) return(PCXFNTFULL);
    
    //Load font from cache memory
    if(GMemRead(sFile,&sCmi,GFLOCKED)!=GFILOK) return(PCXFNTREAD);
    
    //Set char map pointers
    for(i=0;i<iNum;i++)
    {
        _sPxf[iIdx].sPtr[i+iStart]=
        (unsigned char *)&(sCmi.sImg[i*iAx*iAy*2]);//BYTESPERPIXEL]);
    }
    
    //Set rest of font table fields
    sprintf(_sPxf[iIdx].sFile,sFile);
    _sPxf[iIdx].iAx   =iAx;
    _sPxf[iIdx].iAy   =iAy;
    _sPxf[iIdx].iUsed =1;
    
    //Return handle
    *iHandle=iIdx;
    
    //Return code
    return(PCXFNTOK);
}

// ---------------------------------------------------------------------------
// Free memory and close PCX font
void  ClosePcxFont(int iHandle)
{
    if(_sPxf[iHandle].iUsed)
    {
        GMemFree(_sPxf[iHandle].sFile);
        memset((void *)&_sPxf[iHandle],0,sizeof(Pxf));
    }
}

// ---------------------------------------------------------------------------
// Put PCX font char
void PcxFntPutChar(int iHandle,int iPx,int iPy,int iChar)
{
    if(_sPxf[iHandle].sPtr[iChar]!=NULL)
    {
        gl_putboxmask(iPx,iPy,_sPxf[iHandle].iAx,_sPxf[iHandle].iAy,    
                      (void *)_sPxf[iHandle].sPtr[iChar]);
    }
}

// ---------------------------------------------------------------------------
// Put PCX font char
void PcxFntPutStr(int iHandle,int iPx,int iPy,char *sStr,int iOpt)
{
    //Variables
    int i; //Counter
    
    //Calculate position if centered attribute is set
    if(iOpt&PXFCENTERED)
    {
        iPx=WIDTH/2-strlen(sStr)*_sPxf[iHandle].iAx/2;
    }
    
    //String loop
    for(i=0;sStr[i]!=0;i++)
    {
        PcxFntPutChar(iHandle,iPx+i*_sPxf[iHandle].iAx,iPy,sStr[i]);
    }
}

