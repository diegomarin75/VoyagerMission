// Panel functions
#include "console.c"

// --------------------------------------------------------------------------
// Constants

// Panel constants
#define PANELNUM      5 //Number of panels
#define PANELPHV      0 //Physical variables
#define PANELMAP      1 //Map
#define PANELPOS      2 //Spacecraft position
#define PANELCAM      3 //Camera
#define PANELENG      4 //Engines

// Panel ports
#define PANELPORTS    8 //Number of ports
#define PANELPORTCAM  0 //Camera port
#define PANELPORTVIS  1 //Visor port
#define PANELPORTS01  2 //Small panel 1
#define PANELPORTS02  3 //Small panel 2 
#define PANELPORTB01  4 //Big panel 1
#define PANELPORTB02  5 //Big panel 2
#define PANELPORTCON  6 //Console panel
#define PANELPORTCTR  7 //Control panel

//Print Spheric vector options
#define PSVNORMAL 0 //No options
#define PSVNORAD  1 //Do not print radius

// Planet representation options
#define MAPSPATH  "maps/" //Map files path

// Screen backup areas
#define SBANUMBER    3 //Number of screen backup areas 
#define SBASOURCESCR 1 //Source for backup - screen
#define SBASOURCEFIL 2 //Source for backup - main screen file 

// --------------------------------------------------------------------------
// Types
typedef struct Ppr Ppr;    //Panel port
typedef struct Pan Pan;    //Panel definition
typedef struct Sba Sba;    //Screen backup areas

// --------------------------------------------------------------------------
// Structures

// Panel definition
struct Pan
{
    int iDim;     //Window dimensions (2,3)
    W2d sW2d;     //2D window definition
    W3d sW3d;     //3D window definition
    int iTimer;   //Refresh timer
    int iPort;    //Write port
};


// Panel ports
struct Ppr
{
    int iPx,iPy; //Port location
    int iAx,iAy; //Port size
};

// Screen backup areas
struct Sba
{
    int iPx,iPy;         //Location
    int iAx,iAy;         //Size
    int iPort;           //Port assignment
    int iSource;         //Backup source
    unsigned char *sBuf; //Screen contents for file source
};

// --------------------------------------------------------------------------
// Global variables

// Background image data
Cmi _sBkIm;

// Panel definitions
Pan _sPan[PANELNUM];

// Panel ports
Ppr _sPpr[PANELPORTS]={{  0,  0,640,195},  //Camera
                       {517, 12,123, 17},  //Visor
                       { 24,217,132,105},  //Small 01
                       {178,217,132,105},  //Small 02
                       {332,217,132,168},  //Big 01
                       {486,217,132,168},  //Big 02
                       { 24,344,286,116},  //Console
                       {420,407,199, 53}}; //Control

// Screen backup areas
Sba _sSba[SBANUMBER]={{  0,131,106,64,PANELPORTCAM,SBASOURCEFIL,NULL},
                      {508,  3,133,35,PANELPORTCAM,SBASOURCEFIL,NULL}, 
                      {517, 12,123,17,PANELPORTCAM,SBASOURCESCR,NULL}}; 
                       
// --------------------------------------------------------------------------
// Funtion prototypes

// External functions
int  PanelInit(void);
void PanelClose(void);
void PutPanelScreen(void);
void SetPanelPort(int,int);
void PlanetMapLoader(Unv *,Phm *);

// Panel start/end functions
void _PanelStart(int,int);
void _PanelEnd(int);

// Panel refresh functions
void _PhyVarRefresh(void);
void _MapRefresh(void);
void _PositionRefresh(void);
void _CameraRefresh(void);
void _EngineRefresh(void);

// Panel drawing functions
void _PhyVarPanel(Phm *,W2d *);
void _MapPanel(Phm *,Unv *,W2d *);
void _PositionPanel(Spc *,Phm *,W3d *);
void _CameraPanel(Phm *,Unv *,W3d *);
void _EnginePanel(Spc *,Phm *,W2d *);

// Drawing tools
void _SpcDraw(Spc *,Phm *,W3d *);
void _PrintSphVec(Vec,int,int,char *,int);
void _EngineDraw(int,int,int,Flo);

