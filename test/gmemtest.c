#include <stdlib.h>
#include "include/timer.c"
#include "include/graphics.c"
#include "include/keyboard.c"
#include "include/gfiles.c"
#include "include/gflvga.c"

void ShowFile(char *sFile)
{
    int iError;
    Cmi sCmi;
    
    printf("%s...",sFile);
    iError=GMemRead(sFile,&sCmi);       
    printf("%i\n",iError); 
    MainTimerSuspend();
    GPutImage(0,0,WIDTH,HEIGHT,&sCmi); 
    ShowScreen(); 
    MainTimerWakeUp();
    //KbGet();
}

void main(void)
{
    InitGraph(G640x480x64K,OPTFLIPPING);
    GMemInit(CVRGB565);
    MainTimerOn();
    KbInit();
    
    ShowFile("maps/jupiter2.pcx");
    ShowFile("maps/io.pcx");       
    ShowFile("maps/europa.pcx");   
    ShowFile("maps/earth2.pcx");   
    ShowFile("maps/moon.pcx");     
    ShowFile("maps/neptune.pcx");  
    ShowFile("maps/tycho8.pcx");   
    ShowFile("maps/enceladus.pcx");   
    ShowFile("maps/ganymede.pcx");   
    ShowFile("maps/saturn.pcx");   
    
    ShowFile("maps/jupiter2.pcx");
    ShowFile("maps/io.pcx");       
    ShowFile("maps/europa.pcx");   
    ShowFile("maps/earth2.pcx");   
    ShowFile("maps/moon.pcx");     
    ShowFile("maps/neptune.pcx");  
    ShowFile("maps/tycho8.pcx");   
    ShowFile("maps/enceladus.pcx");   
    ShowFile("maps/ganymede.pcx");   
    ShowFile("maps/saturn.pcx");   
    
    KbClose();
    MainTimerOff();
    GMemClos();
    CloseGraph();
}