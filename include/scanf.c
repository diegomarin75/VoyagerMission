// Scanf function

// --------------------------------------------------------------------------
// Function prototypes
void Scanf(char *,char *,...);

// --------------------------------------------------------------------------
// Scan string for data
void Scanf(char *sSrc,char *sFmt,...)
{
    //Variables
    int  i,j,k;                //Counter
    int  iSign;                //Value sign
    int  iESgn;                //Exp signs
    Flo  fExp;                 //Exponent
    Flo  fPnt;                 //Decimal point
    char cExp;                 //Exponential format 
    int  iVal;                 //Integer value
    Flo  fVal;                 //Floating point value
    char sVal[80];             //String value
    int  *pInt;                //Pointer to integer
    Flo  *pFlo;                //Pointer to floating point
    char *pChr;                //Pointer to char
    va_list pArg;              //Argument pointer
    
    //Init variable arguments list
    va_start(pArg,sFmt);

    //Loop on format string
    for(i=0,j=0;sFmt[i]!=0;i++)
    {
        //Delete blanks and new lines
        while(sSrc[j]==' ' || sSrc[j]=='\n'){ j++; }
         
        //Read data
        switch(sFmt[i])
        {
            //Integer
            case 'i':
                
                //Init value & sign
                iVal=0;
                iSign=1;
                
                //Value
                while(sSrc[j]!=' ' && sSrc[j]!='\n' && sSrc[j]!=0)
                {
                    if(sSrc[j]=='+' || sSrc[j]=='-') 
                        iSign=(sSrc[j]=='+'?+1:-1);
                    else if(sSrc[j]>='0' && sSrc[j]<='9')
                        iVal=iVal*10+(sSrc[j]-'0');
                    j++;
                }

                //Result
                iVal=iSign*iVal;
                
                //Return result
                pInt=va_arg(pArg,int *); 
                *pInt=iVal;
                break;

            //Float
            case 'f':
                
                //Init value
                fVal=0;
                fExp=0;
                cExp=' ';
                iSign=1;
                iESgn=1;
                fPnt=1;
                
                //Value
                while(sSrc[j]!=' ' && sSrc[j]!='\n' && sSrc[j]!=0)
                {
                    if(sSrc[j]=='+' || sSrc[j]=='-')
                    {
                        if(cExp==' ') iSign=(sSrc[j]=='+'?+1:-1);
                        if(cExp=='E') iESgn=(sSrc[j]=='+'?+1:-1);
                    }
                    else if(sSrc[j]=='.') 
                        fPnt=0.1;
                    else if(sSrc[j]=='E')
                        cExp='E';
                    else if(sSrc[j]>='0' && sSrc[j]<='9')
                    {
                        if(cExp==' ')
                        {
                            if(fPnt==1) 
                            {
                                fVal=fVal*10+sSrc[j]-'0';
                            }
                            if(fPnt!=1) 
                            {    
                                fVal=fVal+(sSrc[j]-'0')*fPnt;
                                fPnt=fPnt/10;
                            }
                                
                        }
                        if(cExp=='E') fExp=fExp*10+(sSrc[j]-'0');
                    }
                    j++;
                }
                
                //Result
                fVal=iSign*fVal*pow(10,iESgn*fExp);
                
                //Return result
                pFlo=va_arg(pArg,Flo *);
                *pFlo=fVal;
                break;

            //String
            case 's':
                
                //Init value & sign
                sVal[0]=0; k=0;
                
                //Value
                while(sSrc[j]!=' ' && sSrc[j]!='\n' && sSrc[j]!=0)
                {
                    sVal[k]=sSrc[j];
                    j++; k++;
                }
                sVal[k]=0;
                
                //Return result
                pChr=va_arg(pArg,char *);
                for(k=0;sVal[k]!=0;k++) pChr[k]=sVal[k]; pChr[k]=0;
                break;
            
        }
    }

    //Restore variable list pointer
    va_end(pArg);
}
