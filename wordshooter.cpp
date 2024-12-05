//============================================================================
// Name        : cookie-crush.cpp
// Author      : Sibt ul Hussain
// Version     :
// Copyright   : (c) Reserved
// Description : Basic 2D game of Cookie  Crush...
//============================================================================
#ifndef WORD_SHOOTER_CPP
#define WORD_SHOOTER_CPP

//#include <GL/gl.h>
//#include <GL/glut.h>
#include <iostream>
#include<string>
#include<cmath>
#include<fstream>
#include<ctime>
#include "util.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
using namespace std;
#define MAX(A,B) ((A) > (B) ? (A):(B)) // defining single line functions....
#define MIN(A,B) ((A) < (B) ? (A):(B))
#define ABS(A) ((A) < (0) ? -(A):(A))
#define FPS 20

string * dictionary;
int dictionarysize = 369646;
#define KEY_ESC 27 // A

// 20,30,30
const int bradius = 30; // ball radius in pixels...

int width = 930, height = 660;
int byoffset = bradius;

int nxcells = (width - bradius) / (2 * bradius);
int nycells = (height - byoffset /*- bradius*/) / (2 * bradius);
int nfrows = 2; // initially number of full rows //
float score = 0;
float timer=150;
int **board; // 2D-arrays for holding the data...
int bwidth = 130;
int bheight = 10;
int bsx, bsy;
const int nalphabets = 26;
enum alphabets {
	AL_A, AL_B, AL_C, AL_D, AL_E, AL_F, AL_G, AL_H, AL_I, AL_J, AL_K, AL_L, AL_M, AL_N, AL_O, AL_P, AL_Q, AL_R, AL_S, AL_T, AL_U, AL_W, AL_X, AL_y, AL_Z
};
GLuint texture[nalphabets];
GLuint tid[nalphabets];
string tnames[] = { "a.bmp", "b.bmp", "c.bmp", "d.bmp", "e.bmp", "f.bmp", "g.bmp", "h.bmp", "i.bmp", "j.bmp",
"k.bmp", "l.bmp", "m.bmp", "n.bmp", "o.bmp", "p.bmp", "q.bmp", "r.bmp", "s.bmp", "t.bmp", "u.bmp", "v.bmp", "w.bmp",
"x.bmp", "y.bmp", "z.bmp" };
GLuint mtid[nalphabets];
int awidth = 60, aheight = 60; // 60x60 pixels cookies...


//USED THIS CODE FOR WRITING THE IMAGES TO .bin FILE
void RegisterTextures_Write()
//Function is used to load the textures from the
// files and display
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);
	vector<unsigned char> data;
	ofstream ofile("image-data.bin", ios::binary | ios::out);
	// now load each cookies data...

	for (int i = 0; i < nalphabets; ++i) {

		// Read current cookie

		ReadImage(tnames[i], data);
		if (i == 0) {
			int length = data.size();
			ofile.write((char*)&length, sizeof(int));
		}
		ofile.write((char*)&data[0], sizeof(char) * data.size());

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ofile.close();

}
void RegisterTextures()
/*Function is used to load the textures from the
* files and display*/
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);

	vector<unsigned char> data;
	ifstream ifile("image-data.bin", ios::binary | ios::in);

	if (!ifile) {
		cout << " Couldn't Read the Image Data file ";
		//exit(-1);
	}
	// now load each cookies data...
	int length;
	ifile.read((char*)&length, sizeof(int));
	data.resize(length, 0);
	for (int i = 0; i < nalphabets; ++i) {
		// Read current cookie
		//ReadImage(tnames[i], data);
		/*if (i == 0) {
		int length = data.size();
		ofile.write((char*) &length, sizeof(int));
		}*/
		ifile.read((char*)&data[0], sizeof(char)* length);

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ifile.close();
}
void DrawAlphabet(const alphabets &cname, int sx, int sy, int cwidth = 60,
	int cheight = 60)
	/*Draws a specfic cookie at given position coordinate
	* sx = position of x-axis from left-bottom
	* sy = position of y-axis from left-bottom
	* cwidth= width of displayed cookie in pixels
	* cheight= height of displayed cookiei pixels.
	* */
{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mtid[cname]);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}
int GetAlphabet() {
	return GetRandInRange(1, 26);
	
}

