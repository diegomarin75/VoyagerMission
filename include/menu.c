// Menu functions

// ---------------------------------------------------------------------------
// Constants

// Menu definitions
#define MENUENTRYLEN  80                          //Menu entry length
#define MENUGROUND    "data/neptune-faraway.pcx"  //Background image
#define MENUFONT47X30 "data/font47x30.pcx"        //Pcx font file
#define MENUFONTAX    30                          //Menu font size
#define MENUFONTAY    47                          //Menu font size
#define MAXNODE       10                          //Maimum menu nodes
#define MAXITEM       10                          //Maimum menu nodes

// ---------------------------------------------------------------------------
// Types
typedef struct Mit Mit; //Menu item

// ---------------------------------------------------------------------------
// Structures

// Menu item
struct Mit
{
    char sName[MENUENTRYLEN]; //Menu entry name
    int  iCode;               //Return code for this entry
    int  iSubnode;            //Subnode for this node
    int  iLastItem;           //Last item flag
};

// ---------------------------------------------------------------------------
// Global variables

// Pcx Font handle
int _iPcxFont;

// Background image
Cmi _sMenuGround;

// Menu items
Mit _sMenu[MAXNODE][MAXITEM]={{{"Voyager Mission 2.0",0, 0,0},
                               {"New mission"        ,1,-1,0},
                               {"Load mission"       ,2,-1,0},
                               {"Settings"           ,3,-1,0},
                               {"Credits"            ,4,-1,0},
                               {"Help"               ,5, 1,0},
                               {"Quit"               ,6,-1,1}},
                              {{"Help"               ,0, 0,0},
                               {"Commands"           ,7,-1,0},
                               {"Console"            ,8,-1,0},
                               {"Configuration"      ,9,-1,1}}};

// ---------------------------------------------------------------------------
// Function prototypes
int MenuInit(void);
int CallMenu(int,int);
void PrintNode(int);
void ItemMark(int,int);

// ---------------------------------------------------------------------------
// Menu initialization
int MenuInit(void)
{
    //Load background image
    if(GMemRead(MENUGROUND,&_sMenuGround,GFLOCKED)!=GFILOK)
    {
        printf("Cannot load menu background image\n");
        return(-1);
    }
    
    //Load pcx font
    if(LoadPcxFont(MENUFONT47X30,30,47,33,94,&_iPcxFont)!=PCXFNTOK)
    {
        printf("Cannot load pcx menu font\n");
    }
    
    //Return code
    return(0);
    
}

// ---------------------------------------------------------------------------
// Menu close
void MenuClose(void)
{
    //Free background image
    GMemFree(MENUGROUND);
    
    //Free pcx font
    ClosePcxFont(_iPcxFont);
    
}

// ---------------------------------------------------------------------------
// Main menu call function
int CallMenu(int iNode,int iOption0)
{
    //Variables
    int i;                //Counter
    int iExit=0;          //Exit flag
    int iOption=iOption0; //Option
    int iCode;            //Return code
    int iLastOption=0;    //Last option
    int iKey1,iKey2;      //Keyboard keys 
    int iLastItem;        //Last item of node
     
    //Search last item on node
    for(i=0;!_sMenu[iNode][i+1].iLastItem;i++); iLastItem=i;
    
    //Init menu
    TimerSuspend();
    gl_putbox(0,0,WIDTH,HEIGHT,_sMenuGround.sImg);
    PrintNode(iNode);
    ItemMark(iNode,iOption0);
    ShowScreen();
    TimerWakeUp();
    
    //Menu loop
    do
    {
        //Save previous option
        iLastOption=iOption;
        
        //Read key
        if((iKey1=KbGet())==0) iKey2=KbGet();
        
        //Switch on key
        switch(iKey1)
        {    
            case 27: iOption=-1; iExit=1; break;
            case 10: if(_sMenu[iNode][iOption+1].iSubnode!=-1)
                     {
                         iCode=CallMenu(_sMenu[iNode][iOption+1].iSubnode,0);
                         if(iCode!=-1) return(iCode);
                         iLastOption=-1;
                     }
                     else
                     {
                         iExit=1; 
                     }
                     break;
            case 0:
                switch(iKey2)
                {
                    case SCANCODE_CURSORUP:   
                    case SCANCODE_CURSORBLOCKUP:
                    case SCANCODE_PAGEUP:
                        if(iOption>0) iOption--; else iOption=iLastItem; break;
                    case SCANCODE_CURSORDOWN: 
                    case SCANCODE_CURSORBLOCKDOWN:
                    case SCANCODE_PAGEDOWN:
                        if(iOption<iLastItem) iOption++; else iOption=0; break;
                }
                break;
        }
        
        //Refresh screen
        if(!iExit && iOption!=iLastOption)
        {
            TimerSuspend();
            gl_putbox(0,0,WIDTH,HEIGHT,_sMenuGround.sImg);
            PrintNode(iNode);
            ItemMark(iNode,iOption);
            ShowScreen();
            TimerWakeUp();
        }
        
    }while(!iExit);
    
    //Return option
    return(iOption==-1?-1:_sMenu[iNode][iOption+1].iCode);
    
}

// ---------------------------------------------------------------------------
// Print menu node contents
void PrintNode(int iNode)
{
    //Variables
    int i; //Counter
    
    //Print Node title
    PcxFntPutStr(_iPcxFont,0,0,_sMenu[iNode][0].sName,PXFCENTERED);
    
    //Print node contents
    for(i=1;;i++)
    {
        PcxFntPutStr(_iPcxFont,0,(i+1)*MENUFONTAY,
                     _sMenu[iNode][i].sName,PXFCENTERED);
        if(_sMenu[iNode][i].iLastItem) break;
    }
    
}

// ---------------------------------------------------------------------------
// Mark menu item
void ItemMark(int iNode,int iItem)
{
    //Variables
    char sStr[80]; //String
    
    //Build marked item string
    sprintf(sStr,">> %s <<",_sMenu[iNode][iItem+1].sName);
    
    //Print string
    PcxFntPutStr(_iPcxFont,0,(iItem+2)*MENUFONTAY,sStr,PXFCENTERED);
    
}