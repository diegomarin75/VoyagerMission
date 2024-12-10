// System console functions

// --------------------------------------------------------------------------
// Constants
#define CONXLOC   26         //Console location
#define CONYLOC   346        //Console location
#define CONROWS   16         //Console screen rows
#define CONCOLS   40         //Console screen columns
#define CONBUFF   2048       //Console buffer size
#define CONCOLOR  EGACOLOR02 //Console color
#define CONCHRAX  7          //Character size
#define CONCHRAY  7          //Character size
#define CONSTAPX  323        //Console status location
#define CONSTAPY  398        //Console status location
#define CONSTAAX  27         //Console status size 
#define CONSTAAY  11         //Console status size
#define STACOL01  EGACOLOR14 //Status box color active
#define STACOL00  EGACOLOR08 //Status box color inactive

// --------------------------------------------------------------------------
// Global variables

int  _iConx,_iCony;      //Console cursor position
int  _iConax,_iConay;    //Console cursor previous position
int  _iConCurSta;        //Console cursor status (On/Off, for blinking)
int  _iConPtr;           //Console buffer pointer
int  _iConTFlush;        //Console flush timer
int  _iConTCursor;       //Console cursor timer
char _sConBuff[CONBUFF]; //Console buffer

//Screen buffer for screen scroll
unsigned char _sConScr[CONCHRAX*CONCOLS*CONCHRAY*CONROWS*2];

// --------------------------------------------------------------------------
// Function prototypes

// Library functions
void ConInit(void);
void ConClose(void);
void ConPutChr(char);
void ConPutStr(char *);

// Internal functions
void _ConFlush(void);
int  _ConRead(char *);
void _ConWrite(char);
void _ConCurForward(void);
void _ConCurBackward(void);
void _ConDraw(int,int,char);
void _ConCurBlink(void);
void _ConCursorDraw(int,int,int);
void _ConStatusSet(int,int,char *);

// --------------------------------------------------------------------------
// Console initialization
void ConInit(void)
{
    //Init cursor
    _iConx=0;  _iCony=0;
    _iConax=0;_iConay=0;
    _iConCurSta=0;

    //Init buffer
    _iConPtr=-1;
    
    //Automatic flush timer
    _iConTFlush=TimerOpen(3,_ConFlush,GRPTIMERSIM);
    TimerOn(_iConTFlush);
    
    //Cursor blink timer
    _iConTCursor=TimerOpen(14,_ConCurBlink,GRPTIMERSIM);
    TimerOn(_iConTCursor);
}

// --------------------------------------------------------------------------
// Console closing
void ConClose(void)
{
    //Stop timers
    TimerClose(_iConTFlush);
    TimerClose(_iConTCursor);
}

// --------------------------------------------------------------------------
// Puts character to console buffer
void ConPutChr(char c)
{
    //Put character in buffer
    if(_iConPtr<CONBUFF)
    { 
        _iConPtr++;
        _sConBuff[_iConPtr]=c;
    }
}

// --------------------------------------------------------------------------
// Puts string to console buffer
void ConPutStr(char *sStr)
{
    //Variables
    int i; //Counters  
    
    //Dump string to console buffer
    for(i=0;sStr[i]!=0;i++) ConPutChr(sStr[i]); 
}

// --------------------------------------------------------------------------
// Get character from console buffer
int _ConRead(char *pChar)
{
    //Variables
    char c; //Counter
    int i;  //Char to return
    
    //Buffer non empty
    if(_iConPtr>-1)
    {
        //Take char to return
        c=_sConBuff[0];
        
        //Take out char from buffer
        for(i=0;i<=_iConPtr;i++) _sConBuff[i]=_sConBuff[i+1];
        _iConPtr--;
        
        //Return results
        *pChar=c;
        return(1);
    }

    //Buffer empty
    else  
        return(0);
}

// --------------------------------------------------------------------------
// Dumps console buffer to screen
void _ConFlush(void)
{
    //Console change flags
    static int iLastNum=-1, iLastCap=-1, iLastOvr=-1, iLastCon=-1;

    //Variables
    char c;    //Character
    int iStat; //Status
    
    //Write loop
    while(_ConRead(&c)) _ConWrite(c);
    
    //Write num lock status on change
    iStat=KbStatus(KBNUM);
    if(iLastNum!=iStat){ _ConStatusSet(0,iStat,"Num"); iLastNum=iStat; }
    iStat=KbStatus(KBCAPS);
    if(iLastCap!=iStat){ _ConStatusSet(1,iStat,"Cap"); iLastCap=iStat; }
    iStat=KbStatus(KBOVERWR);
    if(iLastOvr!=iStat){ _ConStatusSet(2,iStat,"Ovr"); iLastOvr=iStat; }
    iStat=KbStatus(KBCOMODE);
    if(iLastCon!=iStat){ _ConStatusSet(3,iStat,"Con"); iLastCon=iStat; }
}