void Pixels2Cell(int px, int py, int & cx, int &cy) {
}
void Cell2Pixels(int cx, int cy, int & px, int &py)
// converts the cell coordinates to pixel coordinates...
{
}
void DrawShooter(int sx, int sy, int cwidth = 60, int cheight = 60)

{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, -1);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}
/*
* Main Canvas drawing function.
* */

float X1,Y1;
float dx,dy;
bool limit=0;
int click=0;
float xstep,ystep;
float length;
float speed=20;
int ball[4][15]={0};
char cball[7][15];
int ballnum[90];
float ballx[50] = {465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465, 465};

float bally[50] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};

int ballux[50];
int balluy[50];
bool startball=false,doclick=true,storealphabets=true,tb1=true,endpgexit=false,writeinfile=false;
int bindex=0;
int columnx[15]={10,70,130,190,250,310,370,430,490,550,610,670,730,790,850};
int foundwordcount=1;


void DisplayFunction() {
	// set the background color using function glClearColor.
	// to change the background play with the red, green and blue values below.
	// Note that r, g and b values must be in the range [0,1] where 0 means dim red and 1 means pure red and so on.
	//#if 0
	glClearColor(1/*Red Component*/, 1/*Green Component*/,
		1/*Blue Component*/, 0 /*Alpha component*/); // Red==Green==Blue==1 --> White Colour
	glClear(GL_COLOR_BUFFER_BIT); //Update the colors

	//write your drawing commands here or call your drawing functions...
	if(click>0){   
if(timer>0){	
	
	static int num0=GetAlphabet();
	srand(num0);
	
	for(int i=0 ; i<90 ; i++)
	{
          ballnum[i]=GetAlphabet();   // calling GetAphabet function to generate random numbers for drawing different balls.
        }
        
	
    
	
	int k=15;
	
	int x=10;
	for(int i=0 ; i<2 ; i++)    // this loop prints the first 15 alphabets on the screen . 1st row.
	{
	  if(i==0){
	   for(int j=0 ; j<15 ; j++)
	   {
	   if(tb1){
	    ballux[j]=x;
	    balluy[j]=height/1.2;}
             DrawAlphabet((alphabets)ballnum[j], ballux[j], balluy[j], awidth, aheight);
             x+=60;
	     ball[i][j]=ballnum[j];
	  
	   }
	   }
           if(i==1){
           x=10;
	   for(int j=0 ; j<15 ; j++)    // this loop prints the other 15 alphabets on the screen . 2nd row.
	   {
	    if(tb1){
	     ballux[k]=x;
	     balluy[k]=height/1.35;}
             DrawAlphabet((alphabets)ballnum[k], ballux[k], balluy[k], awidth, aheight);
             x+=60;
	     ball[i][j]=ballnum[k];
	    k++;
	   }
	   }
	}
     tb1=false;
      
      
      
      
      
      if(storealphabets){
	for(int i=0 ; i<7 ; i++)
	{
	   if(i==0)
	   for(int j=0 ; j<15 ; j++)
	   {
	     cball[i][j] = char(ball[i][j] + 97);
	   }
	   if(i==1)
	    for(int j=0 ; j<15 ; j++)
	   {
	     cball[i][j] = char(ball[i][j] + 97);
	   }
	   if(i==2 || i==3 || i==4 || i==5 || i==6 || i==7)
	    for(int j=0 ; j<15 ; j++)
	   {
	     cball[i][j] = ' ';
	   }
        }
 

  
//-------------------------------------------------------------
  //  these loops check the formed words in the start of the game for only two top rows and only run once.
         for (int row=0; row<3; ++row) 
         {
            string word = "";
          for (int col=0; col<15; ++col) 
          {
             if (cball[row][col] != ' ')
          {
          word=word+cball[row][col];
           for (int i=0; i<dictionarysize; ++i) 
           {
             if (dictionary[i] == word)
           {
          cout <<"Word Found (in row): "<<word<<endl;
         score+=word.size();
         {
           ofstream f_write;
           f_write.open("Matchedwords.txt" , ios::out|ios::app);
           f_write<<"Word "<<foundwordcount++<<": "<<word<<endl;
           f_write.close();
         }
            for (int k=col-word.size()+1; k<=col; ++k) 
            {
              cball[row][k] = ' ';
              ballux[row * 15 + k] = -1000; 
              balluy[row * 15 + k] = -1000; 
             }
          break;
          }
          }
          } else {
            word = ""; 
         }
        }
     }

    
           for (int col=0; col<15; ++col) 
           {
             string word = "";
           for (int row=0; row<3; ++row) 
           {
             if (cball[row][col] != ' ') 
           {
                word=word+cball[row][col];
            for (int i=0; i<dictionarysize; ++i) 
           {
                if (dictionary[i] == word) {
                cout <<"Word Found (in col): "<<word<<endl;
                score=score+word.size(); 
                {
           ofstream f_write;
           f_write.open("Matchedwords.txt" , ios::out|ios::app);
           f_write<<"Word "<<foundwordcount++<<": "<<word<<endl;
           f_write.close();
               }
             for (int k=row - word.size() +1; k<=row; ++k)
                {
                  cball[k][col] = ' '; 
                  ballux[k * 15 + col] = -1000; 
                  balluy[k * 15 + col] = -1000; 
                }
             break;
            }
            }
            } else {
              word = ""; 
            }
           }
           }

    
             for (int start_col=0; start_col<15; ++start_col)
             {
                 string word = "";
              for (int row=0, col=start_col; row<3 && col<15; ++row, ++col) 
             {
                 if (cball[row][col] != ' ') 
             {
                 word=word+cball[row][col];
              for (int i=0; i<dictionarysize; ++i) 
             {
                if (dictionary[i] == word)
               {
                cout <<"Word Found (in diagonal): "<<word<<endl;
                score += word.size(); 
                {
           ofstream f_write;
           f_write.open("Matchedwords.txt" , ios::out|ios::app);
           f_write<<"Word "<<foundwordcount++<<": "<<word<<endl;
           f_write.close();
         }
                for (int k=0; k<word.size(); ++k)
               {
                 cball[row - k][col - k] = ' '; 
                 int index = (row - k) * 15 + (col - k);
                 ballux[index] = -1000; 
                 balluy[index] = -1000;
                }
                 break;
                }
                 }
                } else {
                word = ""; 
               }
                }
             }
        
    //----------------------------------------------------------    
        
  }      
        
        
	
storealphabets=false;

//  for drawing shooter balls when recent balls reach a certain height.
	        DrawAlphabet((alphabets)ballnum[30 ], ballx[0], bally[0], awidth, aheight);
	for (int i = 0; i < 50; i++) {
    if (bally[i] > 425) {
        DrawAlphabet((alphabets)ballnum[30 + i], ballx[i + 1], bally[i + 1], awidth, aheight);
    }
}
	
	
	
	
	
	
	
	
	
	
	for(int i=0 ; i<7 ; i++)    // this loop print the character array means the array containg all the alphabets on the terminal.
	{
	   if(i==0)
	   for(int j=0 ; j<15 ; j++)
	   {
	     cout<<cball[i][j]<<" ";
	   }
	   if(i==1)
	    for(int j=0 ; j<15 ; j++)
	   { 
             cout<<cball[i][j]<<" ";
	   }
	  if(i==2 || i==3 || i==4 || i==5 || i==6 || i==7)
	   for(int j=0 ; j<15 ; j++)
	   {
	     cout<<cball[i][j]<<" ";
	   }
	  
	   cout<<endl;
	   if(i==7)
	   cout<<endl;
        }
	
      }
	
	if(timer<=0)  // for first page
	{               
			DrawAlphabet((alphabets)6, 250, height-300, awidth, aheight);
	                DrawAlphabet((alphabets)0, 310, height -300, awidth, aheight);
	                DrawAlphabet((alphabets)12, 370, height -300, awidth, aheight);
	                DrawAlphabet((alphabets)4, 430, height -300, awidth, aheight);
	                DrawAlphabet((alphabets)14, 490, height-300, awidth, aheight);
	                DrawAlphabet((alphabets)21, 550, height -300, awidth, aheight);
	                DrawAlphabet((alphabets)4, 610, height -300, awidth, aheight);
	                DrawAlphabet((alphabets)17, 670, height -300, awidth, aheight);
			DrawString(425, 310, width, height , "Total SCORE : " + Num2Str(score), colors[BLUE_VIOLET]);
			if(endpgexit)
			exit(1);
			
	  }
	  }
	  if(click==0) // for end page
	  { 
		DrawAlphabet((alphabets)0, 400, 500, awidth, aheight);
		DrawAlphabet((alphabets)1, 460, 500, awidth/2, aheight/2);
		DrawAlphabet((alphabets)22, 110, 400, awidth, aheight);
		DrawAlphabet((alphabets)14, 170, 400, awidth, aheight);
		DrawAlphabet((alphabets)17, 230, 400, awidth, aheight);
		DrawAlphabet((alphabets)3, 290, 400, awidth, aheight);
		DrawAlphabet((alphabets)18, 350, 400, awidth, aheight);
		DrawAlphabet((alphabets)7, 410, 400, awidth, aheight);
		DrawAlphabet((alphabets)14, 470, 400, awidth, aheight);
		DrawAlphabet((alphabets)14, 530, 400, awidth, aheight);
		DrawAlphabet((alphabets)19, 590, 400, awidth, aheight);
		DrawAlphabet((alphabets)4, 650, 400, awidth, aheight);
		DrawAlphabet((alphabets)17, 710, 400, awidth, aheight);
		DrawString(300, 300, width, height , "CLICK ANYWHERE TO BEGIN", colors[HOT_PINK]);
	  }
	
	if(timer>0 && click>0){
	
	DrawString(40, height - 20, width, height + 5, "Score " + Num2Str(score), colors[BLUE_VIOLET]);
	DrawString(width / 2.3 - 30, height - 25, width, height,
		"Time Left:" + Num2Str(timer) + " secs", colors[RED]);
	DrawString(650, height - 20, width, height + 5, "Abdul Samad_24I-0095", colors[BLUE_VIOLET]);	
		}

	// #----------------- Write your code till here ----------------------------#
	//DO NOT MODIFY THESE LINES
	DrawShooter((width / 2) - 35, 0, bwidth, bheight);
	glutSwapBuffers();
	//DO NOT MODIFY THESE LINES..
	
}

