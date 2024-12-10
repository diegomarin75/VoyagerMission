// Keyboard management routines
#include <vgakeyboard.h>

// --------------------------------------------------------------------------
// Constants
#define KEYBUFFER   256 //Keyboard buffer

// Keyboard status
#define KBANY       0   //Any status
#define KBNORM      1   //Normal (no shift, no alt, no ctrl)
#define KBSHIFT     2   //Shift status
#define KBALT       3   //Alt status
#define KBCTRL      4   //Control status
#define KBCAPS      5   //Caps lock status
#define KBNUM       6   //Num lock status
#define KBSCROLL    7   //Scroll lock status
#define KBOVERWR    8   //Overwrite status
#define KBCOMODE    9   //Console mode

// ---------------------------------------------------------------------------
// Structures

//Keyboard status
struct sKStat
{
    int iLCtrl;      //Left control key pressed
    int iLShift;     //Left shitft key pressed
    int iLAlt;       //Left alt key pressed
    int iRCtrl;      //Rigth control key pressed
    int iRShift;     //Rigth shitft key pressed
    int iRAlt;       //Rigth alt key pressed
    int iNumLock;    //Num lock
    int iCapsLock;   //Caps lock
    int iScrollLock; //Scroll lock
    int iOverWrite;  //Overwrite mode
    int iConMode;    //Console mode
};

//Keyboard map table
struct sKMap
{
    int iScan;  //Scan code
    int iStat;  //Keyboard status
    int iAscii; //Ascii code
};

// --------------------------------------------------------------------------
// Global variables

//Keyboard variables
int _iKBuffer[KEYBUFFER]; //Keyboard buffer
int _iKBuffPtr;           //Buffer pointer
int _iKTimer;             //Timer handle
struct sKStat _sKStat;    //Keyboard status flags

//Keyboard map table
#include "keymap.c"

// --------------------------------------------------------------------------
// Function prototypes

// External functions
void KbInit(void);
void KbClose(void);
int  KbHit(void);
int  KbGet(void);
int  KbStatus(int);

// Internal functions
void _KeyboardHandler(int,int);
int  _Scan2Ascii(int,int *);
int  _KeyMapSearch(int,int,int *);
void _KeyboardRefresh(void);

// --------------------------------------------------------------------------
// Keyboard initialization
void KbInit(void)
{
    //Keyboard init
    keyboard_init(); 
    
    //Buffer pointer
    _iKBuffPtr=-1; 

    //Init status flags
    _sKStat.iLCtrl=0; 
    _sKStat.iLShift=0;
    _sKStat.iLAlt=0;  
    _sKStat.iRCtrl=0; 
    _sKStat.iRShift=0;
    _sKStat.iRAlt=0;  
    _sKStat.iNumLock=1;
    _sKStat.iCapsLock=0;
    _sKStat.iScrollLock=0;
    _sKStat.iOverWrite=0;
    _sKStat.iConMode=1;

    //Keyboard handler
    keyboard_seteventhandler(_KeyboardHandler);
    
    //Timer setting
    _iKTimer=TimerOpen(1,_KeyboardRefresh,0);
    TimerOn(_iKTimer);

}

// --------------------------------------------------------------------------
// Close keyboard
void KbClose(void)
{
    //Close keyboard
    keyboard_close();
    
    //Close timer
    TimerClose(_iKTimer);
   
}

// --------------------------------------------------------------------------
// Keyboard handler routine
void _KeyboardHandler(int iScan, int iPress)
{ 
    //Variables
    int iAsc; //Ascii code
    
    //Switch on scancode
    switch(iScan)
    {
        //Left Shift
        case SCANCODE_LEFTSHIFT:
            if(iPress==KEY_EVENTPRESS)   _sKStat.iLShift=1;    
            if(iPress==KEY_EVENTRELEASE) _sKStat.iLShift=0; 
            break;   

        //Rigth Shift    
        case SCANCODE_RIGHTSHIFT:
            if(iPress==KEY_EVENTPRESS)   _sKStat.iRShift=1;    
            if(iPress==KEY_EVENTRELEASE) _sKStat.iRShift=0; 
            break;   
        
        //Left control    
        case SCANCODE_LEFTCONTROL:
            if(iPress==KEY_EVENTPRESS)   _sKStat.iLCtrl=1;    
            if(iPress==KEY_EVENTRELEASE) _sKStat.iLCtrl=0; 
            break;   
        
        //Rigth control
        case SCANCODE_RIGHTCONTROL:
            if(iPress==KEY_EVENTPRESS)   _sKStat.iRCtrl=1;    
            if(iPress==KEY_EVENTRELEASE) _sKStat.iRCtrl=0; 
            break;   
        
        //Left alt
        case SCANCODE_LEFTALT:
            if(iPress==KEY_EVENTPRESS)   _sKStat.iLAlt=1;    
            if(iPress==KEY_EVENTRELEASE) _sKStat.iLAlt=0; 
            break;   
        
        //Rigth alt
        case SCANCODE_RIGHTALT:
            if(iPress==KEY_EVENTPRESS)   _sKStat.iRAlt=1;    
            if(iPress==KEY_EVENTRELEASE) _sKStat.iRAlt=0; 
            break;   
        
        //Num Lock
        case SCANCODE_NUMLOCK:
            if(iPress==KEY_EVENTPRESS) 
            {
                _sKStat.iNumLock=(_sKStat.iNumLock==0?1:0);    
            }
            break;   
        
        //Caps Lock
        case SCANCODE_CAPSLOCK:
            if(iPress==KEY_EVENTPRESS) 
            {
                _sKStat.iCapsLock=(_sKStat.iCapsLock==0?1:0);    
            }
            break;   
        
        //Scroll Lock
        case SCANCODE_SCROLLLOCK:
            if(iPress==KEY_EVENTPRESS) 
            {
                _sKStat.iScrollLock=(_sKStat.iScrollLock==0?1:0);    
            }
            break;   
        
        //Overwrite mode
        case SCANCODE_INSERT:
            if(iPress==KEY_EVENTPRESS) 
            {
                _sKStat.iOverWrite=(_sKStat.iOverWrite==0?1:0);    
            }
            break;   

        //Console mode
        case SCANCODE_TAB:
            if(iPress==KEY_EVENTPRESS) 
            {
                _sKStat.iConMode=(_sKStat.iConMode==0?1:0);    
            }
            break;   

        //Ignore print screen & pause
        case SCANCODE_PRINTSCREEN:       break;
        case SCANCODE_BREAK:             break;
        case SCANCODE_BREAK_ALTERNATIVE: break;

        //Rest of keys
        default:
            if(iPress==KEY_EVENTPRESS && _iKBuffPtr<KEYBUFFER)
            { 
                if(_Scan2Ascii(iScan,&iAsc))
                {
                    _iKBuffPtr++; _iKBuffer[_iKBuffPtr]=iAsc;
                }
                else
                {
                    _iKBuffPtr++; _iKBuffer[_iKBuffPtr]=0;
                    _iKBuffPtr++; _iKBuffer[_iKBuffPtr]=iScan;
                }
            }
    }
    
}

