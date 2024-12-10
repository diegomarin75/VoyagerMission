// VOYAGER SPACECRAFT SIMULATOR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/stdarg.h"
#include "include/math.c"
#include "include/scanf.c"
#include "include/timer.c"
#include "include/graphics.c"
#include "include/egacolor.c"
#include "include/2dgraph.c"
#include "include/3dgraph.c"
#include "include/keyboard.c"
#include "include/gfiles.c"
#include "include/simulation.c"
#include "include/panels.c"
#include "include/pcxfont.c"
#include "include/menu.c"

// --------------------------------------------------------------------------
// Funtion prototypes
int  SystemStart(Spc *,Unv *,Phm *); 
void SystemClose(void);
int  SimulationInit(Spc *,Unv *,Phm *);
void SimulationClose(Spc *,Unv *,Phm *);

// --------------------------------------------------------------------------
// System initialization main procedure
int SystemStart(Spc *sSpc,    //Spacecraft data
                Unv *sUnv,    //Universe data
                Phm *sPhm)    //Phisical model data
{
    //Variables
    int iColors; //Graphic mode colors    
    
    //Check graphic mode
    if     (vga_hasmode(G640x480x32K)) iColors=1<<15;
    else if(vga_hasmode(G640x480x64K)) iColors=1<<16;
    else
    {
        printf("Cannot set graphic mode.\n"
               "Neither G640x480x32K nor G640x480x64K are available.\n");
        return(-1);
    }
    
    //Init cache memory for graphic files
    printf("Init graphic files cache memory...\n");
    switch(iColors)
    {
        case 1<<15: GMemInit(CVRGB555); break;
        case 1<<16: GMemInit(CVRGB565); break;
    }
    
    //Load spacecraft data
    printf("Loading spacecraft data...\n"); 
    if(LoadSpc(sSpc,"data/spc00.dat")==-1)
    {
        printf("Unable to load spacecraft data\n");
        return(-1);
    }
    
    //Load universe data
    printf("Loading universe data...\n");   
    if(LoadUnv(sUnv,"data/solarsys.dat","data/stars.dat",iColors)==-1) 
    {
        printf("Unable to load universe data\n");
        return(-1);
    }
    
    //Init timer system
    printf("Init timers...\n");
    TimerInit();
    
    //Set graphic mode
    printf("Init graphics...\n");
    switch(iColors)
    {
        case 1<<15: InitGraph(G640x480x32K,OPTFLIPPING); break;
        case 1<<16: InitGraph(G640x480x64K,OPTFLIPPING); break;
    }

    //Keyboard initialization
    printf("Init keyboard...\n");
    KbInit();
    
    //Init menu
    printf("Init menu...\n");
    MenuInit();
    
    //Activate timers
    TimerWakeUp();
    
    //Return code
    return(0); 
    
}

// --------------------------------------------------------------------------
// System finish main procedure
void SystemClose(void)
{
    //Menu close
    MenuClose();
    
    //Keyboard close
    KbClose();
    
    //Close timer system
    TimerSuspend();
    
    //Close cache memory for graphic files
    GMemClos();
    
    //Close graphics
    CloseGraph();
}

// ---------------------------------------------------------------------------
// Simulation init
int SimulationInit(Spc *sSpc,Unv *sUnv,Phm *sPhm)
{
    //Init variables
    InitSys(sSpc,sUnv,sPhm);

    //Init panel system
    if(PanelInit()==-1)
    {
        printf("Panel initialization failed");
        return(-1);
    }
    
    //Return code
    return(0);
}

// --------------------------------------------------------------------------
// Simulation close
void SimulationClose(Spc *sSpc,Unv *sUnv,Phm *sPhm)
{
    //Panel close
    PanelClose();

    //Close system
    CloseSys(sSpc,sUnv,sPhm);
    
}

// --------------------------------------------------------------------------
// Main program
int main(void)
{
    //Variables
    int iExit=0;       //Exit flag
    Unv sUnv;          //Universe
    Spc sSpc;          //Spacecraft
    Phm sPhm;          //Physical model variables
    Flo fScl=4E7;      //Map window scale
    int iKey1,iKey2;   //Keyboard key

    //Set global pointers
    _sSpc=&sSpc; _sUnv=&sUnv; _sPhm=&sPhm;
    
    //System initialization
    if(SystemStart(&sSpc,&sUnv,&sPhm)==-1)
    {
        printf("Initialization failure.\n");
        exit(0);
    } 
    
    CallMenu(0,0);

    //Start simulation
    if(SimulationInit(&sSpc,&sUnv,&sPhm)==-1)
    {
        printf("Simulation initialization failure.\n");
        exit(0);
    } 
    
    //Loop
    do
    {
        //Attend keystrokes
        if(KbHit())
        {
            //Get key
            if((iKey1=KbGet())==0) iKey2=KbGet();
            
            //Console mode
            if(KbStatus(KBCOMODE))
            {
                ConPutChr(iKey1); if(iKey1==0) ConPutChr(iKey2);
            }
            
            //Keyboard mode
            else
            {
                switch(iKey1)
                {
                    case 27 : iExit=1; break;
                    case '4': SetCom("x-",&sSpc,&sPhm,0.8);    break;
                    case '6': SetCom("x+",&sSpc,&sPhm,0.8);    break;
                    case '2': SetCom("y-",&sSpc,&sPhm,0.8);    break;
                    case '8': SetCom("y+",&sSpc,&sPhm,0.8);    break;
                    case '/': SetCom("z-",&sSpc,&sPhm,0.8);    break;
                    case '*': SetCom("z+",&sSpc,&sPhm,0.8);    break;
                    case 'd': SetCom("s+",&sSpc,&sPhm,0.8);    break;
                    case 's': SetCom("s+",&sSpc,&sPhm,1E-3); break;
                    case 'a': SetCom("s+",&sSpc,&sPhm,1E-5); break;
                    case 'c': SetCom("s-",&sSpc,&sPhm,1);    break;
                    case 'x': SetCom("s-",&sSpc,&sPhm,1E-3); break;
                    case 'z': SetCom("s-",&sSpc,&sPhm,1E-5); break;
                    case '5': SetCom(NULL,&sSpc,&sPhm,0);    break;
                    case '0': sPhm.sV0=Clv(); sPhm.sV1=Clv(); 
                              sPhm.sL0=Clv(); sPhm.sL1=Clv(); break;
                    case 0  :
                        switch(iKey2)
                        {
                            case SCANCODE_PAGEUP:   fScl*=2; break;
                            case SCANCODE_PAGEDOWN: fScl/=2; break;
                        }
                        break;
                }
    
            }
        
        }
        
        //Execute planet map loader
        PlanetMapLoader(&sUnv,&sPhm);
        
        //Set map window
        _sPan[PANELMAP].sW2d.fXmin=sPhm.sP1.x-fScl;
        _sPan[PANELMAP].sW2d.fXmax=sPhm.sP1.x+fScl;
        _sPan[PANELMAP].sW2d.fYmin=sPhm.sP1.y-fScl;
        _sPan[PANELMAP].sW2d.fYmax=sPhm.sP1.y+fScl;

    }while(!iExit);
    
    //Close program
    SimulationClose(&sSpc,&sUnv,&sPhm);
    SystemClose();
    
    //Return code
    return(0);
    
}
