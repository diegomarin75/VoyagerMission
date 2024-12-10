#include <stdio.h>
#include "include/timer.c"
#include "include/graphics.c"
#include "include/keyboard.c"

#define B1ON 0x0001
#define B2ON 0x0002
#define B3ON 0x0004
#define B4ON 0x0008
#define B5ON 0x0010
#define G1ON 0x0020
#define G2ON 0x0040
#define G3ON 0x0080
#define G4ON 0x0100
#define G5ON 0x0200
#define G6ON 0x0400
#define R1ON 0x0800
#define R2ON 0x1000
#define R3ON 0x2000
#define R4ON 0x4000
#define R5ON 0x8000

#define B1OFF 0xFFFE
#define B2OFF 0xFFFD
#define B3OFF 0xFFFB
#define B4OFF 0xFFF7
#define B5OFF 0xFFEF
#define G1OFF 0xFFDF
#define G2OFF 0xFFBF
#define G3OFF 0xFF7F
#define G4OFF 0xFEFF
#define G5OFF 0xFDFF
#define G6OFF 0xFBFF
#define R1OFF 0xF7FF
#define R2OFF 0xEFFF
#define R3OFF 0xDFFF
#define R4OFF 0xBFFF
#define R5OFF 0x7FFF

void main(void)
{
    //Variables
    char sStr[120];
    unsigned int c=0xFFFF;
    int x,y,iExit=0;
    
    //Initialization
    InitGraph(G640x480x64K,OPTFLIPPING);
    MainTimerOn();
    KbInit();

    //Loop
    do
    {
        if(KbHit())
        {
            switch(KbGet())
            {
                case 'b': c=(c&B1ON?c&B1OFF:c|B1ON); break; 
                case 'v': c=(c&B2ON?c&B2OFF:c|B2ON); break; 
                case 'c': c=(c&B3ON?c&B3OFF:c|B3ON); break; 
                case 'x': c=(c&B4ON?c&B4OFF:c|B4ON); break; 
                case 'z': c=(c&B5ON?c&B5OFF:c|B5ON); break; 
                case 'h': c=(c&G1ON?c&G1OFF:c|G1ON); break; 
                case 'g': c=(c&G2ON?c&G2OFF:c|G2ON); break; 
                case 'f': c=(c&G3ON?c&G3OFF:c|G3ON); break; 
                case 'd': c=(c&G4ON?c&G4OFF:c|G4ON); break; 
                case 's': c=(c&G5ON?c&G5OFF:c|G5ON); break; 
                case 'a': c=(c&G6ON?c&G6OFF:c|G6ON); break; 
                case 't': c=(c&R1ON?c&R1OFF:c|R1ON); break; 
                case 'r': c=(c&R2ON?c&R2OFF:c|R2ON); break; 
                case 'e': c=(c&R3ON?c&R3OFF:c|R3ON); break; 
                case 'w': c=(c&R4ON?c&R4OFF:c|R4ON); break; 
                case 'q': c=(c&R5ON?c&R5OFF:c|R5ON); break; 
                case 27:  iExit=1; break;
            }
            
            gl_fillbox(0,0,50,50,c);
            sprintf(sStr,"Color=%04X (R:%03X  G:%03X  B:%03X)",
                    c,(c&0xF800)>>11,(c&0x07E0)>>5,c&0x001F);
            SetColor(0xFFFF);
            gl_write(0,60,sStr);
            ShowScreen();
        }
        
    }while(!iExit);
    
    KbClose();
    MainTimerOff();
    CloseGraph();

}