// --------------------------------------------------------------------------
// Writes status box
void _ConStatusSet(int iBox,int iStatus,char *sText)
{
    //Variables
    int iColor;
    
    //Save color
    iColor=GetColor();
    
    //Clear box
    SetColor(EGACOLOR00);
    FillBox(CONSTAPX,CONSTAPY+iBox*(CONSTAAY+1),CONSTAAX-1,CONSTAAY-1);
    
    //Status box color
    SetColor((iStatus==1?STACOL01:STACOL00));
    
    //Set rectangle box
    Rectangle(CONSTAPX,
              CONSTAPY+iBox*(CONSTAAY+1),
              CONSTAPX+CONSTAAX-1,
              CONSTAPY+iBox*(CONSTAAY+1)+CONSTAAY-1);
              
    //Set box text
    OutTextXY(CONSTAPX+1,CONSTAPY+iBox*(CONSTAAY+1)+2,sText);              
    
    //Restore color
    SetColor(iColor);
}

// --------------------------------------------------------------------------
// Writes character to console screen
void _ConWrite(char c)
{
    //Save previous cursor position
    _iConax=_iConx;
    _iConay=_iCony;
    
    //Put character & attend to control characters
    switch (c)
    {
        case 9:  break;
        case 8:  _ConCurBackward(); _ConDraw(_iConx,_iCony,' '); break;
        case 10: _iConx=CONCOLS; _ConCurForward(); break;
        default: _ConDraw(_iConx,_iCony,c); _ConCurForward(); break;
    }

   //Clear previous cursor position on change
   if(_iConx!=_iConax || _iCony!=_iConay) _ConCursorDraw(_iConax,_iConay,0);

}

// --------------------------------------------------------------------------
// Cursor forward movement
void _ConCurForward(void)
{
    //Forward movement
    if(_iConx<CONCOLS-1)
    {
        _iConx++;
    }
    
    //Downward movement & scroll
    else
    {
        //Set first column
        _iConx=0;

        //Downward movement
        if(_iCony<CONROWS-1)
        {
            _iCony++;
        }

        //Scroll
        else
        {
            //Clear previous cursor position
            _ConCursorDraw(_iConax,_iConay,0);
            
            //Screen scroll
            gl_getbox(CONXLOC,CONYLOC,CONCOLS*CONCHRAX,
                      CONROWS*CONCHRAY,(void *)_sConScr);
            gl_putboxpart(CONXLOC,CONYLOC,CONCOLS*CONCHRAX,
                          CONROWS*CONCHRAY,CONCOLS*CONCHRAX,
                          CONROWS*CONCHRAY,(void *)_sConScr,
                          0,CONCHRAY);

        }
    }
}

// --------------------------------------------------------------------------
// Cursor backward movement
void _ConCurBackward(void)
{
    //Backward movement
    if(_iConx>0)
    {
        _iConx--;
    }
    
    //Upward movement
    else
    {
        //Upward movement
        if(_iCony>0)
        {
            _iConx=CONCOLS-1;
            _iCony--;
        }
    }
}
// --------------------------------------------------------------------------
// Draws character to console screen
void _ConDraw(int iCol, int iRow, char c)
{
    //Variables
    char sStr[2];
    int iColor;
    int iFont;
    
    //Compose string
    sprintf(sStr,"%c",c);
    
    //Save color
    iColor=GetColor();
    
    //Save font
    iFont=GetFont();

    //Write character
    SetFont(FONT8X6); SetColor(CONCOLOR);
    OutTextXY(CONXLOC+iCol*CONCHRAX,CONYLOC+iRow*CONCHRAY,sStr);
    
    //Restore font
    SetFont(iFont);
    
    //Restore color
    SetColor(iColor);
}

// --------------------------------------------------------------------------
// Cursor blinking
void _ConCurBlink(void)
{
    //Variables
    int iColor;
    
    //Set cursor blink status
    if(KbStatus(KBCOMODE))
    {
        _iConCurSta=(_iConCurSta==0?1:0);
    }
    else
    {
        _iConCurSta=0;
    }

    //Draw actual cursor
    switch(_iConCurSta)
    {
        case 0: iColor=0;        break;
        case 1: iColor=CONCOLOR; break; 
    }
    _ConCursorDraw(_iConx,_iCony,_iConCurSta);

}

// ---------------------------------------------------------------------------
// Draws cursor
void _ConCursorDraw(int iX,int iY,int iStatus)
{
    
    //Variables
    int iColor;
    
    //Set color
    switch(iStatus)
    {
        case 0: iColor=0;        break;
        case 1: iColor=CONCOLOR; break; 
    }

    //Draw cursor
    gl_line(CONXLOC+(iX+0)*CONCHRAX,CONYLOC+iY*CONCHRAY+(CONCHRAY-1),
            CONXLOC+(iX+1)*CONCHRAX,CONYLOC+iY*CONCHRAY+(CONCHRAY-1),iColor);

}