// --------------------------------------------------------------------------
// Panel intialization
int PanelInit(void)
{
    //Variables
    int i,k;            //Counters
    int iPx,iPy;        //Sreen cursor
    
    //Load system screen
    if(GMemRead("data/mainscr.pcx",&(_sBkIm),GFLOCKED)!=GFILOK)
    {
        printf("Unable to load main screen file\n");
        return(-1);
    }
    
    //Put panel screen
    PutPanelScreen();
        
    //Set panel window dimensions
    _sPan[PANELPHV].iDim=2; 
    _sPan[PANELMAP].iDim=2;
    _sPan[PANELPOS].iDim=3; 
    _sPan[PANELCAM].iDim=3;
    _sPan[PANELENG].iDim=2;
    
    //Init panel windows
    _sPan[PANELMAP].sW2d=SetWin2D(0,0,0,0,+1E5,-1E5,+7.95E4,-7.95E4);    
    _sPan[PANELPOS].sW3d=SetWin3D(0,0,0,0,+10, -10, +7.95, -7.95, 15);
    _sPan[PANELCAM].sW3d=SetWin3D(0,0,0,0,+150,-150,+45.5,-45.5,1000);
    
    //Set active cameras
    SetActiveCamera(CAMFRONT,&_sPan[PANELCAM].sW3d);
    
    //Assign vga ports to panels
    SetPanelPort(PANELPHV,PANELPORTB01);
    SetPanelPort(PANELMAP,PANELPORTS02);
    SetPanelPort(PANELPOS,PANELPORTS01);
    SetPanelPort(PANELCAM,PANELPORTCAM);
    SetPanelPort(PANELENG,PANELPORTB02);

    //Set up screen back up areas
    for(i=0;i<SBANUMBER;i++)
    {
        if((_sSba[i].sBuf=(unsigned char *)malloc(_sSba[i].iAx*_sSba[i].iAy*2))==NULL)    
        {
            printf("Unable to get memory for screen backup areas\n");
            return(-1);
        }
        if(_sSba[i].iSource==SBASOURCEFIL)
        {
            k=0;
            for(iPy=0;iPy<_sSba[i].iAy;iPy++){
            for(iPx=0;iPx<_sSba[i].iAx;iPx++){
                _sSba[i].sBuf[2*k+0]=
                _sBkIm.sImg[2*(_sBkIm.sGat->iAx*(_sSba[i].iPy+iPy)+(_sSba[i].iPx+iPx))+0];            
                _sSba[i].sBuf[2*k+1]=
                _sBkIm.sImg[2*(_sBkIm.sGat->iAx*(_sSba[i].iPy+iPy)+(_sSba[i].iPx+iPx))+1];            
                k++;
            }}
        }
    }
    
    //Create refresh timers
    _sPan[PANELPHV].iTimer=TimerOpen(15,_PhyVarRefresh,GRPTIMERSIM); 
    _sPan[PANELMAP].iTimer=TimerOpen(10,_MapRefresh,GRPTIMERSIM); 
    _sPan[PANELPOS].iTimer=TimerOpen( 7,_PositionRefresh,GRPTIMERSIM); 
    _sPan[PANELCAM].iTimer=TimerOpen( 7,_CameraRefresh,GRPTIMERSIM);
    _sPan[PANELENG].iTimer=TimerOpen(13,_EngineRefresh,GRPTIMERSIM);
    
    //Activate refresh timers
    TimerOn(_sPan[PANELPHV].iTimer);
    TimerOn(_sPan[PANELMAP].iTimer);
    TimerOn(_sPan[PANELPOS].iTimer);
    TimerOn(_sPan[PANELCAM].iTimer);
    TimerOn(_sPan[PANELENG].iTimer);
    
    //Set up spacecraft console
    ConInit(); ConPutStr("System ready\n> ");
    
    //Call planet map loader
    PlanetMapLoader(_sUnv,_sPhm);
    
    //Return value
    return(0); 
}

