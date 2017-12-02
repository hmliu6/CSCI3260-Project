#ifdef __APPLE__
	#include <GLUT/glut.h>
#elif defined _WIN32 || defined _WIN64
	#include "Dependencies/freeglut/freeglut.h"
#endif 

#include <iostream>
#include <cmath>
#include "Dependencies/glm/glm.hpp"
#include "cameraPosition.hpp"
#include "constant.hpp"

using namespace std;

extern float specularCoefficient, diffuseCoefficient;
extern float cameraPosAngle;

bool passiveMouseMode = false;
extern CameraPosition cameraPosition;

void keyboardClick(unsigned char key, int x, int y){
  if (key == ' ')
	passiveMouseMode = !passiveMouseMode;
  else if (key == 'a'){
  }
  else if (key == 'q')
    exit(0);
}

void arrowKey(int key, int x, int y){
	if (key == GLUT_KEY_UP){
	}
	else if (key == GLUT_KEY_DOWN){
	}
	else if (key == GLUT_KEY_LEFT){
		cameraPosAngle -= 0.1f;
	}
	else if (key == GLUT_KEY_RIGHT){
		cameraPosAngle += 0.1f;
	}
}

void mouseCoordinate(int x, int y){
	
}

void mouseWheelFunc(int button, int state, int x, int y){

}