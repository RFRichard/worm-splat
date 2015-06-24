/*
 *  Drawing and Physics Functions
 *  2010/2011
 *  RRichard
 *  Android NDK Library
 * 
 */

// API > 2.1
#define BMAPCALLS 1

#ifdef BMAPCALLS
#include <android/bitmap.h>
#endif

#include <android/log.h>
 
#include <jni.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define  LOG_TAG    "libwormsplat"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


// API >2.1
#ifndef BMAPCALLS
typedef struct{
  int width;
  int height;
  int stride;
} AndroidBitmapInfo;
#endif

/* TYPES */
enum {NONE, FIRE, PLASMA, ALPHA, BLINE, TUNNEL, PLASMA2, JULIA_SET, NOISE_CLOUD, PHYSICS_SIM, PHYSICS_PARTICLE};
enum {ALPHA_NONE, ALPHA_BLOCK, ALPHA_LINE};

typedef uint16_t Pixel;

/* Set to 1 to enable debug log traces. */
#define DEBUG 0

/* Return current time in milliseconds */
static double now_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec*1000. + tv.tv_usec/1000.;
}


typedef int32_t  Fixed;
static int w, h;

// palette
#define PAL_SIZE 256
#define PALETTE_SIZE 256
static uint16_t  palette[PALETTE_SIZE];

/////////////////////////////////////////////////////////////////////////////////////
//	Graphics Functions
/////////////////////////////////////////////////////////////////////////////////////
#define  PIX_POS(x,y)   (x*y+x)

static void* pal_map;

static uint16_t  make565(int red, int green, int blue)
{
    /* Incorrect 565 implementation */
    return (uint16_t)( ((red   << 8) & 0xf800) |
                       ((green << 2) & 0x03e0) |
                       ((blue  >> 3) & 0x001f) );
	
	
	/* Correct 565 */
	//return (uint16_t) (red << 11 | green << 5 | blue);
	
}

static uint16_t GetPixPos(int x, int y, int w, int h)
{
	return ((y * w) + x);
}


static void createfirepalette(void)
{
			int i;
			for (i = 0; i < 64; i++)
			{				
				palette[i] = make565((i * 4), 0, 0);
				palette[i+64] = make565(255, (i*4), 0);
				palette[i+128] = make565(255,255,i*4);
				palette[i + 192] = make565(255,255,255);
			}
			
			for (i = 0; i < 255; i++)
			{
				LOGE(" palette val=%d - %d",i,palette[i]);
			}
						
			
}

int getPalVal(uint16_t pal)
{

	int x;
	for(x=0;x<255;x++)
	{
		if(pal == palette[x])break;
	}
	return x;

}

Pixel rgb_calc(Pixel r, Pixel g, Pixel b)
{
	// 565 color space
	Pixel pix = (Pixel) (((r << 11) | (g << 5)) | b);

	//LOGE(" rbg_calc=%d, %d, %d  - %x",r, g, b, pix);

	return pix;
}

Pixel getAlpha(Pixel p1, Pixel p2, float alpha)
{			
			Pixel thepixel;
			float thepixelfp, bgcolorfp, fptemp1, fptemp2, fptemp3;	
			Pixel p1r, p1g, p1b, p2r, p2g, p2b;
			
			p1r = ((p1 & 0xf800)>>11);
			p1g = ((p1 & 0x7e0)>>5);
			p1b = (p1 & 0x1f);
						
			//gin gout
			p2r = ((p2 & 0xf800)>>11);
			p2g = ((p2 & 0x7e0)>>5);
			p2b = (p2 & 0x1f);
									
			// alpha composite   C = (1-a)*c1+(a*c2)
			fptemp1 = (1.-alpha)*p1r+(alpha*p2r);
			fptemp2 = (1.-alpha)*p1g+(alpha*p2g);
			fptemp3 = (1.-alpha)*p1b+(alpha*p2b);
		
			//LOGE(" p1=%x  %d, %d, %d",p1, p1r, p1g, p1b);
			//LOGE(" p2=%x  %d, %d, %d",p2, p2r, p2g, p2b);
			//LOGE(" Alpha=%f, %f, %f",fptemp1, fptemp2, fptemp3);
			
			thepixel = rgb_calc((int)fptemp1, (int)fptemp2, (int)fptemp3);	
	
			return thepixel;
}

void createalphapalette()
{

			int i;
			for (i = 0; i < 64; i++)
			{				
				palette[i] = make565((i * 4), 0, 0);
				palette[i+64] = make565(255, (i*4), 0);
				palette[i+128] = make565(255,255,i*4);
				palette[i + 192] = make565(255,255,255);
			}
			
			for (i = 0; i < 255; i++)
			{
				LOGE(" palette val=%d - %d",i,palette[i]);
			}
						
			
}
/* affine Transform Function */

