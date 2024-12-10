// Simulation model

// Gravit factors table
#include "gft.c"

// --------------------------------------------------------------------------
// Constants

// Spacecraft definition
#define SPCMAXP 100             //Spacecaft maximun points
#define SPCMAXE 100             //Spacecaft maximun engines
#define SPCMAXL 200             //Spacecaft maximun graphic lines
#define SPCMAXC 30              //Spacecaft maximun commands
#define SPCMAXA 8               //Spacecaft maximun active engines per com
#define MAXCOSB 100             //Maximun cosmical bodies
#define ENGFRC  (Flo)5E4        //Engines force

// solar system definitions 
#define MAXCOSB 100             //Maximun cosmical bodies (sun,planets & sats)
#define CBONAML 20              //Cosmic body name length 
#define CBONUML 10              //Cosmic body number length 
#define CBODSCL 30              //Cosmic body discoverer length 
#define CBOFILE 255             //Map file name
#define CBOPARM 19              //Solar sys data file parameters

// Star catalog definitions
#define MAXSTAR 500             //Maximum number of starts to load
#define STRCONL 4               //Constelation name length
#define STRNAML 30              //Star name length
#define STRCATL 7               //Star calalog number length
#define STRPARM 10              //Number of parameters in file 
#define STRDIST 1E3             //Star distance for drawing position

//File parameter availability
#define NOAVL '*' //Not available parameter
#define NOSEN '/' //No sense parameter
#define UNKNO '?' //Unknown parameter
#define CHAOS '!' //Chaotic parameter
#define NOPAR(c)  (c==NOAVL||c==NOSEN||c==UNKNO||c==CHAOS?1:0) //Avail. parm.

//Group for simulation timers
#define GRPTIMERSIM 1

// Simulation time interval
#define AT (Flo)0.01

// --------------------------------------------------------------------------
// Types
typedef struct Eng Eng; //Engine
typedef struct Lin Lin; //Drawing line
typedef struct Com Com; //Active engines
typedef struct Spc Spc; //Spacecraft
typedef struct Cbo Cbo; //Cosmical body
typedef struct Unv Unv; //Universe
typedef struct Phm Phm; //Physic model
typedef struct Str Str; //Star catalog

// --------------------------------------------------------------------------
// Structures

//Spacecraft engines
struct Eng
{
    int iPar; //Particle on which the engine is attached
    Vec sDir; //Engine force direction
    Flo fFrc; //Engine maximun force
};

//Spacecraft drawing lines
struct Lin
{
    int iP1; //Starting point
    int iP2; //Ending point
};

//Spacecraft control commands
struct Com
{
    int iAct;          //Number of active engines
    char sNam[5];      //Command name
    int sAct[SPCMAXA]; //Active engines
};

//Spacecraft
struct Spc
{
    int iPar;          //Number of particles of the spacecraft
    int iEng;          //Number of engines
    int iLin;          //Number of graphic lines
    int iCom;          //Control commands
    Par sPar[SPCMAXP]; //Spacecraft particles
    Eng sEng[SPCMAXE]; //Engines
    Lin sLin[SPCMAXL]; //Maximun lines for graphic drawing
    Com sCom[SPCMAXC]; //Maximun control commands
};

//Cosmical body
struct Cbo
{
    int  iCen;          //Orbit center
    Flo  fVmg;          //Visible magnitude
    Flo  fDis;          //Distance to sun
    Flo  fRad;          //Planet radius
    Flo  fMas;          //Mass
    Flo  fDen;          //Density
    Flo  fVes;          //Escape velocity
    Flo  fAlb;          //Albedo
    Flo  fPer;          //Orbital periode
    Flo  fInc;          //Orbit inclination
    Flo  fTil;          //Orbit tilt 
    Flo  fExc;          //Orbital excentricity
    Flo  fRot;          //Rotational periode
    Flo  fMov;          //Mean orbital velocity
    char sDat[5];       //Date of discovery
    char sNam[CBONAML]; //Name
    char sNum[CBONUML]; //Number
    char sDsc[CBODSCL]; //Discoverer
    char sMfl[CBOFILE]; //Map file name
    char sFla[CBOPARM]; //Param flags (Not avail.,not applic.,unknown,chaotic)
};

//Star
struct Str
{
    int  iHLon;           //Longitude - hours
    int  iMLon;           //Longitude - minutes
    Flo  fSLon;           //Longitude - seconds    
    int  iDLat;           //Latitude - degrees
    int  iMLat;           //Latitude - minutes
    int  iSLat;           //Latitude - seconds
    Flo  fVMag;           //Visible magnitude
    char sCons[STRCONL];  //Constelation name
    char sCNum[STRCATL];  //Catalong number
    char sName[STRNAML];  //Star name
    char sFla[STRPARM];   //Parameter flags
    Vec  sPos;            //Star position
    unsigned int uiColor; //Color for representation
};