// --------------------------------------------------------------------------
// Scancode translation to Ascii codes
int _Scan2Ascii(int iScan, int *piAsc)
{
    //Variables
    int iACode; //Ascii code
    
    //Search in keymap table    
    if(KbStatus(KBNUM)   && _KeyMapSearch(iScan,KBNUM,  &iACode))
    { *piAsc=iACode; return(1); }
    if(KbStatus(KBCAPS)  && _KeyMapSearch(iScan,KBCAPS, &iACode))
    { *piAsc=iACode; return(1); }
    if(KbStatus(KBSHIFT) && _KeyMapSearch(iScan,KBSHIFT,&iACode))
    { *piAsc=iACode; return(1); }
    if(KbStatus(KBCTRL)  && _KeyMapSearch(iScan,KBCTRL, &iACode))
    { *piAsc=iACode; return(1); }
    if(KbStatus(KBALT)   && _KeyMapSearch(iScan,KBALT,  &iACode))
    { *piAsc=iACode; return(1); }
    if(KbStatus(KBNORM)  && _KeyMapSearch(iScan,KBNORM, &iACode))
    { *piAsc=iACode; return(1); }
    if(KbStatus(KBANY)   && _KeyMapSearch(iScan,KBANY,  &iACode))
    { *piAsc=iACode; return(1); }
    
    //Return not found
    return(0);
    
}

// --------------------------------------------------------------------------
// Keymap table search
int _KeyMapSearch(int iScan, int iStat, int *piAsc)
{
    //Variables
    int i;        //Counters
    int iFound=0; //Found flag
    
    //Table search
    for(i=0;i<KEYMAPLEN;i++)
    {
        if(_sKMap[i].iScan==iScan &&
           _sKMap[i].iStat==iStat )
        {
            iFound=1;
            break;
        }
    } 
    
    //Return result
    if(iFound)
    {
        *piAsc=_sKMap[i].iAscii;
        return(1);
    }
    else
    {
        return(0);
    }
}

// --------------------------------------------------------------------------
// Keyboard refresh
void _KeyboardRefresh(void)
{
    keyboard_update();
}

// --------------------------------------------------------------------------
// Keyboard hit check
int KbHit(void)
{
    //Return keyboard hit status
    if(_iKBuffPtr>-1) return 1; 
    else              return 0;
    
}

// --------------------------------------------------------------------------
// Keyboard key get
int KbGet(void)
{
    //Variables
    int iKey; //Key to return
    int i;    //Counter

    //Wait for key
    while(_iKBuffPtr==-1);
    
    //Take key to return
    iKey=_iKBuffer[0];
    
    //Take out key from buffer
    for(i=0;i<=_iKBuffPtr;i++) _iKBuffer[i]=_iKBuffer[i+1];
    _iKBuffPtr--;
    
    //Return key
    return(iKey);
    
}

// --------------------------------------------------------------------------
// Keyboard status get
int KbStatus(int iStat)
{
    //Switch on status
    switch(iStat)
    {
        case KBANY: 
            return(1); break;
        case KBNORM: 
            if(!_sKStat.iLShift && !_sKStat.iRShift &&
               !_sKStat.iLCtrl  && !_sKStat.iRCtrl  &&
               !_sKStat.iLAlt   && !_sKStat.iRAlt   &&
               !_sKStat.iCapsLock) return(1); 
            else return(0); 
            break;
        case KBSHIFT: 
            if(_sKStat.iLShift || _sKStat.iRShift) return(1); else return(0); break;
        case KBALT: 
            if(_sKStat.iLAlt || _sKStat.iRAlt) return(1); else return(0); break;
        case KBCTRL: 
            if(_sKStat.iLCtrl || _sKStat.iRCtrl) return(1); else return(0); break;
        case KBNUM: 
            if(_sKStat.iNumLock) return(1); else return(0); break;
        case KBCAPS: 
            if(_sKStat.iCapsLock) return(1); else return(0); break;
        case KBSCROLL: 
            if(_sKStat.iScrollLock) return(1); else return(0); break;
        case KBOVERWR: 
            if(_sKStat.iOverWrite) return(1); else return(0); break;
        case KBCOMODE: 
            if(_sKStat.iConMode) return(1); else return(0); break;
        default: 
            return(0); break;
    }
}