#define BLACK 0
#define WHITE 255
#define PI 3.141592654
#define AFFINE_SIZE_X 12
#define AFFINE_SIZE_Y 12
void affine(double x_rot, double y_rot, double x_expan,
    double y_expan, int x_move, int y_move, int w, int h,
    unsigned char image1[AFFINE_SIZE_X][AFFINE_SIZE_X], unsigned char image2[AFFINE_SIZE_X][AFFINE_SIZE_X])
     /* affine transformation of input image */
     /* by backward transformation and bilinear interpolation */
     /* input: image1[y][x] -------- output: image2[y][x]   */
{
  double affine[2][2], beta[2];
  double det, i_affine[2][2], i_beta[2];
  double x_new, y_new, x_frac, y_frac;
  double gray_new;
  int gv_new;
  int x, y, m, n;
  int x_size1, y_size1, x_size2, y_size2;
  
  x_size1 = w;
  y_size1 = h;
  
  x_size2 = x_size1;
  y_size2 = y_size1;
  
  /* forward affine transformation */
  affine[0][0] = x_expan * cos(x_rot*PI/180.0);
  affine[0][1] = y_expan * sin(y_rot*PI/180.0);
  affine[1][0] = x_expan * sin(x_rot*PI/180.0);
  affine[1][1] = y_expan * cos(y_rot*PI/180.0);
  beta[0] = x_move;
  beta[1] = y_move;
  
  /* determination of inverse affine transformation */
  det = affine[0][0]*affine[1][1] - affine[0][1]*affine[1][0];
  if (det == 0.0) {
    i_affine[0][0] = 1.0;
    i_affine[0][1] = 0.0;
    i_affine[1][0] = 0.0;
    i_affine[1][1] = 1.0;
    i_beta[0] = -beta[0];
    i_beta[1] = -beta[1];
  } else {
    i_affine[0][0] =  affine[1][1]/det;
    i_affine[0][1] = -affine[0][1]/det;
    i_affine[1][0] = -affine[1][0]/det;
    i_affine[1][1] =  affine[0][0]/det;
    i_beta[0] = -i_affine[0][0]*beta[0]-i_affine[0][1]*beta[1];
    i_beta[1] = -i_affine[1][0]*beta[0]-i_affine[1][1]*beta[1];
  }
  
  /* output image generation by inverse affine transformation
     and bilinear transformation  */
  
  for (y = 0; y < y_size2; y++) {
    for (x = 0; x < x_size2; x++) {
      x_new = i_beta[0] + i_affine[0][0]*(x-x_size2/2.0)
	+ i_affine[0][1]*(y-y_size2/2.0) + x_size2/2.0;
      y_new = i_beta[1] + i_affine[1][0]*(x-x_size2/2.0)
	+ i_affine[1][1]*(y-y_size2/2.0) + y_size2/2.0;
      m = (int)floor(x_new);
      n = (int)floor(y_new);
      x_frac = x_new - m;
      y_frac = y_new - n;
      
      if (m >= 0 && m+1 < x_size2 && n >= 0 && n+1 < y_size2) {
	gray_new = (1.0 - y_frac)*((1.0 - x_frac)*image1[n][m] +
				   x_frac *image1[n][m+1]) +
	  y_frac *((1.0 - x_frac)*image1[n+1][m] +
		   x_frac *image1[n+1][m+1]);
	image2[y][x] = (unsigned char)gray_new;
      } else if (m+1 == x_size2 && n >= 0 && n < y_size2 
		 || n+1 == y_size2 && m >= 0 && m < x_size2) {
	image2[y][x] = image1[n][m];
      } else {
	image2[y][x] = WHITE;
      }
    }
  }
}


//////////////////////////////////////////////////////////////////
//  Stats Management functions
//////////////////////////////////////////////////////////////////

/* simple stats management */
typedef struct {
    double  renderTime;
    double  frameTime;
} FrameStats;

#define  MAX_FRAME_STATS  200
#define  MAX_PERIOD_MS    1500

typedef struct {
    double  firstTime;
    double  lastTime;
    double  frameTime;

    int         firstFrame;
    int         numFrames;
    FrameStats  frames[ MAX_FRAME_STATS ];
} Stats;

static void
stats_init( Stats*  s )
{
    s->lastTime = now_ms();
    s->firstTime = 0.;
    s->firstFrame = 0;
    s->numFrames  = 0;
}

static void
stats_startFrame( Stats*  s )
{
    s->frameTime = now_ms();
}

static void
stats_endFrame( Stats*  s )
{
    double now = now_ms();
    double renderTime = now - s->frameTime;
    double frameTime  = now - s->lastTime;
    int nn;

    if (now - s->firstTime >= MAX_PERIOD_MS) {
        if (s->numFrames > 0) {
            double minRender, maxRender, avgRender;
            double minFrame, maxFrame, avgFrame;
            int count;

            nn = s->firstFrame;
            minRender = maxRender = avgRender = s->frames[nn].renderTime;
            minFrame  = maxFrame  = avgFrame  = s->frames[nn].frameTime;
            for (count = s->numFrames; count > 0; count-- ) {
                nn += 1;
                if (nn >= MAX_FRAME_STATS)
                    nn -= MAX_FRAME_STATS;
                double render = s->frames[nn].renderTime;
                if (render < minRender) minRender = render;
                if (render > maxRender) maxRender = render;
                double frame = s->frames[nn].frameTime;
                if (frame < minFrame) minFrame = frame;
                if (frame > maxFrame) maxFrame = frame;
                avgRender += render;
                avgFrame  += frame;
            }
            avgRender /= s->numFrames;
            avgFrame  /= s->numFrames;

            LOGI("frame/s (avg,min,max) = (%.1f,%.1f,%.1f) "
                 "render time ms (avg,min,max) = (%.1f,%.1f,%.1f)\n",
                 1000./avgFrame, 1000./maxFrame, 1000./minFrame,
                 avgRender, minRender, maxRender);
        }
        s->numFrames  = 0;
        s->firstFrame = 0;
        s->firstTime  = now;
    }

    nn = s->firstFrame + s->numFrames;
    if (nn >= MAX_FRAME_STATS)
        nn -= MAX_FRAME_STATS;

    s->frames[nn].renderTime = renderTime;
    s->frames[nn].frameTime  = frameTime;

    if (s->numFrames < MAX_FRAME_STATS) {
        s->numFrames += 1;
    } else {
        s->firstFrame += 1;
        if (s->firstFrame >= MAX_FRAME_STATS)
            s->firstFrame -= MAX_FRAME_STATS;
    }

    s->lastTime = now;
}

/////////////////////////////////////////////////////////////////////
// Bresenham Draw Functions
/////////////////////////////////////////////////////////////////////
static int BresenhamWidth = 320;
static int BresenhamHeight = 480;

void setBresenhamWidthHeight(int x, int y)
{
  BresenhamWidth = x;
  BresenhamHeight = y;
}

void csetpixel(int x, int y, Pixel* data, Pixel point){
     
     data[(y*BresenhamWidth)+x] = point;
     //LOGE(" setpixel w=%d data = %X  x=%d, y=%d val=%x",w, data, x,y, point);
}