//Universe
struct Unv
{
    int iCbo;          //Number of cosmical bodies
    int iStr;          //Number of stars
    Cbo sCbo[MAXCOSB]; //Cosmical bodies
    Str sStr[MAXSTAR]; //Star catalog
};

//Physical model
struct Phm
{
    Flo fItr;          //Iteration counter
    Flo fMTm;          //Mission time
    Flo fSTm;          //System time
    Vec sA0;           //Cm acceleration
    Vec sV0,sV1;       //Cm velocity
    Vec sP0,sP1;       //Cm position
    Vec sN0;           //Force x position
    Vec sL0,sL1;       //Angular momentum
    Vec sW0,sW1;       //Angular speed
    Vec sDir;          //Direction
    Mtx sRot;          //Rotation matrix
    Mtx sIRo;          //Inverse rotation matrix
    Gft sGft;          //Gravit factors table
    Par sPmv[SPCMAXP]; //Movement spacecraft particles
    Par sPrt[SPCMAXP]; //Rotation spacecraft particles
    Vec sEdr[SPCMAXE]; //Rotated spacecraft engines
    Flo sEac[SPCMAXE]; //Active engines power
    int sERf[SPCMAXE]; //Engine refresh flags
    Flo fCoa[MAXCOSB]; //Cosmic bodies orbit angles
    Flo fCra[MAXCOSB]; //Cosmic bodies rotation angles
    Par sCpr[MAXCOSB]; //Cosmic bodies particles
    Vec sCdp[MAXCOSB]; //Cosmic bodies drawing position
    int iPMA[MAXCOSB]; //Planet map available?
    Cmi sPMp[MAXCOSB]; //Planet map
    Vec sSdp[MAXSTAR]; //Star drawing position
};

// --------------------------------------------------------------------------
// Global variables

// --------------------------------------------------------------------------
// Global variables

// IterSys() parameters
Spc *_sSpc;  //Pointer to spacecraft data
Unv *_sUnv;  //Pointer to universe data
Phm *_sPhm;  //Pointer to physical data

// Main loop timer
int _iTMain;

// Solar system file parameter name
char *_sCboTxt[CBOPARM]={"Name","Visible magnitude","Distance","Radius","Mass",
    "Density","Escape velocity","Albedo","Orbital periode","Orbit inclination",
    "Tilt of axis","Orbital excentricy","Rotational periode",
    "Mean orbital velocity","Number","Orbit center","Date of discovery",
    "Discoverer","Map file name"};

// Star catalog file parameters name
char *_sStrTxt[STRPARM]={"Long(hours)","Long(min)","Long(sec)","Lat(deg)",
    "Lat(min)","Lat(seg)","Visible Magnitude","Constelation","Star Name",
    "Catalog number"};

// --------------------------------------------------------------------------
// Funtion prototypes
int  SaveSpc(Spc *,char *);
int  LoadSpc(Spc *,char *);
int  LoadUnv(Unv *,char *,char *,int);
int  LoadCbo(Unv *,char *);
int  LoadStr(Unv *,char *,int);
void InitSys(Spc *,Unv *,Phm *);
void CloseSys(Spc *,Unv *,Phm *);
void MoveSys(Spc *,Unv *,Phm *);
void IterSys(void);
void SetCom(char *,Spc *,Phm *,Flo);