/* Function sets canvas size (drawing area) in pixels...
*  that is what dimensions (x and y) your game will have
*  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
* */
void SetCanvasSize(int width, int height) {
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.*/
}

/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
* is pressed from the keyboard
*
* You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
*
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
*
* */

void NonPrintableKeys(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT /*GLUT_KEY_LEFT is constant and contains ASCII for left arrow key*/) {
		// what to do when left key is pressed...

	}
	else if (key == GLUT_KEY_RIGHT /*GLUT_KEY_RIGHT is constant and contains ASCII for right arrow key*/) {

	}
	else if (key == GLUT_KEY_UP/*GLUT_KEY_UP is constant and contains ASCII for up arrow key*/) {
	}
	else if (key == GLUT_KEY_DOWN/*GLUT_KEY_DOWN is constant and contains ASCII for down arrow key*/) {
	}

	/* This function calls the Display function to redo the drawing. Whenever you need to redraw just call
	* this function*/
	/*
	glutPostRedisplay();
	*/
}
/*This function is called (automatically) whenever your mouse moves within inside the game window
*
* You will have to add the necessary code here for finding the direction of shooting
*
* This function has two arguments: x & y that tells the coordinate of current position of move mouse
*
* */

void MouseMoved(int x, int y) {
	//If mouse pressed then check than swap the balls and if after swaping balls dont brust then reswap the balls

}

