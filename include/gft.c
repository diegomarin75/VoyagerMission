// Gravitational factor table routines

// --------------------------------------------------------------------------
// Constants
#define MAXGFT 100 //Number of entries in GFT (not less that cosmical bodies)

// --------------------------------------------------------------------------
// Types
typedef struct Gft Gft; //Gravit factors table

// --------------------------------------------------------------------------
// Structures

//Gravit factors table
struct Gft
{
    int iNum;         //List elements 
    int iIdx;         //Index
    int sBod[MAXGFT]; //Body
    Flo sFac[MAXGFT]; //Factor
};

// --------------------------------------------------------------------------
// Funtion prototypes
void RunGft(Gft *,Vec,Par *,int);
void InitGft(Gft *,Vec,Par *,int);
void InsertGft(Gft *,Flo,int);
void DeleteGft(Gft *,int);

// --------------------------------------------------------------------------
// Run gravit factors table
void RunGft(Gft *sGft,Vec sPos,Par *sCpr,int iCbo)
{
    //Variables
    int iIdx;        //Index
    Flo fFac;        //Gravit factor
    int iBod;        //Cosmic body
    Par sPar;        //Spacecraft particle
    
    //Init particle
    sPar.r=Asv(sPos);
    sPar.m=1;
    
    //Calculate new factor
    iIdx=sGft->iIdx;
    fFac=Mod(Gra(sPar,sCpr[sGft->sBod[iIdx]]));
    iBod=sGft->sBod[iIdx];
    DeleteGft(sGft,iIdx);
    InsertGft(sGft,fFac,iBod);

    //Increase index
    if(sGft->iIdx+1>iCbo-1) sGft->iIdx=0; else sGft->iIdx++;
}

// --------------------------------------------------------------------------
// Init gravit factors table
void InitGft(Gft *sGft,Vec sPos,Par *sCpr,int iCbo)
{
    //Variables
    int i;    //Counter
    Flo fFac; //Gravit factor
    Par sPar; //Spacecraft particle
    
    //Init particle
    sPar.r=Asv(sPos);
    sPar.m=1;
    
    //Loop
    for(i=0;i<iCbo;i++)
    {
        fFac=Mod(Gra(sPar,sCpr[i]));
        InsertGft(sGft,fFac,i);
    }
    
    //First index
    sGft->iIdx=0;
}

// --------------------------------------------------------------------------
// Insert factor in gravit factors table
void InsertGft(Gft *sGft,Flo fFac,int iBod)
{
    //Variables
    int i;                   //Counters 
    int iPos,iIni,iEnd,iMid; //Binary searching
    
    //First element
    if(sGft->iNum==0)
    {
        sGft->sFac[0]=fFac;
        sGft->sBod[0]=iBod;
        sGft->iNum++;
        return;
    }

    //Outside of list inserting
    if(fFac>sGft->sFac[0])
    {
        for(i=sGft->iNum;i>0;i--)
        {
            sGft->sFac[i]=sGft->sFac[i-1];
            sGft->sBod[i]=sGft->sBod[i-1];
        }
        sGft->sFac[0]=fFac;
        sGft->sBod[0]=iBod;
        sGft->iNum++;
        return;
    }
    if(fFac<sGft->sFac[sGft->iNum-1])
    {
        sGft->sFac[sGft->iNum]=fFac;
        sGft->sBod[sGft->iNum]=iBod;
        sGft->iNum++;
        return;
    }
    
    //Binary searching
    iIni=0;
    iEnd=sGft->iNum-1;
    do
    {
        //Loop ending
        if(iIni==iEnd){ iPos=iEnd+1; break; }
        else if(iIni+1==iEnd)
        {
            if     (fFac==sGft->sFac[iIni]){ iPos=iIni+1; break; }
            else if(fFac==sGft->sFac[iEnd]){ iPos=iEnd+1; break; }
            else{ iPos=iIni+1; break; }
        }
        
        //Divide list
        iMid=(iIni+iEnd)/2;
        if(fFac>=sGft->sFac[iMid] && fFac<=sGft->sFac[iIni])
        {
            iIni=iIni;
            iEnd=iMid;
        }
        if(fFac<sGft->sFac[iMid] && fFac>=sGft->sFac[iEnd])
        {
            iIni=iMid;
            iEnd=iEnd;
        }
    
    }while(1);
    
    //Insert new element
    for(i=sGft->iNum;i>iPos;i--)
    {
        sGft->sFac[i]=sGft->sFac[i-1];
        sGft->sBod[i]=sGft->sBod[i-1];
    }
    sGft->sFac[iPos]=fFac;
    sGft->sBod[iPos]=iBod;
    
    //Increase list length
    sGft->iNum++;
    
}

// --------------------------------------------------------------------------
// Delete factor in gravit factors table
void DeleteGft(Gft *sGft,int iIndex)
{
    //Variables
    int i; //Counters 
    
    //Delete element
    for(i=iIndex;i<sGft->iNum-1;i++)
    {
        sGft->sFac[i]=sGft->sFac[i+1];
        sGft->sBod[i]=sGft->sBod[i+1];
    }
    
    //Decrease list length
    sGft->iNum--;
    
}