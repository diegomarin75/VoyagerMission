#include "graphics.c"

void main(void)
{
    //Variables
    int i;
    int iMode=G640x480x256;
    char sStr[80];
    
    //Initialize screen & keyboard
    InitGraph(iMode,OPTFLIPPING);
    
    //Loop
    for(i=0;i<150;i++)
    {
        
        if(keyboard_update()) break;
        //if(keyboard_keypressed(SCANCODE_ESCAPE)) break;
        gl_clearscreen(0);
        gl_line(00+i,00+i,50+i,00+i,15);
        gl_line(50+i,00+i,50+i,50+i,15);
        gl_line(50+i,50+i,00+i,50+i,15);
        gl_line(00+i,50+i,00+i,00+i,15);
        gl_circle(i+25,i+25,20,9);
        SetFontColor(12);
        sprintf(sStr,"Iter=%i",i); gl_write(0,0,sStr);
        vga_waitretrace();
        ShowScreen();
        
    }
  
    CloseGraph();
    
} 