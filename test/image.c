#include <stdio.h>
#include "include/timer.c"
#include "include/graphics.c"
#include "include/keyboard.c"
#include "maps/delete.c"

void main(void)
{
    InitGraph(G640x480x64K,OPTFLIPPING);
    MainTimerOn();
    KbInit();
    for(i=0;i<gimp_image.width;i++){
    for(j=0;j<gimp_image.heigth;j++){
        if(i<WIDTH && j<HEIGTH) 
            gl_setpixelrgb(i,j,gimp_image.pixel_data[3*(gimp_image.width*j+i)+0],
                               gimp_image.pixel_data[3*(gimp_image.width*j+i)+1],
                               gimp_image.pixel_data[3*(gimp_image.width*j+i)+2]);
    }}
    while(!KbHit());
    KbClose();
    MainTimerOff();
    CloseGraph();

}