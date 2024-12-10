// 3D Graphics

// --------------------------------------------------------------------------
// Constants
#define PLANETRAD 2   //Visible planet minimum radius
#define DEFLIMIT  120 //Definiton limit

// Camera constants
#define CAMNORMAL 2
#define CAMFRONT  0
#define CAMREAR   1
#define CAMLEFT   2
#define CAMRIGHT  3  
#define CAMUP     4
#define CAMDOWN   5

// --------------------------------------------------------------------------
// Type definitions
typedef struct W3d W3d; //3D window

// --------------------------------------------------------------------------
// Structures

//3D window
struct W3d
{
    int iAx,iAy;      //Heigth & width in pixels 
    int iPx,iPy;      //screen location in pixels
    Flo fXmax,fXmin;  //Proyection screen size X  
    Flo fYmax,fYmin;  //Proyection screen size Y  
    Flo fX0,fY0,fZ0;  //Adding vector
    Flo fFocus;       //Focus  
    int iCamera;      //Active camera
    Flo fCosx,fCosy;  //Maximun cosine for plotting
};

// ---------------------------------------------------------------------------
// Global variables

// Cameras
Mtx _sCamera[6]={{ 0,-1, 0,+1, 0, 0, 0, 0, 1},
                 { 1, 0, 0, 0,-1, 0, 0, 0, 1},
                 { 1, 0, 0, 0, 1, 0, 0, 0, 1},
                 { 0, 1, 0,-1, 0, 0, 0, 0, 1},                       
                 { 0, 0,-1, 0, 1, 0, 1, 0, 0},
                 { 0, 0, 1, 0, 1, 0,-1, 0, 0}};

// --------------------------------------------------------------------------
// Function prototypes

// External functions
W3d   SetWin3D(int,int,int,int,Flo,Flo,Flo,Flo,Flo);
void  SetActiveCamera(int,W3d *);
int   Length3DPixel(Flo,Flo,Flo,Flo,W3d *);
void  Trans3DPixel(Flo,Flo,Flo,int *,int *,W3d *);
void  Line3D(Flo,Flo,Flo,Flo,Flo,Flo,W3d *);
void  Plot3D(Flo,Flo,Flo,W3d *);
void  Text3D(Flo,Flo,Flo,char *,W3d *);
void  Circle3D(Flo,Flo,Flo,Flo,W3d *);
void  Planet3DMap(Flo,Flo,Flo,Flo,Flo,Flo,Mtx,int,Vec,unsigned char *,int,int,W3d *);
void  Planet3DNoMap(Flo,Flo,Flo,Flo,W3d *);
int   VisibleBody(Flo,Flo,Flo,Flo,W3d *);

// Internal functions
void _PlanetMap(Flo,Flo,Flo,int,int,int,int,int,Flo,Mtx,int,Vec,unsigned char *,int,int);             //Planet definition (pixels)

// --------------------------------------------------------------------------    int iAx,iAy;      //Heigth & width in pixels 
// 3D Window
W3d SetWin3D(int iAx,int iAy,int iPx,int iPy,Flo fXmax,Flo fXmin,
             Flo fYmax,Flo fYmin,Flo fFocus)
{
    //Variables
    W3d sW3d;
    
    //Parameter setting
    sW3d.iAx=iAx;
    sW3d.iAy=iAy;
    sW3d.iPx=iPx;
    sW3d.iPy=iPy;
    sW3d.fXmax=fXmax;
    sW3d.fXmin=fXmin;
    sW3d.fYmax=fYmax;
    sW3d.fYmin=fYmin;
    sW3d.fFocus=fFocus;
    sW3d.iCamera=CAMNORMAL;

    //Calculate maximun cosines
    sW3d.fCosx=fFocus/sqrt(fFocus*fFocus+fXmax*fXmax);
    sW3d.fCosy=fFocus/sqrt(fFocus*fFocus+fYmax*fYmax);
    
    //Return result
    return(sW3d);

}
    
// --------------------------------------------------------------------------
// Set active camera
void SetActiveCamera(int iCamera,W3d *sW3d)
{
    sW3d->iCamera=iCamera;
}