void drawLine(int x0, int y0, int x1, int y1, Pixel* data, Pixel pixel)
{
    
    int dy = y1 - y0;
    int dx = x1 - x0;
    int stepx, stepy;

    if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
    if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
    dy <<= 1;                                                  // dy is now 2*dy
    dx <<= 1;                                                  // dx is now 2*dx

    
    csetpixel(x0, y0, data, pixel);
    if (dx > dy) {
        int fraction = dy - (dx >> 1);                         // same as 2*dy - dx
        while (x0 != x1) {
            if (fraction >= 0) {
                y0 += stepy;
                fraction -= dx;                                // same as fraction -= 2*dx
            }
            x0 += stepx;
            fraction += dy;                                    // same as fraction -= 2*dy
            
            csetpixel(x0, y0, data, pixel);
        }
    } else {
        int fraction = dx - (dy >> 1);
        while (y0 != y1) {
            if (fraction >= 0) {
                x0 += stepx;
                fraction -= dy;
            }
            y0 += stepy;
            fraction += dx;
            
            csetpixel(x0, y0, data, pixel);
        }
    }
}



////////////////////////////////
// Circle drawing algorithm 
////////////////////////////////
// Using the symmetry of Circle to draw points in other octants 

void drawCircleOctants(int xc, int yc, int x, int y, Pixel* data, Pixel point) {
    csetpixel(xc+x, yc+y, data, point);      // Ist      Octant
    csetpixel(xc+y, yc+x, data, point);      // IInd     Octant
    csetpixel(xc-y, yc+x, data, point);      // IIIrd    Octant
    csetpixel(xc-x, yc+y, data, point);      // IVth     Octant
    csetpixel(xc-x, yc-y, data, point);      // Vth      Octant
    csetpixel(xc-y, yc-x, data, point);      // VIth     Octant
    csetpixel(xc+y, yc-x, data, point);      // VIIth    Octant
    csetpixel(xc+x, yc-y, data, point);      // VIIIth   Octant
}

void circleMidpoint(int xc, int yc, int r, Pixel* data, Pixel point) {
    int p = 1 - r;
    int x = 0,       y = r;
    do{
          drawCircleOctants(xc, yc, x, y, data, point);
          ++x;
          if (p < 0)
             p += 1 + (x<<1);
          else{
               --y;
               p += 1 - ((y-x) << 1) ;
          }
          //wait();
    }while(y >= x);
}

// Using the symmetry of Ellipse to draw points in other octants 
inline void drawElipseOctants(int xc, int yc, int x, int y, Pixel* data, Pixel point) {
    csetpixel(xc+x, yc+y, data, point);      // Ist      Quadrant
    csetpixel(xc-x, yc+y, data, point);      // IInd     Quadrant
    csetpixel(xc-x, yc-y, data, point);      // IIIrd    Quadrant
    csetpixel(xc+x, yc-y, data, point);      // IVth     Quadrant
}

// Ellipse drawing algorithm 
void ellipseMidpoint(int xc, int yc, int rx, int ry, Pixel* data, Pixel point) {
    long long int rx_2 = rx*rx,   ry_2 = ry*ry;
    long long int p = ry_2 - rx_2*ry + (ry_2>>2);
    int x = 0,       y = ry;
    long long int two_ry_2_x = 0, two_rx_2_y = (rx_2<<1)*y;
    drawElipseOctants(xc, yc, x, y, data, point);
    while(two_rx_2_y >= two_ry_2_x){
          ++x;
          two_ry_2_x += (ry_2<<1);
         
          p +=  two_ry_2_x + ry_2;
            
          if(p >= 0){
               --y;
               two_rx_2_y -= (rx_2<<1);
              
               p -= two_rx_2_y ;
          }
          //wait();
          drawElipseOctants(xc, yc, x, y, data, point);
    }
   
    p = (long long int)(ry_2*(x+1/2.0)*(x+1/2.0) + rx_2*(y-1)*(y-1) - rx_2*ry_2);
    while (y>=0) {
          p += rx_2;
          --y;
          two_rx_2_y -= (rx_2<<1);
          p -= two_rx_2_y;
         
          if(p <= 0) {
               ++x;
               two_ry_2_x += (ry_2<<1);
               p += two_ry_2_x;
          }
          //wait();
          drawElipseOctants(xc, yc, x, y, data, point);
    }
}


////////////////////////////////
// B Line drawing Test 
////////////////////////////////

static void lineTest( AndroidBitmapInfo*  info, void*  pixels, double  t )
{
			int _width = info->width;
 			int _height = info->height;
			int rnum = 0;
			int x = 0;
			int y = 0;
			int i = 0;
			
			uint16_t*  bmdata = (uint16_t*)pixels;
			uint16_t*  line = (uint16_t*)pixels;
			Pixel thepixel;
			Pixel bgcolor = rgb_calc(0, 0, 0);
			static float alphavalue = 0.9;
			float thepixelfp, bgcolorfp, fptemp1, fptemp2, fptemp3;			
			static int bInit=0;
			static int seed=0;
			static int xhold;
			static int cnt,cr,cb,cg;
			
			 
			if(!bInit)
			{	createalphapalette();
				bInit=1;
				w=_width;
				h=_height;
				xhold =_width-1;
				cnt = 0;
				cr=32;
				cg=63;
				cb=31;
			}
		
			//circleMidpoint(_width/2, _height/2, cnt, (Pixel*) pixels, 0);	
			//circleMidpoint(_width/2, _height/2, cnt+1, (Pixel*) pixels, 0);
			
			ellipseMidpoint(_width/2, _height/2, cnt-10, cnt+5, (Pixel*) pixels, 0);
					
			alphavalue-=0.025;
			if(alphavalue<0.1)
				alphavalue = 0.9;
			
			//LOGE(" palette sz=%d",sizeof(palette));
			
			// seed random number
			if(seed>3)
			{
				srand((unsigned int)time(NULL));
				seed = 0;
			}else
			{ seed++; }
			
		
		
			if(cnt++>31)
			{
				cnt=0;
				cr=32;
				cg=63;
				cb=31;
			} 
			
			cg--;cb--;cr--;
						 
			thepixel = getAlpha(bgcolor, rgb_calc(cr,cg,cb), alphavalue); 
			
			//if(xhold<1)xhold=_width-1;
			
			//drawLine(0, 0, xhold--, _height-1, (Pixel*) pixels, thepixel);
			//LOGE(" start = %X",pixels);
			
			csetpixel(5,_height-1, (Pixel*) pixels, thepixel);
			
			//circleMidpoint(_width/2, _height/2, cnt, (Pixel*) pixels, thepixel);
			//circleMidpoint(_width/2, _height/2, cnt+1, (Pixel*) pixels, thepixel);
			
			ellipseMidpoint(_width/2, _height/2, cnt+2, cnt+5, (Pixel*) pixels, thepixel);
			
			/*
			for(y=_height;y>0;y--)
			{
				for (x = 0; x < _width; x++)
				{
					// display
					if(x<200)
					{
						//thepixel = palette[128];
						//thepixelfp = (float) thepixel;
						//bgcolorfp = (float)bgcolor;
						//thepixel = (Pixel)(1-alphavalue)*bgcolorfp+(alphavalue*thepixelfp);
						bmdata[(y*_width)+x] = thepixel;
						//LOGE(" palette tst=%d",palette[x]);
					    
						
					
					}
				}
			}
			*/
	
}




