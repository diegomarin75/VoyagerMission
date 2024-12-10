// Vector, matrix operations & physic formulas
#include <math.h>

// --------------------------------------------------------------------------

// Math & physic constants
#define PI0 (Flo)3.141592654  //Pi
#define P12 (Flo)1.570796327  //Pi/2
#define P21 (Flo)6.283185307  //Pi*2
#define P32 (Flo)4.712388980  //3*Pi/2
#define GRV (Flo)6.6726E-11   //Gravitational constant

//Other
#define SINCOSTB 720 //Sin cos table length

// --------------------------------------------------------------------------
// Types
typedef double     Flo; //Real number
typedef struct Vec Vec; //Vector
typedef struct Mtx Mtx; //Matrix
typedef struct Par Par; //Particle

// --------------------------------------------------------------------------
// Structures

struct Vec{ Flo x,y,z; };             //Vector
struct Mtx{ Flo a,b,c,d,e,f,g,h,i; }; //Matrix
struct Par{ Vec r; Flo m; };          //Particle

// --------------------------------------------------------------------------
// Global variables
Flo _sSin[SINCOSTB];
Flo _sCos[SINCOSTB];

// --------------------------------------------------------------------------
// Funtion prototypes

//Vector operations
Vec Clv(void);
Vec Eqv(Flo,Flo,Flo);
Vec Asv(Vec);
Vec Adv(Vec,Vec);
Vec Sbv(Vec,Vec);
Vec Vpr(Vec,Vec);
Flo Spr(Vec,Vec);
Vec Scv(Vec,Flo);
Vec Uni(Vec);
Flo Mod(Vec);
Flo Mo2(Vec);
Flo M2d(Vec);
Vec Rox(Vec,Flo);
Vec Roy(Vec,Flo);
Vec Roz(Vec,Flo);

//Matrix operations
Mtx Clm(void);
Mtx Eqm(Flo,Flo,Flo,Flo,Flo,Flo,Flo,Flo,Flo);
Mtx Asm(Mtx);
Mtx Adm(Mtx,Mtx);
Mtx Sbm(Mtx,Mtx);
Mtx Scm(Mtx,Flo);
Mtx Mxm(Mtx,Mtx);
Mtx Trs(Mtx);
Mtx Inv(Mtx);
Mtx Tfm(Vec);

//Matrix & vector operations
Vec Mxv(Mtx,Vec);

//Other
Flo Ang(Flo,Flo);
Vec Vsc(Flo,Flo,Flo);
Flo Sfi(Vec);
Flo Sth(Vec);
Mtx Rmx(Flo);
Mtx Rmy(Flo);
Mtx Rmz(Flo);

//Physic formulas
Vec Gra(Par,Par);
Flo Mas(Par *,int);
Mtx Inr(Par *,int);

//Sin cos tables
void SinCosTb(void);
Flo  Sin(Flo);
Flo  Cos(Flo);
    
// --------------------------------------------------------------------------
//Vector clear
Vec Clv(void)
{
    Vec r;
    r.x=0; r.y=0; r.z=0;
    return(r);
}

// --------------------------------------------------------------------------
//Matrix asignment
Mtx Clm(void)
{
    Mtx r;
    r.a=0; r.b=0; r.c=0;
    r.d=0; r.e=0; r.f=0;
    r.g=0; r.h=0; r.i=0;
    return(r);
}

// --------------------------------------------------------------------------
//Vector asignment
Vec Eqv(Flo x,Flo y,Flo z)
{
    Vec r;
    r.x=x;
    r.y=y;
    r.z=z;
    return(r);
}

// --------------------------------------------------------------------------
//Matrix asignment
Mtx Eqm(Flo a,Flo b,Flo c,Flo d,Flo e,Flo f,Flo g,Flo h,Flo i)
{
    Mtx r;
    r.a=a; r.b=b; r.c=c;
    r.d=d; r.e=e; r.f=f;
    r.g=g; r.h=h; r.i=i;
    return(r);
}

// --------------------------------------------------------------------------
// Vector asignation
Vec Asv(Vec v)
{
    Vec r;
    r.x=v.x; r.y=v.y; r.z=v.z;
    return(r);
}

// --------------------------------------------------------------------------
// Matrix asignation
Mtx Asm(Mtx m)
{
    Mtx r;
    r.a=m.a; r.b=m.b; r.c=m.c;
    r.d=m.d; r.e=m.e; r.f=m.f;
    r.g=m.g; r.h=m.h; r.i=m.i;
    return(r);
}