// --------------------------------------------------------------------------
// Length 3D to pixels
int Length3DPixel(Flo fR,Flo fX,Flo fY,Flo fZ,W3d *sW3d)
{
    //Variables
    Vec sPnt;        //Point 3D
    Flo fXp,fYp,fRp; //2D coordinates
    Flo fPx,fPy,fPr; //pixel coordinates
    
    //Set camera
    sPnt=Eqv(fX,fY,fZ); 
    sPnt=Mxv(_sCamera[sW3d->iCamera],sPnt);
    fX=sPnt.x; fY=sPnt.y; fZ=sPnt.z;
    
    //Calculate 2D coordinates & radius
    fXp=sW3d->fFocus*fX/(fY+sW3d->fFocus);
    fYp=sW3d->fFocus*fZ/(fY+sW3d->fFocus);
    fRp=sW3d->fFocus*fR/(fY+sW3d->fFocus);

    //Calculate pixel coordinates
    fPx=sW3d->iAx*(fXp-sW3d->fXmin)/(sW3d->fXmax-sW3d->fXmin);
    fPy=sW3d->iAy*(fYp-sW3d->fYmax)/(sW3d->fYmin-sW3d->fYmax);
    fPr=sW3d->iAx*fRp/(sW3d->fXmax-sW3d->fXmin);

    //Return value
    return((int)fPr);

}

// --------------------------------------------------------------------------
// 3D to pixels transformation
void Trans3DPixel(Flo fX,Flo fY,Flo fZ,int *iPx,int *iPy,W3d *sW3d)
{
   //Variables
    Vec sPnt;    //Point 3D
    Flo fXp,fYp; //2D coordinates
    Flo fPx,fPy; //Pixel corrdinates

    //Set camera
    sPnt=Eqv(fX,fY,fZ); 
    sPnt=Mxv(_sCamera[sW3d->iCamera],sPnt);
    fX=sPnt.x; fY=sPnt.y; fZ=sPnt.z;
    
    //Calculate 2D coordinates
    fXp=sW3d->fFocus*fX/(fY+sW3d->fFocus);
    fYp=sW3d->fFocus*fZ/(fY+sW3d->fFocus);

    //Calculate pixel coordinates
    fPx=sW3d->iPx+sW3d->iAx*(fXp-sW3d->fXmin)/(sW3d->fXmax-sW3d->fXmin);
    fPy=sW3d->iPy+sW3d->iAy*(fYp-sW3d->fYmax)/(sW3d->fYmin-sW3d->fYmax);

    //Return value
    *iPx=(int)fPx;
    *iPy=(int)fPy;
    
}

// --------------------------------------------------------------------------
// 3D line
void Line3D(Flo fX1,Flo fY1,Flo fZ1,Flo fX2,Flo fY2,Flo fZ2,W3d *sW3d)
{
    //Variables
    int iPx1,iPy1,iPx2,iPy2; //pixel coordinates

    //Calculate pixel coordinates
    Trans3DPixel(fX1,fY1,fZ1,&iPx1,&iPy1,sW3d);
    Trans3DPixel(fX2,fY2,fZ2,&iPx2,&iPy2,sW3d);

    //Draw line
    gl_line(iPx1,iPy1,iPx2,iPy2,_iColor);

}

// --------------------------------------------------------------------------
// 3D Point
void Plot3D(Flo fX,Flo fY,Flo fZ,W3d *sW3d)
{
    //Variables
    int iPx,iPy; //pixel coordinates

    //Get pixel coordinates
    Trans3DPixel(fX,fY,fZ,&iPx,&iPy,sW3d);
    
    //Draw point
    gl_setpixel(iPx,iPy,_iColor);

}

// --------------------------------------------------------------------------
// 3D text
void Text3D(Flo fX,Flo fY,Flo fZ,char *sStr,W3d *sW3d)
{
    //Variables
    int iPx,iPy; //pixel coordinates

    //Calculate pixel coordinates
    Trans3DPixel(fX,fY,fZ,&iPx,&iPy,sW3d);

    //Draw text
    gl_write(iPx,iPy,sStr);

}

// --------------------------------------------------------------------------
// 3D Circle
void Circle3D(Flo fX,Flo fY,Flo fZ,Flo fR,W3d *sW3d)
{
    //Variables
    int iPx,iPy,iPr; //pixel coordinates
    
    //Calculate pixel coordinates
    Trans3DPixel(fX,fY,fZ,&iPx,&iPy,sW3d);
    iPr=Length3DPixel(fR,fX,fY,fZ,sW3d);

    //Draw circle
    if(iPr==0) iPr=1; gl_circle(iPx,iPy,iPr,_iColor);

}

