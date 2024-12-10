// PCX Viewer 
// by DMH April 1999

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "include/graphics.c"
#include "include/gfiles.c"

#define PCX 1
#define BMP 2

void main( int argc,char **argv ){

  //Variables
  int i,j;
  int iAx,iAy;
  int iMode;
  int iFile;
  int iError;
  char *sBuf;
  Rgb sRgb[256];
  Gat sGat;

  //Cadena de bienvenida
  printf("Pcx & Bmp files viewer\n");
  
  //Comprobar argumentos de la linea de comandos
  if( argc!=2 ){
      printf( "Usage is: %s <file>\n", argv[0] );
      exit( 0 );
  }

  //Comprobar que el fichero es accesible
  if( fopen( argv[1],"rb" )==NULL ){
      printf( "Can't open file\n" );
      exit( 0 );
  }

  //Pcx o bmp?
  i=strlen(argv[1]);
  if(argv[1][i-3]=='p'&& argv[1][i-2]=='c' && argv[1][i-1]=='x') iFile=PCX;
  else if(argv[1][i-3]=='b'&& argv[1][i-2]=='m' && argv[1][i-1]=='p') iFile=BMP;
  else
  {
      printf("Can't read this file type\n");
      exit(0);
  }
  
  //Leer tamanyo del fichero
  if(iFile==PCX) iError=PcxGetAtt(argv[1],&sGat);
  if(iFile==BMP) iError=BmpGetAtt(argv[1],&sGat);
  if(iError!=GFILOK)
  {
      printf("Error loading file\n");
      exit(0);
  }
 
  //Reservar memoria
  if((sBuf=malloc( ((long)sGat.iAx)*((long)sGat.iAy)))==NULL)
  {
      printf("Not enough free memory\n");
      exit(0);
  }
 
  //Cargar fichero
  if(iFile==PCX) iError=PcxLoad(argv[1],sBuf,sRgb,&sGat);
  if(iFile==BMP) iError=BmpLoad(argv[1],sBuf,sRgb,&sGat);
  if(iError!=GFILOK)
  {
      printf("Error loading file\n");
      exit(0);
  }
 
  //Iniciar modo de video
  if(sGat.iAx<= 320 && sGat.iAy<=200) iMode=G320x200x256;
  else if((sGat.iAx>320||sGat.iAy>200) && sGat.iAx<= 640 && sGat.iAy<=480) iMode=G640x480x256;
  else if((sGat.iAx>640||sGat.iAy>480) && sGat.iAx<= 800 && sGat.iAy<=600) iMode=G800x600x256;
  else if((sGat.iAx>800||sGat.iAy>600)                                   ) iMode=G1024x768x256;
  printf("File size.: %ix%i\n",sGat.iAx,sGat.iAy);
  switch(iMode)
  {
      case G320x200x256:  printf("Video mode: G320x200x256\n");  break;
      case G640x480x256:  printf("Video mode: G640x480x256\n");  break;
      case G800x600x256:  printf("Video mode: G800x600x256\n");  break;
      case G1024x768x256: printf("Video mode: G1024x768x256\n"); break;
  }
  InitGraph(iMode,OPTNORMAL);
  keyboard_init();

  //Escribir fichero en la pantalla
  gl_setpalettecolors(0,256,sRgb);
  iAx=(sGat.iAx>WIDTH?WIDTH:sGat.iAx);
  iAy=(sGat.iAy>HEIGHT?HEIGHT:sGat.iAy);
  for(i=0;i<iAx;i++){
  for(j=0;j<iAy;j++){
      gl_setpixel(i,j,sBuf[sGat.iAx*j+i]);
  }}

  //Esperar tecla 
  do{ if(keyboard_update()) break; }while(1);
 
  //Restaurar modo de video y liberer mamoria
  free(sBuf);
  keyboard_close();
  CloseGraph();

}