/*This function is called (automatically) whenever your mouse button is clicked witin inside the game window
*
* You will have to add the necessary code here for shooting, etc.
*
* This function has four arguments: button (Left, Middle or Right), state (button is pressed or released),
* x & y that tells the coordinate of current position of move mouse
*
* */

void MouseClicked(int button, int state, int x, int y) {

if(doclick){
	if (button == GLUT_LEFT_BUTTON) // dealing only with left button
	{   
		if (state == GLUT_UP)
		{
		
		X1=x;
                Y1=660-y;// as the origin of the mouse is the upper left corner so we have to subtract the height of mouse from total height to find the height of mouse fromball.
                dy=(Y1-bally[bindex]);   // calculates the difference of hight .
                dx=(X1-ballx[bindex]);   // calculates the difference of hight .
                int w2,h2;
                w2=dx*dx;         // this calculates the square of the width means the x coordinate.
                h2=dy*dy;          // this calculates the square of the height means the y coordinate.
                length=sqrt(w2+h2);   // this takes square root to calculate the total distance of the mouse from the ball.
                xstep=(dx/length)*speed;   // now we finally got the length with which we increment the initial position to reach at that point in given distance.
                ystep=(dy/length)*speed;    // now we finally got the length with which we increment the initial position to reach at that point in given distance.
              
                click+=1;
                startball=true;
                limit=0;
                if(timer<=0)   
                {
                 if(click++)  // this condition displays ending page when the time is 0 or less then 0;
                 endpgexit=true;
                }
                
                }
	}
	
}	
else 
            
                 if (button == GLUT_RIGHT_BUTTON) // dealing with right button
	         exit(1);
	glutPostRedisplay();
}
/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
* is pressed from the keyboard
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
* */
void PrintableKeys(unsigned char key, int x, int y) {
	if (key == KEY_ESC/* Escape key ASCII*/) {
		exit(1); // exit the program when escape key is pressed.
	}
}

