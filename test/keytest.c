// Graphic screen management
#include <vga.h>
#include <vgagl.h>
#include <vgakeyboard.h>

// --------------------------------------------------------------------------
// Constants
#define OPTNORMAL   1   //Normal treatement
#define OPTFLIPPING 2   //Flipping treatement
#define KEYBUFFER   256 //Keyboard buffer

// --------------------------------------------------------------------------
// Global variables

// Graphics setting
int _iMode;                              //Graphic mode
int _iColor=15;                          //Graphic color
void *_pFont;                            //Font pointer
GraphicsContext _sScr;                   //Physical screen context
int _iClipx1,_iClipy1,_iClipx2,_iClipy2; //Clip window

//Keyboard variables
int _iKBuffer[KEYBUFFER];
int _iKBuffPtr;

// --------------------------------------------------------------------------
// Function prototypes
void  InitGraph(int,int);
void  CloseGraph(void);
void  ShowScreen(void);
void  SetColor(int);
int   GetColor(void);
void  Rectangle(int,int,int,int);
void  Line(int,int,int,int);
void  Circle(int,int,int);
void  SetClipWin(int,int,int,int);
void  GetClipWin(int *,int *,int *,int *);
void  KeyboardHandler(int,int);
int   KbHit(void);
int   KbGet(void);

// --------------------------------------------------------------------------
// Set video mode
void InitGraph(int iMode,int iOpt)
{
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
    
    //Keyboard init
    keyboard_init();
    _iKBuffPtr=-1;
    keyboard_seteventhandler(KeyboardHandler);

    //Set graphic font
    _pFont=(void *)malloc(256*8*8*BYTESPERPIXEL);
    gl_expandfont(8,8,15,gl_font8x8,_pFont);
    gl_setfont(8,8,_pFont);
    gl_setwritemode(WRITEMODE_OVERWRITE);

}

// --------------------------------------------------------------------------
// Close graphics
void CloseGraph(void)
{
    //Free font
    free(_pFont);
    
    //Close keyboard
    keyboard_close();
    
    //Set text mode
    vga_setmode(TEXT);
    
}

// --------------------------------------------------------------------------
// Keyboard handler routine
void KeyboardHandler(int iScan, int iPress)
{
    //Attend to keyboard press only
    if(iPress==KEY_EVENTPRESS && _iKBuffPtr+1>=KEYBUFFER)
    { 
        _iKBuffPtr++;
        _iKBuffer[_iKBuffPtr]=iScan;
    }
}

// --------------------------------------------------------------------------
// Keyboard hit check
int KbHit(void)
{
    if(_iKBuffPtr>-1) return 1; 
    else              return 0;
}

// --------------------------------------------------------------------------
// Keyboard key get
int KbGet(void)
{
    if(_iKBuffPtr>-1)
    {
        _iKBuffPtr--;
        return(_iKBuffer[_iKBuffPtr+1]);
    }
    else
    {
        return 0;
    }
}

// --------------------------------------------------------------------------
// Copy virtual screen to physical screen
void ShowScreen(void)
{
    gl_copyscreen(&_sScr);
}

// --------------------------------------------------------------------------
// Set color
void SetColor(int iColor)
{
    
    //Set graphic color
    _iColor=iColor;
    
    //Set font color
    gl_colorfont(8,8,iColor,_pFont);

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
