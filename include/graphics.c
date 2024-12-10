// Graphic screen management routines
#include <vga.h>
#include <vgagl.h>

// --------------------------------------------------------------------------
// Constants
#define OPTNORMAL   1   //Normal treatement
#define OPTFLIPPING 2   //Flipping treatement
#define FONT8X8     1   //Font 8x8
#define FONT8X6     2   //Font 8x6

// --------------------------------------------------------------------------
// Global variables

// Graphic settings
int _iMode;                              //Graphic mode
int _iColor=15;                          //Graphic color
int _iFont;                              //Current font
void *_pFont8x8;                         //Font 8x8 pointer
void *_pFont8x6;                         //Font 8x6 pointer
GraphicsContext _sScr;                   //Physical screen context
int _iClipx1,_iClipy1,_iClipx2,_iClipy2; //Clip window

// --------------------------------------------------------------------------
// Function prototypes

// External functions
void  InitGraph(int,int);
void  CloseGraph(void);
void  SetFont(int);
int   GetFont(void);
void  ShowScreen(void);
void  ClearScreen(int);
void  SetColor(int);
int   GetColor(void);
void  Rectangle(int,int,int,int);
void  Line(int,int,int,int);
void  Circle(int,int,int);
void  FillBox(int,int,int,int);
void  OutTextXY(int,int,char *);
void  SetClipWin(int,int,int,int);
void  GetClipWin(int *,int *,int *,int *);
void  ColorByte2RGB(int,unsigned int,unsigned int *,unsigned int *,unsigned int *);
void  ColorRGB2Byte(int,unsigned int,unsigned int,unsigned int,unsigned int *);

// --------------------------------------------------------------------------
// Set video mode
void InitGraph(int iMode,int iOpt)
{
    FILE *fp;
    char sFont[6*256];
    
    //Set graphic mode global variable
    _iMode=iMode;
    
    //Set graphics mode
    vga_setmode(iMode);
    
    //Set physical screen context
    gl_setcontextvga(iMode);
    gl_getcontext(&_sScr);
    
    //Flipping treatement
    if(iOpt==OPTFLIPPING) gl_setcontextvgavirtual(iMode);    
    
    //Clipping
    gl_enableclipping();
    
    //Load font 8x8
    _pFont8x8=(void *)malloc(256*8*8*BYTESPERPIXEL);
    gl_expandfont(8,8,15,gl_font8x8,_pFont8x8);

    //Load font 8x6
    fp=fopen("data/gr8x6.psf","rb");
    fread(sFont,1,6*256,fp);
    _pFont8x6=(void *)malloc(256*8*6*BYTESPERPIXEL);
    gl_expandfont(8,6,15,sFont,_pFont8x6);
    
    //Set current font
    SetFont(FONT8X8);
         
    //Set write mode
    gl_setwritemode(WRITEMODE_OVERWRITE);

}

// --------------------------------------------------------------------------
// Close graphics
void CloseGraph(void)
{
    //Free font
    free(_pFont8x8);
    free(_pFont8x6);
    
    //Set text mode
    vga_setmode(TEXT);
    
}

// --------------------------------------------------------------------------
// Sets font
void SetFont(int iFont)
{
    //Set font for writing
    switch(iFont)
    {
        case FONT8X8: gl_setfont(8,8,_pFont8x8); break;
        case FONT8X6: gl_setfont(8,6,_pFont8x6); break;
    }
    
    //Save current font
    _iFont=iFont;
    
}

// --------------------------------------------------------------------------
// Get current font
int GetFont(void)
{
    return(_iFont);
}

// --------------------------------------------------------------------------
// Copy virtual screen to physical screen
void ShowScreen(void)
{
    gl_copyscreen(&_sScr);
}

// --------------------------------------------------------------------------
// Clear screen
void ClearScreen(int iColor)
{
    gl_clearscreen(iColor);
}

// --------------------------------------------------------------------------
// Set color
void SetColor(int iColor)
{
    
    //Set graphic color
    _iColor=iColor;
    
    //Set font color
    if(iColor) 
    {
        switch(_iFont)
        {
            case FONT8X8: gl_colorfont(8,8,iColor,_pFont8x8); break;
            case FONT8X6: gl_colorfont(8,6,iColor,_pFont8x6); break;
        }
    }            

}

// --------------------------------------------------------------------------
// Get color
int GetColor(void)
{
    //Return value
    return(_iColor);
}

// --------------------------------------------------------------------------
// Line
void Line(int iX1,int iY1,int iX2,int iY2)
{
    gl_line(iX1,iY1,iX2,iY2,_iColor);
}

// --------------------------------------------------------------------------
// Rectangle
void Rectangle(int iX1,int iY1,int iX2,int iY2)
{
    gl_line(iX1,iY1,iX2,iY1,_iColor);
    gl_line(iX2,iY1,iX2,iY2,_iColor);    
    gl_line(iX2,iY2,iX1,iY2,_iColor);
    gl_line(iX1,iY2,iX1,iY1,_iColor);
}

// --------------------------------------------------------------------------
// Circle
void Circle(int iX,int iY,int iR)
{
    gl_circle(iX,iY,iR,_iColor);
}

// --------------------------------------------------------------------------
// Fill box
void FillBox(int iX,int iY,int iAx,int iAy)
{
    gl_fillbox(iX,iY,iAx,iAy,_iColor);
}

// --------------------------------------------------------------------------
// Output text to graphics screen
void OutTextXY(int iX,int iY,char *sStr)
{
    gl_write(iX,iY,sStr);
}

// --------------------------------------------------------------------------
// Set clipping window
void SetClipWin(int iX1,int iY1,int iX2,int iY2)
{
    //Set variables
    _iClipx1=iX1;
    _iClipy1=iY1;
    _iClipx2=iX2;
    _iClipy2=iY2;
    
    //Set clipping window
    gl_setclippingwindow(iX1,iY1,iX2,iY2);
    
}

// --------------------------------------------------------------------------
// Get clipping window
void GetClipWin(int *iX1,int *iY1,int *iX2,int *iY2)
{
    //Set variables
    *iX1=_iClipx1;
    *iY1=_iClipy1;
    *iX2=_iClipx2;
    *iY2=_iClipy2;
    
}

// --------------------------------------------------------------------------
// Color format from byte string to RGB
void ColorByte2RGB(int iColMode,unsigned int uiByte,unsigned int *uiRed,
                   unsigned int *uiGreen,unsigned int *uiBlue)
{                   

    //Jump on color mode
    switch(iColMode)
    {
        //Convert to RGB 555
        case 1<<15: *uiRed  =(uiByte&0x7C00)>>10;
                    *uiGreen=(uiByte&0x03E0)>>5;
                    *uiBlue =uiByte&0x001F;
                    break;
        
        //Convert to RGB 565
        case 1<<16: *uiRed  =(uiByte&0xF800)>>11;
                    *uiGreen=(uiByte&0x07E0)>>5;
                    *uiBlue =uiByte&0x001F;
                    break;
    }            

}

// --------------------------------------------------------------------------
// Color format from RGB to byte string
void ColorRGB2Byte(int iColMode,unsigned int uiRed,unsigned int uiGreen,
                   unsigned int uiBlue,unsigned int *uiByte)
{                   

    //Jump on color mode
    switch(iColMode)
    {
        case 1<<15: *uiByte=(uiRed<<10)|(uiGreen<<5)|(uiBlue); break;
        case 1<<16: *uiByte=(uiRed<<11)|(uiGreen<<5)|(uiBlue); break;
    }            

}
