
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/timer.c"
#include "include/graphics.c"
#include "include/keyboard.c"
#include "include/gfiles.c"
#include "include/gflvga.c"
#include "include/pcxfont.c"
#include "include/menu.c"

void main(void)
{

    GMemInit(CVRGB565);
    TimerInit();
    InitGraph(G640x480x64K,OPTFLIPPING);
    KbInit();
    MenuInit();
    TimerWakeUp();
    
    CallMenu(0); ShowScreen(); KbGet();
    
    MenuClose();
    KbClose();
    TimerSuspend();
    GMemClos();
    CloseGraph();
}