/////////////////////////////////////////////////////////
// Vector Physics Structures and Routines
/////////////////////////////////////////////////////////
#define FIXED 1
#define NOT_FIXED 0

typedef struct {
    double    x;
    double    y;
} Vector;

typedef struct {
    Vector velocity;
    Vector acceleration;
    Vector position;
    Vector previous;
    int fixed;
    int id;
} Point;

#define CTYPE_VERTICAL 1
#define CTYPE_HORIZONTAL 2
#define CTYPE_DIAGONAL 3
#define CTYPE_NO_DRAW 4
#define CTYPE_OTHER 5

typedef struct {
    Point* p1;
    Point* p2;
    double target;
    int ctyp;
} Constraint;


typedef struct {
    int x;
    int y;
    int length;
    int height;
    int segs;
    int total_points;
    int total_constraints;
    Constraint* constraints;
    Point* points;
} basicStructure;



double distance(Vector v1, Vector v2);
Vector sub(Vector vout, Vector vin);
void isub(Vector* vout, Vector vin);
Vector add(Vector vout, Vector vin);
void iadd(Vector* vout, Vector vin);
Vector mul(Vector vout, double scalar);
void imul(Vector* vout, double scalar);
double length(Vector vin);
void zero(Vector *vin);
void display_constraint(Constraint *c1, Constraint c2, int id);

Vector newVector(double x, double y)
{
  Vector v;
  v.x = x;
  v.y = y;
  
  return v;
}

Point newPoint(Vector v, Vector a, Vector p, int fixed, int id)
{
  Point pt;
  pt.velocity.x = v.x;
  pt.velocity.y = v.y;
  
  pt.acceleration.x = a.x;
  pt.acceleration.y = a.y;
  
  pt.position.x = p.x;
  pt.position.y = p.y;
  
  pt.previous.x = p.x;
  pt.previous.y = p.y;
  
  pt.fixed = fixed;
  pt.id = id;
  
  return pt;
 
}


Constraint newConstraint(Point* point1, Point* point2, int typ)
{
  Constraint c;
  c.p1 = point1;
  c.p2 = point2;
  c.target = distance(point1->position, point2->position);
  c.ctyp = typ;
  return c;
}

void accelerate(Point *p, Vector v)
{
    iadd(&p->acceleration, v);
    //LOGE(" TEST accelerate x=%f y=%f",p->acceleration.x, p->acceleration.y); 
}

void simulate(Point *p1, Point *p2, double delta)
{
  Vector ma = mul(p1->acceleration, delta);
  iadd(&p1->velocity, ma);
  Vector mv = mul(p1->velocity, delta);
  iadd(&p1->position, mv);

}
/*
Constraint resolveConstraint(Constraint c)
{
  
  //LOGE("------------------- resolveConstraint ---------------------");
  
  Vector pos1 = c.p1.position;
  Vector pos2 = c.p2.position;
  double force = c.target - distance(pos1,pos2);
  Vector direction = sub(pos1, pos2);
  double poslength = length(direction);
  double factor = (poslength-c.target)/(poslength*2.1);
  Vector correction = mul(direction, factor);
  iadd(&pos1, correction);
  imul(&correction, -1.0);
  iadd(&pos2, correction);   
  
  c.p1.position = pos1;
  c.p2.position = pos2;
  
  return(c);
  
  //LOGE("-----------------------------------------------------------");
}
*/
#define PRINT_CONSTRAINT 0
Constraint resolveConstraint(Constraint *c)
{
  double mass1 = 1.6;
  double mass2 = 1.0;
  
  if(PRINT_CONSTRAINT)LOGE("------------------- resolveConstraint ---------------------");
  
  Vector pos1 =  c->p1->position;
  if(PRINT_CONSTRAINT)LOGE("Pos1  x=%f y=%f ",pos1.x, pos1.y);
  Vector pos2 =  c->p2->position;
  double force = c->target - distance(pos2,pos1);
  Vector direction = sub(pos2, pos1);
  if(PRINT_CONSTRAINT)LOGE("direction  x=%f y=%f ",direction.x, direction.y);
  double poslength = length(direction);
  if(PRINT_CONSTRAINT)LOGE(" poslength  %d  ",poslength);
  double factor = (poslength-(c->target))/(poslength*(mass1+mass2));
  if(PRINT_CONSTRAINT)LOGE(" factor  %d  Constraint Target %f",factor, c->target);
  Vector correction = mul(direction, factor);
  if(PRINT_CONSTRAINT)LOGE("Correction  x=%f y=%f ",correction.x, correction.y);
  iadd(&pos1, correction);
  imul(&correction, -1.0);
  iadd(&pos2, correction);   
  
  if(!c->p1->fixed)
    c->p1->position = pos1;
  if(!c->p2->fixed)
    c->p2->position = pos2;
  
  
  
  if(PRINT_CONSTRAINT)LOGE("Pos1  x=%f y=%f ",pos1.x, pos1.y);
  if(PRINT_CONSTRAINT)LOGE("Pos2  x=%f y=%f ",pos2.x, pos2.y);
  if(PRINT_CONSTRAINT)LOGE("Pos1  x=%f y=%f ",c->p1->position.x, c->p1->position.y);
  
  return(*c);
  
  if(PRINT_CONSTRAINT)LOGE("-----------------------------------------------------------");
}