// ---------------------------------------------------------------------------
// Calculates if a body is visible in the window
int VisibleBody(Flo fX,Flo fY,Flo fZ,Flo fR,W3d *sW3d)
{
    //Variables
    Vec sPos;                  //Generic vector
    //Flo fCosx,fCosy;         //Cosines
    //Flo fModx,fMody;         //Modules
    //int iPx1,iPx2,iPy1,iPy2; //Circle corners
    
    //Simple visible body check
    sPos.x=fX; sPos.y=fY; sPos.z=fZ; 
    sPos=Mxv(_sCamera[sW3d->iCamera],sPos);
    if(sPos.y<0) return(0); else return(1);    
    
    //Calculate cosines
    //fX=fabs(fX);//-fR;
    //fY=fabs(fY);//-fR;
    //fModx=sqrt(fX*fX+(fZ+sW3d->fFocus)*(fZ+sW3d->fFocus));
    //fMody=sqrt(fY*fY+(fZ+sW3d->fFocus)*(fZ+sW3d->fFocus));
    //fCosx=(fZ+sW3d->fFocus)/fModx;
    //fCosy=(fZ+sW3d->fFocus)/fMody;
    
    //Calculate visibility
    //if(fCosx>sW3d->fCosx && fCosy>sW3d->fCosy) return(1);
    //else return(0);

}

// ---------------------------------------------------------------------------
// Planet representation woth surface map
void Planet3DMap(Flo fX,Flo fY,Flo fZ,Flo fRad,Flo fRAng,Flo fTilt,Mtx sRot,
                 int iShine,Vec sShine, unsigned char *sBuf, int iMAx,int iMAy,W3d *sW3d)
{
    //Variables
    int iRad2D;  //Radius 2D
    int iCx,iCy; //Planet center 
    int iDef;    //Planet definition
    Mtx sRmt;    //Rotation matrix
    
    //Calculate planet 2D radius & planet center
    iRad2D=Length3DPixel(fRad,fX,fY,fZ,sW3d);
    Trans3DPixel(fX,fY,fZ,&iCx,&iCy,sW3d);
                
    //Near enough?
    if(iRad2D>=PLANETRAD)
    {
        sRmt=Mxm(Mxm(Rmz(-fRAng),Rmx(fTilt)),Mxm(sRot,_sCamera[sW3d->iCamera]));
        iDef=(iRad2D>DEFLIMIT?2:1);
        _PlanetMap(iRad2D,iCx,iCy,sW3d->iPx,sW3d->iPy,sW3d->iAx,sW3d->iAy,
                   iDef,sW3d->fFocus,sRmt,iShine,sShine,sBuf,iMAx,iMAy);
    }
    else
    {
        SetColor(EGACOLOR15);
        Circle(iCx,iCy,iRad2D);
    }
    
}

// --------------------------------------------------------------------------
// Planet representation without surface map
void Planet3DNoMap(Flo fX,Flo fY,Flo fZ,Flo fRad,W3d *sW3d)
{
    SetColor(EGACOLOR15);
    Circle3D(fX,fY,fZ,fRad,sW3d);
}
        