// --------------------------------------------------------------------------
// Panel close
void PanelClose(void)
{
    //Close refresh timers
    TimerClose(_sPan[PANELPHV].iTimer);
    TimerClose(_sPan[PANELMAP].iTimer);
    TimerClose(_sPan[PANELPOS].iTimer);
    TimerClose(_sPan[PANELCAM].iTimer);
    TimerClose(_sPan[PANELENG].iTimer);
    
    //Close spacecraft console
    ConClose();
     
}

// --------------------------------------------------------------------------
// Set panel background image
void PutPanelScreen(void)
{
    gl_putbox(0,0,WIDTH,HEIGHT,_sBkIm.sImg);
}

// --------------------------------------------------------------------------
// Set panel port for 2D window panel
void SetPanelPort(int iPanel,int iPort)
{
    //Switch on window dimensions
    switch(_sPan[iPanel].iDim)
    {
        
        //2D window
        case 2: _sPan[iPanel].sW2d.iPx=_sPpr[iPort].iPx;
                _sPan[iPanel].sW2d.iPy=_sPpr[iPort].iPy;
                _sPan[iPanel].sW2d.iAx=_sPpr[iPort].iAx;      
                _sPan[iPanel].sW2d.iAy=_sPpr[iPort].iAy;
                break;
        
        //3D window
        case 3: _sPan[iPanel].sW3d.iPx=_sPpr[iPort].iPx;
                _sPan[iPanel].sW3d.iPy=_sPpr[iPort].iPy;
                _sPan[iPanel].sW3d.iAx=_sPpr[iPort].iAx;      
                _sPan[iPanel].sW3d.iAy=_sPpr[iPort].iAy;
                break;
    }
    
    //Set port assignment
    _sPan[iPanel].iPort=iPort;
}

// --------------------------------------------------------------------------
// Planet map loader function. Loads planet map files from cache memory when 
// distance to planet is that that the planet is visible.
void PlanetMapLoader(Unv *sUnv,Phm *sPhm)
{
    //Variables
    int i;           //Counter
    int iRad2D;      //Planet 2D radius
    char sFile[255]; //Map file path
    
    //Planet loop
    for(i=0;i<sUnv->iCbo;i++)
    {
        //Get planet 2D radius
        iRad2D=Length3DPixel(sUnv->sCbo[i].fRad,sPhm->sCdp[i].x,
               sPhm->sCdp[i].y,sPhm->sCdp[i].z,&_sPan[PANELCAM].sW3d);
               
        //Enough near to planet and picture not available?
        if(iRad2D>PLANETRAD && !sPhm->iPMA[i] && sUnv->sCbo[i].sMfl[0]!=0)
        {
            sprintf(sFile,"%s%s",MAPSPATH,sUnv->sCbo[i].sMfl);
            if(GMemRead(sFile,&(sPhm->sPMp[i]),GFVOLATILE)==GFILOK) 
                sPhm->iPMA[i]=1;
            else
                sPhm->iPMA[i]=0;
        }
    }
    
}

// ---------------------------------------------------------------------------
// Panel start
void _PanelStart(int iPanel,int iClear)
{
    //Variables
    int i;     //Counter
    int iPort; //Port
    
    //Get port number
    iPort=_sPan[iPanel].iPort;
    
    //Get screen back areas content
    for(i=0;i<SBANUMBER;i++)
    {
        if(_sSba[i].iPort==iPort && _sSba[i].iSource==SBASOURCESCR)
        {
            gl_getbox(_sSba[i].iPx,_sSba[i].iPy,
                      _sSba[i].iAx,_sSba[i].iAy,(void *)_sSba[i].sBuf);
        }
    }
    
    //Set clipping window & clear window
    SetClipWin(_sPpr[iPort].iPx,
               _sPpr[iPort].iPy,
               _sPpr[iPort].iPx+_sPpr[iPort].iAx-1,
               _sPpr[iPort].iPy+_sPpr[iPort].iAy-1); 
    
    //Clear port
    if(iClear) ClearScreen(0); 
    
}

// ---------------------------------------------------------------------------
// Panel end
void _PanelEnd(int iPanel)
{
    //Variables
    int i;     //Counter
    
    //Restore clipping window
    SetClipWin(0,0,WIDTH-1,HEIGHT-1);
    
    //Restore screen backup areas
    for(i=0;i<SBANUMBER;i++)
    {
        if(_sSba[i].iPort==_sPan[iPanel].iPort)
        {
            gl_putboxmask(_sSba[i].iPx,_sSba[i].iPy,
                          _sSba[i].iAx,_sSba[i].iAy,(void *)_sSba[i].sBuf);
        }
    }    
    
}