// --------------------------------------------------------------------------
//Vector addition
Vec Adv(Vec a,Vec b)
{
    Vec r;
    r.x=a.x+b.x;
    r.y=a.y+b.y;
    r.z=a.z+b.z;
    return(r);
}

// --------------------------------------------------------------------------
//Vector substraction
Vec Sbv(Vec a,Vec b)
{
    Vec r;
    r.x=a.x-b.x;
    r.y=a.y-b.y;
    r.z=a.z-b.z;
    return(r);
}

// --------------------------------------------------------------------------
//Vectorial product
Vec Vpr(Vec a,Vec b)
{
    Vec r;
    r.x=a.y*b.z-a.z*b.y;
    r.y=a.z*b.x-a.x*b.z;
    r.z=a.x*b.y-a.y*b.x;
    return(r);
}

// --------------------------------------------------------------------------
// Scalar product
Flo Spr(Vec a,Vec b)
{
    return(a.x*b.x+a.y*b.y+a.z*b.z);
}

// --------------------------------------------------------------------------
//Vector scale
Vec Scv(Vec v,Flo f)
{
    Vec r;
    r.x=v.x*f;
    r.y=v.y*f;
    r.z=v.z*f;
    return(r);
}

// --------------------------------------------------------------------------
//Matrix x vector multiplication
Vec Mxv(Mtx m,Vec v)
{
    Vec r;
    r.x=m.a*v.x+m.b*v.y+m.c*v.z;
    r.y=m.d*v.x+m.e*v.y+m.f*v.z;
    r.z=m.g*v.x+m.h*v.y+m.i*v.z;
    return(r);
}

// --------------------------------------------------------------------------
//Module
Flo Mod(Vec v)
{
    return(sqrt(v.x*v.x+v.y*v.y+v.z*v.z));
}

// --------------------------------------------------------------------------
//Module^2
Flo Mo2(Vec v)
{
    return(v.x*v.x+v.y*v.y+v.z*v.z);
}

// --------------------------------------------------------------------------
//Module 2D
Flo M2d(Vec v)
{
    return(sqrt(v.x*v.x+v.y*v.y));
}


// --------------------------------------------------------------------------
//Module 1 scale
Vec Uni(Vec v)
{
    Flo m;
    m=Mod(v);
    if(m!=0) return(Scv(v,1/m));
    else     return(v);
}

// --------------------------------------------------------------------------
// Matrix addition
Mtx Adm(Mtx a,Mtx b)
{
    Mtx r;
    r.a=a.a+b.a; r.b=a.b+b.b; r.c=a.c+b.c;
    r.d=a.d+b.d; r.e=a.e+b.e; r.f=a.f+b.f;
    r.g=a.g+b.g; r.h=a.h+b.h; r.i=a.i+b.i;
    return(r);
}

// --------------------------------------------------------------------------
// Matrix substraction
Mtx Sbm(Mtx a,Mtx b)
{
    Mtx r;
    r.a=a.a-b.a; r.b=a.b-b.b; r.c=a.c-b.c;
    r.d=a.d-b.d; r.e=a.e-b.e; r.f=a.f-b.f;
    r.g=a.g-b.g; r.h=a.h-b.h; r.i=a.i-b.i;
    return(r);
}

// --------------------------------------------------------------------------
// Matrix scalar
Mtx Scm(Mtx m,Flo f)
{
    Mtx r;
    r.a=m.a*f; r.b=m.b*f; r.c=m.c*f;
    r.d=m.d*f; r.e=m.e*f; r.f=m.f*f;
    r.g=m.g*f; r.h=m.h*f; r.i=m.i*f;
    return(r);
}

// --------------------------------------------------------------------------
// Matrix product
Mtx Mxm(Mtx m,Mtx n)
{
    Mtx r;
    r.a=m.a*n.a+m.b*n.d+m.c*n.g;
    r.b=m.a*n.b+m.b*n.e+m.c*n.h;
    r.c=m.a*n.c+m.b*n.f+m.c*n.i;
    r.d=m.d*n.a+m.e*n.d+m.f*n.g;
    r.e=m.d*n.b+m.e*n.e+m.f*n.h;
    r.f=m.d*n.c+m.e*n.f+m.f*n.i;
    r.g=m.g*n.a+m.h*n.d+m.i*n.g;
    r.h=m.g*n.b+m.h*n.e+m.i*n.h;
    r.i=m.g*n.c+m.h*n.f+m.i*n.i;
    return(r);
}