#define _OUTPUT 0
void pointSimulate(Point *p, double delta)
{
  
  //x' = 2x - x* + a·Δt2
  //    x* = x
      
  //LOGE("------------------- pointSimulate ---------------------");
  //LOGE(" TEST pointSimulate val=%f %f delta=%f",p->position.x, p->position.y, delta);
  if(1)
  {     imul(&p->acceleration, delta*delta);
        
        
        if(_OUTPUT)LOGE(" TEST pointSimulate imul val=%f %f delta=%f",p->acceleration.x, p->acceleration.y, delta);
        
        Vector position = p->position;
        
        
        position = mul(position, 2.0);
        if(_OUTPUT)LOGE(" TEST pointSimulate mul 2.0 val=%f %f",position.x, position.y);      
        
        position = sub(position, p->previous);
        if(_OUTPUT)LOGE(" TEST pointSimulate sub val=%f %f",position.x, position.y);
        
        position = add(position, p->acceleration);
        if(_OUTPUT)LOGE(" TEST pointSimulate add val=%f %f",position.x, position.y);
        
          
        p->previous = p->position;
        p->position = position;
        if(_OUTPUT)LOGE(" TEST pointSimulate new val=%f %f",p->position.x, p->position.y);
        
        
        zero(&p->acceleration);
  }
  if(_OUTPUT)LOGE("-------------------------------------------------------");
  
}




void testStructs();

#define TOTAL_CONSTRAINTS 50
#define TOTAL_POINTS 50


/*
void testStructs()
{
  Vector t1, t2, t3, t4, t5, t6;
  Vector *pt1, *pt2;
  Point p1, p2, p3, p4;
  Point *pp1, *pp2;
  Constraint c1, c2;
  Constraint *cp1, cp2;
   
  p1 = newPoint(newVector(0,0),newVector(0,0),newVector(1,1), NOT_FIXED , 1);
  p2 = newPoint(newVector(0,0),newVector(0,0),newVector(2,2), NOT_FIXED , 2);
  p3 = newPoint(newVector(0,0),newVector(0,0),newVector(3,3), NOT_FIXED , 3);
  p4 = newPoint(newVector(0,0),newVector(0,0),newVector(5,5), NOT_FIXED , 4);
  
  LOGE(" TEST p2 val=%f %f",p2.position.x, p2.position.y);
  LOGE(" TEST p4 val=%f %f",p4.position.x, p4.position.y);  
  pp1 = &p2;
  pp2 = &p4;
  
  LOGE(" TEST *pp1 val=%f %f",pp1->position.x, pp1->position.y);
  LOGE(" TEST *pp2 val=%f %f",pp2->position.x, pp2->position.y);  
  
  
  c1 = newConstraint(p1, p2);
  c2 = newConstraint(p3, p2);
  
  // isub OK
  // 5-2
  //isub(&pp2->position, pp1->position);    
  //LOGE(" TEST isub val=%f %f",p4.position.x, p4.position.y);
  
  // sub OK
  // 5-2
  //p3.position = sub(pp2->position, pp1->position);    
  //LOGE(" TEST p3 sub val=%f %f",p3.position.x, p3.position.y);

  // point simulate
  LOGE(" TEST pointsim val=%f %f",p2.position.x, p2.position.y);
  pointSimulate(pp1, 2.0);
  LOGE(" TEST pointsim val=%f %f",p2.position.x, p2.position.y);
  
   

}
*/

void display_point(Point *p1, Point p2, int id)
{
  LOGE(" POINT %d-----------", id);  
  LOGE(" Point  pos p1(ID=%d x=%f y=%f) p2(ID=%d x=%f y=%f)", p1->id, p1->position.x, p1->position.y, p2.id, p2.position.x, p2.position.y);
  LOGE(" Point  pre p1(ID=%d x=%f y=%f) p2(ID=%d x=%f y=%f)", p1->id, p1->previous.x, p1->previous.y, p2.id, p2.previous.x, p2.previous.y);
  LOGE(" Point  vel p1(ID=%d x=%f y=%f) p2(ID=%d x=%f y=%f)", p1->id, p1->velocity.x, p1->velocity.y, p2.id, p2.velocity.x, p2.velocity.y);
  LOGE(" Point  acc p1(ID=%d x=%f y=%f) p2(ID=%d x=%f y=%f)", p1->id, p1->acceleration.x, p1->acceleration.y, p2.id, p2.acceleration.x, p2.acceleration.y);
  LOGE(" ----------------");
}

void display_constraint(Constraint *c1, Constraint c2, int id)
{
  LOGE(" CONSTRAINT %d   %d ----------------------------------", id, c1->ctyp);  
  display_point(c1->p1, (Point) *c2.p1, 1);
  display_point(c1->p2, (Point) *c2.p2, 2);
  LOGE(" ------------------------------------------------");
}

void display_points(basicStructure* bstruct, Point points[])
{
  
  int i;
  
  Point blank = newPoint(newVector(0,0),newVector(0,0),newVector(0,0), FIXED, 0);
  
  for(i=0;i<bstruct->total_points;i++)
  {
    display_point(&points[i], blank, 0);
    
  }

}

void display_constraints(basicStructure* bstruct, Constraint constraints[])
{
  
  int i;
  Point blank1 = newPoint(newVector(1,1),newVector(1,1),newVector(1,1), FIXED, 0);
  Point blank2 = newPoint(newVector(2,2),newVector(2,2),newVector(2,2), FIXED, 0);
  Constraint blank = newConstraint(&blank1, &blank2, CTYPE_OTHER);
  
  for(i=0;i<bstruct->total_constraints;i++)
  {
    display_constraint(&constraints[i], blank, i);
  }

}


