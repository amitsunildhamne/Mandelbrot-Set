// Calculate and display the Mandelbrot set
// ECE4893/8893 final project, Fall 2011

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <vector> 
#include <GL/glut.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "complex.h"

using namespace std;

// Min and max complex plane values
Complex  minC(-2.0, -1.2);
Complex  maxC( 1.0, 1.8);

int      maxIt = 2000;     // Max iterations for the set computations

static int updateRate(50);
void drawMB(void);
void recompute();
void* Thread(void*);
void sqr_draw();
int activeThreads;
bool flag=false;
std::vector<Complex> v;
//v.push_back(maxC);
//v.push_back(minC);
int nThreads =16;
class Point
{
public:
  float  x;
  float  y;
  Point(): x(0.0f),y(0.0f){}
};
Point start,end;
bool sqr = false;
float dx,dy,disp,diff;
static int zoom= 1;
//pthread_mutex_t calcMutex;
pthread_mutex_t exitMutex;
pthread_mutex_t activeMutex;
pthread_cond_t allDoneCondition;
//stack< pair<Complex,Complex> > stk;
//3
void randomize();
int array[512*512];
Complex c[512][512];
float *r1,*r2,*r3;

 // Your OpenGL display code here
void recompute() 
  {
    minC = v.back();
    v.pop_back();
    maxC  = v.back();
    v.pop_back();
    flag = false;
   }
void display()
{
 static int pass;
 cout<<"Displaying pass "<<++pass<<endl;
 glClear(GL_COLOR_BUFFER_BIT);
 glLoadIdentity();
 gluLookAt(0.0,0.0, 30.0,0.0,0.0,0.0,0.0,10.0,0.0);
 glTranslatef(0,0,0);
 glScalef(1.0,1.0,0);
 glClear(GL_DEPTH_BUFFER_BIT);
 glEnable(GL_DEPTH_TEST); 
 drawMB();
 if (sqr==true)
   {
    sqr_draw();
    /*
    glBegin(GL_LINE_LOOP);
    glColor3f(1.0,1.0,1.0);
    glVertex2f(start.x,start.y);
    glVertex2f(start.x,end.y);
    glVertex2f(end.x,start.y);
    glVertex2f(end.x,end.y);
    glEnd();*/   
   }
 glutSwapBuffers();
}