// --------------------------------------------------------------------------
// Matrix transpose
Mtx Trs(Mtx m)
{
    Mtx r;
    r.a=m.a;
    r.b=m.d;
    r.c=m.g;
    r.d=m.b;
    r.e=m.e;
    r.f=m.h;
    r.g=m.c;
    r.h=m.f;
    r.i=m.i;
    return(r);
}

// --------------------------------------------------------------------------
// Matrix inversion
Mtx Inv(Mtx m)
{
    //Variables
    Flo d; //Matrix det
    Mtx r; //Result

    //Matrix det
    d=+m.a*m.e*m.i+m.b*m.f*m.g+m.c*m.d*m.h
      -m.g*m.e*m.c-m.d*m.b*m.i-m.a*m.h*m.f;

    //There isn't inverted matrix
    if(d==0) return(Clm());

    //Transpose matrix
    m=Trs(m);

    //Calculate matrix
    r.a=+(m.e*m.i-m.h*m.f)/d; r.b=-(m.d*m.i-m.g*m.f)/d; r.c=+(m.d*m.h-m.g*m.e)/d;
    r.d=-(m.b*m.i-m.h*m.c)/d; r.e=+(m.a*m.i-m.g*m.c)/d; r.f=-(m.a*m.h-m.g*m.b)/d;
    r.g=+(m.b*m.f-m.e*m.c)/d; r.h=-(m.a*m.f-m.d*m.c)/d; r.i=+(m.a*m.e-m.d*m.b)/d;

    //Return result
    return(r);
}

// --------------------------------------------------------------------------
// Rotation matrix around x axis
Mtx Rmx(Flo a)
{
    Mtx m;
    m.a=1; m.b=0;       m.c=0;
    m.d=0; m.e=+cos(a); m.f=-sin(a);
    m.g=0; m.h=+sin(a); m.i=+cos(a);
    return(m);
}

// --------------------------------------------------------------------------
// Rotation matrix around y axis
Mtx Rmy(Flo a)
{
    Mtx m;
    m.a=+sin(a); m.b=0; m.c=+cos(a);
    m.d=0;       m.e=1; m.f=0;
    m.g=+cos(a); m.h=0; m.i=-sin(a);
    return(m);
}

// --------------------------------------------------------------------------
// Rotation matrix around z axis
Mtx Rmz(Flo a)
{
    Mtx m;
    m.a=+cos(a); m.b=-sin(a); m.c=0;
    m.d=+sin(a); m.e=+cos(a); m.f=0;
    m.g=0;       m.h=0;       m.i=1;
    return(m);
}

// --------------------------------------------------------------------------
// Vector rotation around x axis
Vec Rox(Vec v,Flo a)
{
    Vec r;
    r.x=v.x;
    r.y=+cos(a)*v.y-sin(a)*v.z;
    r.z=+sin(a)*v.y+cos(a)*v.z;
    return(r);
}

// --------------------------------------------------------------------------
// Vector rotation around y axis
Vec Roy(Vec v,Flo a)
{
    Vec r;
    r.x=+sin(a)*v.x+cos(a)*v.z;
    r.y=v.y;
    r.z=+cos(a)*v.x-sin(a)*v.z;
    return(r);
}

// --------------------------------------------------------------------------
// Vector rotation around z axis
Vec Roz(Vec v,Flo a)
{
    Vec r;
    r.x=+cos(a)*v.x-sin(a)*v.y;
    r.y=+sin(a)*v.x+cos(a)*v.y;
    r.z=v.z;
    return(r);
}

// --------------------------------------------------------------------------
// Angle (cos^-1)
Flo Ang(Flo x,Flo y)
{
    //Variables
    Flo m;   //Modules
    Flo a=0; //Angle

    //Module
    m=sqrt(x*x+y*y);

    //Calculate angle
    if(y >0)         a=000+acos(+x/m);
    if(y <0)         a=PI0+acos(-x/m);
    if(y==0 && x >0) a=000;
    if(y==0 && x <0) a=PI0;
    if(y==0 && x==0) a=000;
    
    //Return result
    return(a);
}

