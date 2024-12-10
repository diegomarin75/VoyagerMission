// 2D Graphics

// --------------------------------------------------------------------------
// Type definitions
typedef struct W2d W2d; //2D window

// --------------------------------------------------------------------------
// Structures

//2D window
struct W2d
{
    int iAx,iAy;      //Heigth & width in pixels 
    int iPx,iPy;      //screen location in pixels
    Flo fXmax,fXmin;  //X maximun & minimun  
    Flo fYmax,fYmin;  //Y maximun & minimun  
};

// --------------------------------------------------------------------------
// Function prototypes
W2d  SetWin2D(int,int,int,int,Flo,Flo,Flo,Flo);
int  Lenght2DPixel(Flo,W2d *);
void Trans2DPixel(Flo,Flo,int *,int *,W2d *);
void Line2D(Flo,Flo,Flo,Flo,W2d *);
void Plot2D(Flo,Flo,W2d *);
void Text2D(Flo,Flo,char *,W2d *);
void Circle2D(Flo,Flo,Flo,W2d *);
int  Visible2d(Flo,Flo,Flo,W2d *);

// --------------------------------------------------------------------------    int iAx,iAy;      //Heigth & width in pixels 
// 2D Window
W2d SetWin2D(int iAx,int iAy,int iPx,int iPy,
             Flo fXmax,Flo fXmin,Flo fYmax,Flo fYmin)
{
    //Variables
    W2d sW2d;
    
    //Parameter setting
    sW2d.iAx=iAx;
    sW2d.iAy=iAy;
    sW2d.iPx=iPx;
    sW2d.iPy=iPy;
    sW2d.fXmax=fXmax;
    sW2d.fXmin=fXmin;
    sW2d.fYmax=fYmax;
    sW2d.fYmin=fYmin;
    
    //Return result
    return(sW2d);

}
    
// --------------------------------------------------------------------------
// 2D Lenght
int Lenght2DPixel(Flo fR,W2d *sW2d)
{
    //Variables
    int fPr; //pixel coordinates
    
    //Calculate pixel coordinates
    fPr=sW2d->iAx*fR/(sW2d->fXmax-sW2d->fXmin);

    //Return result
    return((int)fPr);

}
// --------------------------------------------------------------------------
// 2D Transformation to pixels
void Trans2DPixel(Flo fX,Flo fY,int *iPx,int *iPy,W2d *sW2d)
{
    //Variables
    Flo fPx,fPy; //pixel coordinates

    //Calculate pixel coordinates
    fPx=sW2d->iAx*(fX-sW2d->fXmin)/(sW2d->fXmax-sW2d->fXmin);
    fPy=sW2d->iAy*(fY-sW2d->fYmax)/(sW2d->fYmin-sW2d->fYmax);

    //Return result
    *iPx=sW2d->iPx+(int)fPx;
    *iPy=sW2d->iPy+(int)fPy;
    
}

// --------------------------------------------------------------------------
// 2D line
void Line2D(Flo fX1,Flo fY1,Flo fX2,Flo fY2,W2d *sW2d)
{
    //Variables
    int iPx1,iPy1,iPx2,iPy2; //pixel coordinates

    //2D transformation
    Trans2DPixel(fX1,fY1,&iPx1,&iPy1,sW2d);
    Trans2DPixel(fX2,fY2,&iPx2,&iPy2,sW2d);
    
    //Draw line
    gl_line(iPx1,iPy1,iPx2,iPy2,_iColor);

}

// --------------------------------------------------------------------------
// 2D Point
void Plot2D(Flo fX,Flo fY,W2d *sW2d)
{
    //Variables
    int iPx,iPy; //pixel coordinates

    //2D transformation
    Trans2DPixel(fX,fY,&iPx,&iPy,sW2d);
    
    //Draw point
    gl_setpixel(iPx,iPy,_iColor);

}

// --------------------------------------------------------------------------
// 2D text
void Text2D(Flo fX,Flo fY,char *sStr,W2d *sW2d)
{
    //Variables
    int iPx,iPy; //pixel coordinates

    //2D transformation
    Trans2DPixel(fX,fY,&iPx,&iPy,sW2d);

    //Draw text
    gl_write(iPx,iPy,sStr);

}

// --------------------------------------------------------------------------
// 2D Circle
void Circle2D(Flo fX,Flo fY,Flo fR,W2d *sW2d)
{
    //Variables
    int iPx,iPy,iPr; //pixel coordinates
    
    //Calculate pixel coordinates
    Trans2DPixel(fX,fY,&iPx,&iPy,sW2d);
    iPr=Lenght2DPixel(fR,sW2d);

    //Draw circle
    gl_circle(iPx,iPy,iPr,_iColor);

}

// ---------------------------------------------------------------------------
// Calculates if a body is visible in the window
int Visible2D(Flo fX,Flo fY,Flo fR,W2d *sW2d)
{
    //Calculate visibility
    if((fX-fR>=sW2d->fXmin && fX-fR<=sW2d->fXmax) || 
       (fX+fR>=sW2d->fXmin && fX+fR<=sW2d->fXmax) || 
       (fY-fR>=sW2d->fYmin && fY-fR<=sW2d->fYmax) || 
       (fY+fR>=sW2d->fYmin && fY+fR<=sW2d->fYmax)) 
        return(1);
    else return(0);

}