// --------------------------------------------------------------------------
// Save spacecraft data
int SaveSpc(Spc *sSpc,char *sFile)
{
    //Variables
    int i,j;     //Counters
    FILE *pFile; //File pointer
    
    //Open file
    if((pFile=fopen(sFile,"wb"))==NULL) return(-1);
    
    //Initial string
    fprintf(pFile,"# Spacecraft data\n");
    
    //Particles
    fprintf(pFile,"#\n# Particles (x,y,z,mass)\n#\n");
    for(i=0;i<sSpc->iPar;i++)
        fprintf(pFile,"P%03i:  %+01.5E  %+01.5E  %+01.5E  %+01.5E\n",i,
        sSpc->sPar[i].r.x,sSpc->sPar[i].r.y,
        sSpc->sPar[i].r.z,sSpc->sPar[i].m);

    //Drawing lines
    fprintf(pFile,"#\n# Drawing lines (particle1,particle2)\n#\n");
    for(i=0;i<sSpc->iLin;i++)
        fprintf(pFile,"L%03i:  %03i  %03i\n",i,sSpc->sLin[i].iP1,sSpc->sLin[i].iP2);
        
    //Engines
    fprintf(pFile,"#\n# Engines (particle,x,y,z,force)\n#\n");
    for(i=0;i<sSpc->iEng;i++)
        fprintf(pFile,"E%03i:  %03i  %+01.5E  %+01.5E  %+01.5E  %+01.5E\n",i,
        sSpc->sEng[i].iPar,
        sSpc->sEng[i].sDir.x,sSpc->sEng[i].sDir.y,sSpc->sEng[i].sDir.z,
        sSpc->sEng[i].fFrc);

    //Commands
    fprintf(pFile,"#\n# Commands (active engines,name,(engine1,engine2,...))\n#\n");
    for(i=0;i<sSpc->iCom;i++)
    {
        fprintf(pFile,"C%03i:  %02i  %s  ",i,sSpc->sCom[i].iAct,sSpc->sCom[i].sNam);
        for(j=0;j<sSpc->sCom[i].iAct;j++) fprintf(pFile,"%03i  ",sSpc->sCom[i].sAct[j]);
        fprintf(pFile,"\n");
    }        
        
    //Close file
    fclose(pFile);
    
    //Return code
    return(0);

}

// --------------------------------------------------------------------------
// Load spacecraft data
int LoadSpc(Spc *sSpc,char *sFile)
{
    //Variables
    int i;                    //Counter
    FILE *pFile;              //File pointer
    char sBuf[512];           //String
    int  iPar,iLin,iEng,iCom; //Spacecraft counters
    Vec sCm;                  //Mass center
    
    //Open file
    if((pFile=fopen(sFile,"rb"))==NULL) return(-1);
    
    //Init structure
    memset((void *)sSpc,0,sizeof(Spc));
    
    //Init counters
    iPar=0; iLin=0; iEng=0; iCom=0;

    //First line
    fgets(sBuf,512,pFile);
    if(feof(pFile)) return(0);

    //Loop
    do
    {        
        //Process line
        switch(sBuf[0])
        {
            //Comment line
            case '#': break;
            
            //Particles
            case 'P':
                Scanf(&sBuf[5],"ffff",
                &sSpc->sPar[iPar].r.x,&sSpc->sPar[iPar].r.y,
                &sSpc->sPar[iPar].r.z,&sSpc->sPar[iPar].m);
                iPar++;
                break;

            //Drawing lines
            case 'L':
                Scanf(&sBuf[5],"ii",
                &sSpc->sLin[iLin].iP1,&sSpc->sLin[iLin].iP2);
                iLin++;
                break;

            //Engines
            case 'E':
                Scanf(&sBuf[5],"iffff",
                &sSpc->sEng[iEng].iPar,&sSpc->sEng[iEng].sDir.x,
                &sSpc->sEng[iEng].sDir.y,&sSpc->sEng[iEng].sDir.z,
                &sSpc->sEng[iEng].fFrc);
                iEng++;
                break;

            //Commands
            case 'C':
                Scanf(&sBuf[5],"isiiiiiiii",
                &sSpc->sCom[iCom].iAct,&sSpc->sCom[iCom].sNam,
                &sSpc->sCom[iCom].sAct[0],&sSpc->sCom[iCom].sAct[1],
                &sSpc->sCom[iCom].sAct[2],&sSpc->sCom[iCom].sAct[3],
                &sSpc->sCom[iCom].sAct[4],&sSpc->sCom[iCom].sAct[5],
                &sSpc->sCom[iCom].sAct[6],&sSpc->sCom[iCom].sAct[7]);
                iCom++;
                break;
        }
    
        //Read next line  
        fgets(sBuf,512,pFile);
        
    }while(!feof(pFile));
        
    //Set counters
    sSpc->iPar=iPar;    
    sSpc->iLin=iLin;    
    sSpc->iEng=iEng;    
    sSpc->iCom=iCom;    
    
    //Close file
    fclose(pFile);
    
    //Calculate mass center
    for(i=0,sCm=Clv();i<sSpc->iPar;i++)
        sCm=Adv(sCm,Scv(sSpc->sPar[i].r,sSpc->sPar[i].m));
    sCm=Scv(sCm,1/Mas(sSpc->sPar,sSpc->iPar));

    //Set coordinates origin at mass center
    for(i=0;i<sSpc->iPar;i++) sSpc->sPar[i].r=Sbv(sSpc->sPar[i].r,sCm);

    //Return code
    return(0);

}