// ---------------------------------------------------------------------------
// Panel refresh functions
void _PhyVarRefresh  (void){ _PhyVarPanel  (_sPhm,      &_sPan[PANELPHV].sW2d); }
void _MapRefresh     (void){ _MapPanel     (_sPhm,_sUnv,&_sPan[PANELMAP].sW2d); }
void _PositionRefresh(void){ _PositionPanel(_sSpc,_sPhm,&_sPan[PANELPOS].sW3d); }
void _CameraRefresh  (void){ _CameraPanel  (_sPhm,_sUnv,&_sPan[PANELCAM].sW3d); }
void _EngineRefresh  (void){ _EnginePanel  (_sSpc,_sPhm,&_sPan[PANELENG].sW2d); }

// --------------------------------------------------------------------------
// Physical variables panel
void _PhyVarPanel(Phm *sPhm,W2d *sW2d)
{

    //Variables
    int iPx,iPy;       //Panel location
    static int iCnt=0; //Refreshing counter
    Vec sDir;          //Direction vector

    //Panel start
    _PanelStart(PANELPHV,0);
    
    //Set panel location
    iPx=sW2d->iPx+0; iPy=sW2d->iPy+7;

    //Vectors
    if(iCnt==0){ sDir=Eqv(_sPhm->sRot.a,_sPhm->sRot.d,_sPhm->sRot.g);
                 _PrintSphVec(sDir,      iPx,iPy+0*22,"Dir",PSVNORAD ); }
    if(iCnt==1)  _PrintSphVec(_sPhm->sA0,iPx,iPy+1*22,"Acl",PSVNORMAL);
    if(iCnt==2)  _PrintSphVec(_sPhm->sV1,iPx,iPy+2*22,"Vel",PSVNORMAL);
    if(iCnt==3)  _PrintSphVec(_sPhm->sP1,iPx,iPy+3*22,"Pos",PSVNORMAL);
    if(iCnt==4)  _PrintSphVec(_sPhm->sN0,iPx,iPy+4*22,"Tor",PSVNORMAL);
    if(iCnt==5)  _PrintSphVec(_sPhm->sL1,iPx,iPy+5*22,"Amo",PSVNORMAL);
    if(iCnt==6)  _PrintSphVec(_sPhm->sW1,iPx,iPy+6*22,"Ave",PSVNORMAL);

    //Increase refreshing counter
    iCnt++; if(iCnt==7) iCnt=0;
    
    //Panel end
    _PanelEnd(PANELPHV);

}

