#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "include/graphics.c"
#include "include/keyboard.c"
#include "include/console.c"

// --------------------------------------------------------------------------
// Keyboard initialization
void KbInitTest(void)
{
    //Buffer pointer
    _iKBuffPtr=-1;
                
    //Keyboard status flags
    memset((void *)&_sKStat,0,sizeof(_sKStat));
                        
}

// --------------------------------------------------------------------------
// Main program
void main(void)
{
    //Variables
    char c;
    int i;
    
    //Initialization
    KbInitTest();
    ConInit();
    
    //Insert some keystrokes
    _KeyboardHandler(SCANCODE_0,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_1,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_2,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_3,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_4,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_5,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_6,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_7,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_8,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_9,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_A,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_B,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_C,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_D,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_E,KEY_EVENTPRESS);
    _KeyboardHandler(SCANCODE_F,KEY_EVENTPRESS);
    
    //Keys in keyboard buffer
    printf("Keys:");
    for(i=0;i<=_iKBuffPtr;i++) printf("%c ",_iKBuffer[i]);
    printf("\n");

    //Read keys and insert in console buffer
    printf("KbGet():");
    while(KbHit()){ c=KbGet(); ConPutChr(c); printf("%c ",c); }
    printf("\n");

    //Extract keys from console buffer
    printf("ConRead():");
    while(_ConRead(&c)) printf("%c ",c);
    printf("\n");

}