// --------------------------------------------------------------------------
// Load universe data
int LoadUnv(Unv *sUnv,char *sCboFile,char *sStrFile,int iColors)
{
    //Init memory
    memset((void *)sUnv,0,sizeof(Unv));
    
    //Load solar system data
    if(LoadCbo(sUnv,sCboFile)==-1)
    { 
        printf("Cannot load solar system data\n");
        return(-1);
    }

    //Load star catalog
    if(LoadStr(sUnv,sStrFile,iColors)==-1)
    { 
        printf("Cannot load star catalog data\n");
        return(-1);
    }
    
    //Return code
    return(0);
    
}

// --------------------------------------------------------------------------
// Load solar system data
int LoadCbo(Unv *sUnv,char *sFile)
{
    //Variables
    int i,k;                  //Counter
    int iCbo=0;               //Cosmic bodies counter
    int iLen;                 //String length
    char c;                   //First field char
    FILE *pFile;              //File pointer
    char sBuf[512];           //Buffer
    char *sStr;               //String
    char *sFld[CBOPARM];      //Fields
    char sCen[CBONAML];       //Center planet
    
    //Open file
    if((pFile=fopen(sFile,"rb"))==NULL) return(-1);
    
    //First line
    fgets(sBuf,512,pFile); if(feof(pFile)) return(0);

    //Loop
    do
    {        
        //Delete blanks at the begining
        for(i=0,sStr=sBuf;strlen(sBuf);i++)
        { 
            if(sBuf[i]==' '){ sStr++; } else break; 
        }
        
        //Read data lines only
        if(sStr[0]!='\n' && sStr[0]!='#')
        {
            //Separate fields
            sFld[0]=&sStr[0]; 
            iLen=strlen(sStr);
            for(i=0,k=1;i<iLen;i++)
            { 
                if(sStr[i]==':')
                { 
                    if(k<CBOPARM){ sFld[k]=&sStr[i+1]; k++; }
                    sStr[i]=0; 
                }
            }
            
            //Read fields
            for(i=0;i<CBOPARM;i++)
            {
                
                //First field char
                c=sFld[i][0];
                
                //Read fields
                switch(i)
                {
                    //Name
                    case 0: 
                        strcpy(sUnv->sCbo[iCbo].sNam,sFld[i]); 
                        break;

                    //Visual magnitude
                    case 1: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sCbo[iCbo].fVmg)); 
                        break;
                    
                    //Distance
                    case 2: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sCbo[iCbo].fDis)); 
                        break;
                    
                    //Radius
                    case 3: 
                        
                        //Ignore non spheric shapes
                        iLen=strlen(sFld[i]);
                        for(k=0;k<iLen;k++) 
                            if(sFld[i][k]=='x') sFld[i][k]=0;
                        
                        //Read parameter
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sCbo[iCbo].fRad)); 
                        break;
                    
                    //Mass
                    case 4: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sCbo[iCbo].fMas)); 
                        break;
                    
                    //Density
                    case 5: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sCbo[iCbo].fDen)); 
                        break;
                    
                    //Escape velocity
                    case 6: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sCbo[iCbo].fVes)); 
                        break;
                    
                    //Albedo
                    case 7: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sCbo[iCbo].fAlb)); 
                        break;
                        
                    //Orbital periode    
                    case 8: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sCbo[iCbo].fPer)); 
                        break;
                    
                    //Orbit inclination
                    case 9: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sCbo[iCbo].fInc)); 
                        break;
                        
                    //Tilt of axis
                    case 10: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sCbo[iCbo].fTil)); 
                        break;
                        
                    //Excentricity
                    case 11: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sCbo[iCbo].fExc)); 
                        break;
                        
                    //Rotational period
                    case 12: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sCbo[iCbo].fRot)); 
                        break;
                        
                    //Mean orbital velocity
                    case 13: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sCbo[iCbo].fMov)); 
                        break;

                    //Number
                    case 14: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else strcpy(sUnv->sCbo[iCbo].sNum,sFld[i]); 
                        break;

                    //Orbit center
                    case 15: 
                        
                        //Read parameter
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else strcpy(sCen,sFld[i]); 
                        
                        //Search planet index
                        sUnv->sCbo[iCbo].iCen=0;
                        for(k=0;k<iCbo;k++)
                        {
                            if(!strcmp(sUnv->sCbo[k].sNam,sCen))
                                sUnv->sCbo[iCbo].iCen=k;
                        }
                        break;
                        
                    //Date of discovery
                    case 16: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else strcpy(sUnv->sCbo[iCbo].sDat,sFld[i]); 
                        break;
                    
                    //Discoverer
                    case 17: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else strcpy(sUnv->sCbo[iCbo].sDsc,sFld[i]); 
                        break;
                    
                    //Map file name
                    case 18: 
                        if(NOPAR(c)) sUnv->sCbo[iCbo].sFla[i]=c;
                        else strcpy(sUnv->sCbo[iCbo].sMfl,sFld[i]); 
                        break;
                    
                }        
                
            }
            
            //Adjust units
            sUnv->sCbo[iCbo].fDis*=1E+6;
            sUnv->sCbo[iCbo].fRad*=1E+3;
            sUnv->sCbo[iCbo].fDen*=1E-3;
            sUnv->sCbo[iCbo].fVes*=1E+3;
            sUnv->sCbo[iCbo].fPer*=24*3600;
            sUnv->sCbo[iCbo].fRot*=24*3600;
            sUnv->sCbo[iCbo].fMov*=1E+3;
 
            //Increase cosmic bodies counter
            iCbo++;
        
        }
    
        //Read next line  
        fgets(sBuf,512,pFile);
        
    }while(!feof(pFile));
    
    //Close file
    fclose(pFile);
    
    //Set cosmic bodies counter
    sUnv->iCbo=iCbo;
    
    //Return code
    return(0);

}