// --------------------------------------------------------------------------
// Map window drawing
void _MapPanel(Phm *sPhm,Unv *sUnv,W2d *sW2d)
{
    //Variables
    int i;        //Counter
    Flo fTh;      //Theta angle
    int iPx,iPy;  //Position
    Flo fAx,fAy;  //Increments
    Vec sPos;     //Position
    Flo fRad;     //Radius
    char sNam[4]; //Planet name
    
    //Panel start
    _PanelStart(PANELMAP,1);
    
    //Axis
    SetColor(EGACOLOR07);
    OutTextXY(sW2d->iPx          +16,sW2d->iPy,"x-");
    OutTextXY(sW2d->iPx+sW2d->iAx-16,sW2d->iPy,"x+");
    OutTextXY(sW2d->iPx,sW2d->iPy          +10,"y+");
    OutTextXY(sW2d->iPx,sW2d->iPy+sW2d->iAy-10,"y-");
    
    //Satellite loop
    SetColor(EGACOLOR07);
    for(i=0;i<sUnv->iCbo;i++)
    {
        if(sUnv->sCbo[i].iCen!=0)
        {
            sPos=Asv(sPhm->sCpr[i].r);
            fRad=sUnv->sCbo[i].fRad;
            if(Visible2D(sPos.x,sPos.y,fRad,sW2d))
            {
                memcpy(sNam,sUnv->sCbo[i].sNam,3); sNam[3]=0;
                Circle2D(sPos.x,sPos.y,fRad,sW2d);
                Text2D(sPos.x,sPos.y,sNam,sW2d);
            }            
        }
    }    

    //Planet loop
    SetColor(EGACOLOR15);
    for(i=0;i<sUnv->iCbo;i++)
    {
        if(sUnv->sCbo[i].iCen==0)
        {
            sPos=Asv(sPhm->sCpr[i].r);
            fRad=sUnv->sCbo[i].fRad;
            if(Visible2D(sPos.x,sPos.y,fRad,sW2d))
            {
                Circle2D(sPos.x,sPos.y,fRad,sW2d);
                Text2D(sPos.x,sPos.y,sUnv->sCbo[i].sNam,sW2d);
            }            
        }
    }    

    //Direction vector
    SetColor(EGACOLOR12);
    fTh=Sth(sPhm->sDir);
    fAx=((sW2d->fXmax-sW2d->fXmin)/6)*cos(fTh);
    fAy=((sW2d->fYmax-sW2d->fYmin)/6)*sin(fTh);
    Line2D(sW2d->fXmin+(sW2d->fXmax-sW2d->fXmin)/2,
           sW2d->fYmin+(sW2d->fYmax-sW2d->fYmin)/2,
           sW2d->fXmin+(sW2d->fXmax-sW2d->fXmin)/2+fAx,
           sW2d->fYmin+(sW2d->fYmax-sW2d->fYmin)/2+fAy,
           sW2d);
    Trans2DPixel(sW2d->fXmin+(sW2d->fXmax-sW2d->fXmin)/2+fAx,
                 sW2d->fYmin+(sW2d->fYmax-sW2d->fYmin)/2+fAy,
                 &iPx,&iPy,sW2d);
    Circle(iPx,iPy,2);       
    
    //Velocity vector
    SetColor(EGACOLOR11);
    fTh=Sth(sPhm->sV1);
    fAx=((sW2d->fXmax-sW2d->fXmin)/8)*cos(fTh);
    fAy=((sW2d->fYmax-sW2d->fYmin)/8)*sin(fTh);
    Line2D(sW2d->fXmin+(sW2d->fXmax-sW2d->fXmin)/2,
           sW2d->fYmin+(sW2d->fYmax-sW2d->fYmin)/2,
           sW2d->fXmin+(sW2d->fXmax-sW2d->fXmin)/2+fAx,
           sW2d->fYmin+(sW2d->fYmax-sW2d->fYmin)/2+fAy,
           sW2d);
    Trans2DPixel(sW2d->fXmin+(sW2d->fXmax-sW2d->fXmin)/2+fAx,
                 sW2d->fYmin+(sW2d->fYmax-sW2d->fYmin)/2+fAy,
                 &iPx,&iPy,sW2d);
    Circle(iPx,iPy,2);       
            
    //Acceleration vector
    SetColor(EGACOLOR10);
    fTh=Sth(sPhm->sA0);
    fAx=((sW2d->fXmax-sW2d->fXmin)/12)*cos(fTh);
    fAy=((sW2d->fYmax-sW2d->fYmin)/12)*sin(fTh);
    Line2D(sW2d->fXmin+(sW2d->fXmax-sW2d->fXmin)/2,
           sW2d->fYmin+(sW2d->fYmax-sW2d->fYmin)/2,
           sW2d->fXmin+(sW2d->fXmax-sW2d->fXmin)/2+fAx,
           sW2d->fYmin+(sW2d->fYmax-sW2d->fYmin)/2+fAy,
           sW2d);
    Trans2DPixel(sW2d->fXmin+(sW2d->fXmax-sW2d->fXmin)/2+fAx,
                 sW2d->fYmin+(sW2d->fYmax-sW2d->fYmin)/2+fAy,
                 &iPx,&iPy,sW2d);
    Circle(iPx,iPy,2);       
            
    //Spacecraft
    SetColor(EGACOLOR14);
    Trans2DPixel(sW2d->fXmin+(sW2d->fXmax-sW2d->fXmin)/2,
                 sW2d->fYmin+(sW2d->fYmax-sW2d->fYmin)/2,
                 &iPx,&iPy,sW2d);
    Circle(iPx,iPy,3);       

    //Panel end
    _PanelEnd(PANELMAP);
}

