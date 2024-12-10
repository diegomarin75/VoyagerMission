// Timer management	

// ---------------------------------------------------------------------------
// Include files
#include <signal.h>
#include <sys/time.h>

// --------------------------------------------------------------------------
// Constants
#define TV_SEC     0    //Time interval for main timer (secs)
#define TV_USEC    1E4  //Time interval for main timer (microsecs)
#define TIMERTABLE 100  //Timer table length

// ---------------------------------------------------------------------------
// Types
typedef void (*TmHd)(void); //Timer handler function pointer

// ---------------------------------------------------------------------------
// Structures

// Timer table
struct sTimer
{
    int  iUsed;    //Used timer flag
    int  iActive;  //Active timer flag
    int  iFrec;    //Frecuency
    TmHd fHandler; //Handler function
    int  iCicle;   //Cicle counter
    int  iGroup;   //Timer group
};

// ---------------------------------------------------------------------------
// Global variables

// Timer table
struct sTimer _sTimer[TIMERTABLE]; 

// ---------------------------------------------------------------------------
// External functions
void TimerInit(void);
void TimerWakeUp(void);
void TimerSuspend(void);
int  TimerOpen(int,TmHd,int);
void TimerClose(int);
void TimerOn(int);
void TimerOff(int);
void TimerGrpOn(int);
void TimerGrpOff(int);

// Internal functions
void _MainTimerHandler(int);

// ---------------------------------------------------------------------------
// Alarm signal handler
void _MainTimerHandler(int sig)
{
    //Variables
    int i; //Counter

    //Set alarm handler for the next signal
    signal(SIGALRM,_MainTimerHandler);
    
    //Launch timers
    for(i=0;i<TIMERTABLE;i++)
    {
        if(_sTimer[i].iActive)
        {
            _sTimer[i].iCicle++;
            if(_sTimer[i].iCicle==_sTimer[i].iFrec)
            {
                _sTimer[i].iCicle=0;
                (_sTimer[i].fHandler)();
            }
        }
    }
    
}

// ---------------------------------------------------------------------------
// Init timer system
void TimerInit(void)
{
    //Init timer table
    memset((void *)_sTimer,0,sizeof(struct sTimer)*TIMERTABLE);
    
    //Set prgram begaviour when system are interrupted by signals
    siginterrupt(SIGALRM,0);
    
}

// ---------------------------------------------------------------------------
// Wake up main timer
void TimerWakeUp(void)
{
    //Variables
    struct itimerval sAlarm;  //Alarm time interval 

    //Set alarm handler
    signal(SIGALRM, _MainTimerHandler);
    
    //Set time interval
    sAlarm.it_interval.tv_sec = TV_SEC;   
    sAlarm.it_interval.tv_usec = TV_USEC; 
    sAlarm.it_value.tv_sec = TV_SEC;      
    sAlarm.it_value.tv_usec = TV_USEC;    
    setitimer(ITIMER_REAL,&sAlarm,NULL);
}

// ---------------------------------------------------------------------------
// Suspend main timer
void TimerSuspend(void)
{
    //Variables
    struct itimerval sAlarm; //Alarm time interval 

    //Stop alarm triggering
    sAlarm.it_value.tv_sec = 0;
    sAlarm.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL,&sAlarm,NULL);

}

// ---------------------------------------------------------------------------
// Open timer
int TimerOpen(int iFrec,TmHd fHandler,int iGroup)
{
    //Variables
    int i;        //Counter
    int iFound=0; //Found flag
    int iHandle;  //Timer handle
    
    //Search for first non used timer
    for(i=0;i<TIMERTABLE;i++)
    {
        if(!_sTimer[i].iUsed)
        {
            iFound=1;
            iHandle=i;
        }
    }
    
    //Return not found
    if(!iFound) return(-1);
    
    //Init timer & return handle
    _sTimer[iHandle].iUsed=1;
    _sTimer[iHandle].iActive=0;
    _sTimer[iHandle].iFrec=iFrec;
    _sTimer[iHandle].fHandler=fHandler;
    _sTimer[iHandle].iGroup=iGroup;
    return(iHandle);
}

// ---------------------------------------------------------------------------
// Close timer
void TimerClose(int iHandle)
{
    memset((void *)&(_sTimer[iHandle]),0,sizeof(struct sTimer));
}

// ---------------------------------------------------------------------------
// Turn on timer
void TimerOn(int iHandle)
{
    _sTimer[iHandle].iActive=1;
}

// ---------------------------------------------------------------------------
// Turn off timer
void TimerOff(int iHandle)
{
    _sTimer[iHandle].iActive=0;
}

// ---------------------------------------------------------------------------
// Turn on timer
void TimerGrpOn(int iGroup)
{
    //Variables
    int i;
    
    //Activate all timers in the group
    for(i=0;i<TIMERTABLE;i++)
    {
        if(_sTimer[i].iUsed && _sTimer[i].iGroup==iGroup) 
            _sTimer[i].iActive=1;
    }
        
}

// ---------------------------------------------------------------------------
// Turn off timer
void TimerGrpOff(int iGroup)
{
    //Variables
    int i;
    
    //Activate all timers in the group
    for(i=0;i<TIMERTABLE;i++)
    {
        if(_sTimer[i].iUsed && _sTimer[i].iGroup==iGroup) 
            _sTimer[i].iActive=0;
    }
}