// --------------------------------------------------------------------------
// Transformation matrix
Mtx Tfm(Vec t)
{

    //Variables
    Flo tx,ty,fi; //Angles
    Vec x,y,z;    //Rotated base
    Flo m;        //Module
    Mtx r;        //Transformation matrix

    //Z vector
    z=t;

    //Z modules
    m=Mod(z);

    //No transformation
    if(m==0) return(Eqm(1,0,0,0,1,0,0,0,1));

    //Calculate angles
    tx=Ang(z.y,z.x);
    ty=tx+P12;
    fi=acos(z.z/m);

    //Normalize z vector
    z=Uni(z);

    //Calculate rotated x y vectors
    x=Eqv(1-sin(tx)*sin(tx)*(1-cos(fi)),
           -sin(tx)*cos(tx)*(1-cos(fi)),
           -sin(tx)*sin(fi));
    y=Eqv( +sin(ty)*cos(ty)*(1-cos(fi)),
          1-sin(ty)*sin(ty)*(1-cos(fi)),
          -sin(ty)*sin(fi));

    //Transformation matrix
    r=Eqm(x.x,y.x,z.x,x.y,y.y,z.y,x.z,y.z,z.z);

    //Return result
    return(r);
}

// ---------------------------------------------------------------------------
// Spherical coordinates theta angle calculation
Flo Sth(Vec v)
{
    return(Ang(v.x,v.y));
}

// ---------------------------------------------------------------------------
// Spherical coordinates fi angle calculation
Flo Sfi(Vec v)
{
    return(Ang(M2d(v),v.z));
}

// --------------------------------------------------------------------------
// Compose vector via spherical coordinates
Vec Vsc(Flo rad,Flo th,Flo fi)
{
    Vec r;
    r.x=rad*sin(fi)*cos(th);
    r.y=rad*sin(fi)*sin(th);
    r.z=rad*cos(fi);
    return(r);
}

// --------------------------------------------------------------------------
// Gravitational force betwen two particles
Vec Gra(Par a,Par b)
{
    //Variables
    Vec sFrc; //Force
    Vec sDst; //Distance
    Flo fDst; //Distance

    //Distance betwen the two particles
    sDst=Sbv(a.r,b.r); fDst=Mod(sDst);

    //Gravitational force
    sFrc=Scv(sDst,GRV*a.m*b.m/(fDst*fDst*fDst));

    //Return result
    return(sFrc);
}

// --------------------------------------------------------------------------
// Total mass calculation
Flo Mas(Par *p,int n)
{
    //Variables
    int i; //Counter
    Flo m; //Mass

    //Loop
    for(i=0,m=0;i<n;i++) m+=p[i].m;

    //Return result
    return(m);

}

// --------------------------------------------------------------------------
// Inertia tensor calculation
Mtx Inr(Par *p,int n)
{
    //Variables
    int i;       //Counter
    Mtx sI;      //Particle inertia tensor
    Mtx sIt;     //System inertia tensor

    //Loop
    for(i=0,sIt=Clm();i<n;i++)
    {
        //Inertia tensor
        sI=Eqm(+p[i].r.y*p[i].r.y+p[i].r.z*p[i].r.z,
               -p[i].r.x*p[i].r.y,
               -p[i].r.x*p[i].r.z,
               -p[i].r.x*p[i].r.y,
               +p[i].r.x*p[i].r.x+p[i].r.z*p[i].r.z,
               -p[i].r.y*p[i].r.z,
               -p[i].r.x*p[i].r.z,
               -p[i].r.y*p[i].r.z,
               +p[i].r.x*p[i].r.x+p[i].r.y*p[i].r.y);
        sI=Scm(sI,p[i].m);

        //Add to total result
        sIt=Adm(sIt,sI);
    }

    //Return result
    return(sIt);

}

// --------------------------------------------------------------------------
// Sin cos tables calculation
void SinCosTb(void)
{
    //Variables
    int i; //Counters
    
    //Sin/cos table
    for(i=0;i<SINCOSTB;i++){
        _sSin[i]=sin(P21*((float)i)/((float)SINCOSTB));
        _sCos[i]=cos(P21*((float)i)/((float)SINCOSTB));
    }
}

// --------------------------------------------------------------------------
// Sine from table
Flo Sin(Flo a)
{
    //Variables
    Flo f; //Index
    
    //Reduce angle to range 0 - 2PI
    while(a<0 || a>P21)
    {
        if(a<0  ) a+=P21;
        if(a>P21) a-=P21;
    }
    
    //Calculate angle index    
    f=(SINCOSTB-1)*a/P21;
    
    //Return value
    return(_sSin[(int)f]);
    
}

// --------------------------------------------------------------------------
// Cosine from table
Flo Cos(Flo a)
{
    //Variables
    Flo f; //Index
    
    //Reduce angle to range 0 - 2PI
    while(a<0 || a>P21)
    {
        if(a<0  ) a+=P21;
        if(a>P21) a-=P21;
    }
    
    //Calculate angle index    
    f=(SINCOSTB-1)*a/P21;
    
    //Return value
    return(_sCos[(int)f]);
    
}
