// Graphic files vga functions

// ---------------------------------------------------------------------------
// Function prototypes
void GPutImage(int,int,int,int,Cmi *);

// ---------------------------------------------------------------------------
// Put image
void GPutImage(int iPx,int iPy,int iAx,int iAy,Cmi *sCmi)
{
    //Variables
    int iX,iY;         //Screen coordinates
    int iAx0,iAy0;     //Size correction
    int iColor;        //RGB color
    int iByte1,iByte2; //Bytes
        
    //Check location
    if(iPx<0 || iPx>WIDTH-1 || iPy<0 || iPy>HEIGHT-1) return; 
    
    //Size correction
    iAx0=sCmi->sGat->iAx; if(iAx && iAx<iAx0) iAx0=iAx;
    iAy0=sCmi->sGat->iAy; if(iAy && iAy<iAy0) iAy0=iAy;
    if(iPx+iAx0>WIDTH)  iAx0=WIDTH -iPx;
    if(iPy+iAy0>HEIGHT) iAy0=HEIGHT-iPy;
    
    //Algorithm for 16 & 256 color modes
    if(COLORS==16 || COLORS==256)
    {
        for(iX=0;iX<iAx0;iX++){
        for(iY=0;iY<iAy0;iY++){
            gl_setpixel(iX+iPx,iY+iPy,sCmi->sImg[(sCmi->sGat->iAx)*iY+iX]);
        }}
    }
    
    //Algorithm for 32K and 64K color modes
    else if(COLORS==1<<15 || COLORS==1<<16)
    {
        for(iX=0;iX<iAx0;iX++){
        for(iY=0;iY<iAy0;iY++){
            iByte1=sCmi->sImg[2*(sCmi->sGat->iAx*iY+iX)+1];
            iByte2=sCmi->sImg[2*(sCmi->sGat->iAx*iY+iX)+0];
            iColor=(iByte1<<8)+iByte2;
            gl_setpixel(iX+iPx,iY+iPy,iColor);
        }}
    }
    
}