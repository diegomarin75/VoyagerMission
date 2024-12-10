#include <stdio.h>
#include "include/timer.c"
#include "include/graphics.c"
#include "include/keyboard.c"
#include "include/egacolor.c"

void main(void)
{
    int i;
    
    InitGraph(G640x480x64K,OPTFLIPPING);
    MainTimerOn();
    KbInit();
    
    for(i=0;i<16;i++)
    {
        switch(i)
        {
            case  0: SetColor(EGACOLOR00); break;
            case  1: SetColor(EGACOLOR01); break;
            case  2: SetColor(EGACOLOR02); break;
            case  3: SetColor(EGACOLOR03); break;
            case  4: SetColor(EGACOLOR04); break;
            case  5: SetColor(EGACOLOR05); break;
            case  6: SetColor(EGACOLOR06); break;
            case  7: SetColor(EGACOLOR07); break;
            case  8: SetColor(EGACOLOR08); break;
            case  9: SetColor(EGACOLOR09); break;
            case 10: SetColor(EGACOLOR10); break;
            case 11: SetColor(EGACOLOR11); break;
            case 12: SetColor(EGACOLOR12); break;
            case 13: SetColor(EGACOLOR13); break;
            case 14: SetColor(EGACOLOR14); break;
            case 15: SetColor(EGACOLOR15); break;
            
        }
        
        gl_fillbox(10*i,0,10*i+10,10,_iColor);
    }
    ShowScreen();
    
    KbGet();
    KbClose();
    MainTimerOff();
    CloseGraph();
    
}