/*
* This function is called after every 1000.0/FPS milliseconds
* (FPS is defined on in the beginning).
* You can use this function to animarandnumber()te objects and control the
* speed of different moving objects by varying the constant FPS.
*
* */
void Timer(int m) {

	glutPostRedisplay();
	glutTimerFunc(1000.0/FPS, Timer, 0);
if(click>=2){	               // checks if the click by the mouse is greater then 1 so that when user click on starting page so game doesnot  start.
	   if (startball) {
	   doclick=false;
           int targety=((height/1.35)-62.11);
	   int cindex=bindex;
	   if(bally[bindex]<targety){      // the loop runs until the appropriate height is reached.
      
                bally[bindex]+=ystep;      // increment the height to the shooter .
   
    if (limit == 0) {  
        ballx[bindex]+=xstep;              // increment the width to the shooter .
        if (ballx[bindex]>= 870 || ballx[bindex]<=0) {    //  checks for boundary collision
            limit = 1;  
        }
    }
    
    else {  
        ballx[bindex] -= xstep;              // increment the width to the shooter .
        if (ballx[bindex]<= 0 || ballx[bindex] >=870) {     //  checks for boundary collision
            limit = 0;   
        }
    }
}
else{
bindex++;
startball=false;
doclick=true;

    float nowX = ballx[bindex-1];        // Get the x-coordinate of the ball at this instant.
    float nearColX = columnx[0];
    float minDif = (nowX - columnx[0]);
    if(minDif<0)
    minDif=-minDif;

   
    for (int i = 1; i < 15; i++) {          // Find the column position which is most closest to it.
        float dif = (nowX  - columnx[i]);
        if(dif<0)
        dif=-dif;
        if (dif < minDif) {
            minDif = dif;
            nearColX = columnx[i];
        }
    }

    
    if (minDif != 0) {             // put the ball's x-coordinate with the column closest to it.
        if (nowX < nearColX) {
            ballx[bindex-1] += (nearColX - nowX); // if the ball is more toward left move it towards right.
        } else if (nowX > nearColX) {
            ballx[bindex-1] -= (nowX - nearColX); // if the ball is more toward right move it towards left.
        }
    }
    
    if(bally[bindex-1]>((height/1.35)-62.21))
    bally[bindex-1]-=bally[bindex-1]-((height/1.35)-62.11);
    
    for(int i=0 ; i<1; i++)
    {
      for(int j=0 ; j<15 ; j++)
      {
        if(ballx[bindex-1]==10+(j*60))
        {
          if(bindex==1)
          cball[2][j]= char (ballnum[30+(bindex-1)]+97);       // stores the alphabet to a character array.
          if(bindex>1)
          cball[2][j]= char (ballnum[30+(bindex-2)]+97);       // stores the alphabet to a character array.
        }
      }
    }
//---------------------------------------------------------------------------------------------------------------------------------------------------------

    for (int row=0; row<3; ++row) 
         {
            string word = "";
          for (int col=0; col<15; ++col) 
          {
             if (cball[row][col] != ' ')
          {
          word=word+cball[row][col];
           for (int i=0; i<dictionarysize; ++i) 
           {
             if (dictionary[i] == word)
           {
          cout<<"Word Found (in row): "<<word<<endl;
         score=score+word.size();
         {
           ofstream f_write;
           f_write.open("Matchedwords.txt" , ios::out|ios::app);
           f_write<<"Word "<<foundwordcount++<<": "<<word<<endl;
           f_write.close();
         }
            for (int k=col-word.size()+1; k<=col; ++k) 
            {
              cball[row][k] = ' ';
              ballux[row * 15 + k] = -1000; 
              balluy[row * 15 + k] = -1000; 
                for(int b=0 ; b<50 ; b++)
              {
              if(ballx[bindex-1]=ballx[b]+60)
              ballx[b]-=1000;
              if(ballx[bindex-1]=ballx[b]-60){
              ballx[b]-=1000;
              break;}
              
              }
              ballx[bindex-1]=-1000;
              bally[bindex-1]=+1000;
             }
          break;
          }
          }
          } else {
            word = ""; 
         }
        }
     }

    
           for (int col=0; col<15; ++col) 
           {
             string word = "";
           for (int row=0; row<3; ++row) 
           {

             if (cball[row][col] != ' ') 
           {
                word=word+cball[row][col];
            for (int i=0; i<dictionarysize; ++i) 
           {
                if (dictionary[i] == word) {
                cout<<"Word Found (in col): "<<word<<endl;
                score=score+word.size(); 
                {
           ofstream f_write;
           f_write.open("Matchedwords.txt" , ios::out|ios::app);
           f_write<<"Word "<<foundwordcount++<<": "<<word<<endl;
           f_write.close();
         }
             for (int k=row-word.size()+1; k<=row; ++k)
                {
                  cball[k][col] = ' '; 
                  ballux[k * 15 + col] = -1000; 
                  balluy[k * 15 + col] = -1000; 
                  ballx[bindex-1]=-1000;
                  bally[bindex-1]=+1000;
                }
             break;
            }
            }
            } else {
              word = ""; 
            }
           }
           }

    
             for (int start_col=0; start_col<15; ++start_col)
             {
                 string word = "";
              for (int row=0, col=start_col; row<3 && col<15; ++row, ++col) 
             {
                 if (cball[row][col] != ' ') 
             {
                 word=word+cball[row][col];
              for (int i=0; i<dictionarysize; ++i) 
             {
                if (dictionary[i] == word)
               {
                cout<<"Word Found (in diagonal): "<<word<<endl;
                score=score+word.size(); 
               {
               ofstream f_write;
               f_write.open("Matchedwords.txt" , ios::out|ios::app);
               f_write<<"Word "<<foundwordcount++<<": "<<word<<endl;
               f_write.close();
               }
                for (int k=0; k<word.size(); ++k)
               {
                 cball[row - k][col - k] = ' '; 
                 int index = (row - k) * 15 + (col - k);
                 ballux[index] = -1000; 
                 balluy[index] = -1000;
                 ballx[bindex-1]=-1000;
                 bally[bindex-1]=+1000;
                }
                 break;
                }
                 }
                } else {
                word = ""; 
               }
                }
             }
    

}
}
}
      
	if(click>0)
	 timer-=0.05;
    
	}