// --------------------------------------------------------------------------
// Load star catalog data
int LoadStr(Unv *sUnv,char *sFile,int iColors)
{
    //Variables
    int i,k;                           //Counter
    int iStr=0;                        //Star number
    int iLen;                          //String length
    Flo fLon,fLat;                     //Star coordinates
    Flo fFactor;                       //Color factor
    char c;                            //First field char
    FILE *pFile;                       //File pointer
    char sBuf[512];                    //Buffer
    char *sStr;                        //String
    char *sFld[STRPARM];               //Fields
    unsigned int uiRed,uiGreen,uiBlue; //Star color components
    
    //Open file
    if((pFile=fopen(sFile,"rb"))==NULL) return(-1);
    
    //First line
    fgets(sBuf,512,pFile); if(feof(pFile)) return(0);

    //Loop
    do
    {        
        //Delete blanks at the begining
        for(i=0,sStr=sBuf;strlen(sBuf);i++)
        { 
            if(sBuf[i]==' '){ sStr++; } else break; 
        }
        
        //Read data lines only
        if(sStr[0]!='\n' && sStr[0]!='#')
        {
            //Separate fields
            sFld[0]=&sStr[0]; 
            iLen=strlen(sStr);
            for(i=0,k=1;i<iLen;i++)
            { 
                if(sStr[i]==':')
                { 
                    if(k<STRPARM){ sFld[k]=&sStr[i+1]; k++; }
                    sStr[i]=0; 
                }
            }
            
            //Read fields
            for(i=0;i<STRPARM;i++)
            {
                
                //First field char
                c=sFld[i][0];
                
                //Read fields
                switch(i)
                {
                    //Longitude hours
                    case 0:
                        if(NOPAR(c)) sUnv->sStr[iStr].sFla[i]=c;
                        else Scanf(sFld[i],"i",&(sUnv->sStr[iStr].iHLon)); 
                        break;
                    
                    //Longitude minutes
                    case 1:
                        if(NOPAR(c)) sUnv->sStr[iStr].sFla[i]=c;
                        else Scanf(sFld[i],"i",&(sUnv->sStr[iStr].iMLon)); 
                        break;
                    
                    //Longitude seconds
                    case 2:
                        if(NOPAR(c)) sUnv->sStr[iStr].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sStr[iStr].fSLon)); 
                        break;
                    
                    //Latitude degrees
                    case 3:
                        if(NOPAR(c)) sUnv->sStr[iStr].sFla[i]=c;
                        else Scanf(sFld[i],"i",&(sUnv->sStr[iStr].iDLat)); 
                        break;
                    
                    //Latitude minutes
                    case 4:
                        if(NOPAR(c)) sUnv->sStr[iStr].sFla[i]=c;
                        else Scanf(sFld[i],"i",&(sUnv->sStr[iStr].iMLat)); 
                        break;
                    
                    //Latitude seconds
                    case 5:
                        if(NOPAR(c)) sUnv->sStr[iStr].sFla[i]=c;
                        else Scanf(sFld[i],"i",&(sUnv->sStr[iStr].iSLat)); 
                        break;
                    
                    //Visual magnitude
                    case 6:
                        if(NOPAR(c)) sUnv->sStr[iStr].sFla[i]=c;
                        else Scanf(sFld[i],"f",&(sUnv->sStr[iStr].fVMag)); 
                        break;
                    
                    //Constelation
                    case 7: 
                        if(NOPAR(c)) sUnv->sStr[iStr].sFla[i]=c;
                        else strcpy(sUnv->sStr[iStr].sCons,sFld[i]); 
                        break;
                    
                    //Catalog number
                    case 8: 
                        if(NOPAR(c)) sUnv->sStr[iStr].sFla[i]=c;
                        else strcpy(sUnv->sStr[iStr].sCNum,sFld[i]); 
                        break;
                    
                    //Star name
                    case 9: 
                        if(NOPAR(c)) sUnv->sStr[iStr].sFla[i]=c;
                        else strcpy(sUnv->sStr[iStr].sName,sFld[i]); 
                        break;
                    
                }        
                
            }
            
            //Calculate star position
            fLon=360*((3600*(Flo)sUnv->sStr[iStr].iHLon)
                    +(  60*(Flo)sUnv->sStr[iStr].iMLon)
                    +(          sUnv->sStr[iStr].fSLon))
                    /((Flo)24*(Flo)3600);
            fLat=(Flo)sUnv->sStr[iStr].iDLat
                +((Flo)sUnv->sStr[iStr].iMLat)/(Flo)60
                +((Flo)sUnv->sStr[iStr].iSLat)/(Flo)3600;
            fLon=P21*fLon/360;
            fLat=P12-P21*fLat/360;
            sUnv->sStr[iStr].sPos=Vsc(STRDIST,fLon,fLat);
            
            //Calculate color for representation
            ColorByte2RGB(iColors,0xFFFF,&uiRed,&uiGreen,&uiBlue);
            fFactor=0.3+(0.3-1)*(sUnv->sStr[iStr].fVMag-5)/(1.46+5);
            uiRed*=fFactor; uiGreen*=fFactor; uiBlue*=fFactor;
            ColorRGB2Byte(iColors,uiRed,uiGreen,uiBlue,
                          &sUnv->sStr[iStr].uiColor);
                          
            //Increase star number
            iStr++;
        
        }
    
        //Finish loop if star limit reached
        if(iStr==MAXSTAR) break;
        
        //Read next line  
        fgets(sBuf,512,pFile);
        
    }while(!feof(pFile));
    
    //Close file
    fclose(pFile);
    
    //Set star counter
    sUnv->iStr=iStr;
    
    //Return code
    return(0);

}

