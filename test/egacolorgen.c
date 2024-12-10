// EGA color generator

// --------------------------------------------------------------------------
// Includes
#include <stdio.h>
#include "include/gfiles.c"

// --------------------------------------------------------------------------
// Main function
void main(int argc,char **argv)
{

    //Variables
    FILE *fp;                //Output file
    int iIndex;              //Indexed color
    int iRGB555,iRGB565;     //RGB Color
    Cmi sCmi;                //Cache memory interface

    //Check arguments
    if(argc!=2)
    {
        printf("Usage: %s <extractfile.pcx/bmp>\n",argv[0]);
        exit(0);
    }
    
    //Init cache memory
    GMemInit(CVNULL);
    
    //Load file via graphic cache memory
    printf("Loading file...\n");
    if(GMemRead(argv[1],&sCmi)!=GFILOK)
    {
        printf("Error loading file\n");
        GMemClos();
        exit(0);
    }
    
    //Open output file
    if((fp=fopen("egacolor.c","wb"))==NULL)
    {
        printf("Cannot create output file\n");
        GMemClos();
        exit(0);
    }
    
    //Output file header
    fprintf(fp,"//EGA color definitions in RGB555 / RGB565\n\n");
    
    //Conversion loop
    for(iIndex=0;iIndex<16;iIndex++)
    {
        //Generate RGB colors
        RGBColor(CVRGB555,iIndex,sCmi.sRgb,&iRGB555);
        RGBColor(CVRGB565,iIndex,sCmi.sRgb,&iRGB565);
        
        //Output to file
        fprintf(fp,"#define EGACOLOR%02i "
                   "(COLORS==(1<<15)?0x%04X:0x%04X) //Ega Color %02i\n",
                   iIndex,iRGB555,iRGB565,iIndex);
    }
    
    //Close file
    fclose(fp);

    //Close cache memory
    GMemClos();

}



