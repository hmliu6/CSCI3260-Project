#ifdef __APPLE__
	#include <GLUT/glut.h>
#elif defined _WIN32 || defined _WIN64
	#include "Dependencies/freeglut/freeglut.h"
#endif 

#include <iostream>
#include <cmath>

using namespace std;

 extern float specularCoefficient, diffuseCoefficient;

void keyboardClick(unsigned char key, int x, int y){
  if (key == ' ')
    exit(0);
}

void arrowKey(int key, int x, int y){
	if (key == GLUT_KEY_UP){
	}
	else if (key == GLUT_KEY_DOWN){
	}
	else if (key == GLUT_KEY_LEFT){
	}
	else if (key == GLUT_KEY_RIGHT){
	}
}

void mouseCoordinate(int x, int y){

}
