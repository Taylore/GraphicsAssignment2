/*		Ocean CLASS
 *	AUTHOR: STEWART TAYLOR
 *------------------------------------
 * This class generates Procedurally Generated Ocean 
 * It also simulates the oceans animation
 * 
 * Last Updated: 09/11/2012
*/
#include "Ocean.h"
#include "TextureLoader.h"
#include <freeglut.h>
#include <math.h>
#include "NormalHelper.h"
#include <time.h>
  
GLfloat heightPoints[64+1][64+1];
GLfloat seascO[64+1][64+1];
GLfloat cd[64+1];
GLfloat normals[64+1][64+1][3];

Ocean::Ocean()
{

}

Ocean::Ocean(int size)
{
	xPosition = -30;
	yPosition = 0;
	zPosition = -30;
	scale = 1;
	timer = 0;

	for (int x = 0; x <= size; x++)
	{
		cd[x] = x;
	}

	texName = TextureLoader::loadTexture("Textures\\ocean.bmp");

	genMap(size);
}

Ocean::~Ocean(void)
{

}


void Ocean::genMap(int mapSize)
{	
	srand(time(0));
	int random_seed = rand();
	srand(random_seed);

	for (int i = 0; i <= mapSize; i++)
	{
		for (int j = 0; j <= mapSize; j++)
		{
			int rSize = ((rand()%20)+2);

			if(  (i < rSize) || (j < rSize) || (i > mapSize-rSize - 1) || ( j > mapSize-rSize))
			{
				heightPoints[i][j] = (float)rand()/((float)RAND_MAX/2.11f);
			}
			seascO[i][j] = heightPoints[i][j];
		}
	}

	update();
}


void Ocean::calculateNormals(int size)
{
	int i, j;
	float v1[3],v2[3],out[3];
	v1[0] = cd[1];
	v1[1] = 0;
	v2[0] = 0;
	v2[1] = cd[1];
	for (j = 0; j < size; j++)
      for (i = 0; i < size; i++) {
		  v1[2] = heightPoints[i+1][j] - heightPoints[i][j];
          v2[2] = heightPoints[i][j+1] - heightPoints[i][j];
		  NormalHelper::normcrossprod(v1,v2,out);
		  normals[i][j][0] = -out[0];
		  normals[i][j][1] = -out[1];
		  normals[i][j][2] = -out[2];
	  }
   for (j = 0; j < size; j++) {
	  normals[size][j][0] = normals[size-1][j][0];
      normals[size][j][1] = normals[size-1][j][1];
      normals[size][j][2] = normals[size-1][j][2];
	  }
   for (i = 0; i < size; i++) {
	  normals[i][size][0] = normals[i][size-1][0];
	  normals[i][size][1] = normals[i][size-1][1];
	  normals[i][size][2] = normals[i][size-1][2];
	  }
   normals[size][size][0] = normals[size-1][size-1][0];
   normals[size][size][1] = normals[size-1][size-1][1];
   normals[size][size][2] = normals[size-1][size-1][2];
}



void Ocean::getColor(GLfloat color[4], int x , int y)
{
	float distance = distanceT(x, y, 32, 32);  
	float perc = distance/46;

	float r =  (0.4-perc);
	float g =  (0.8-perc) ;
	float b =  (0.75-perc) ;
	float a = (1-perc) ;

	color[0] = r; color[1] = g; color[2] = b; color[3] = a;  
}


double Ocean::distanceT(double dX0, double dY0, double dX1, double dY1)
{
    return sqrt((dX1 - dX0)*(dX1 - dX0) + (dY1 - dY0)*(dY1 - dY0));
}


void Ocean::display(void)
{
	int size = 64;

	glPushMatrix(); 

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D); 
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, texName);

	glMatrixMode(GL_MODELVIEW);

	glTranslated(xPosition ,yPosition ,zPosition);
	glRotatef(90, 1.0, 0.0, 0.0);
	glTranslated(0,0 ,0);
	glScaled(scale ,scale ,1);


   for (int j = 0; j < size; j++)
      for (int i = 0; i < size; i++)
	  {
		  glBegin(GL_POLYGON);
		  drawVertex(i,j);
		  glEnd();
	  }
      glEnd();  

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
}



void Ocean::drawVertex(int i , int j)
{
	GLfloat color[4] = {0,0,0 , 0};

	getColor(color , i,j);
	glColor4f(color[0], color[1], color[2] , color[3]);	  glNormal3fv(&normals[i][j][0]);   glTexCoord2f(0,0);glVertex3f(cd[i],cd[j],-heightPoints[i][j]);

	getColor(color , i+1,j);
	glColor4f(color[0], color[1], color[2], color[3]);	  glNormal3fv(&normals[i+1][j][0]);  glTexCoord2f(1,0); glVertex3f(cd[i+1],cd[j],-heightPoints[i+1][j]);

	getColor(color , i+1,j+1);
	glColor4f(color[0], color[1], color[2], color[3]);	  glNormal3fv(&normals[i+1][j+1][0]); glTexCoord2f(1,1); glVertex3f(cd[i+1],cd[j+1],-heightPoints[i+1][j+1]);

	getColor(color , i, j+1);
	glColor4f(color[0], color[1], color[2], color[3]);  glNormal3fv(&normals[i][j+1][0]); glTexCoord2f(0,1); glVertex3f(cd[i],cd[j+1],-heightPoints[i][j+1]);
}


void Ocean::update()
{
	timer+= 0.008f;

	for (int i = 0; i <= 64; i++)
	{
		for (int j = 0; j <= 64; j++)
		{
			GLfloat val1 = 0.1f *sin((float)(i+ timer))  ;
			GLfloat val2 = 0.3f *sin((float)(j+ (timer*2)))  ;
			GLfloat val3 = 0.3f *sin((float)(seascO[i][j] + timer) ) ;

			heightPoints[i][j]  =val1 + val2 + val3 - 0.4f ;
		}
	}

	calculateNormals(64);
}