// ---------------------------------------------------------------------------
// Position window drawing
void _PositionPanel(Spc *sSpc,Phm *sPhm,W3d *sW3d)
{
    //Variables
    Flo fVecSize;  //Vector size
    Vec sV;        //Generic vector
    
    //Panel start
    _PanelStart(PANELPOS,1);
    
    //Draw axis
    SetColor(EGACOLOR07);
    OutTextXY(sW3d->iPx          +16,sW3d->iPy,"x-");
    OutTextXY(sW3d->iPx+sW3d->iAx-16,sW3d->iPy,"x+");
    OutTextXY(sW3d->iPx,sW3d->iPy          +10,"z+");
    OutTextXY(sW3d->iPx,sW3d->iPy+sW3d->iAy-10,"z-");

    //Draw spacecraft
    SetColor(EGACOLOR15); 
    _SpcDraw(sSpc,sPhm,sW3d);
    
    //Calculate vector size
    fVecSize=(0.5)*(sW3d->fXmax-sW3d->fXmin)/2;
    
    //Draw movement vectors
    SetColor(EGACOLOR09); sV=Asv(sPhm->sA0); sV=Scv(Uni(sV),fVecSize);
    Line3D(0,0,0,sV.x,sV.y,sV.z,sW3d); Text3D(sV.x,sV.y,sV.z,"A",sW3d);
    SetColor(EGACOLOR13); sV=Asv(sPhm->sV1); sV=Scv(Uni(sV),fVecSize);
    Line3D(0,0,0,sV.x,sV.y,sV.z,sW3d); Text3D(sV.x,sV.y,sV.z,"V",sW3d);
    SetColor(EGACOLOR14); sV=Asv(sPhm->sP1); sV=Scv(Uni(sV),fVecSize);
    Line3D(0,0,0,sV.x,sV.y,sV.z,sW3d); Text3D(sV.x,sV.y,sV.z,"P",sW3d);

    //Draw rotation vectors
    SetColor(EGACOLOR12); sV=Asv(sPhm->sN0); sV=Scv(Uni(sV),fVecSize);
    Line3D(0,0,0,sV.x,sV.y,sV.z,sW3d); Text3D(sV.x,sV.y,sV.z,"N",sW3d);
    SetColor(EGACOLOR10); sV=Asv(sPhm->sL1); sV=Scv(Uni(sV),fVecSize);
    Line3D(0,0,0,sV.x,sV.y,sV.z,sW3d); Text3D(sV.x,sV.y,sV.z,"L",sW3d);
    SetColor(EGACOLOR11); sV=Asv(sPhm->sW1); sV=Scv(Uni(sV),fVecSize);
    Line3D(0,0,0,sV.x,sV.y,sV.z,sW3d); Text3D(sV.x,sV.y,sV.z,"W",sW3d);
    
    //Panel end
    _PanelEnd(PANELPOS);
}

// --------------------------------------------------------------------------
// Camera window drawing
void _CameraPanel(Phm *sPhm,Unv *sUnv,W3d *sW3d)
{
    //Variables
    int i;       //Counter
    Vec sPos;    //Position
    Flo fRad;    //Radius
    
    //Panel start
    _PanelStart(PANELCAM,1);
    
    //Draw stars
    for(i=0;i<sUnv->iStr;i++)
    {
        sPos=Asv(sPhm->sSdp[i]);
        if(VisibleBody(sPos.x,sPos.y,sPos.z,0,sW3d))
        {
            SetColor(sUnv->sStr[i].uiColor);
            Plot3D(sPos.x,sPos.y,sPos.z,sW3d);
        }            
    }
    
    //Draw planets
    for(i=0;i<sUnv->iCbo;i++)
    {
        sPos=Asv(sPhm->sCdp[i]);
        fRad=sUnv->sCbo[i].fRad;
        if(VisibleBody(sPos.x,sPos.y,sPos.z,fRad,sW3d))
        {
            //Planet map available?
            if(sPhm->iPMA[i])
            {
                Planet3DMap(sPos.x,sPos.y,sPos.z,fRad,
                            sPhm->fCra[i],PI0*sUnv->sCbo[i].fTil/180,
                            sPhm->sRot,(i!=0?1:0),Uni(sPhm->sCpr[i].r),
                            sPhm->sPMp[i].sImg,
                            sPhm->sPMp[i].sGat->iAx,
                            sPhm->sPMp[i].sGat->iAy,sW3d);
            }
            else
            {
                Planet3DNoMap(sPos.x,sPos.y,sPos.z,fRad,sW3d);
            }
        }            
    }    

     //Panel end
    _PanelEnd(PANELCAM);
}