// --------------------------------------------------------------------------
// Init system
void InitSys(Spc *sSpc,Unv *sUnv,Phm *sPhm)
{
    //Variables
    int i;         //Counter

    //Time & iterations
    sPhm->fMTm=0;
    sPhm->fSTm=0;
    sPhm->fItr=0;

    //Clear accelerations, velocities & positions
    sPhm->sA0=Clv(); sPhm->sN0=Clv();
    sPhm->sV0=Clv(); sPhm->sV1=Clv();
    sPhm->sP0=Clv(); sPhm->sP1=Clv();
    sPhm->sL0=Clv(); sPhm->sL1=Clv();
    sPhm->sW0=Clv(); sPhm->sW1=Clv();

    //Set spacecraft initial state
    sPhm->sP1=Eqv(1.496E11-2E7,0,0); sPhm->sV1=Eqv(0,0,0);

    //Set motion & rotation of spacecraft particles
    for(i=0;i<sSpc->iPar;i++)
    {
        sPhm->sPmv[i].m=sSpc->sPar[i].m;
        sPhm->sPrt[i].m=sSpc->sPar[i].m;
        sPhm->sPmv[i].r=Adv(sPhm->sP1,sSpc->sPar[i].r);
        sPhm->sPrt[i].r=Asv(sSpc->sPar[i].r);
    }

    //Set spacecraft engines
    for(i=0;i<sSpc->iEng;i++) sPhm->sEdr[i]=Asv(sSpc->sEng[i].sDir);

    //Clear engine active powers
    for(i=0;i<sSpc->iEng;i++) sPhm->sEac[i]=0;
    
    //Set engine refresh flags
    for(i=0;i<sSpc->iEng;i++) sPhm->sERf[i]=1;
    
    //Rotation matrix & direction vector
    sPhm->sRot=Eqm(1,0,0,0,1,0,0,0,1);
    sPhm->sDir=Eqv(sPhm->sRot.a,sPhm->sRot.d,sPhm->sRot.g);

    //Init cosmic bodies
    srandom(2);
    for(i=0;i<sUnv->iCbo;i++) 
    {
        sPhm->fCoa[i]=P21*random()/RAND_MAX;
        if(i==3) sPhm->fCoa[i]=0;
        sPhm->sCpr[i].m=sUnv->sCbo[i].fMas;
        sPhm->sCpr[i].r=Eqv(sUnv->sCbo[i].fDis,0,0);
        sPhm->sCdp[i]=Sbv(sPhm->sCpr[i].r,sPhm->sP1);
    }
    
    //Init stars
    for(i=0;i<sUnv->iStr;i++) 
    {
        sPhm->sSdp[i]=sUnv->sStr[i].sPos;
    }
    
    //Init gravit factors table
    InitGft(&sPhm->sGft,sPhm->sP1,sPhm->sCpr,sUnv->iCbo);

    //Set main loop timer
    _iTMain=TimerOpen(7,IterSys,GRPTIMERSIM); 
    TimerOn(_iTMain);
}

