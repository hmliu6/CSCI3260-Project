#ifdef __APPLE__
	#include <GLUT/glut.h>
#elif defined _WIN32 || defined _WIN64
	#include "Dependencies/freeglut/freeglut.h"
#endif 

#include <iostream>
#include <cmath>
#include "Dependencies/glm/glm.hpp"
#include "cameraPosition.h"
#include "constant.h"


using namespace std;

 extern float specularCoefficient, diffuseCoefficient;
 GLushort passiveMouseMode = 0;
 extern CameraPosition cameraPosition;

void keyboardClick(unsigned char key, int x, int y){
	if (key == ' ')
	{
		if (passiveMouseMode == 0) {
			passiveMouseMode = 1;
		}
		else {
			passiveMouseMode = 0;
		}
	}
  if (key == 'q')
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
	//TODO: Use Mouse to do interactive events and animation
	if (passiveMouseMode == 1) {
		cameraPosition.x = glm::clamp((WINDOW_WIDTH / 2.0f - x) / 5.0f, -50.0f, 50.0f);
		cameraPosition.y = glm::clamp((WINDOW_HEIGHT / 2.0f - y) / 5.0f, 0.0f, 70.0f);
	}
}