// --------------------------------------------------------------------------
// Engine panel
void _EnginePanel(Spc *sSpc,Phm *sPhm,W2d *sW2d)
{
    //Variables
    int i;       //Counter
    int iEng;    //Engine index
    int iCx,iCy; //Engine coordinates
    int iPx,iPy; //Panel location
    int iFound;  //Found flag
    
    //Exit if there is no engine to refresh
    for(i=0,iFound=0;i<sSpc->iEng;i++){ if(sPhm->sERf[i]) iFound=1; break; }
    if(!iFound) return;
    
    //Panel start
    _PanelStart(PANELENG,0);
    
    //Set panel location
    iPx=sW2d->iPx+1; iPy=sW2d->iPy+8;

    //Refresh loop
    for(iCx=0;iCx<2;iCx++){
    for(iCy=0;iCy<14;iCy++){
        iEng=iCy*2+iCx;
        if(iEng>sSpc->iEng) break;
        if(sPhm->sERf[iEng])
        { 
            _EngineDraw(iPx+iCx*66,iPy+iCy*12,iEng,100*sPhm->sEac[iEng]); 
        }
    }}
        
    //Panel end
    _PanelEnd(PANELENG);

}

// --------------------------------------------------------------------------
// Spacecraft drawing
void _SpcDraw(Spc *sSpc,Phm *sPhm,W3d *sW3d)
{
    //Variables
    int i;        //Counter

    //Draw lines
    SetColor(EGACOLOR15);
    for(i=0;i<sSpc->iLin;i++)
    {
        Line3D(sPhm->sPrt[sSpc->sLin[i].iP1].r.x,
               sPhm->sPrt[sSpc->sLin[i].iP1].r.y,
               sPhm->sPrt[sSpc->sLin[i].iP1].r.z,
               sPhm->sPrt[sSpc->sLin[i].iP2].r.x,
               sPhm->sPrt[sSpc->sLin[i].iP2].r.y,
               sPhm->sPrt[sSpc->sLin[i].iP2].r.z,sW3d);
    }
    
    //Draw engines
    SetColor(EGACOLOR14);
    for(i=0;i<sSpc->iEng;i++)
    {
        if(sPhm->sEac[i]>0)
        {
            Line3D(sPhm->sPrt[sSpc->sEng[i].iPar].r.x,
                   sPhm->sPrt[sSpc->sEng[i].iPar].r.y,
                   sPhm->sPrt[sSpc->sEng[i].iPar].r.z,
                   sPhm->sPrt[sSpc->sEng[i].iPar].r.x-sPhm->sEdr[i].x,
                   sPhm->sPrt[sSpc->sEng[i].iPar].r.y-sPhm->sEdr[i].y,
                   sPhm->sPrt[sSpc->sEng[i].iPar].r.z-sPhm->sEdr[i].z,sW3d);
        }
    }


}