// --------------------------------------------------------------------------
// Close system
void CloseSys(Spc *sSpc,Unv *sUnv,Phm *sPhm)
{
    //Clear system variables
    memset((void *)sSpc,0,sizeof(Spc));
    memset((void *)sUnv,0,sizeof(Unv));
    memset((void *)sPhm,0,sizeof(Phm));
    
    //Clear main loop timer
    TimerClose(_iTMain);
    
}

// --------------------------------------------------------------------------
// Move system
void MoveSys(Spc *sSpc,Unv *sUnv,Phm *sPhm)
{
    //Variables
    int i,j;            //Counter
    int iIdx;           //Index
    Mtx sInr;           //Inertia tensor
    Flo fMas;           //Mass
    Flo fAng;           //Angle
    Vec sFrc;           //Force
    Vec sRxf;           //Position x force
    Vec sTfv;           //Transformed vector
    Vec sFPn[SPCMAXP];  //Points forces
    Vec sRx,sRy,sRz;    //Rotation vectors
    Mtx sTfm,sTfi;      //Transformation matrix & inverted matrix

    //Cosmical bodies movement
    i=((int)sPhm->fItr)%sUnv->iCbo;
    if(sUnv->sCbo[i].fPer) 
    {
        sPhm->fCra[i]+=2*PI0*AT*sUnv->iCbo/sUnv->sCbo[i].fRot;
        sPhm->fCoa[i]+=2*PI0*AT*sUnv->iCbo/sUnv->sCbo[i].fPer;
        sPhm->sCpr[i].r.x=sPhm->sCpr[sUnv->sCbo[i].iCen].r.x
                         +sUnv->sCbo[i].fDis*cos(sPhm->fCoa[i]);
        sPhm->sCpr[i].r.y=sPhm->sCpr[sUnv->sCbo[i].iCen].r.y
                         +sUnv->sCbo[i].fDis*sin(sPhm->fCoa[i]);
    }
    
    //Time & iterations
    sPhm->fMTm+=AT;
    sPhm->fSTm+=AT;
    sPhm->fItr++;
    
    //Previous state
    sPhm->sP0=Asv(sPhm->sP1);
    sPhm->sV0=Asv(sPhm->sV1);
    sPhm->sL0=Asv(sPhm->sL1);
    sPhm->sW0=Asv(sPhm->sW1);

    //Initiate force vectors
    for(i=0;i<sSpc->iPar;i++) sFPn[i]=Clv();

    //Run gravit factors table
    RunGft(&sPhm->sGft,sPhm->sP1,sPhm->sCpr,sUnv->iCbo);
    
    //Calculate gravitational forces
    for(i=0;i<3;i++){
    for(j=0;j<sSpc->iPar;j++){
        iIdx=sPhm->sGft.sBod[i];
        sFrc=Gra(sPhm->sCpr[iIdx],sPhm->sPmv[j]);
        sFPn[j]=Adv(sFPn[j],sFrc);
    }}
    
    //Calculate engine forces
    for(i=0;i<sSpc->iEng;i++)
    {
        if(sPhm->sEac[i]>0)
        {
            sFrc=Scv(sPhm->sEdr[i],sSpc->sEng[i].fFrc*sPhm->sEac[i]);
            sFPn[sSpc->sEng[i].iPar]=Adv(sFPn[sSpc->sEng[i].iPar],sFrc);
        }
    }

    //Calculate total mass & inertia tensor
    fMas=Mas(sSpc->sPar,sSpc->iPar);
    sInr=Inr(sPhm->sPrt,sSpc->iPar);

    //Calculate mass center movement
    for(i=0,sFrc=Clv();i<sSpc->iPar;i++) sFrc=Adv(sFrc,sFPn[i]);
    sPhm->sA0=Scv(sFrc,1/fMas);
    sPhm->sV1=Adv(sPhm->sV0,Scv(sPhm->sA0,AT));
    sPhm->sP1=Adv(sPhm->sP0,Adv(Scv(sPhm->sV0,AT),Scv(sPhm->sA0,AT*AT/2)));

    //Calculate rotational movement
    for(i=0,sRxf=Clv();i<sSpc->iPar;i++) 
        sRxf=Adv(sRxf,Vpr(sPhm->sPrt[i].r,sFPn[i]));
    sPhm->sN0=sRxf;
    sPhm->sL1=Adv(Scv(sPhm->sN0,AT),sPhm->sL0);
    sPhm->sW1=Mxv(Inv(sInr),sPhm->sL1);

    //Calculate rotation angle & transformation matrix
    fAng=AT*Mod(sPhm->sW1);
    sTfm=Tfm(sPhm->sW1);
    sTfi=Inv(sTfm);

    //Calculate rotation matrix
    sRx=Eqv(sPhm->sRot.a,sPhm->sRot.d,sPhm->sRot.g);
    sRy=Eqv(sPhm->sRot.b,sPhm->sRot.e,sPhm->sRot.h);
    sRz=Eqv(sPhm->sRot.c,sPhm->sRot.f,sPhm->sRot.i);
    sTfv=Mxv(sTfi,sRx); sTfv=Roz(sTfv,fAng); sRx=Mxv(sTfm,sTfv);
    sTfv=Mxv(sTfi,sRy); sTfv=Roz(sTfv,fAng); sRy=Mxv(sTfm,sTfv);
    sTfv=Mxv(sTfi,sRz); sTfv=Roz(sTfv,fAng); sRz=Mxv(sTfm,sTfv);
    sPhm->sRot=Eqm(sRx.x,sRy.x,sRz.x,sRx.y,sRy.y,sRz.y,sRx.z,sRy.z,sRz.z);
    sPhm->sIRo=Inv(sPhm->sRot);

    //Set direction vector
    sPhm->sDir=Eqv(sPhm->sRot.a,sPhm->sRot.d,sPhm->sRot.g);
    
    //Rotate spacecraft
    for(i=0;i<sSpc->iPar;i++) sPhm->sPrt[i].r=Mxv(sPhm->sRot,sSpc->sPar[i].r);

    //Rotate spacecraft engines
    for(i=0;i<sSpc->iEng;i++) sPhm->sEdr[i]=Mxv(sPhm->sRot,sSpc->sEng[i].sDir);

    //Calculate cosmic bodies drawing position
    for(i=0;i<sUnv->iCbo;i++) 
        sPhm->sCdp[i]=Mxv(sPhm->sIRo,Sbv(sPhm->sCpr[i].r,sPhm->sP1));
    
    //Calculate star drawing position
    for(i=0;i<sUnv->iStr;i++) 
        sPhm->sSdp[i]=Mxv(sPhm->sIRo,sUnv->sStr[i].sPos);

    //Translation movement execution
    for(i=0;i<sSpc->iPar;i++)
        sPhm->sPmv[i].r=Adv(sPhm->sP1,sPhm->sPrt[i].r);

}