///////////////////////////////////////////////
// vector support functions
///////////////////////////////////////////////
void isub(Vector* vout, Vector vin)
{
        vout->x -= vin.x;
        vout->y -= vin.y;
}

Vector sub(Vector vout, Vector vin)
{
        vout.x -= vin.x;
        vout.y -= vin.y;
        return vout;
}

void iadd(Vector* vout, Vector vin)
{
        vout->x += vin.x;
        vout->y += vin.y;
}

Vector add(Vector vout, Vector vin)
{
        vout.x += vin.x;
        vout.y += vin.y;
        return vout;
}

void imul(Vector* vout, double scalar)
{
        vout->x *= scalar;
        vout->y *= scalar;
}

Vector mul(Vector vout, double scalar)
{
        vout.x *= scalar;
        vout.y *= scalar;
        return vout;
}

void idiv(Vector* vout, double scalar)
{
        vout->x /= scalar;
        vout->y /= scalar;
}

Vector vdiv(Vector vout, double scalar)
{
        vout.x /= scalar;
        vout.y /= scalar;
        return vout;
}

Vector normalized(Vector vin)
{
  double x = vin.x;
  double y = vin.y;
  double length = sqrt(x*x + y*y);
  
  if(length > 0){
      vin.x /= length;
      vin.y /= length;
  }
  else{
      vin.x = 0;
      vin.y = 0;
  }

  return vin;
  
}

void normalize(Vector *vin)
{
  double x = vin->x;
  double y = vin->y;
  double length = sqrt(x*x + y*y);
  
  if(length > 0){
      vin->x /= length;
      vin->y /= length;
  }
    
}

double length(Vector vin)
{
  double x = vin.x;
  double y = vin.y;
  double length = sqrt(x*x + y*y);

  return length;
}

double distance(Vector v1, Vector v2)
{
  double x = v1.x - v2.x;
  double y = v1.y - v2.y;
  double dist = sqrt(x*x + y*y);

  return dist;
}

Vector copy(Vector* vin)
{
  Vector vout;
  vout.x = vin->x;
  vout.y = vin->y;
  
  return vout;
  
}

void zero(Vector *vin)
{
  vin->x = 0;
  vin->y = 0;

}

void validate(Vector* v1)
{
  
  
}

/////////////////////////////////////
// Dot Physics
/////////////////////////////////////
#define BMAP_W 320
#define BMAP_H 480
#define TOTAL_PARTICLES 100
#define ACTIVE 0
#define INACTIVE 1

static uint16_t bmap[BMAP_W*BMAP_H];

typedef struct {
  
  int total_particles;
  double speed_limit; //6.0 
  double bounce_damping;
  double factor; // controls particle congregation Particles:Factor 100:9.0  200:3.0 400:1.0
  double min_proximity; // How close particles to no effect                                       
  
} SystemSettings;  
  
static SystemSettings psysset; 
Point points[500];
Point *thepoints = points;
Vector lasthit;