// --------------------------------------------------------------------------
// Planet representation with surface map
void _PlanetMap(Flo fRad0,                       //2D radius (pixels)
                Flo fCx,Flo fCy,                 //2D planet center (pixels)
                int iPx,int iPy,int iAx,int iAy, //Vga port (pixels)
                int iDef,                        //Planet definition (pixels)
                Flo fFocus,                      //3D focus factor
                Mtx sRot,                        //Rotation matrix
                int iShine,                      //Apply shine
                Vec sShine,                      //Shine vector (unitary)
                unsigned char *sBuf,             //Planet definition (pixels)
                int iMAx,int iMAy)               //Map size (pixels)
{
    //Variables
    Vec sPos,sPsr;                     //3D coordinates
    Flo fAlp;                          //Alpha factor (X2d^2 + Y2d^2)
    Flo fDF;                           //Distance plus focus factor
    Flo fPx,fPy;                       //Screen coordinates
    Flo fXm,fYm;                       //Map coordinates  
    Flo fFi,fTh;                       //Sphere angles
    Flo fRXmin,fRXmax;                 //2D radius representation limits X
    Flo fRYmin,fRYmax;                 //2D radius representation limits Y
    Flo fA,fB,fC,fD;                   //2nd degree equation factors
    Flo fFoc2;                         //Focus^2;
    Flo fRad2=fRad0*fRad0;             //2D square radius   
    Flo fRed,fGreen,fBlue;             //Accurate RGB components
    Flo fFactor;                       //Color shade factor
    int iByte1,iByte2;                 //Color bytes
    unsigned int uiColor;              //Map color
    unsigned long ulOffset;            //Color map offset
    unsigned int uiRed,uiGreen,uiBlue; //Color components

    //Print variables
    /*
    printf("PlanetMap(): Rad=%03.2E Center(%03.2f,%03.2f) Port(%03i,%03i,%03i,%03i) "
           "Def=%02i Focus=%05.f\n",fRad0,fCx,fCy,iPx,iPy,iPx+iAx,iPy+iAy,
           iDef,fFocus);
    */

    //Square focus
    fFoc2=fFocus*fFocus;
    
    //Center correction
    fCx=fCx-iDef/2;
    fCy=fCy-iDef/2;
    
    //Clip planet to vga port
    fRXmax=(fCx+fRad0>=iPx+iAx-iDef?-fCx+iPx+iAx-iDef+1
                                   :+fRad0);
    fRXmin=(fCx-fRad0<iPx          ?-fCx+iPx-((iPx-(int)fCx)%iDef)
                                   +(iPx-fCx>0?+iDef:0)    
                                   :-fRad0+(((int)fRad0)%iDef));
    fRYmax=(fCy+fRad0>=iPy+iAy-iDef?-fCy+iPy+iAy-iDef+1
                                   :+fRad0);
    fRYmin=(fCy-fRad0<iPy          ?-fCy+iPy-((iPy-(int)fCy)%iDef)
                                   +(iPy-fCy>0?+iDef:0)   
                                   :-fRad0+(((int)fRad0)%iDef));
    
    //Distance plus focus factor
    fDF=sqrt((fFocus*fFocus+fRad0*fRad0)/(fRad0*fRad0));
    
    //Planet loop
    for(fPx=fRXmin;fPx<fRXmax;fPx+=iDef){
    for(fPy=fRYmin;fPy<fRYmax;fPy+=iDef){

        //Alpha factor
        fAlp=fPx*fPx+fPy*fPy;
        
        //is the point inside of the circle?
        if(fAlp<=fRad2)
        {
            //2nd equation factors
            fA=fFoc2+fAlp;
            fB=-2*(fDF)*fAlp;
            fC=fDF*fDF*fAlp-fFoc2;
            fD=fB*fB-4*fA*fC;

            //3D coordinates
            sPos.y=(fB-sqrt(fD))/(2*fA);
            sPos.x=fPx*(sPos.y+fDF)/fFocus;
            sPos.z=fPy*(sPos.y+fDF)/fFocus;

            //Apply rotation matrix
            sPsr=Mxv(sRot,sPos);
            
            //Sphere angles
            fFi=acos(sPsr.z);
            if(fFi<0) fFi+=P21;
            fTh=Sth(sPsr);
            
            //Read color from map
            fXm=(iMAx-1)*fTh/P21;
            fYm=iMAy-1-(iMAy-1)*fFi/PI0;
            ulOffset=iMAx*(int)fYm+(int)fXm;
            iByte1=sBuf[2*ulOffset+1];
            iByte2=sBuf[2*ulOffset+0];
            uiColor=(iByte1<<8)+iByte2;

            //Apply shine vector
            if(iShine)
            {
                fFactor=Spr(Uni(sPsr),sShine);
                if(fFactor<0)
                { 
                    ColorByte2RGB(COLORS,uiColor,&uiRed,&uiGreen,&uiBlue);
                    if(fFactor>=-0.7)
                    {
                        fFactor=1+(1-20)*fFactor/0.7;
                    }
                    else
                    {
                        fFactor=15;
                    }
                    fRed  =uiRed; 
                    fGreen=uiGreen; 
                    fBlue =uiBlue;   
                    fRed  =fRed  /fFactor; 
                    fGreen=fGreen/fFactor; 
                    fBlue =fBlue /fFactor;    
                    uiRed  =(unsigned int)fRed;
                    uiGreen=(unsigned int)fGreen;
                    uiBlue =(unsigned int)fBlue;
                    ColorRGB2Byte(COLORS,uiRed,uiGreen,uiBlue,&uiColor);
                }
            }
            
            //Plot
            if(iDef>1)
                gl_fillbox((int)fPx+(int)fCx,
                           (int)fPy+(int)fCy,
                           iDef,iDef,uiColor);
            else
                gl_setpixel((int)fPx+(int)fCx,
                            (int)fPy+(int)fCy,
                            uiColor);
            
        }

    }}

}
