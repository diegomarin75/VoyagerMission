// PCX FONT TEST

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/timer.c"
#include "include/graphics.c"
#include "include/keyboard.c"
#include "include/gfiles.c"
#include "include/gflvga.c"
#include "include/pcxfont.c"

void main(void)
{
    //Variables
    int iHandle;
    Cmi sCmi;
 
    GMemInit(CVRGB565);
    GMemRead("data/neptune-faraway.pcx",&sCmi,GFLOCKED);
    printf("LoadPcxFont()=%i\n",
    LoadPcxFont("data/font47x30.pcx",30,47,33,94,&iHandle));
        
    InitGraph(G640x480x64K,OPTFLIPPING);
    TimerInit();
    KbInit();
    TimerWakeUp();
    
    GPutImage(0,0,640,480,&sCmi);
    PcxFntPutStr(iHandle,0,   0,"Voyager Mission",PXFCENTERED);
    PcxFntPutStr(iHandle,0,40*3,"* New mission *",PXFCENTERED);
    PcxFntPutStr(iHandle,0,40*4,"Load mission",PXFCENTERED);
    PcxFntPutStr(iHandle,0,40*5,"Settings",PXFCENTERED);
    PcxFntPutStr(iHandle,0,40*6,"Credits",PXFCENTERED);
    PcxFntPutStr(iHandle,0,40*7,"Help",PXFCENTERED);
    PcxFntPutStr(iHandle,0,40*8,"Quit",PXFCENTERED);
    
    ShowScreen(); KbGet();
    ClosePcxFont(iHandle);

    TimerSuspend();
    KbClose();
    CloseGraph();
    GMemClos();
    
}