static void draw_particles( AndroidBitmapInfo*  info, void*  pixels, int reset )
{
    
    int _width = info->width;
		int _height = info->height;
		int rnum = 0;
		int x = 0;
		int y = 0;
		double py = 0;
		double px = 0;
		int i,j,k;
    int w = BMAP_W;
    int h = BMAP_H;

    uint16_t*  bmdata = (uint16_t*)pixels;
		uint16_t*  line = (uint16_t*)pixels;
		Pixel thepixel;
		Pixel theTemppixel;
		Pixel bgcolor = rgb_calc(0, 0, 0);
		static float alphavalue = 0.9;
		float aval, avaldec;
		float thepixelfp, bgcolorfp, fptemp1, fptemp2, fptemp3;			
		static int bInit=0;
		static int seed=0;
		static Pixel linepalette[PAL_SIZE*2];
    int zoom = 1;
    double palindex;
    
    int totalmem = BMAP_W*BMAP_H;
    //uint16_t bmap[];
    //static uint16_t bmap[BMAP_W*BMAP_H];

    //void* bmaploc = malloc(totalmem*sizeof(uint16_t));

    static int bDir;
	
  	if(seed>3)
  	{
  		srand((unsigned int)time(NULL));
  		seed = 0;
  	}else
  	{ seed++; }
  
    
    // single pass
    if(!bInit)
		{   bInit = 1;
        bDir =1;
        
        // Set Bresenham W H
        setBresenhamWidthHeight(w, h);
        
        //create palette
        
        aval = 1.0;
				avaldec =aval/(PAL_SIZE);	
        		
				for(i=0;i<PAL_SIZE;i++)
				{   			
					thepixel = getAlpha(rgb_calc(31,63,31), rgb_calc(0,0,0), aval);	
					linepalette[i] = thepixel;
					LOGE(" palette Init val=%d %d %f",i,linepalette[i], aval);
					aval-=avaldec;
				}   
          
        aval = 1.0;
				avaldec =aval/(PAL_SIZE);	
        		
				for(i=0;i<PAL_SIZE;i++)
				{   			
					thepixel = getAlpha(rgb_calc(0,0,0), rgb_calc(31,0,0), aval);	
					linepalette[i+PAL_SIZE] = thepixel;
					LOGE(" palette Init val=%d %d %f",i+PAL_SIZE,linepalette[i+PAL_SIZE], aval);
					aval-=avaldec;
				}
                
        // init fade
        for(y = 0; y < h; y++)
        {
          for(x = 0; x < w; x++)
          { 
            palindex = (PAL_SIZE/w)*x;
            //LOGE(" palette val=%d %d",x,(int)palindex);
            bmap[x+(y*w)] = 0;//(int)palindex;
            
          }
        }
                
    }
    
    /*
    if(reset==1)
    {
    
      for(y = 0; y < h; y++)
      {
        for(x = 0; x < w; x++)
        { 
          
          bmap[x+(y*w)] = 0;
          
        }
      }
   } 
    */
    
    //clear wipe
    for(y = 0; y < h; y++)
    {
      for(x = 0; x < w; x++)
      { 
        palindex = bmap[x+(y*BMAP_W)];
        //palindex = (PAL_SIZE/w)*x;
        //LOGE(" palette val=%d %d",x,(int)palindex);
        if(palindex<256)palindex = (palindex)*.9;
        bmap[x+(y*w)] = palindex;//(int)palindex;
        
      }
    }
    
    // plot constraints and Points
    // points
    // CTYPE_VERTICAL 1
      // CTYPE_HORIZONTAL 2
      // CTYPE_DIAGONAL 3
      // CTYPE_NO_DRAW 4
      // CTYPE_OTHER 5
    
    j = psysset.total_particles;
    for(k=0;k<j;k++)
    {
      //if(points[k].fixed==INACTIVE)continue;
      //LOGE(" Points tot=%d cur=%d posx=%f posy=%f  ",bs->total_points, k, points[k].position.x, points[k].position.y);
      //circleMidpoint(points[k].position.x, points[k].position.y, 3, bmap, 250);
      x = points[k].position.x;
      y = points[k].position.y;
      palindex = 64; //length(points[k].velocity);
      if(palindex!=0){
          palindex = PAL_SIZE - ((1/palindex)*PAL_SIZE);
      }
      //bmap[x+(y*BMAP_W)]=palindex;
      if(points[k].fixed==INACTIVE)
      {  palindex=255;
         circleMidpoint(points[k].position.x, points[k].position.y, 4, bmap, palindex);
         palindex=256;
         circleMidpoint(points[k].previous.x, points[k].previous.y, 4, bmap, palindex);
      }else
      {  
         circleMidpoint(points[k].position.x, points[k].position.y, 4, bmap, palindex);
        
      }
      // multi size
      /*
      bmap[x+1+(y*BMAP_W)]=palindex;
      bmap[x-1+(y*BMAP_W)]=palindex;
      bmap[x+((y-1)*BMAP_W)]=palindex;
      bmap[x+((y+1)*BMAP_W)]=palindex;
      */
    }
       
   
    
    //bmap[(int)lasthit.x+((int)lasthit.y*w)] = linepalette[256];
    
    // draw data 
    for(y = 0; y < h; y++)
    {
      for(x = 0; x < w; x++)
      { 
        palindex = (0.8)*x;
        thepixel = linepalette[bmap[x+(y*w)]];
        //if(y==0)LOGE(" palette val=%d %d  %d  %d",x,bmap[x+(y*w)], (int)palindex, thepixel);
        //bmdata[(y*_width)+x] = thepixel;
        //thepixel = linepalette[128];
        bmdata[x+(y*(_width))] = thepixel;
      
        /*
        bmdata[x+1+(y*_width)]=thepixel;
        bmdata[x-1+(y*_width)]=thepixel;
        bmdata[x+((y-1)*_width)]=thepixel;
        bmdata[x+((y+1)*_width)]=thepixel;
        
        bmdata[x+2+(y*_width)]=thepixel;
        bmdata[x-2+(y*_width)]=thepixel;
        bmdata[x+((y-2)*_width)]=thepixel;
        bmdata[x+((y+2)*_width)]=thepixel;
        */
      }
    }
  
    


}


static void setup_particles( int total)
{ 
   int i;
   Vector v, a, p;
   int fixed = 0;
   double vx, vy;
   int px, py;
   int id;
      
   double initial_speed = 1.0;
   
    
   
   
   for(i=0;i<total;i++)
   {
        /* setup initial values */
        id = i;
        
        /* velocity */
        vx = ((double)rand()/(double)RAND_MAX) * initial_speed - initial_speed * 0.5;
        vy = ((double)rand()/(double)RAND_MAX) * initial_speed - initial_speed * 0.5;
        v = newVector(vx, vy);
        
        /* position */ 
        px = rand()%BMAP_W;
        py = rand()%BMAP_H;
        p = newVector(px, py);
        
        /* acceleration */
        a = newVector(0.0,0.0);
                
        points[i] = newPoint( v,  a,  p, fixed, id);
 
                
   }
   

}


static void step_particle(Point *point)
{
        double speed_limit = psysset.speed_limit; //6.0 
        double bounce_damping = psysset.bounce_damping;
        double speed;
        double h = BMAP_H;
        double w = BMAP_W;
        
        h=h-40;
        
        // validate acceleration
        validate(&point->acceleration);
        
        // calculate new velocity
        iadd(&point->velocity, point->acceleration); 
        
        // speed
        speed = length(point->velocity);
        if(speed > speed_limit){
                idiv(&point->velocity, speed/speed_limit);
        }
      
        // add velocity to position
        iadd(&point->position, point->velocity); 
         
        // reset acceleration
        zero(&point->acceleration);
      
        // border bounce
        if(point->position.x < 5){
            point->position.x = 5;
            point->velocity.x *= -bounce_damping;
        }
        else if(point->position.x > w-5){
            point->position.x = w-5;
            point->velocity.x *= -bounce_damping;
        }
      
        if(point->position.y < 5){
            point->position.y = 5;
            point->velocity.y *= -bounce_damping;
        }
        else if(point->position.y > h-5){
            point->position.y = h-5;
            point->velocity.y *= -bounce_damping;
        }
      
        
        
  

}

static void particle_simulation()
{
    int j, k;
    Point p1, p2;
    Vector v1;
    double vlen;
    
    
    double factor = psysset.factor; // controls particle congregation
    // 100:9.0  200:3.0 400:1.0
    
    double min_proximity = psysset.min_proximity;
    
    
    // n^2
           
    for(k=0;k<psysset.total_particles;k++)
    {
          p1 = points[k];
          if(p1.fixed==INACTIVE)continue;
          for(j=k+1;j<psysset.total_particles;j++)
          {
                p2 = points[j];
                if(p2.fixed==INACTIVE)continue;
                v1 = sub(p1.position, p2.position);
                vlen = length(v1);
                idiv(&v1, ((vlen*vlen*vlen)/factor));      
                if(vlen>min_proximity){
                  iadd(&points[j].acceleration, v1);
                  isub(&points[k].acceleration, v1);
                  //display_point(&points[k], points[k], k);
                }
            
          }
          
          step_particle(&points[k]);  
          //display_point(&points[k], points[k], k);
    }
    
    
    


}