/*
* our gateway main function
* */
int main(int argc, char*argv[]) {
	InitRandomizer(); // seed the random number generator...

	//Dictionary for matching the words. It contains the  369646 words.
	dictionary = new string[dictionarysize]; 
	ReadWords("words_alpha.txt", dictionary); // dictionary is an array of strings
	//print first 5 words from the dictionary
	for(int i=0; i < 5 ; ++i)
	{	cout<< " word "<< i << " =" << dictionary[i] <<endl;}
	
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT,2,2048);
	Mix_Music* bgMusic = Mix_LoadMUS("mymsc4.mp3");
	Mix_PlayMusic(bgMusic, -1);

	//Write your code here for filling the canvas with different Alphabets. You can use the Getalphabet function for getting the random alphabets
        


	glutInit(&argc, argv); // initialize the graphics library...
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
	glutInitWindowPosition(50, 50); // set the initial position of our window
	glutInitWindowSize(width, height); // set the size of our window
	glutCreateWindow("24I-0095 Abdul Samad's Word Shooter"); // set the title of our game window
	//SetCanvasSize(width, height); // set the number of pixels...

	// Register your functions to the library,
	// you are telling the library names of function to call for different tasks.
	RegisterTextures();
	glutDisplayFunc(DisplayFunction); // tell library which function to call for drawing Canvas.
	glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
	glutKeyboardFunc(PrintableKeys); // tell library which function to call for printable ASCII characters
	glutMouseFunc(MouseClicked);
	glutPassiveMotionFunc(MouseMoved); // Mouse

	//// This function tells the library to call our Timer function after 1000.0/FPS milliseconds...
	glutTimerFunc(1000.0/FPS, Timer, 0);

	//// now handle the control to library and it will call our registered functions when
	//// it deems necessary...

	glutMainLoop();

	return 1;
}
#endif /* ab17 */