// --------------------------------------------------------------------------
// Iterate system. Input data are global pointers: _sSps, _sUnv, _sPhm
void IterSys(void)
{
    //System motion
    MoveSys(_sSpc,_sUnv,_sPhm);

    //Show screen
    ShowScreen();
    
}

// --------------------------------------------------------------------------
// Set command: set active engines per command
void SetCom(char *sNam,Spc *sSpc,Phm *sPhm,Flo fPwr)
{
    //Variables
    int i;      //Counter
    int iFound; //Flag found
    int iCom;   //Command
    
    //Null command
    if(sNam==NULL)
    {
        //Clear all engine active flags
        for(i=0;i<sSpc->iEng;i++){ sPhm->sEac[i]=0; sPhm->sERf[i]=1; }
        
        //Return
        return;
    }
    
    //Other command
    else
    {
        for(i=0,iFound=0;i<sSpc->iCom;i++) 
        {
            if(!strcmp(sNam,sSpc->sCom[i].sNam)){ iFound=1; iCom=i; }
        }
        if(!iFound) return;

        //Turn off any active engine
        for(i=0;i<sSpc->iEng;i++)
        { 
            if(sPhm->sEac[i]>0)
            {
                sPhm->sEac[i]=0;
                sPhm->sERf[i]=1;
            }
        }

        //Set active engines
        for(i=0;i<sSpc->sCom[iCom].iAct;i++)
        {
            sPhm->sEac[sSpc->sCom[iCom].sAct[i]]=fPwr;
            sPhm->sERf[sSpc->sCom[iCom].sAct[i]]=1;
        }
    }
}