// --------------------------------------------------------------------------
// Print vector spheric coordinates
void _PrintSphVec(Vec sV,int iPx,int iPy,char *sStr,int iOpt)
{
    //Variables
    char sTxt[20];   //String
    Flo fTh,fFi,fRd; //Spheric coordinates
    Flo fThx,fThy;   //Theta coordinates
    Flo fFix,fFiy;   //Theta coordinates
    int iTh,iFi;     //Integer angles
    
    //Clear rectangle
    SetColor(EGACOLOR00);
    FillBox(iPx,iPy,132,18);    
    
    //Get spheric coordinates
    fTh=Sth(sV); fFi=Sfi(sV); fRd=Mod(sV); 
    
    //Set integer angles
    iTh=180*(fTh/PI0); iFi=180*(fFi/PI0); 
    if(iFi>90 && iFi<360) iFi=iFi-360;
    
    //Calculate coordinates
    fThx=7*cos(fTh); fThy=7*sin(fTh);
    fFix=7*cos(fFi); fFiy=7*sin(fFi);    

    //Text
    if(iOpt==PSVNORMAL){ SetColor(EGACOLOR13); OutTextXY(iPx,iPy+0,sStr); }
    if(iOpt==PSVNORAD ){ SetColor(EGACOLOR13); OutTextXY(iPx,iPy+9,sStr); }
    
    //Radius
    if(iOpt==PSVNORMAL)
    { 
        SetColor(EGACOLOR12); 
        sprintf(sTxt,"%01.5E",fRd); 
        OutTextXY(iPx,iPy+9,sTxt);
    }
    
    //Values for theta and fi
    SetColor(EGACOLOR12); 
    if(iOpt==PSVNORMAL)
    {
        sprintf(sTxt,"%03i",iTh);  OutTextXY(iPx+32,iPy,sTxt);
        sprintf(sTxt,"%+03i",iFi); OutTextXY(iPx+64,iPy,sTxt);    
    }
    else
    {
        sprintf(sTxt,"%03i",iTh);  OutTextXY(iPx+32,iPy+9,sTxt);
        sprintf(sTxt,"%+03i",iFi); OutTextXY(iPx+64,iPy+9,sTxt);    
    }
    
    //Theta
    SetColor(EGACOLOR13); Circle(iPx+100,iPy+7,9);
    SetColor(EGACOLOR12); Line(iPx+100,iPy+7,iPx+100+fThx,iPy+7-fThy);    

    //Fi
    SetColor(EGACOLOR13); Circle(iPx+121,iPy+7,9);
    SetColor(EGACOLOR12); Line(iPx+121,iPy+7,iPx+121+fFix,iPy+7-fFiy);

    //Ending line
    //SetColor(EGACOLOR13); Line(iPx,iPy+18,iPx+132,iPy+18);
}

// --------------------------------------------------------------------------
// Print engine 
void _EngineDraw(int iPx,int iPy,int iEng,Flo fPercent)
{
    //Variables
    Flo fLevel;      //Percentage level
    char sTxt[20];   //String
    
    //Limit percentage
    if(fPercent>99) fPercent=99;
    
    //Engine number
    SetColor(EGACOLOR13); sprintf(sTxt,"%02i",iEng); OutTextXY(iPx,iPy,sTxt);
    
    //Force percentage
    if(fPercent>0) SetColor(EGACOLOR12); else SetColor(EGACOLOR08);
    sprintf(sTxt,"%02.0f",fPercent); OutTextXY(iPx+17,iPy,sTxt);
    
    //Clear percentage indicator
    SetColor(EGACOLOR00);
    FillBox(iPx+34,iPy-1,28,8);
    
    //Percentage rectangle
    SetColor(EGACOLOR15);
    Rectangle(iPx+34,iPy-1,iPx+61,iPy+7);
    
    //Percentage level
    SetColor(EGACOLOR14);
    if(fPercent!=0)
    {
        SetColor(EGACOLOR14);
        fLevel=(Flo)28*fPercent/(Flo)100; 
        if(fLevel>28) fLevel=28;
        if(fLevel>0)
        {
            if(fLevel<15)
            { 
                SetColor(EGACOLOR02); 
                FillBox(iPx+35,iPy,(int)fLevel,7);
            }
            if(fLevel>=15 && fLevel<24)
            { 
                SetColor(EGACOLOR02); 
                FillBox(iPx+35,iPy,15,7);
                SetColor(EGACOLOR14); 
                FillBox(iPx+35+15,iPy,(int)fLevel-15,7);
            }
            if(fLevel>=24)
            { 
                SetColor(EGACOLOR02); 
                FillBox(iPx+35,iPy,15,7);
                SetColor(EGACOLOR14); 
                FillBox(iPx+35+15,iPy,9,7);
                SetColor(EGACOLOR04); 
                FillBox(iPx+35+24,iPy,(int)fLevel-24-1,7);
            }
        }
    }
}