static void particle_physics( AndroidBitmapInfo*  info, void*  pixels, double  t, int reset )
{
    static int bInit = 0;
       
    
    if(!bInit)
    {
      bInit = 1;
      setup_particles( psysset.total_particles);
      
            
    }
    
    if(reset==1)
    {
      bInit = 1;
      setup_particles( psysset.total_particles);
      //draw_particles(info, pixels,  1);
    }
    
    //LOGE("Physics pts=%d cons=%d",bstruct.total_points, bstruct.total_constraints);
    
    // run
    particle_simulation();
    // draw 
    draw_particles(info, pixels, 0);
    
}

/////////////////////////////////////
// Set Up Particle System
/////////////////////////////////////
JNIEXPORT void JNICALL Java_com_game_hanzi_wormsplat_GraphicsView_psystemsettings
(JNIEnv * env, jobject  obj, jint  total_particles, jdouble  speed_limit, jdouble bounce_damping, jdouble factor, jdouble min_proximity)
{
  /*
  psysset.total_particles = 10;
  psysset.speed_limit = 6.0;
  psysset.bounce_damping = 0.75;
  psysset.factor = 9.0;
  psysset.min_proximity = 20.0;
  */
  psysset.total_particles = total_particles;
  psysset.speed_limit = speed_limit;
  psysset.bounce_damping = bounce_damping;
  psysset.factor = factor;
  psysset.min_proximity = min_proximity;
}

/////////////////////////////////////
// UI Handler  and Proximity tests
/////////////////////////////////////
JNIEXPORT jint JNICALL Java_com_game_hanzi_wormsplat_GraphicsView_uihandler
(JNIEnv * env, jobject  obj, jint x, jint y, jint w, jint h, jint action)
{
  // check proximity
    int j, k;
    Point p1;
    Vector v1;
    double vlen;
    double dist;
    int hit;
    double cx, cy;
    
    
    v1.x = x;
    v1.y = y;
    // n^2
           
    hit = 0;
    //LOGE("================point x=%d  y=%d", v1.y, v1.x);
    //return hit;
    
    lasthit = v1;    
    LOGE("================point x=%d  y=%d cx=%d cy=%d", x, y, w, h);   
    for(k=0;k<psysset.total_particles;k++)
    {
          p1 = points[k];
          //if(action==1)LOGE("point=%d  fixed=%d", p1->id, p1->fixed);
          if(p1.fixed==INACTIVE)continue;      
          dist = distance(v1, p1.position);
          if(dist<30.0)
          {
            points[k].fixed=1;
            points[k].previous.x = x;
            points[k].previous.y = y;
            
            hit += 1;
            //LOGE("point=%d  %d  x=%d y=%d  dist=%f %d  %d", k, p1->id ,x, y, dist, INACTIVE, ACTIVE);
          }
    }
  
    return hit;
  
}


/////////////////////////////////////
// Main Entry / Render Routine
/////////////////////////////////////
JNIEXPORT void JNICALL Java_com_game_hanzi_wormsplat_GraphicsView_render(JNIEnv * env, jobject  obj, jobject bitmap,   jobject bitmap2, jlong  time_ms, jint type, jint w, jint h, jint action)
{
    AndroidBitmapInfo  infoA;
    AndroidBitmapInfo  infoB;
    
    void*              pixels;
    void*              backpixels;
    Pixel* src;
    Pixel* dst;
    int                ret;
    static Stats       stats;
    static int         init;


    

    // lock down bitmap A - one

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &infoA)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    if (infoA.format != ANDROID_BITMAP_FORMAT_RGB_565) {
        LOGE("Bitmap format is not RGB_565 !");
        return;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }
    
     // lock down bitmap B - two

    if ((ret = AndroidBitmap_getInfo(env, bitmap2, &infoB)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }

    if (infoB.format != ANDROID_BITMAP_FORMAT_RGB_565) {
        LOGE("Bitmap format is not RGB_565 !");
        return;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap2, &backpixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }
    
 
    if (!init) {
                        
        stats_init(&stats);
        init = 1;
    }

    stats_startFrame(&stats);

    
    switch(type)
    {	
      
      case PHYSICS_PARTICLE:
      particle_physics(&infoA, pixels, time_ms, action );
    		break;
    
    }
    

  // backcopy
     
  if(backpixels!=NULL)
  {
    dst = (Pixel*) pixels;
    src = backpixels;
    Pixel pixel1, pixel2;
    int i;
    
    for( i = 0; i<(w*h);i++)  
    {
        if(*dst<32)
        {
          
          *dst=*src;
        }
      
      //*dst=0xffff;
      src++;dst++;
    }
  }
  
    // release
    AndroidBitmap_unlockPixels(env, bitmap);
    AndroidBitmap_unlockPixels(env, bitmap2);

    stats_endFrame(&stats);
}

////////////////////////////////////////////////
// Call back Java functions STUB
////////////////////////////////////////////////
/*
  JNIEnv *_my_jnienv = 0;
  void set_jnienv(JNIEnv *env) { _my_jnienv = env; }
  JNIEnv *env *get_jnienv() { return _my_jnienv; }
/*
/*
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  JNIEnv* env;
  if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
      MYLOG("JNI_OnLoad(): GetEnv failed");
      return -1;
  }
  return JNI_VERSION_1_4;
}
*/
/*
int jni_cal_java(const char *s) {
    JNIEnv *env = get_jnienv();
    jclass cls = env->FindClass("com/example/plasma/GraphicsView/function");
    jmethodID mid = env->GetStaticMethodID(cls,
	       "load_texture",
	       "(Ljava/lang/String;)I");
    // there could be some exception handling happening here, but there isn't
    jint ret;
    jstring mystr = env->NewStringUTF(s);
    ret = env->CallStaticIntMethod(cls, mid, mystr);
    return ret;
}




*/
