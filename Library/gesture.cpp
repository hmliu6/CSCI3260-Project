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
extern float cameraRotation, verticalRotation;

GLushort passiveMouseMode = 0;
extern CameraPosition cameraPosition;
float radius = 30.0f;
float initViewHorizontal = -90.0f;
float initViewVertical = -90.0f;
float viewRotateDegree[3] = { 0.0f, 0.0f, 0.0f };

void keyboardClick(unsigned char key, int x, int y){
  if (key == ' '){
	if (passiveMouseMode == 0)
		passiveMouseMode = 1;
	else
		passiveMouseMode = 0;
	}

  if (key == 'q')
    exit(0);
}

void arrowKey(int key, int x, int y){
	if (key == GLUT_KEY_UP){
		verticalRotation += 0.1f;
	}
	else if (key == GLUT_KEY_DOWN){
		verticalRotation -= 0.1f;
	}
	else if (key == GLUT_KEY_LEFT){
		cameraRotation -= 0.1f;
	}
	else if (key == GLUT_KEY_RIGHT){
		cameraRotation += 0.1f;
	}
}

void mouseCoordinate(int x, int y){
	//TODO: Use Mouse to do interactive events and animation
	if (passiveMouseMode == 1) {
		cameraPosition.x = glm::clamp((WINDOW_WIDTH / 2.0f - x) / 5.0f, -50.0f, 50.0f);
		cameraPosition.y = glm::clamp((WINDOW_HEIGHT / 2.0f - y) / 5.0f, 0.0f, 70.0f);
	}
}

void mouseWheelFunc(int button, int state, int x, int y){
	if ((button == 3) || (button == 4)){
		if (state == GLUT_UP)
			return;
		if (button == 3){
			radius -= 1.0f;
			cameraPosition.x = radius* cos(glm::radians(initViewHorizontal + viewRotateDegree[1]))*sin(glm::radians(initViewVertical + viewRotateDegree[0]));
			cameraPosition.y = radius* cos(glm::radians(initViewVertical + viewRotateDegree[0]));
			cameraPosition.z = radius* sin(glm::radians(initViewHorizontal + viewRotateDegree[1]))*sin(glm::radians(initViewVertical + viewRotateDegree[0]));
		}
		else{
			radius += 1.0f;
			cameraPosition.x = radius* cos(glm::radians(initViewHorizontal + viewRotateDegree[1]))*sin(glm::radians(initViewVertical + viewRotateDegree[0]));
			cameraPosition.y = radius* cos(glm::radians(initViewVertical + viewRotateDegree[0]));
			cameraPosition.z = radius* sin(glm::radians(initViewHorizontal + viewRotateDegree[1]))*sin(glm::radians(initViewVertical + viewRotateDegree[0]));
		}
	}
}