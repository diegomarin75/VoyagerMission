#include "include/timer.c"

int _iTimer1=0, _iTimer2=0, _iTimer3=0;

void Timer1(void)
{
    _iTimer1++;
}

void Timer2(void)
{
    _iTimer2++;
}
void Timer3(void)
{
    _iTimer3++;
}

void main(void)
{
    MainTimerOn();
    TimerOn(0,1,Timer1);
    TimerOn(1,10,Timer2);
    TimerOn(2,100,Timer3);
    do
    {
        printf("Timer1:%05i Timer2:%05i Timer3:%05i\n",
        _iTimer1,_iTimer2,_iTimer3);
    }while(1);
}