void randomize()
{
 r1 =  new float[2000];
 r2 =  new float[2000];
 r3 = new float[2000];
 	srand(time(NULL));
 for(int i=0;i<2000;i++)
   {
    r1[i] = (rand()%2000);
    r2[i] = (rand()%2000);
    r3[i] = (rand()%2000);
   }
}
void drawMB()
{
/* if (flag==true)
{
recompute();
}*/
 /*Complex M[512][512];
 Complex div(1/512.0,1/512.0);
 Complex temp;
 for(int i=0;i<512;i++)
    {
     for(int j=0;j<512;j++)
	{
	  M[i][j].real = minC.real + ((maxC.real - minC.real)*i*div.real);
	  M[i][j].imag = minC.imag + ((maxC.imag - minC.imag)*j*div.imag);
	  temp  = M[i][j];
	  int itr = 0;
	  while(itr<2000)
		{
	 	  if(M[i][j].Mag2() >=4.0) break;
		  M[i][j] = (M[i][j]*M[i][j]) + temp; 
		 itr++;
	       }
	  array[i*512+j] = itr;
	}
    }*/ // Single Thread Execution


 pthread_mutex_init(&activeMutex,0);
 pthread_mutex_init(&exitMutex,0);
 pthread_cond_init(&allDoneCondition,0);
 //pthread_mutex_init(&calcMutex,0);
 pthread_mutex_lock(&exitMutex);
 activeThreads = nThreads;
 for(int i =0;i< nThreads; i++)
    {
     pthread_t t;
     pthread_create(&t, 0,Thread, (void*)i);
   }
 pthread_cond_wait(&allDoneCondition, &exitMutex);
 //pthread_mutex_unlock(&exitMutex);
 //for(int o=0;o<512*512;o++) cout<<array[o];
 glBegin(GL_POINTS);
 glLineWidth(2.0);
 for(int x =0;x<512;x++)
    {
     for(int y=0;y<512;y++)
	{
	 float r,g,b;
	 if (array[x*512+y]==2000) {r =0;g=0;b=0; }
	 else
	    {
	     r = r1[array[x*512+y]]/2000.0;
	     g = r2[array[x*512+y]]/2000.0;
	     b = r3[array[x*512+y]]/2000.0;
	    }
	 glColor3f(r,g,b);
	 glVertex2f(x,512-y);
	}
     }
  glEnd();
}
//if (sqr==true)
void sqr_draw()
   {
    glLineWidth(2.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBegin(GL_POLYGON);   
    //glBegin(GL_LINE_LOOP);
    glColor3f(0.6,0.6,0.6);
    glVertex2f(start.x,512-start.y);
    glVertex2f(end.x,512-start.y);
    glVertex2f(end.x,512-end.y);
    glVertex2f(start.x,512-end.y);
    glEnd();       		 	
}

void* Thread(void* v)
{
 unsigned long myId = (unsigned long)v;
 Complex var[512/nThreads][512];
 Complex div(1/512.0,1/512.0);
 Complex temp;
 //cout<<"Hello from thread "<<myId<<endl;
 //pthread_mutex_lock(&calcMutex);
 for(int i=0;i<32;i++)
    {
	//cout<<"Row number "<<i<<"My ID is "<<myId<<endl;
      for(int j=0;j<512;j++)
	{
	 // cout<<"Col No. "<<j<<"My ID is "<<myId<<endl;
          var[i][j].real = minC.real + ((maxC.real - minC.real)*(myId*32+i)*div.real);
          var[i][j].imag = minC.imag + ((maxC.imag - minC.imag)*j*div.imag);
          temp  = var[i][j];
	  c[myId*32+i][j] = temp;
          int itr = 0;
          while(itr<2000)

                {
                  if(var[i][j].Mag2() >=4.0) break;
                  var[i][j] = (var[i][j]*var[i][j]) + temp; 
                 itr++;
               }
        
        array[(myId*32+i)*512+j] = itr; 
	/*if (myId==1)
	{ cout<<array[(myId*32+i)+j];}	 	 	 
	}*/
    } 
}
 // pthread_mutex_unlock(&calcMutex);
 pthread_mutex_lock(&activeMutex);
 activeThreads--;
 if(activeThreads==0) 
  {
   pthread_mutex_unlock(&activeMutex);
   pthread_mutex_lock(&exitMutex);
   pthread_cond_signal(&allDoneCondition);
   pthread_mutex_unlock(&exitMutex);
  }
 else
 pthread_mutex_unlock(&activeMutex);
}

void init() //2
{ // Your OpenGL initialization code here
  glClearColor(0.0,0.0,0.0,0.0);
  glShadeModel(GL_FLAT);
}

void reshape(int w, int h)
{ // Your OpenGL window reshape code here
  glViewport(0,0,(GLsizei)w,(GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //glFrustum(0.0, (GLdouble)w, (GLdouble)0.0, h, (GLdouble)1, (GLdouble)40);
  glOrtho(0.0, (GLdouble)w,(GLdouble)0.0,h,(GLdouble)-w,(GLdouble)w);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/*void timer(int)
{
 glutPostRedisplay();
 glutTimerFunc(1000.0/updateRate, timer, 0);
}*/

void mouse(int button, int state, int x, int y)
{ // Your mouse click processing here
  // state == 0 means pressed, state != 0 means released
  // Note that the x and y coordinates passed in are in
  // PIXELS, with y = 0 at the top.
 
  if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
    {
     start.x = x; 
     end.x = x;
     start.y = y;
     end.y = y;
     sqr =  true;
       v.push_back(maxC);
      v.push_back(minC);

   }
  
  if(button == GLUT_LEFT_BUTTON && state==GLUT_UP)
    {
      
      //int i,j;
      dx = abs(x - start.x);
      dy = abs(y - start.y );
      if (dx>=dy) diff = dy;
      else if (dy>dx)  diff = dx;
      if(x>start.x && y>start.y)
	{
	 end.x = start.x + diff;
         end.y = start.y + diff;
	// minC = c[start.x][start.y];
	
	 minC = c[(int)start.x][(int)end.y];
         maxC = c[(int)end.x][(int)start.y]; 
	}  
     else if(x<start.x && y>start.y)
      {
	end.x = start.x - diff;
	end.y = start.y + diff;
        maxC = c[(int)start.x][(int)start.y];
	minC = c[(int)end.x][(int)end.y];
      }
     else if(x>start.x&& y<start.y)
	{
	 end.x = start.x + diff;
	 end.y = start.y - diff;
	 minC = c[(int)start.x][(int)start.y];
	 maxC = c[(int)end.x][(int)end.y];
	}
     else if(x<start.x && y<start.y)
	{
	 end.x = start.x - diff;
	 end.y = start.y - diff;
	 minC = c[(int)end.x][(int)start.y];
	 maxC = c[(int)start.x][(int)end.y];
	}
  //    v.push_back(maxC);
//      v.push_back(minC);
      drawMB();
      sqr = false;
      cout<<"zoom level "<<zoom++<<endl;
     glutPostRedisplay();
     // display();
    }
 //sqr = false;
}

void motion(int x, int y)
{ // Your mouse motion here, x and y coordinates are as above
  dx = abs(x - start.x);
  dy = abs(y - start.y );
  if (dx>dy) disp = dy;
  else if (dy>dx)  disp = dx;
  if(x>start.x && y>start.y)
        {
         end.x = start.x + disp;
         end.y = start.y + disp;
        }
     else if(x<start.x && y>start.y)
      {
        end.x = start.x - disp;
        end.y = start.y + disp;
      //  maxC = c[start.x][end.y];
       // minC = c[end.x][start.y];
      }
     else if(x>start.x && y<start.y)
        {
         end.x = start.x + disp;
         end.y = start.y - disp;
       //  minC = c[start.x][end.y];
       //  maxC = c[end.x][start.y];
        }
     else if(x<start.x && y<start.y)
        {
         end.x = start.x - disp;
         end.y = start.y - disp;
       //  minC = [end.x][end.y];
       //  maxC = [start.x][start.y];
        }
 glutPostRedisplay();
  
}

void keyboard(unsigned char c, int x, int y)
{ // Your keyboard processing here
 if (c== 66 || c==98)
   {
     flag = true; 
     recompute();          
   }
 //display();
 //drawMB();
 glutPostRedisplay();
}

int main(int argc, char** argv) //1
{
  // Initialize OpenGL, but only on the "master" thread or process.
  // See the assignment writeup to determine which is "master" 
  // and which is slave.
 glutInit(&argc,argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); //| GLUT_DEPTH);
 glutInitWindowSize(512,512);
 glutInitWindowPosition(100,100);
 glutCreateWindow("MBSet");
 init();
 randomize();
 v.push_back(maxC);
 v.push_back(minC);
 glutDisplayFunc(display);
 glutReshapeFunc(reshape);
 //glutTimerFunc(1000.0/updateRate, timer,0);
 glutMouseFunc(mouse);
 glutMotionFunc(motion);
 glutKeyboardFunc(keyboard);
 glutMainLoop(); 
  return 0;
}

